"""Runs the OpenCLSolarSystem kernels without the GUI, the same way CLModel::ExecuteKernels does.

Loads the first --bodies bodies of an SLF file (same rules as InitialState::ImportSLF),
integrates them for --days with the chosen kernels, and saves the final positions and
velocities to a .npy file (shape [2, bodies, 4]: positions in Gm, velocities in km/s).

Startup modes:
  rk4  (default) one call per step for steps 0..15: --substeps x 4 RK4 stages of
       rungeKutta4Startup (current host flow)
  adams          per-stage adamsStartup for steps 0..15 (host flow before H1). Use it with
                 an older adamsfma.cl, e.g.  git show <commit>:src/OpenCLSolarSystem/adamsfma.cl
"""
import argparse
import os
import time

import numpy as np
import pyopencl as cl

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))

# Must match G_SLF_SCALED in src/OpenCLSolarSystem/initialstate.cpp
G_SLF_SCALED = 6.67259849874148E-08
# Default softening; must match espSqr in CLModel::CLModel
EPS_SQR = 0.0
# Default work-group size, as CLModel::groupSize
GROUP_SIZE = 64


def parse_args():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--kernel", default=os.path.join(REPO, "src", "OpenCLSolarSystem", "adamsfma.cl"))
    ap.add_argument("--slf", default=os.path.join(REPO, "src", "OrbToSlf", "OrbToSlfConsole", "mpcsmall.slf"))
    ap.add_argument("--dt", type=float, default=14400.0, help="time step in seconds (default 4 h)")
    ap.add_argument("--steps", type=int, help="number of steps (overrides --days)")
    ap.add_argument("--days", type=float, default=365.25, help="length of run, rounded to whole steps")
    ap.add_argument("--startup", choices=["rk4", "adams"], default="rk4")
    ap.add_argument("--substeps", type=int, default=10, help="RK4 sub-steps per startup step (CLModel::startupSubSteps)")
    ap.add_argument("--acc", default="relativistic", help="acceleration kernel (newtonian, relativistic)")
    ap.add_argument("--ab", default="adamsBashforth11")
    ap.add_argument("--am", default="adamsMoulton10")
    ap.add_argument("--bodies", type=int, default=64, help="number of bodies to load")
    ap.add_argument("--group", type=int, default=GROUP_SIZE, help="work-group size (CLModel::groupSize)")
    ap.add_argument("--grav", type=int, default=16, help="number of gravitating bodies")
    ap.add_argument("--eps", type=float, default=EPS_SQR, help="softening length squared in Gm^2 (1e-6 = before M1)")
    ap.add_argument("--platform", default="", help="substring of the OpenCL platform name (default: first with fp64)")
    ap.add_argument("--out", required=True, help="output .npy file")
    return ap.parse_args()


def load_slf(path, n):
    lines = [l.split("#")[0].split() for l in open(path, encoding="utf-8").read().splitlines()]
    pos = np.zeros((n, 4))
    vel = np.zeros((n, 4))
    names = []
    raw = open(path, encoding="utf-8").read().splitlines()
    li = 1
    for i in range(n):
        hdr = lines[li][1:] if i == 0 else lines[li]  # the first body has an extra leading token
        mass, rel = float(hdr[0]), float(hdr[3])
        names.append(raw[li].split("#")[1].strip() if "#" in raw[li] else str(i))
        pos[i, :3] = [float(x) for x in lines[li + 1]]
        vel[i, :3] = [float(x) for x in lines[li + 2]]
        li += 3
        pos[i, 3] = (2.83E-09 if mass < 1e-30 else mass) * G_SLF_SCALED
        vel[i, 3] = rel
    # The Moon is geocentric in SLF files; ImportSLF adds the Earth's state
    earth = next((i for i, s in enumerate(names) if s.startswith("Earth")), -1)
    moon = next((i for i, s in enumerate(names) if s.startswith("Moon")), -1)
    if earth >= 0 and moon >= 0:
        pos[moon, :3] += pos[earth, :3]
        vel[moon, :3] += vel[earth, :3]
    return pos, vel


def pick_device(platform_sub):
    for p in cl.get_platforms():
        if platform_sub and platform_sub not in p.name:
            continue
        for d in p.get_devices():
            if d.double_fp_config:
                return p, d
    raise SystemExit("no OpenCL device with fp64 found" + (f" on platform matching '{platform_sub}'" if platform_sub else ""))


