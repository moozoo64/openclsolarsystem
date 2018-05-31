// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Kepler.cs" company="Michael William Simmons">
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
//   Routines to solve Kepler's equation
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace OrbToSlf
{
    using System;

    /// <summary>
    ///     Routines to solve Kepler's equation
    /// </summary>
    internal class Kepler
    {
        /// <summary>
        ///     Using sinnott's method.
        /// </summary>
        /// <param name="m">The m.</param>
        /// <param name="e">The e.</param>
        /// <returns>
        ///     The <see cref="double" />.
        /// </returns>
        public double Sinnott(double m, double e)
        {
            const double Pi = Math.PI;
            var sign = m > 0 ? 1.0 : -1.0;
            m = Math.Abs(m) / (2 * Pi);
            m = (m - Math.Floor(m)) * 2 * Pi * sign;
            sign = 1.0;
            if (m > Pi)
            {
                sign = -1;
                m = 2 * Pi - m;
            }

            var e0 = Pi / 2;
            var d = Pi / 4;
            for (var j = 0; j < 64; j++)
            {
                var m1 = e0 - e * Math.Sin(e0);

                e0 = m > m1 ? e0 + d : e0 - d;
                d = d / 2;
            }

            return e0 * sign;
        }

        /// <summary>
        ///     The solve.
        /// </summary>
        /// <param name="m0">The m 0.</param>
        /// <param name="ec">The ec.</param>
        /// <returns>
        ///     The <see cref="double" />.
        /// </returns>
        public double Solve(double m0, double ec)
        {
            var eca = m0 + ec / 2;
            var diff = 10000.0;
            var eps = 0.00000001;
            double e1 = 0;

            while (diff > eps)
            {
                e1 = eca - (eca - ec * Math.Sin(eca) - m0) / (1 - ec * Math.Cos(eca));
                diff = Math.Abs(e1 - eca);
                eca = e1;
            }

            return eca;
        }
    }
}