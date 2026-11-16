# Kernel test harness

Runs the OpenCL kernels from `src/OpenCLSolarSystem/adamsfma.cl` without the GUI, in the same
order as `CLModel::ExecuteKernels`: acceleration, then the startup or Adams kernel, then the
buffer copies. Use it to measure accuracy before and after changes to the kernels or integrator.

It needs Python 3, an OpenCL device with fp64 (any recent GPU driver, Intel CPU runtime, or PoCL),
and the packages in `requirements.txt`:

```sh
python -m venv .venv
.venv/Scripts/pip install -r tools/kernel-harness/requirements.txt   # .venv/bin/pip on Linux
```

## Example: startup accuracy (H1)

The reference is the same kernels at Δt/16 (15 min). Both runs cover the same simulated time
(2192 × 4 h = 35072 × 15 min, about 1 year).

```sh
cd tools/kernel-harness
python harness.py --dt 900   --steps 35072 --out ref_1y.npy
python harness.py --dt 14400 --steps 2192  --out new_1y.npy

# The old adamsStartup, from the commit before H1
git show d088776:src/OpenCLSolarSystem/adamsfma.cl > old.cl
python harness.py --kernel old.cl --startup adams --dt 14400 --steps 2192 --out old_1y.npy

python compare.py ref_1y.npy old_1y.npy new_1y.npy
```

Results on an RTX 4070 Ti SUPER (first 64 bodies of `mpcsmall.slf`, 16 gravitating,
AB11/AM10, relativistic):

| Body | old startup, 1 yr | RK4 startup, 1 yr |
|---|---|---|
| Mercury | 866,795 km | 1.6 cm |
| Earth | 42,748 km | 2 cm |
| Moon (geocentric) | 239,147 km | 2 cm |
| Mars | 8,623 km | < 1 cm |
| Jupiter | 65 km | 1 cm |

## Example: softening (M1)

Before M1 the force kernels used a 1,000 km softening length (`espSqr = 1e-6` Gm²).
`--eps 1e-6` reproduces that with the current kernels.

```sh
python harness.py --steps 2192 --out eps0.npy
python harness.py --steps 2192 --eps 1e-6 --out eps6.npy
python compare.py eps0.npy eps6.npy
```

After 1 year the softening had moved the Moon about 635 km relative to the Earth
(Earth 7.6 km, Mercury 1.2 km).

## Options

`python harness.py --help` lists them all. The main ones:

| Option | Default | |
|---|---|---|
| `--kernel` | `src/OpenCLSolarSystem/adamsfma.cl` | kernel source |
| `--slf` | `src/OrbToSlf/OrbToSlfConsole/mpcsmall.slf` | initial state |
| `--dt` | 14400 | step in seconds |
| `--steps` / `--days` | 365.25 days | run length |
| `--acc` | `relativistic` | acceleration kernel: `newtonian`, `relativistic` or `relativisticLocal` |
| `--eps` | 0 | softening length squared in Gm² (`1e-6` = before M1) |
| `--ab`, `--am` | `adamsBashforth11`, `adamsMoulton10` | predictor and corrector kernels |
| `--startup` | `rk4` | `adams` for the pre-H1 host flow (needs an old kernel file) |
| `--substeps` | 10 | RK4 sub-steps per startup step |
| `--bodies`, `--grav` | 64, 16 | bodies loaded and how many of them gravitate |
| `--platform` | first with fp64 | substring of the OpenCL platform name, e.g. `NVIDIA`, `Intel`, `Portable` |

The constants `G_SLF_SCALED` and `EPS_SQR` at the top of `harness.py` must match
`initialstate.cpp` and `CLModel::CLModel`.
The harness warns if any final position or velocity is not finite (NaN or inf).
