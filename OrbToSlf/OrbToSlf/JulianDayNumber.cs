// --------------------------------------------------------------------------------------------------------------------
// <copyright file="JulianDayNumber.cs" company="Michael William Simmons">
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
//   Computes the julian day number from a date
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace OrbToSlf
{
    using System;

    /// <summary>
    ///     Computes the julian day number from a date
    /// </summary>
    public static class JulianDayNumber
    {
        /// <summary>
        ///     The from date.
        /// </summary>
        /// <param name="yr">The yr.</param>
        /// <param name="mn">The mn.</param>
        /// <param name="dy">The dy.</param>
        /// <returns>
        ///     The <see cref="double" />.
        /// </returns>
        public static double FromDate(int yr, int mn, int dy)
        {
            mn = Math.Max(1, mn);
            dy = Math.Max(1, dy);

            if (mn < 3)
            {
                mn = mn + 12;
                yr = yr - 1;
            }

            var b = 0;
            var isJulian = yr < 1582 || yr == 1582 && mn < 10 || yr == 1582 && mn == 10 && dy < 15;
            if (!isJulian)
            {
                var a = yr / 100;
                b = 2 - a + a / 4;
            }

            var dj1 = Math.Floor(365.25 * (yr + 4716)) + Math.Floor(30.6001f * (mn + 1)) + b - 1524.5 + dy;
            return dj1;
        }
    }
}