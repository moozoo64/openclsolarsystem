"""Fetches the JPL Horizons reference vectors used by `checks.py --only ephemeris`.

Gets heliocentric state vectors (centre 500@10 = the Sun's centre, ICRF equatorial frame, km and
km/s, TDB), the same settings OrbToSlf uses to build SLF files, at the mpcsmall.slf epoch and
1 and 10 years later. Saves them to horizons_reference.json next to this script, which is
committed so the check runs without network access.

usage: python fetch_horizons.py [--epoch 2461200.5] [--days 0,365,3650]
"""
import argparse
import datetime
import json
import os
import re
import urllib.parse
import urllib.request

API = "https://ssd.jpl.nasa.gov/api/horizons.api"
HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "horizons_reference.json")

# Horizons target ids. OrbToSlf builds Mercury, Venus and Earth from their centres (199-399), the
# Moon from 301, and Mars to Neptune from their system barycentres (4-8). Pluto comes from its MPC
# orbital elements; checks.py starts it from the Pluto system barycentre (9). The planet centres
# (499-999) are fetched too for comparison.
TARGETS = {
    "Mercury": "199", "Venus": "299", "Earth": "399", "Moon": "301", "Mars": "499",
    "Jupiter": "599", "Saturn": "699", "Uranus": "799", "Neptune": "899", "Pluto": "999",
    "Mars barycenter": "4", "Jupiter barycenter": "5", "Saturn barycenter": "6",
    "Uranus barycenter": "7", "Neptune barycenter": "8", "Pluto barycenter": "9",
}


def fetch(target, jds):
    params = {
        "format": "json",
        "COMMAND": f"'{target}'",
        "OBJ_DATA": "'NO'",
        "MAKE_EPHEM": "'YES'",
        "EPHEM_TYPE": "'VECTORS'",
        "CENTER": "'500@10'",
        "REF_PLANE": "'FRAME'",
        "REF_SYSTEM": "'ICRF'",
        "VEC_TABLE": "'2'",
        "VEC_CORR": "'NONE'",
        "OUT_UNITS": "'KM-S'",
        "CSV_FORMAT": "'YES'",
        "VEC_LABELS": "'NO'",
        "TIME_TYPE": "'TDB'",
        "TLIST_TYPE": "'JD'",
        "TLIST": " ".join(f"'{jd}'" for jd in jds),
    }
    url = API + "?" + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=60) as response:
        reply = json.load(response)
    text = reply.get("result", "")
    if "$$SOE" not in text:
        raise SystemExit(f"Horizons returned no vectors for {target}:\n{text[:2000]}")

    rows = []
    for line in text.split("$$SOE")[1].split("$$EOE")[0].strip().splitlines():
        fields = [f.strip() for f in line.split(",")]
        # JDTDB, Calendar Date, X, Y, Z, VX, VY, VZ
        rows.append({"jd_tdb": float(fields[0]), "position_km": [float(v) for v in fields[2:5]],
                     "velocity_km_s": [float(v) for v in fields[5:8]]})
    ephemeris = re.search(r"Target body name:.*?\{source:\s*([^}]+)\}", text)
    return rows, (ephemeris.group(1).strip() if ephemeris else "unknown"), reply.get("signature", {})


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--epoch", type=float, default=2461200.5, help="JD (TDB) of the SLF file")
    ap.add_argument("--days", default="0,365,3650", help="days after the epoch")
    a = ap.parse_args()
    days = [float(d) for d in a.days.split(",")]
    jds = [a.epoch + d for d in days]

    bodies = {}
    signature = {}
    for name, target in TARGETS.items():
        rows, source, signature = fetch(target, jds)
        bodies[name] = {"horizons_id": target, "source": source, "states": rows}
        print(f"{name:20s} {target:>4s}  {source}")

    reference = {
        "description": "JPL Horizons heliocentric state vectors (centre 500@10, ICRF equatorial frame, "
                       "km, km/s, TDB) for tools/kernel-harness/checks.py --only ephemeris",
        "fetched_utc": datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "horizons_signature": signature,
        "epoch_jd_tdb": a.epoch,
        "days": days,
        "bodies": bodies,
    }
    with open(OUT, "w", encoding="utf-8") as f:
        json.dump(reference, f, indent=1)
        f.write("\n")
    print(f"saved {OUT}")


if __name__ == "__main__":
    main()
