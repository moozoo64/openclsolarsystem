"""Prints the position difference (km) of selected bodies between harness.py outputs.

usage: python compare.py reference.npy run1.npy [run2.npy ...]
The runs must cover the same simulated time, e.g. --dt 900 --steps 35072 vs --dt 14400 --steps 2192.
Body indices are those of mpcsmall.slf.
"""
import os
import sys

import numpy as np

BODIES = {1: "Mercury", 3: "Earth", 4: "Moon", 5: "Mars", 6: "Jupiter"}
EARTH, MOON = 3, 4
GM_TO_KM = 1e6

if len(sys.argv) < 3:
    raise SystemExit(__doc__)

ref = np.load(sys.argv[1])
runs = [(os.path.basename(f), np.load(f)) for f in sys.argv[2:]]

print(f"{'body':9s}" + "".join(f"{name:>22s}" for name, _ in runs))
for i, body in BODIES.items():
    errs = [np.linalg.norm(r[0, i, :3] - ref[0, i, :3]) * GM_TO_KM for _, r in runs]
    print(f"{body:9s}" + "".join(f"{e:19.6f} km" for e in errs))

geo_ref = ref[0, MOON, :3] - ref[0, EARTH, :3]
errs = [np.linalg.norm((r[0, MOON, :3] - r[0, EARTH, :3]) - geo_ref) * GM_TO_KM for _, r in runs]
print(f"{'Moon-geo':9s}" + "".join(f"{e:19.6f} km" for e in errs))
