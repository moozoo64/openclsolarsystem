// --------------------------------------------------------------------------------------------------------------------
// <copyright file="BodyInfo.cs" company="Michael William Simmons">
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
    public class BodyInfo
    {
        /// <summary>
        /// The absolute magnitude of the body. 
        /// </summary>
        public double AbsoluteMagnitude { get; set; }

        /// <summary>
        /// The bodies argument of perihelion.
        /// </summary>
        public double ArgumentOfPerihelion { get; set; }

        /// <summary>
        /// The density of the body
        /// </summary>
        public double Density { get; set; }

        /// <summary>
        /// The epoch of the bodies orbital elements.
        /// </summary>
        public double Epoch { get; set; }

        /// <summary>
        /// The bodies inclination to the ecliptic.
        /// </summary>
        public double InclinationToTheEcliptic { get; set; }

        /// <summary>
        /// The bodies longitude of the ascending node.
        /// </summary>
        public double LongitudeOfTheAscendingNode { get; set; }

        /// <summary>
        /// The bodies mass.
        /// </summary>
        public double Mass { get; set; }

        /// <summary>
        /// The bodies mean anomaly.
        /// </summary>
        public double MeanAnomaly { get; set; }

        /// <summary>
        /// The name of the body.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// The bodies orbital eccentricity.
        /// </summary>
        public double OrbitalEccentricity { get; set; }

        /// <summary>
        /// The bodies radius.
        /// </summary>
        public double Radius { get; set; }

        /// <summary>
        /// Was the body read in from the details file
        /// </summary>
        public bool ReadIn { get; set; }

        /// <summary>
        /// The bodies relativistic parameter.
        /// </summary>
        public double RelativisticParam { get; set; }

        /// <summary>
        /// The bodies semi major axis.
        /// </summary>
        public double SemiMajorAxis { get; set; }

        /// <summary>
        /// The bodies state vectors.
        /// </summary>
        public Double4[] StateVectors { get; set; }

        /// <summary>
        /// The bodies type.
        /// </summary>
        public string Type { get; set; }

        /// <summary>
        /// The uncertainty parameter for the bodies orbital parameters
        /// </summary>
        public int UncertaintyParameter { get; set; }

        /// <summary>
        /// The bodies vx offset (used to create another copy of the solar system)
        /// </summary>
        public double VxOffset { get; set; }

        /// <summary>
        /// The bodies vx offset (used to create another copy of the solar system)
        /// </summary>
        public double VyOffset { get; set; }

        /// <summary>
        /// The bodies vz offset (used to create another copy of the solar system)
        /// </summary>
        public double VzOffset { get; set; }

        /// <summary>
        /// The bodies x offset (used to create another copy of the solar system)
        /// </summary>
        public double XOffset { get; set; }

        /// <summary>
        /// The bodies y offset (used to create another copy of the solar system)
        /// </summary>
        public double YOffset { get; set; }

        /// <summary>
        /// The bodies z offset (used to create another copy of the solar system)
        /// </summary>
        public double ZOffset { get; set; }
        }
}