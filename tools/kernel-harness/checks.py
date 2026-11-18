"""Regression checks for the physics constants and the relativistic kernel.

  gm       (review verification item 3) GM consistency: the G in initialstate.cpp matches the
           SLF mass convention, so pos.w for the Sun, Earth, Moon and Jupiter equals the standard
           GM values; ImportSLF uses that constant; relativisticC1 in adamsfma.cl is 6 GM_sun / c^2.
           Catches regressions of review findings C1 and C2. No OpenCL needed.
  mercury  (review verification item 2) Mercury's perihelion advance: Sun + Mercury for --years
           with the relativistic and the newtonian kernel. The difference in the perihelion
           precession rate must be 43 +/- 1 arcsec per century, as General Relativity predicts.
  ephemeris (review verification item 4) Comparison with JPL Horizons (DE441): the 16 gravitating
           bodies of mpcsmall.slf for 10 years at 4 h steps with AB11/AM10 and the relativistic
           kernel; heliocentric positions after 1 and 10 years must be within the limits in
           EPHEMERIS_LIMITS_KM. The reference vectors are in horizons_reference.json
           (fetch_horizons.py). The run starts from mpcsmall.slf as the app loads it, except
           Pluto, which starts from its Horizons state (see check_ephemeris for why).

usage: python checks.py [--only gm,mercury,ephemeris] [--years 20] [--platform NVIDIA]
Exits with status 1 if any check fails.
"""
import argparse
import math
import os
import re
import sys
import time

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import harness  # noqa: E402  (tools/kernel-harness/harness.py)

SRC = os.path.join(harness.REPO, "src", "OpenCLSolarSystem")
KERNEL = os.path.join(SRC, "adamsfma.cl")
SLF = os.path.join(harness.REPO, "src", "OrbToSlf", "OrbToSlfConsole", "mpcsmall.slf")

# Standard GM values in km^3/s^2 (IAU / JPL DE ephemerides)
GM_REFERENCE = {
    "Sun": (1.32712440018e11, 1e-9),
    "Earth": (398600.4418, 1e-7),
    "Moon": (4902.800066, 1e-7),
    "Jupiter": (126712764.8, 1e-7),
}
C_KMS = 299792.458
# pos.w (G' x mass) is GM in km^3/s^2 times this: acceleration in km/s^2 with distances in Gm
GM_KM3S2_TO_POSW = 1e-12

ARCSEC_PER_RAD = 180.0 / math.pi * 3600.0
SECONDS_PER_CENTURY = 36525.0 * 86400.0
# GR perihelion advance of Mercury: 6 pi GM / (c^2 a (1 - e^2)) per orbit = 42.98 arcsec/century
MERCURY_EXPECTED = 42.98
MERCURY_TOLERANCE = 1.0
# The newtonian two-body orbit should not precess at all; anything left is integration error
NEWTONIAN_DRIFT_LIMIT = 0.5

