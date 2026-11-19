// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Kepler.cs" company="Michael William Simmons">
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
    public class Kepler
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

            // m is now in (-2 pi, 2 pi). Bring a negative m into [0, 2 pi) too: the code below
            // only handles 0..2 pi, and used to return E = 0 for every negative mean anomaly.
            if (m < 0)
            {
                m += 2 * Pi;
            }

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
        ///     Solves Kepler's equation E - e sin(E) = M for the eccentric anomaly E with Newton's method.
        ///     Newton iteration is limited to <see cref="MaxNewtonIterations" /> steps; if it has not
        ///     converged by then (which can happen for e close to 1) the answer comes from
        ///     <see cref="Sinnott" />, which always converges.
        /// </summary>
        /// <param name="m0">The mean anomaly M in radians (any value).</param>
        /// <param name="ec">The eccentricity, 0 &lt;= e &lt; 1.</param>
        /// <returns>
        ///     The eccentric anomaly E in radians, in the same revolution as <paramref name="m0" />
        ///     (E - m0 is between -pi and pi).
        /// </returns>
        public double Solve(double m0, double ec)
        {
            if (!(ec >= 0.0 && ec < 1.0))
            {
                throw new ArgumentOutOfRangeException(nameof(ec), ec, "Kepler's equation for elliptic orbits needs 0 <= e < 1");
            }

            // Reduce M to [-pi, pi]; the whole revolutions are added back at the end
            var revolutions = Math.Round(m0 / (2 * Math.PI)) * 2 * Math.PI;
            var m = m0 - revolutions;

            // Danby's starting value, which keeps Newton's method well behaved for high e
            var eca = m + (Math.Sign(m) * 0.85 * ec);
            for (var iteration = 0; iteration < MaxNewtonIterations; iteration++)
            {
                var delta = (eca - (ec * Math.Sin(eca)) - m) / (1 - (ec * Math.Cos(eca)));
                eca -= delta;
                if (double.IsNaN(eca) || double.IsInfinity(eca))
                {
                    break;
                }

                if (Math.Abs(delta) <= NewtonTolerance)
                {
                    return eca + revolutions;
                }
            }

            // Newton did not converge: use the bisection method, which always does
            return this.Sinnott(m, ec) + revolutions;
        }

        /// <summary>
        ///     The maximum number of Newton iterations in <see cref="Solve" />.
        /// </summary>
        public const int MaxNewtonIterations = 50;

        /// <summary>
        ///     The Newton step size (radians) at which <see cref="Solve" /> has converged.
        /// </summary>
        public const double NewtonTolerance = 1e-14;
    }
}