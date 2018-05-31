// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Planets.cs" company="Michael William Simmons">
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
//   Computes the positions of the planets using the method detailed in
//   "Keplerian Elements for Approximate Positions of the Major Planets"
//   by EM Standish, Solar System Dynamics Group" JPL/Caltech
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace OrbToSlf
{
    using System;

    /// <summary>
    ///     Computes the positions of the planets using the method detailed in
    ///     "Keplerian Elements for Approximate Positions of the Major Planets"
    ///     by EM Standish, Solar System Dynamics Group" JPL/Caltech
    /// </summary>
    internal class Planets
    {
        /// <summary>
        ///     The au in km.
        /// </summary>
        private const double AuInKm = 149597870691;

        /// <summary>
        ///     Computes the approximate position of earth.
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     1.00000261      0.01671123     -0.00001531      100.46457166    102.93768193      0.0
        ///     0.00000562     -0.00004392     -0.01294668    35999.37244981      0.32327364      0.0
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Earth(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 1.00000261 + 0.00000562 * tc;
            var orbitalEccentricity = 0.01671123 - 0.00004392 * tc;
            var inclinationToTheEcliptic = -0.00001531 - 0.01294668 * tc;

            var meanLongitude = 100.46457166 + 35999.37244981 * tc;
            var longitudeOfPerihelion = 102.93768193 + 0.32327364 * tc;
            var longitudeOfTheAscendingNode = 0.0 - 0.0 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     Computes the approximate position of Jupiter using
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     5.20288700      0.04838624      1.30439695       34.39644051     14.72847983    100.47390909
        ///     -0.00011607     -0.00013253     -0.00183714     3034.74612775      0.21252668      0.20469106
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Jupiter(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 5.20288700 - 0.00011607 * tc;
            var orbitalEccentricity = 0.04838624 - 0.00013253 * tc;
            var inclinationToTheEcliptic = 1.30439695 - 0.00183714 * tc;

            var meanLongitude = 34.39644051 + 3034.74612775 * tc;
            var longitudeOfPerihelion = 14.72847983 + 0.21252668 * tc;
            var longitudeOfTheAscendingNode = 100.47390909 - 0.20469106 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     Computes the approximate positions of Mars using
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     1.52371034      0.09339410      1.84969142       -4.55343205    -23.94362959     49.55953891
        ///     0.00001847      0.00007882     -0.00813131    19140.30268499      0.44441088     -0.29257343
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Mars(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 1.52371034 + 0.00001847 * tc;
            var orbitalEccentricity = 0.09339410 - 0.00007882 * tc;
            var inclinationToTheEcliptic = 1.84969142 - 0.00813131 * tc;

            var meanLongitude = -4.55343205 + 19140.30268499 * tc;
            var longitudeOfPerihelion = -23.94362959 + 0.44441088 * tc;
            var longitudeOfTheAscendingNode = 49.55953891 - 0.29257343 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     Computes the approximate position of Mercury using.
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     0.38709927      0.20563593      7.00497902      252.25032350     77.45779628     48.33076593
        ///     0.00000037      0.00001906     -0.00594749   149472.67411175      0.16047689     -0.12534081
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Mercury(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 0.38709927 + 0.00000037 * tc;
            var orbitalEccentricity = 0.20563593 + 0.00001906 * tc;
            var inclinationToTheEcliptic = 7.00497902 - 0.00594749 * tc;

            var meanLongitude = 252.25032350 + 149472.67411175 * tc;
            var longitudeOfPerihelion = 77.45779628 + 0.16047689 * tc;
            var longitudeOfTheAscendingNode = 48.33076593 - 0.12534081 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));

            return state;
        }

        /// <summary>
        ///     Computes the approximate position of Neptune using
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     30.06992276      0.00859048      1.77004347      -55.12002969     44.96476227    131.78422574
        ///     0.00026291      0.00005105      0.00035372      218.45945325     -0.32241464     -0.00508664
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Neptune(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 30.06992276 + 0.00026291 * tc;
            var orbitalEccentricity = 0.00859048 + 0.00005105 * tc;
            var inclinationToTheEcliptic = 1.77004347 + 0.00035372 * tc;

            var meanLongitude = 304.87997031 + 218.45945325 * tc; // -55.12002969 + 218.45945325 * tc;
            var longitudeOfPerihelion = 44.96476227 - 0.32241464 * tc;
            var longitudeOfTheAscendingNode = 131.78422574 - 0.00508664 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     Computes the approximate position of Pluto using
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     39.48211675      0.24882730     17.14001206      238.92903833    224.06891629    110.30393684
        ///     -0.00031596      0.00005170      0.00004818      145.20780515     -0.04062942     -0.01183482
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Pluto(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 39.48211675 - 0.00031596 * tc;
            var orbitalEccentricity = 0.24882730 + 0.00005170 * tc;
            var inclinationToTheEcliptic = 17.14001206 + 0.00004818 * tc;

            var meanLongitude = 238.92903833 + 145.20780515 * tc;
            var longitudeOfPerihelion = 224.06891629 - 0.04062942 * tc;
            var longitudeOfTheAscendingNode = 110.30393684 - 0.01183482 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     Computes the approximate position of Saturn using
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     9.53667594      0.05386179      2.48599187       49.95424423     92.59887831    113.66242448
        ///     -0.00125060     -0.00050991      0.00193609     1222.49362201     -0.41897216     -0.28867794
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Saturn(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 9.53667594 - 0.00125060 * tc;
            var orbitalEccentricity = 0.05386179 - 0.00050991 * tc;
            var inclinationToTheEcliptic = 2.48599187 + 0.00193609 * tc;

            var meanLongitude = 49.95424423 + 1222.49362201 * tc;
            var longitudeOfPerihelion = 92.59887831 - 0.41897216 * tc;
            var longitudeOfTheAscendingNode = 113.66242448 - 0.28867794 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     Computes the approximate position of Uranus using
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     19.18916464      0.04725744      0.77263783      313.23810451    170.95427630     74.01692503
        ///     -0.00196176     -0.00004397     -0.00242939      428.48202785      0.40805281      0.042405
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Uranus(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 19.18916464 - 0.00196176 * tc;
            var orbitalEccentricity = 0.04725744 - 0.00004397 * tc;
            var inclinationToTheEcliptic = 0.77263783 - 0.00242939 * tc;

            var meanLongitude = 313.23810451 + 428.48202785 * tc;
            var longitudeOfPerihelion = 170.95427630 + 0.40805281 * tc;
            var longitudeOfTheAscendingNode = 74.01692503 + 0.04240589 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     Computes the approximate position of Venus using
        ///     a              e               I                L            long.peri.      long.node.
        ///     AU, AU/Cy     rad, rad/Cy     deg, deg/Cy      deg, deg/Cy      deg, deg/Cy     deg, deg/Cy
        ///     0.72333566      0.00677672      3.39467605      181.97909950    131.60246718     76.67984255
        ///     0.00000390     -0.00004107     -0.00078890    58517.81538729      0.00268329     -0.27769418
        /// </summary>
        /// <param name="t">
        ///     The t.
        /// </param>
        /// <param name="semiMajorAxis">
        ///     The semi major axis.
        /// </param>
        /// <returns>
        ///     The <see cref="Double4[]" />.
        /// </returns>
        public Double4[] Venus(double t, out double semiMajorAxis)
        {
            var tc = (t - 2451545.0) / 36525;

            semiMajorAxis = 0.72333566 + 0.00000390 * tc;
            var orbitalEccentricity = 0.00677672 - 0.00004107 * tc;
            var inclinationToTheEcliptic = 3.39467605 - 0.00078890 * tc;

            var meanLongitude = 181.97909950 + 58517.81538729 * tc;
            var longitudeOfPerihelion = 131.60246718 + 0.00268329 * tc;
            var longitudeOfTheAscendingNode = 76.67984255 - 0.27769418 * tc;

            var meanAnomaly = this.Modulus(meanLongitude - longitudeOfPerihelion);
            var argumentOfPerihelion = longitudeOfPerihelion - longitudeOfTheAscendingNode;

            var massPrimary = 1.98894729428839E+30;
            var ellipticMotion = new EllipticMotion();
            var state = ellipticMotion.ComputeStateVectors(
                massPrimary,
                this.DegreeToRadian(meanAnomaly),
                semiMajorAxis * AuInKm,
                orbitalEccentricity,
                this.DegreeToRadian(inclinationToTheEcliptic),
                this.DegreeToRadian(argumentOfPerihelion),
                this.DegreeToRadian(longitudeOfTheAscendingNode));
            return state;
        }

        /// <summary>
        ///     The degree to radian.
        /// </summary>
        /// <param name="angle">
        ///     The angle.
        /// </param>
        /// <returns>
        ///     The <see cref="double" />.
        /// </returns>
        private double DegreeToRadian(double angle)
        {
            return Math.PI * angle / 180.0;
        }

        /// <summary>
        ///     The modulus.
        /// </summary>
        /// <param name="angle">
        ///     The angle.
        /// </param>
        /// <returns>
        ///     The <see cref="double" />.
        /// </returns>
        private double Modulus(double angle)
        {
            var sign = angle > 0 ? 1.0 : -1.0;
            angle = Math.Abs(angle) / 360;
            angle = (angle - Math.Floor(angle)) * 360 * sign;
            return angle;
        }
    }
}