HORIZONS_REFERENCE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "horizons_reference.json")
# SLF index and Horizons reference name of the bodies compared. Mars to Pluto use the system
# barycentres, which is what a point mass with the system's mass follows.
EPHEMERIS_BODIES = {
    "Mercury": (1, "Mercury"), "Venus": (2, "Venus"), "Earth": (3, "Earth"), "Moon": (4, "Moon"),
    "Mars": (5, "Mars barycenter"), "Jupiter": (6, "Jupiter barycenter"),
    "Saturn": (7, "Saturn barycenter"), "Uranus": (8, "Uranus barycenter"),
    "Neptune": (9, "Neptune barycenter"), "Pluto": (10, "Pluto barycenter"),
}
# Heliocentric position error limits in km after (1 year, 10 years), about 2-3 times the errors
# measured when the check was written (README). "Moon-geo" is the Moon relative to the Earth.
EPHEMERIS_LIMITS_KM = {
    "Mercury": (100, 1000), "Venus": (5, 30), "Earth": (5, 40), "Moon-geo": (50, 600),
    "Mars": (20, 150), "Jupiter": (5, 30), "Saturn": (5, 10), "Uranus": (5, 10),
    "Neptune": (5, 25), "Pluto": (5, 40),
}
# OrbToSlf takes these from Horizons (Mars to Neptune as system barycentres), so their SLF epoch
# states must match the reference. Pluto is not included: OrbToSlf builds it from its MPC orbital
# elements like the asteroids, which puts it about 2300 km from Horizons.
EPOCH_MATCH_BODIES = ("Mercury", "Venus", "Earth", "Moon", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune")
# Bodies started from their Horizons epoch state instead of the SLF file
HORIZONS_START_BODIES = ("Pluto",)
EPOCH_MATCH_LIMIT_KM = 1.0
EPOCH_MATCH_LIMIT_M_S = 0.001


class Results:
    def __init__(self):
        self.failures = 0

    def report(self, ok, name, detail):
        print(f"  [{'PASS' if ok else 'FAIL'}] {name}: {detail}")
        if not ok:
            self.failures += 1


def read_define(path, pattern):
    text = open(path, encoding="utf-8").read()
    m = re.search(pattern, text, re.MULTILINE)
    return (float(m.group(1)) if m else None), text


def check_gm(r):
    print("GM consistency (verification item 3, findings C1 and C2)")

    # The app's G must be the one the harness uses, and the one ImportSLF multiplies by
    g_app, initialstate = read_define(os.path.join(SRC, "initialstate.cpp"),
                                      r"static\s+const\s+double\s+G_SLF_SCALED\s*=\s*([0-9.eE+-]+)\s*;")
    r.report(g_app is not None, "G_SLF_SCALED defined in initialstate.cpp",
             f"{g_app!r}" if g_app is not None else "not found")
    if g_app is None:
        return
    r.report(g_app == harness.G_SLF_SCALED, "harness uses the app's G",
             f"app {g_app!r}, harness {harness.G_SLF_SCALED!r}")

    # OrbToSlf writes the SLF masses with its own G; the app must divide them out with the same one
    g_orb, _ = read_define(os.path.join(harness.REPO, "src", "OrbToSlf", "OrbToSlf", "Constants.cs"),
                           r"public\s+const\s+double\s+GravitationalConstant\s*=\s*([0-9.eE+-]+)\s*;")
    if g_orb is None:
        r.report(False, "OrbToSlf G matches the app's G", "GravitationalConstant not found in Constants.cs")
    else:
        rel = (g_app - g_orb * 1e3) / (g_orb * 1e3)
        r.report(abs(rel) <= 1e-8, "OrbToSlf G matches the app's G",
                 f"OrbToSlf {g_orb!r} x 1e3 vs app {g_app!r}, relative difference {rel:.1e} (limit 1e-8)")
    uses = len(re.findall(r"\*\s*G_SLF_SCALED", initialstate))
    stray = re.findall(r"6\.67\d*E-0?8", initialstate.split("static const double G_SLF_SCALED")[1].split(";", 1)[1], re.I)
    r.report(uses >= 2 and not stray, "ImportSLF multiplies the masses by G_SLF_SCALED",
             f"{uses} uses" + (f", stray G literals {stray}" if stray else ""))

    # pos.w as the app would compute it, against the standard GM values
    pos, _ = harness.load_slf(SLF, 16)
    names = [line.split("#")[1].strip() for line in open(SLF, encoding="utf-8").read().splitlines()[1::3][:16]]
    for body, (gm_ref, tol) in GM_REFERENCE.items():
        idx = next(i for i, n in enumerate(names) if n.startswith(body))
        gm = pos[idx, 3] / GM_KM3S2_TO_POSW
        rel = (gm - gm_ref) / gm_ref
        r.report(abs(rel) <= tol, f"{body} GM", f"{gm:.12g} km^3/s^2, relative error {rel:.2e} (limit {tol:g})")

    # relativisticC1 must be 6 GM_sun / c^2 in Gm (the position unit), not km or 1000 km
    c1, _ = read_define(KERNEL, r"^#define\s+relativisticC1\s+([0-9.eE+-]+)")
    c1_expected = 6.0 * GM_REFERENCE["Sun"][0] / (C_KMS * C_KMS) * 1e-6
    ok = c1 is not None and abs(c1 - c1_expected) / c1_expected <= 1e-9
    r.report(ok, "relativisticC1 = 6 GM_sun / c^2 in Gm",
             f"{c1!r}, expected {c1_expected:.12e}" if c1 is not None else "not found in adamsfma.cl")


def perihelion_rate(samples, mu_posw):
    """Least-squares precession rate (arcsec/century) of the heliocentric eccentricity vector."""
    mu = mu_posw / GM_KM3S2_TO_POSW  # km^3/s^2
    t = np.array([s[0] for s in samples])
    sun = np.array([s[1][:, 0, :3] for s in samples])      # [k, 2(pos,vel), 3]
    merc = np.array([s[1][:, 1, :3] for s in samples])
    rv = (merc[:, 0] - sun[:, 0]) * 1e6                   # km
    vv = merc[:, 1] - sun[:, 1]                           # km/s
    rn = np.linalg.norm(rv, axis=1)[:, None]
    ev = ((vv * vv).sum(1)[:, None] - mu / rn) * rv / mu - (rv * vv).sum(1)[:, None] * vv / mu

    # Angle of e in the (fixed) orbital plane, from its initial direction
    h0 = np.cross(rv[0], vv[0])
    p_hat = ev[0] / np.linalg.norm(ev[0])
    q_hat = np.cross(h0 / np.linalg.norm(h0), p_hat)
    angle = np.unwrap(np.arctan2(ev @ q_hat, ev @ p_hat))
    slope = np.polyfit(t, angle, 1)[0]                    # rad/s
    return slope * SECONDS_PER_CENTURY * ARCSEC_PER_RAD, np.linalg.norm(ev[0])


def check_mercury(r, years, platform):
    print(f"Mercury perihelion advance (verification item 2), Sun + Mercury for {years:g} years")
    pos, vel = harness.load_slf(SLF, 2)  # body 0 Sun, body 1 Mercury
    dt = 43200.0                          # 12 h: ~176 steps per Mercury orbit
    steps = int(round(years * 365.25 * 86400 / dt))
    sample_every = 10                     # every 5 days

    rates = {}
    for acc in ("newtonian", "relativistic"):
        t0 = time.time()
        final, samples, dev = harness.simulate(pos, vel, kernel=KERNEL, steps=steps, dt=dt, ngrav=2,
                                               acc_kernel=acc, platform=platform,
                                               sample_every=sample_every, sample_bodies=[0, 1])
        if not np.isfinite(final).all():
            r.report(False, f"{acc} run", "non-finite positions or velocities")
            return
        rates[acc], ecc = perihelion_rate(samples, pos[0, 3] + pos[1, 3])
        print(f"    {acc:12s} {rates[acc]:+9.3f} arcsec/century  (e = {ecc:.5f}, {steps} steps, "
              f"{time.time() - t0:.0f} s on {dev.name.strip()})")

    r.report(abs(rates["newtonian"]) <= NEWTONIAN_DRIFT_LIMIT, "newtonian orbit does not precess",
             f"{rates['newtonian']:+.3f} arcsec/century (limit +/-{NEWTONIAN_DRIFT_LIMIT})")
    gr = rates["relativistic"] - rates["newtonian"]
    r.report(abs(gr - MERCURY_EXPECTED) <= MERCURY_TOLERANCE, "relativistic advance",
             f"{gr:.3f} arcsec/century (expected {MERCURY_EXPECTED} +/- {MERCURY_TOLERANCE})")


def check_ephemeris(r, platform):
    import json

    print("Comparison with JPL Horizons (verification item 4)")
    ref = json.load(open(HORIZONS_REFERENCE, encoding="utf-8"))
    pos, vel = harness.load_slf(SLF, 16)

    def horizons(name, k):
        state = ref["bodies"][name]["states"][k]
        return np.array(state["position_km"]), np.array(state["velocity_km_s"])

    # The SLF file was made from Horizons, so its epoch states must match the reference.
    # This confirms the bodies, frame (ICRF equatorial, Sun-centred) and time scale (TDB) agree,
    # and that the file has the system barycentres for Mars to Neptune.
    if abs(ref["epoch_jd_tdb"] - float(open(SLF, encoding="utf-8").readline())) > 1e-9:
        r.report(False, "horizons_reference.json epoch matches mpcsmall.slf",
                 f"{ref['epoch_jd_tdb']} vs the SLF file's; rerun fetch_horizons.py --epoch <SLF epoch>")
        return
    worst = (0.0, 0.0, "")
    for name in EPOCH_MATCH_BODIES:
        i, ref_name = EPHEMERIS_BODIES[name]
        p, v = horizons(ref_name, 0)
        dp = np.linalg.norm((pos[i, :3] - pos[0, :3]) * 1e6 - p)
        dv = np.linalg.norm((vel[i, :3] - vel[0, :3]) - v) * 1e3
        if dp > worst[0] or dv > worst[1]:
            worst = (max(dp, worst[0]), max(dv, worst[1]), name)
    r.report(worst[0] <= EPOCH_MATCH_LIMIT_KM and worst[1] <= EPOCH_MATCH_LIMIT_M_S,
             "mpcsmall.slf epoch states match Horizons",
             f"largest difference {worst[0]:.3f} km, {worst[1]:.4f} m/s ({', '.join(EPOCH_MATCH_BODIES)})")

    # Run from the SLF file as the app loads it. Pluto starts from its Horizons barycentre state
    # instead: its SLF state comes from MPC orbital elements, about 2300 km from Horizons, which
    # would hide any integration error in it.
    for name in HORIZONS_START_BODIES:
        i, ref_name = EPHEMERIS_BODIES[name]
        p, v = horizons(ref_name, 0)
        pos[i, :3] = pos[0, :3] + p / 1e6
        vel[i, :3] = vel[0, :3] + v

    dt = 14400.0
    days = ref["days"]
    steps_per_sample = int(round(days[1] * 86400 / dt))
    if any(abs(d * 86400 / dt - k * steps_per_sample) > 1e-9 for k, d in zip((0, 1, 10), days)):
        raise SystemExit(f"horizons_reference.json days {days} are not 0, 1 and 10 x {days[1]}")

    t0 = time.time()
    final, samples, dev = harness.simulate(pos, vel, kernel=KERNEL, steps=10 * steps_per_sample, dt=dt, ngrav=16,
                                           acc_kernel="relativistic", platform=platform,
                                           sample_every=steps_per_sample, sample_bodies=list(range(16)))
    print(f"    16 bodies, {10 * steps_per_sample} steps of 4 h in {time.time() - t0:.0f} s on {dev.name.strip()}")
    if not np.isfinite(final).all():
        r.report(False, "ephemeris run", "non-finite positions or velocities")
        return

    for k, (sample_index, label) in enumerate(((1, "1 year"), (10, "10 years")), start=1):
        state = samples[sample_index][1][0]  # positions [16, 4], Gm
        errors = {}
        for name, (i, ref_name) in EPHEMERIS_BODIES.items():
            p, _ = horizons(ref_name, k)
            errors[name] = np.linalg.norm((state[i, :3] - state[0, :3]) * 1e6 - p)
        pe, _ = horizons("Earth", k)
        pm, _ = horizons("Moon", k)
        errors["Moon-geo"] = np.linalg.norm((state[4, :3] - state[3, :3]) * 1e6 - (pm - pe))

        for name, limits in EPHEMERIS_LIMITS_KM.items():
            limit = limits[k - 1]
            r.report(errors[name] <= limit, f"{name} after {label}",
                     f"{errors[name]:.1f} km (limit {limit} km)")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--only", default="gm,mercury,ephemeris", help="comma-separated checks to run")
    ap.add_argument("--years", type=float, default=20.0, help="length of the Mercury run (100 in the review; 20 gives the same rate to 0.001 arcsec)")
    ap.add_argument("--platform", default="", help="substring of the OpenCL platform name")
    a = ap.parse_args()

    r = Results()
    selected = [c.strip() for c in a.only.split(",") if c.strip()]
    for c in selected:
        if c == "gm":
            check_gm(r)
        elif c == "mercury":
            check_mercury(r, a.years, a.platform)
        elif c == "ephemeris":
            check_ephemeris(r, a.platform)
        else:
            raise SystemExit(f"unknown check '{c}'")

    print("All checks passed" if r.failures == 0 else f"{r.failures} check(s) FAILED")
    sys.exit(1 if r.failures else 0)


if __name__ == "__main__":
    main()
