// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Constants.cs" company="Michael William Simmons">
//   (c) 2013-2025 Michael William Simmons
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//   
//      http://www.apache.org/licenses/LICENSE-2.0
//   
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
// </copyright>
// --------------------------------------------------------------------------------------------------------------------

namespace OrbToSlf
{
    static class Constants
    {
    /// <summary>
    ///     One AU in metres (the DE405 value; the IAU 2012 definition is 149597870700 m).
    /// </summary>
    public const double AuInMetres = 149597870691;

    /// <summary>
    ///     The meters in a Giga Meter.
    /// </summary>
    public const double Gm = 1000000000;

    /// <summary>
    ///     The gravitational constant (m^3 kg^-1 s^-2) that the SLF masses are derived with
    ///     (the Solex convention, GM_sun / M_sun). It is deliberately not the CODATA value.
    ///     OpenCLSolarSystem multiplies the SLF masses by the same G (G_SLF_SCALED in
    ///     src/OpenCLSolarSystem/initialstate.cpp, which is this value x 1e3), so the two
    ///     programs must be changed together. tools/kernel-harness/checks.py --only gm checks this.
    /// </summary>
    public const double GravitationalConstant = 6.6725985e-11;

    /// <summary>
    ///     The speed of light.
    /// </summary>
    public const double SpeedOfLight = 299792458;

    /// <summary>
    ///     The Sun's GM in m^3/s^2 (DE440, the same value as the Sun row of Info.csv).
    ///     EllipticMotion turns MPC/astorb orbital elements into state vectors with it, so it must
    ///     be the GM the simulation uses: a 1.5e-5 error here (the old SunMass) made every asteroid's
    ///     velocity about 8e-6 too fast.
    /// </summary>
    public const double SunGM = 1.32712440041279E+20;

    /// <summary>
    ///     The Sun's mass in SLF units: SunGM / GravitationalConstant, so that G x SunMass = SunGM.
    /// </summary>
    public const double SunMass = SunGM / GravitationalConstant;
  }
}
