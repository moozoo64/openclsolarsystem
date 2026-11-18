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

## Regression checks

`checks.py` runs pass/fail checks and exits with status 1 if any fail, so it can run in CI.

```sh
python tools/kernel-harness/checks.py                 # all checks, about 40 s on a GPU
python tools/kernel-harness/checks.py --only gm       # constants only, no OpenCL needed
python tools/kernel-harness/checks.py --years 100     # the review's full-length Mercury run
```

| Check | What it verifies | Catches |
| --- | --- | --- |
| `gm` | `G_SLF_SCALED` in `initialstate.cpp` matches the harness and OrbToSlf's `Constants.GravitationalConstant` (× 1e3, to 1e-8), and is what `ImportSLF` multiplies by; the Sun's `pos.w` equals GM☉ = 1.32712440018e11 km³/s² to 1e-9, and Earth, Moon and Jupiter match their standard GM to 1e-7; `relativisticC1` in `adamsfma.cl` equals 6 GM☉/c² in Gm | C1 (wrong G), C2 (`relativisticC1` in the wrong unit) |
| `mercury` | Sun + Mercury with the `relativistic` and `newtonian` kernels. A straight-line fit to the direction of Mercury's eccentricity vector gives each run's perihelion precession. The difference must be 43 ± 1″/century (GR predicts 42.98″), and the Newtonian run must not precess (within ±0.5″) | C2, and anything else that breaks the relativistic term or the integrator |
| `ephemeris` | The 16 gravitating bodies of `mpcsmall.slf` for 10 years (4 h steps, AB11/AM10, `relativistic`) against JPL Horizons (DE441). First confirms the SLF epoch states match Horizons (Mercury to Uranus, within 1 km and 1 mm/s). Then the heliocentric positions after 1 and 10 years must be within `EPHEMERIS_LIMITS_KM`, about 2–3 × today's errors | Anything that makes the simulation less accurate: constants, force kernels, startup, integrator |

Current results (RTX 4070 Ti SUPER): GR advance 42.980″/century over 20 years and 42.981″ over 100;
Newtonian drift 0.000″. With the C1 and C2 bugs put back in, the `gm` checks fail and the advance
comes out at 43,031″/century.

### Comparison with JPL Horizons

Heliocentric position error against Horizons, running `mpcsmall.slf` (epoch JD 2461200.5) as the
app loads it (RTX 4070 Ti SUPER):

| Body | after 1 year | after 10 years |
| --- | --- | --- |
| Mercury | 12.8 km | 147 km |
| Venus | 0.6 km | 5.7 km |
| Earth | 2.0 km | 16.1 km |
| Moon (relative to Earth) | 21.5 km | 257 km |
| Mars | 4.5 km | 43 km |
| Jupiter | 0.2 km | 10.7 km |
| Saturn | 0.0 km | 0.7 km |
| Uranus | 0.1 km | 3.3 km |
| Neptune | 0.1 km | 7.3 km |
| Pluto | 0.1 km | 13.3 km |

With the C2 bug put back in, the Earth is about 109,000 km off after 1 year.

The remaining errors are what the model leaves out rather than integration error: DE441 has
343 asteroids, the Earth's and Moon's shapes and tides, and full relativity for every body, where
the simulation has 16 point masses and the Sun-only relativistic term.

The SLF states for Mercury to Neptune match Horizons exactly at the epoch, with the system
**barycentres** for Mars to Neptune (OrbToSlf's `JPLHorizons.cs` fetches Horizons ids 4 to 8).
The simulation moves each of those as one point mass with the system's mass, which follows the
barycentre. The previous `mpcsmall.slf` (epoch 2460800.5) had the planet centres, which move
around the barycentre with the moons (Jupiter 0.6 m/s, Saturn 1.3 m/s, Neptune 0.9 m/s); that put
Jupiter 17,000 km and Saturn 42,000 km off after 1 year.

Pluto is the exception. OrbToSlf builds it from its MPC orbital elements like the asteroids, which
puts it about 2,300 km from Horizons, so the check starts Pluto from its Horizons state.

The reference vectors are in `horizons_reference.json`, so the check needs no network access.
When `mpcsmall.slf` is regenerated with a new epoch, refresh them with
`python fetch_horizons.py --epoch <the SLF's first line>`; the check fails if the epochs differ.

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
| --- | --- | --- |
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

## Example: relativisticLocal (H3)

`relativisticLocal` must give exactly the same result as `relativistic`. Test it with more
gravitating bodies than the work-group size, and a last block that is only partly full:

```sh
python harness.py --steps 365 --grav 40 --bodies 128 --group 16 --acc relativistic      --out rel.npy
python harness.py --steps 365 --grav 40 --bodies 128 --group 16 --acc relativisticLocal --out local.npy
python compare.py rel.npy local.npy
```

After the H3 fix the two are bit-identical. Before it, the Moon was off by about 600,000 km after 61 days.

## Options

`python harness.py --help` lists them all. The main ones:

| Option | Default | |
| --- | --- | --- |
| `--kernel` | `src/OpenCLSolarSystem/adamsfma.cl` | kernel source |
| `--slf` | `src/OrbToSlf/OrbToSlfConsole/mpcsmall.slf` | initial state |
| `--dt` | 14400 | step in seconds |
| `--steps` / `--days` | 365.25 days | run length |
| `--acc` | `relativistic` | acceleration kernel: `newtonian`, `relativistic` or `relativisticLocal` |
| `--eps` | 0 | softening length squared in Gm² (`1e-6` = before M1) |
| `--ab`, `--am` | `adamsBashforth11`, `adamsMoulton10` | predictor and corrector kernels |
| `--startup` | `rk4` | `adams` for the pre-H1 host flow (needs an old kernel file) |
| `--substeps` | 10 | RK4 sub-steps per startup step |
| `--bodies`, `--grav` | 64, 16 | bodies loaded and how many of them gravitate (`--grav` is capped at `--bodies`) |
| `--group` | 64 | work-group size; any `--bodies` works, the launch is rounded up as in the app |
| `--platform` | first with fp64 | substring of the OpenCL platform name, e.g. `NVIDIA`, `Intel`, `Portable` |

The constants `G_SLF_SCALED` and `EPS_SQR` at the top of `harness.py` must match
`initialstate.cpp` and `CLModel::CLModel`.
The harness warns if any final position or velocity is not finite (NaN or inf).
