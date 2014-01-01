// --------------------------------------------------------------------------------------------------------------------
// <copyright file="EllipticMotion.cs" company="Michael William Simmons">
//   (c) 2013 Michael William Simmons
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
// <summary>
//   The elliptic motion.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace OrbToSlf
{
    using System;

    /// <summary>
    ///     The elliptic motion.
    /// </summary>
    public class EllipticMotion
    {
        #region Constants

        /// <summary>
        ///     The gravitational constant.
        /// </summary>
        private const double GravitationalConstant = 6.6725985e-11;

        #endregion

        #region Public Methods and Operators

        /// <summary>
        /// The compute state vectors.
        /// </summary>
        /// <param name="mass">The mass.</param>
        /// <param name="meanAnomaly">The mean anomaly.</param>
        /// <param name="semiMajorAxis">The semi major axis.</param>
        /// <param name="eccentricity">The eccentricity.</param>
        /// <param name="inclination">The inclination.</param>
        /// <param name="argumentOfPerihelion">The argument of perihelion.</param>
        /// <param name="longitudeOfAscendingNode">The longitude of ascending node.</param>
        /// <returns>
        /// The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] ComputeStateVectors(
            double mass, 
            double meanAnomaly, 
            double semiMajorAxis, 
            double eccentricity, 
            double inclination, 
            double argumentOfPerihelion, 
            double longitudeOfAscendingNode)
        {
            var u = GravitationalConstant * mass;
            var kepler = new Kepler();
            var eccentricAnomaly = kepler.Sinnott(meanAnomaly, eccentricity);
            var radius = semiMajorAxis * (1 - (eccentricity * Math.Cos(eccentricAnomaly)));

            // with x in the direction of perihelion, z perpendicular to the plane of the orbit
            var ox = semiMajorAxis * (Math.Cos(eccentricAnomaly) - eccentricity);
            var oy = semiMajorAxis * Math.Sqrt(1 - (eccentricity * eccentricity)) * Math.Sin(eccentricAnomaly);
            double oz = 0.0f;

            var p = Math.Sqrt(u * semiMajorAxis) / radius;
            var ovx = -p * Math.Sin(eccentricAnomaly);
            var ovy = p * Math.Sqrt(1 - (eccentricity * eccentricity)) * Math.Cos(eccentricAnomaly);

            // rotate to ecliptic co-ordinates
            var cosArg = Math.Cos(argumentOfPerihelion); // cos w
            var sinArg = Math.Sin(argumentOfPerihelion);
            var cosLong = Math.Cos(longitudeOfAscendingNode); // cos U
            var sinLong = Math.Sin(longitudeOfAscendingNode);
            var cosInc = Math.Cos(inclination);
            var sinInc = Math.Sin(inclination);

            var x = ox * (cosArg * cosLong - sinArg * cosInc * sinLong)
                    - oy * (sinArg * cosLong + cosArg * cosInc * sinLong);
            var y = ox * (cosArg * sinLong + sinArg * cosInc * cosLong)
                    + oy * (cosArg * cosInc * cosLong - sinArg * sinLong);
            var z = ox * (sinArg * sinInc) + oy * (cosArg * sinInc);

            var vx = ovx * (cosArg * cosLong - sinArg * cosInc * sinLong)
                     - ovy * (sinArg * cosLong + cosArg * cosInc * sinLong);
            var vy = ovx * (cosArg * sinLong + sinArg * cosInc * cosLong)
                     + ovy * (cosArg * cosInc * cosLong - sinArg * sinLong);
            var vz = ovx * (sinArg * sinInc) + ovy * (cosArg * sinInc);

            // rotate to Equatorial Co-ordinates
            var ObliquityOfTheEcliptic = DegreeToRadian(23.43928f); // 23.4375679356f
            var CosObliquityOfTheEcliptic = Math.Cos(ObliquityOfTheEcliptic);
            var SinObliquityOfTheEcliptic = Math.Sin(ObliquityOfTheEcliptic);
            var yE = y * Math.Cos(ObliquityOfTheEcliptic) - z * Math.Sin(ObliquityOfTheEcliptic);
            var zE = y * Math.Sin(ObliquityOfTheEcliptic) + z * Math.Cos(ObliquityOfTheEcliptic);

            var vyE = vy * Math.Cos(ObliquityOfTheEcliptic) - vz * Math.Sin(ObliquityOfTheEcliptic);
            var vzE = vy * Math.Sin(ObliquityOfTheEcliptic) + vz * Math.Cos(ObliquityOfTheEcliptic);

            var position = new Double4();
            position.X = x;
            position.Y = yE;
            position.Z = zE;
            position.W = 0.0f;
            var velocity = new Double4();
            velocity.X = vx;
            velocity.Y = vyE;
            velocity.Z = vzE;
            velocity.W = 0.0f;

            return new[] { position, velocity };
        }

        #endregion

        #region Methods

        /// <summary>
        /// The degree to radian.
        /// </summary>
        /// <param name="angle">
        /// The angle.
        /// </param>
        /// <returns>
        /// The <see cref="double"/>.
        /// </returns>
        private static double DegreeToRadian(double angle)
        {
            return Math.PI * angle / 180.0;
        }

        #endregion
    }
}