def simulate(pos, vel, *, kernel, steps, dt=14400.0, ngrav=16, acc_kernel="relativistic",
             ab="adamsBashforth11", am="adamsMoulton10", startup="rk4", substeps=10, eps=EPS_SQR,
             group=GROUP_SIZE, platform="", sample_every=0, sample_bodies=None):
    """Integrates pos/vel ([n, 4] arrays as from load_slf) for `steps` steps of `dt` seconds.

    Runs the kernels in the same order as CLModel::ExecuteKernels. Returns (final, samples, device):
      final   [2, n, 4] positions (Gm) and velocities (km/s) after the last step
      samples if sample_every > 0: list of (time_s, [2, len(sample_bodies), 4]) taken every
              sample_every steps (and at step 0), else []
    """
    n = pos.shape[0]
    ngrav = min(ngrav, n)  # as CLModel::CreateBufferObjects
    # As CLModel::GlobalWorkSize: rounded up to whole work-groups, the kernels skip the extra work-items
    global_size = (n + group - 1) // group * group
    sample_bodies = list(range(n)) if sample_bodies is None else list(sample_bodies)

    plat, dev = pick_device(platform)
    ctx = cl.Context([dev])
    q = cl.CommandQueue(ctx)
    # Same prefix and options as CLModel::CompileProgramAndCreateKernels
    src = "#pragma OPENCL EXTENSION cl_khr_fp64 : enable \r\n" + open(kernel, encoding="utf-8").read()
    prg = cl.Program(ctx, src).build(options="-cl-mad-enable")

    mf = cl.mem_flags

    def buf(count):
        return cl.Buffer(ctx, mf.READ_WRITE, size=count * 32)

    pos = np.ascontiguousarray(pos, dtype=np.float64)
    vel = np.ascontiguousarray(vel, dtype=np.float64)
    currPos = cl.Buffer(ctx, mf.READ_WRITE | mf.COPY_HOST_PTR, hostbuf=pos)
    gravPos = cl.Buffer(ctx, mf.READ_WRITE | mf.COPY_HOST_PTR, hostbuf=pos[:ngrav].copy())
    currVel = cl.Buffer(ctx, mf.READ_WRITE | mf.COPY_HOST_PTR, hostbuf=vel)
    newPos, newVel, acc, posLast, velLast = buf(n), buf(n), buf(n), buf(n), buf(n)
    velHist, accHist, sumVel, sumAcc = buf(16 * n), buf(16 * n), buf(n), buf(n)

    i32, f64 = np.int32, np.float64
    accK = getattr(prg, acc_kernel)
    # Same argument order as CLModel::SetKernelArgumentsAndGroupSize
    if acc_kernel == "newtonian":
        accK.set_args(gravPos, currPos, i32(ngrav), f64(eps), acc, i32(n))
    elif acc_kernel == "relativisticLocal":
        accK.set_args(gravPos, currPos, currVel, i32(ngrav), f64(eps), acc, cl.LocalMemory(32 * group), i32(n))
    else:
        accK.set_args(gravPos, currPos, currVel, i32(ngrav), f64(eps), acc, i32(n))
    abK, amK = getattr(prg, ab), getattr(prg, am)
    startK = prg.rungeKutta4Startup if startup == "rk4" else prg.adamsStartup

    def adams_args(k, stage, step, *extra):
        k.set_args(currPos, currVel, acc, f64(dt), newPos, newVel, i32(stage), i32(step), i32(n),
                   posLast, velLast, velHist, accHist, *extra)

    def run(k):
        cl.enqueue_nd_range_kernel(q, k, (global_size,), (group,))

    def copy_new_to_current():
        cl.enqueue_copy(q, currPos, newPos, byte_count=n * 32)
        cl.enqueue_copy(q, gravPos, newPos, byte_count=ngrav * 32)
        cl.enqueue_copy(q, currVel, newVel, byte_count=n * 32)

    def read_state():
        res = np.empty((2, n, 4))
        cl.enqueue_copy(q, res[0], currPos)
        cl.enqueue_copy(q, res[1], currVel)
        q.finish()
        return res

    samples = []
    if sample_every > 0:
        samples.append((0.0, read_state()[:, sample_bodies]))

    for step in range(steps):
        if step < 16 and startup == "rk4":
            for sub in range(substeps):
                for rk in range(4):
                    run(accK)
                    adams_args(startK, rk, step, i32(sub), i32(substeps), sumVel, sumAcc)
                    run(startK)
                    copy_new_to_current()
        else:
            for stage in (1, 0):  # predictor, corrector (numStages = 1)
                run(accK)
                k = startK if step < 16 else (abK if stage == 1 else amK)
                adams_args(k, stage, step)
                run(k)
                copy_new_to_current()
        if sample_every > 0 and (step + 1) % sample_every == 0:
            samples.append(((step + 1) * dt, read_state()[:, sample_bodies]))

    return read_state(), samples, dev


def main():
    a = parse_args()
    steps = a.steps if a.steps is not None else int(round(a.days * 86400 / a.dt))
    pos, vel = load_slf(a.slf, a.bodies)

    t0 = time.time()
    res, _, dev = simulate(pos, vel, kernel=a.kernel, steps=steps, dt=a.dt, ngrav=a.grav, acc_kernel=a.acc,
                           ab=a.ab, am=a.am, startup=a.startup, substeps=a.substeps, eps=a.eps,
                           group=a.group, platform=a.platform)
    if not np.isfinite(res).all():
        bad = np.unique(np.where(~np.isfinite(res))[1])
        print(f"WARNING: non-finite values for bodies {bad.tolist()}")
    np.save(a.out, res)
    print(f"{a.startup} startup, {a.ab}/{a.am}, {a.acc}, eps={a.eps:g}, dt={a.dt:g} s, {steps} steps "
          f"({steps * a.dt / 86400:.3f} d) in {time.time() - t0:.1f} s on {dev.name.strip()}  -> {a.out}")


if __name__ == "__main__":
    main()
