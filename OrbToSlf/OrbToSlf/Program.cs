// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Program.cs" company="Michael William Simmons">
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
// --------------------------------------------------------------------------------------------------------------------

namespace OrbToSlf
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Net.Http;

#if NET48
    using System.Threading.Tasks;
#endif

    using Microsoft.Extensions.Configuration;

    /// <summary>
    ///     A program that reads asteroid data and creates a .SLF file of the solar system
    ///     It computes the positions of the planets using the method detailed in
    ///     "Keplerian Elements for Approximate Positions of the Major Planets"
    ///     by EM Standish, Solar System Dynamics Group" JPL/Caltech
    ///     It can also add random Oort cloud objects
    /// </summary>
    public static class Program
    {
        /// <summary>
        ///     One AU in km.
        /// </summary>
        private const double AuInKm = 149597870691;

        /// <summary>
        ///     The meters in a Giga Meter.
        /// </summary>
        private const double Gm = 1000000000;

        /// <summary>
        ///     The gravitational constant.
        /// </summary>
        private const double GravitationalConstant = 6.6725985e-11;

        /// <summary>
        ///     The speed of light.
        /// </summary>
        private const double SpeedOfLight = 299792458;

        /// <summary>
        ///     The suns mass.
        /// </summary>
        private const double SunMass = 1.98894729428839E+30;

        /// <summary>
        ///     Adds duplicate star system
        /// </summary>
        private static bool addDuplicateStarSystem;

        /// <summary>
        ///     Flag to indicate that NEO and NEO! body types should be added first after body with masses.
        /// </summary>
        private static bool neoFirst;

        /// <summary>
        ///     The bodies vx offset (used to create another copy of the solar system)
        /// </summary>
        private static double vxOffset;

        /// <summary>
        ///     The bodies x offset (used to create another copy of the solar system)
        /// </summary>
        private static double xOffset;

        /// <summary>
        ///     The bodies y offset (used to create another copy of the solar system)
        /// </summary>
        private static double yOffset;

        /// <summary>
        ///     The converts and angle in degrees to radians.
        /// </summary>
        /// <param name="angle">
        ///     The angle in degrees
        /// </param>
        /// <returns>
        ///     The angle in Radians
        /// </returns>
        private static double DegreeToRadian(double angle)
        {
            return Math.PI * angle / 180.0;
        }

        /// <summary>
        ///     The fetch astrob body infos.
        /// </summary>
        /// <param name="astrorbUri">The astrorb uri.</param>
        /// <param name="detailsPathName">The details path name.</param>
        /// <returns>
        ///     The <see cref="Dictionary" />.
        /// </returns>
        /// <exception cref="Exception">If it fails to download the url content</exception>
        private static Dictionary<string, BodyInfo> FetchAstroOrBbodyInfos(Uri astrorbUri, string detailsPathName)
        {
            var bodyInfos = ReadDetails(detailsPathName);
            long lineCount = 0;

            var astrorbHttpClient = new HttpClient();
            var astrorbResponseTask = astrorbHttpClient.GetStreamAsync(astrorbUri);
            astrorbResponseTask.Wait();
#if NET48
            if (astrorbResponseTask.Status != TaskStatus.RanToCompletion)
            {
                throw new Exception("Failed to fetch data");
            }
#else
            if (!astrorbResponseTask.IsCompletedSuccessfully)
            {
                throw new Exception("Failed to fetch data");
            }
#endif
            using (var astrorbStream = astrorbResponseTask.Result)
            {
                if (astrorbStream == null)
                {
                    return bodyInfos;
                }

                // var file = new StreamReader(mpcorbPathName);
                using (var streamReader = new StreamReader(astrorbStream))
                {
                    string line;
                    while ((line = streamReader.ReadLine()) != null)
                    {
                        lineCount++;

                        if (lineCount % 100 == 0)
                        {
                            Console.WriteLine("{0}", lineCount);
                        }

                        var epochString = line.Substring(107 - 1, 8);
                        if (!int.TryParse(epochString.Substring(0, 4), out var epochYear))
                        {
                            throw new Exception(string.Format("Bad Year {0}", epochString));
                        }

                        if (!int.TryParse(epochString.Substring(4, 2), out var epochMonth))
                        {
                            throw new Exception(string.Format("Bad Month {0}", epochString));
                        }

                        if (!int.TryParse(epochString.Substring(6, 2), out var epochDay))
                        {
                            throw new Exception(string.Format("Bad Day {0}", epochString));
                        }

                        var epoch = JulianDayNumber.FromDate(epochYear, epochMonth, epochDay);
                        var absoluteMagnitudeString = line.Substring(43 - 1, 5);
                        if (!double.TryParse(absoluteMagnitudeString, out var absoluteMagnitude))
                        {
                            absoluteMagnitude = 20.0;
                        }

                        var meanAnomalyString = line.Substring(116 - 1, 10);
                        if (!double.TryParse(meanAnomalyString, out var meanAnomaly))
                        {
                            throw new Exception(string.Format("Bad meanAnomaly {0}", meanAnomalyString));
                        }

                        meanAnomaly = DegreeToRadian(meanAnomaly);

                        var argumentOfPerihelionString = line.Substring(127 - 1, 10);
                        if (!double.TryParse(argumentOfPerihelionString, out var argumentOfPerihelion))
                        {
                            throw new Exception(
                                string.Format("Bad argumentOfPerihelion {0}", argumentOfPerihelionString));
                        }

                        argumentOfPerihelion = DegreeToRadian(argumentOfPerihelion);

                        var longitudeOfTheAscendingNodeString = line.Substring(138 - 1, 10);
                        if (!double.TryParse(longitudeOfTheAscendingNodeString, out var longitudeOfTheAscendingNode))
                        {
                            throw new Exception(
                                string.Format(
                                    "Bad longitudeOfTheAscendingNode {0}",
                                    longitudeOfTheAscendingNodeString));
                        }

                        longitudeOfTheAscendingNode = DegreeToRadian(longitudeOfTheAscendingNode);

                        var inclinationToTheEclipticString = line.Substring(148 - 1, 10);
                        if (!double.TryParse(inclinationToTheEclipticString, out var inclinationToTheEcliptic))
                        {
                            throw new Exception(
                                string.Format("Bad inclinationToTheEcliptic {0}", inclinationToTheEclipticString));
                        }

                        inclinationToTheEcliptic = DegreeToRadian(inclinationToTheEcliptic);

                        var orbitalEccentricityString = line.Substring(159 - 1, 10);
                        if (!double.TryParse(orbitalEccentricityString, out var orbitalEccentricity))
                        {
                            throw new Exception(
                                string.Format("Bad orbitalEccentricity {0}", orbitalEccentricityString));
                        }

                        //// var meanDailyMotion = line.Substring(81 - 1, 91 - 81 + 1);
                        //// meanDailyMotionInRadiansPerDay = 0.0f; // double.Parse(meanDailyMotion);

                        var semiMajorAxisString = line.Substring(170 - 1, 12);
                        if (!double.TryParse(semiMajorAxisString, out var semiMajorAxis))
                        {
                            throw new Exception(string.Format("Bad semiMajorAxis {0}", semiMajorAxisString));
                        }

                        semiMajorAxis = semiMajorAxis * AuInKm;

                        var readableDesignation = line.Substring(8 - 1, 18).Trim().Replace(" ", string.Empty);

                        if (readableDesignation.ToUpperInvariant() == "NAME")
                        {
                            continue;
                        }

                        BodyInfo bodyInfo;
                        if (bodyInfos.ContainsKey(readableDesignation.ToUpperInvariant()))
                        {
                            bodyInfo = bodyInfos[readableDesignation.ToUpperInvariant()];
                            bodyInfo.MeanAnomaly = meanAnomaly;
                            bodyInfo.AbsoluteMagnitude = absoluteMagnitude;
                            bodyInfo.ArgumentOfPerihelion = argumentOfPerihelion;
                            bodyInfo.LongitudeOfTheAscendingNode = longitudeOfTheAscendingNode;
                            bodyInfo.InclinationToTheEcliptic = inclinationToTheEcliptic;
                            bodyInfo.OrbitalEccentricity = orbitalEccentricity;
                            bodyInfo.SemiMajorAxis = semiMajorAxis;
                            bodyInfo.Epoch = epoch;
                        }
                        else
                        {
                            bodyInfo = new BodyInfo
                                           {
                                               Name = readableDesignation,
                                               Radius = 0.1,
                                               Mass = 0.0,
                                               MeanAnomaly = meanAnomaly,
                                               AbsoluteMagnitude = absoluteMagnitude,
                                               ArgumentOfPerihelion = argumentOfPerihelion,
                                               LongitudeOfTheAscendingNode = longitudeOfTheAscendingNode,
                                               InclinationToTheEcliptic = inclinationToTheEcliptic,
                                               OrbitalEccentricity = orbitalEccentricity,
                                               SemiMajorAxis = semiMajorAxis,
                                               RelativisticParam = 0.0,
                                               Epoch = epoch
                                           };
                            bodyInfos.Add(readableDesignation.ToUpperInvariant(), bodyInfo);
                        }
                    }
                }
            }

            return bodyInfos;
        }

        /// <summary>
        ///     Fetches the MPC or bbody infos.
        /// </summary>
        /// <param name="mpcorbUrl">
        ///     The mpcorb URL.
        /// </param>
        /// <param name="detailsPathName">
        ///     Name of the details path.
        /// </param>
        /// <returns>
        ///     The <see cref="Dictionary" />.
        /// </returns>
        /// <exception cref="System.Exception"> Exception if it fails to download the url
        /// </exception>
        private static Dictionary<string, BodyInfo> FetchMpcOrBbodyInfos(Uri mpcorbUrl, string detailsPathName)
        {
            var bodyInfos = ReadDetails(detailsPathName);
            long lineCount = 0;

            var mpcorbHttpClient = new HttpClient();
            var mpcorbResponseTask = mpcorbHttpClient.GetStreamAsync(mpcorbUrl);
            mpcorbResponseTask.Wait();
#if NET48            
            if (mpcorbResponseTask.Status != TaskStatus.RanToCompletion)
            {
                throw new Exception("Failed to fetch data");
            }
#else
            if (!mpcorbResponseTask.IsCompletedSuccessfully)
            {
                throw new Exception("Failed to fetch data");
            }
#endif
            using (var mpcorbStream = mpcorbResponseTask.Result)
            {
                // var file = new StreamReader(mpcorbPathName);
                using (var streamReader = new StreamReader(mpcorbStream))
                {
                    var processLine = false;

                    string line;
                    while ((line = streamReader.ReadLine()) != null)
                    {
                        lineCount++;

                        if (lineCount % 100 == 0)
                        {
                            Console.WriteLine("{0}", lineCount);
                        }

                        // We skip over lines until we hit ------
                        if (line.StartsWith("------"))
                        {
                            processLine = true;
                            continue;
                        }

                        if (!processLine)
                        {
                            continue;
                        }

                        if (string.IsNullOrEmpty(line))
                        {
                            continue;
                        }

                        var epochString = line.Substring(21 - 1, 5);
                        int epochMonth;
                        int epochDay;
                        if (!int.TryParse(epochString.Substring(1, 2), out var epochYear))
                        {
                            throw new Exception(string.Format("Bad Year {0}", epochString));
                        }

                        if (epochString.Substring(0, 1) == "K")
                        {
                            epochYear += 2000;
                        }

                        var epochMonthChar = epochString.Substring(3, 1)[0];
                        if (epochMonthChar <= '9')
                        {
                            epochMonth = epochMonthChar - '0';
                        }
                        else
                        {
                            epochMonth = epochMonthChar - 'A' + 10;
                        }

                        var epochDayChar = epochString.Substring(4, 1)[0];
                        if (epochDayChar <= '9')
                        {
                            epochDay = epochDayChar - '0';
                        }
                        else
                        {
                            epochDay = epochDayChar - 'A' + 10;
                        }

                        var epoch = JulianDayNumber.FromDate(epochYear, epochMonth, epochDay);

                        var absoluteMagnitudeString = line.Substring(9 - 1, 13 - 9 + 1);
                        if (!double.TryParse(absoluteMagnitudeString, out var absoluteMagnitude))
                        {
                            absoluteMagnitude = 20.0;
                        }

                        var meanAnomalyString = line.Substring(27 - 1, 35 - 27 + 1);
                        if (!double.TryParse(meanAnomalyString, out var meanAnomaly))
                        {
                            throw new Exception(string.Format("Bad meanAnomaly {0}", meanAnomalyString));
                        }

                        meanAnomaly = DegreeToRadian(meanAnomaly);

                        var argumentOfPerihelionString = line.Substring(38 - 1, 46 - 38 + 1);
                        if (!double.TryParse(argumentOfPerihelionString, out var argumentOfPerihelion))
                        {
                            throw new Exception(string.Format("Bad argumentOfPerihelion {0}", argumentOfPerihelionString));
                        }

                        argumentOfPerihelion = DegreeToRadian(argumentOfPerihelion);

                        var longitudeOfTheAscendingNodeString = line.Substring(49 - 1, 57 - 49 + 1);
                        if (!double.TryParse(longitudeOfTheAscendingNodeString, out var longitudeOfTheAscendingNode))
                        {
                            throw new Exception(string.Format("Bad longitudeOfTheAscendingNode {0}", longitudeOfTheAscendingNodeString));
                        }
                        
                        longitudeOfTheAscendingNode = DegreeToRadian(longitudeOfTheAscendingNode);

                        var inclinationToTheEclipticString = line.Substring(60 - 1, 68 - 60 + 1);
                        if (!double.TryParse(inclinationToTheEclipticString, out var inclinationToTheEcliptic))
                        {
                            throw new Exception(string.Format("Bad inclinationToTheEcliptic {0}", inclinationToTheEclipticString));
                        }

                        inclinationToTheEcliptic = DegreeToRadian(inclinationToTheEcliptic);

                        var orbitalEccentricityString = line.Substring(71 - 1, 79 - 71 + 1);
                        if (!double.TryParse(orbitalEccentricityString, out var orbitalEccentricity))
                        {
                            throw new Exception(string.Format("Bad orbitalEccentricity {0}", orbitalEccentricityString));
                        }

                        //// var meanDailyMotionString = line.Substring(81 - 1, 91 - 81 + 1);
                        //// var meanDailyMotionInRadiansPerDay = DegreeToRadian(double.Parse(meanDailyMotionString));

                        var semiMajorAxisString = line.Substring(93 - 1, 103 - 93 + 1);
                        if (!double.TryParse(semiMajorAxisString, out var semiMajorAxis))
                        {
                            throw new Exception(string.Format("Bad semiMajorAxis {0}", semiMajorAxisString));
                        }

                        semiMajorAxis = semiMajorAxis * AuInKm;

                        var uncertaintyParameterString = line.Substring(106 - 1, 1);
                        if (!int.TryParse(uncertaintyParameterString, out var uncertaintyParameter))
                        {
                            uncertaintyParameter = 100;
                        }

                        string type;
                        var typeCodeString = line.Substring(162 - 1, 4).Trim();
                        if (!int.TryParse(typeCodeString, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out var typeCode))
                        {
                            typeCode = 0;
                        }

                        if ((typeCode & 4096) != 0)
                        {
                            type = "NEO!";
                        }
                        else if ((typeCode & 2048) != 0)
                        {
                            type = "NEO";
                        }
                        else
                        {
                            switch (typeCode & 0x3F)
                            {
                                case 2: // Aten
                                    type = "Aten";
                                    break;
                                case 3: // Apollo
                                    type = "Apollo";
                                    break;
                                case 4: // Amor
                                    type = "Amor";
                                    break;
                                case 5: // Object with q < 1.665 AU
                                    type = "QLess1665";
                                    break;
                                case 6: // Hungaria
                                    type = "Hungaria";
                                    break;
                                case 7: // Phocaea
                                    type = "Phocaea";
                                    break;
                                case 8: // Hilda
                                    type = "Hilda";
                                    break;
                                case 9: // Jupiter Trojan
                                    type = "JTrojan";
                                    break;
                                case 10: // Centaur
                                    type = "Centaur";
                                    break;
                                case 14: // Plutino
                                    type = "Plutino";
                                    break;
                                case 15: // Other resonant TNO
                                    type = "TNO";
                                    break;
                                case 16: // Cubewano
                                    type = "Cubewano";
                                    break;
                                case 17: // Scattered disk
                                    type = "Scattered";
                                    break;
                                default:
                                    type = string.Empty;
                                    break;
                            }
                        }

                        var readableDesignation = line.Substring(176 - 1, line.Length - 176 + 1).Trim().Replace(" ", string.Empty);

                        if (readableDesignation.ToUpperInvariant() == "NAME")
                        {
                            continue;
                        }

                        var readableName = line.Substring(176 - 1, 19).Trim().Replace(" ", string.Empty);

                        BodyInfo bodyInfo;
                        if (bodyInfos.ContainsKey(readableName.ToUpperInvariant()))
                        {
                            bodyInfo = bodyInfos[readableName.ToUpperInvariant()];
                            if (!bodyInfo.ReadIn)
                            {
                                continue;
                            }

                            bodyInfo.MeanAnomaly = meanAnomaly;
                            bodyInfo.AbsoluteMagnitude = absoluteMagnitude;
                            bodyInfo.ArgumentOfPerihelion = argumentOfPerihelion;
                            bodyInfo.LongitudeOfTheAscendingNode = longitudeOfTheAscendingNode;
                            bodyInfo.InclinationToTheEcliptic = inclinationToTheEcliptic;
                            bodyInfo.OrbitalEccentricity = orbitalEccentricity;
                            bodyInfo.SemiMajorAxis = semiMajorAxis;
                            bodyInfo.Epoch = epoch;
                            bodyInfo.Type = type;
                            bodyInfo.UncertaintyParameter = uncertaintyParameter;
                        }
                        else
                        {
                            if (!bodyInfos.ContainsKey(readableDesignation.ToUpperInvariant()))
                            {
                                bodyInfo = new BodyInfo
                                               {
                                                   Name = readableDesignation,
                                                   Radius = 0.1,
                                                   Mass = 0.0,
                                                   MeanAnomaly = meanAnomaly,
                                                   AbsoluteMagnitude = absoluteMagnitude,
                                                   ArgumentOfPerihelion = argumentOfPerihelion,
                                                   LongitudeOfTheAscendingNode = longitudeOfTheAscendingNode,
                                                   InclinationToTheEcliptic = inclinationToTheEcliptic,
                                                   OrbitalEccentricity = orbitalEccentricity,
                                                   SemiMajorAxis = semiMajorAxis,
                                                   RelativisticParam = 0.0,
                                                   Epoch = epoch,
                                                   Type = type,
                                                   UncertaintyParameter = uncertaintyParameter,
                                                   ReadIn = false
                                               };
                                bodyInfos.Add(readableDesignation.ToUpperInvariant(), bodyInfo);
                            }
                            else
                            {
                                Console.WriteLine("Body {0} already added", readableDesignation);
                            }
                        }
                    }
                }
            }

            return bodyInfos;
        }

        /// <summary>
        ///     Mains the specified args.
        /// </summary>
        /// <param name="args">
        ///     The args.
        /// </param>
        /// <exception cref="System.ArgumentException">
        ///     Bad usage
        ///     or
        ///     Extra number provided
        /// </exception>
        private static void Main(string[] args)
        {
            Uri uri;
            var fetchFromAstorb = false;
            var gotNumberCount = 0;
            var maxBodies = 0;
            var numOort = 0;

            foreach (var arg in args)
            {
                switch (arg)
                {
                    case "NEOFirst":
                        neoFirst = true;
                        break;
                    case "ASTORB":
                        fetchFromAstorb = true;
                        break;
                    case "AddDuplicate":
                        addDuplicateStarSystem = true;
                        break;
                    default:
                        int number;
                        if (!int.TryParse(arg, out number))
                        {
                            throw new ArgumentException("Bad usage");
                        }

                        switch (gotNumberCount)
                        {
                            case 0:
                                maxBodies = number;
                                break;
                            case 1:
                                numOort = number;
                                break;
                            case 2:
                                if (!addDuplicateStarSystem)
                                {
                                    throw new ArgumentException("Extra number provided");
                                }

                                xOffset = number;
                                break;
                            case 3:
                                if (!addDuplicateStarSystem)
                                {
                                    throw new ArgumentException("Extra number provided");
                                }

                                yOffset = number;
                                break;
                            case 4:
                                if (!addDuplicateStarSystem)
                                {
                                    throw new ArgumentException("Extra number provided");
                                }

                                vxOffset = number;
                                break;
                            default:
                                throw new ArgumentException("Extra number provided");
                        }

                        gotNumberCount++;
                        break;
                }
            }

            var builder = new ConfigurationBuilder();
            builder.SetBasePath(Directory.GetCurrentDirectory());
            builder.AddJsonFile("appsettings.json");
            var configuration = builder.Build();

            if (fetchFromAstorb)
            {
                uri = new Uri(configuration["AppSettings:UriAstOrb"]);
            }
            else
            {
                uri = new Uri(configuration["AppSettings:UriMpcOrb"]);
            }

            Console.WriteLine("Fetching data from {0}", uri);
            var count = ProcessUri(uri, @"Info.csv", "FINAL.SLF", maxBodies, numOort);
            Console.WriteLine();
            Console.WriteLine(" outputed {0}", count);
        }

        /// <summary>
        ///     The process mpcorb.
        /// </summary>
        /// <param name="mpcorbUrl">
        ///     The mpcorb URL.
        /// </param>
        /// <param name="detailsPathName">
        ///     The details Path Name.
        /// </param>
        /// <param name="fileOutName">
        ///     The file out name.
        /// </param>
        /// <param name="maxBodies">
        ///     The maximum number bodies.
        /// </param>
        /// <param name="numOort">
        ///     The number of random oort cloud objects to add.
        /// </param>
        /// <returns>
        ///     The <see cref="long" />.
        /// </returns>
        private static long ProcessUri(
            Uri mpcorbUrl,
            string detailsPathName,
            string fileOutName,
            int maxBodies,
            int numOort)
        {
            Dictionary<string, BodyInfo> bodyInfos;
            if (mpcorbUrl.AbsolutePath.ToUpperInvariant().Contains("MPCORB"))
            {
                bodyInfos = FetchMpcOrBbodyInfos(mpcorbUrl, detailsPathName);
            }
            else
            {
                bodyInfos = FetchAstroOrBbodyInfos(mpcorbUrl, detailsPathName);
            }

            // find out which epoch has the most entries and use it
            var epochCount = new Dictionary<double, int>();
            foreach (var bodyInfom in bodyInfos)
            {
                if (epochCount.ContainsKey(bodyInfom.Value.Epoch))
                {
                    epochCount[bodyInfom.Value.Epoch]++;
                }
                else
                {
                    epochCount.Add(bodyInfom.Value.Epoch, 0);
                }
            }

            var epochMaxCount = epochCount.OrderByDescending(x => x.Value).ToArray();
            var epoch = epochMaxCount[0].Key;
            Console.WriteLine("Using epoch {0}", epoch);

            // Add the Sun
            var bodyInfo = bodyInfos["SUN"];
            var pos = new Double4 { X = 0.0, Y = 0.0, Z = 0.0 };
            var vel = new Double4 { X = 0.0, Y = 0.0, Z = 0.0 };
            bodyInfo.StateVectors = new[] { pos, vel };
            bodyInfo.Epoch = epoch;

            // Add the planets
            var planets = new Planets();
            bodyInfo = bodyInfos["Mercury".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Mercury(epoch, out var planetSemiMajorAxis);
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.Epoch = epoch;

            bodyInfo = bodyInfos["Venus".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Venus(epoch, out planetSemiMajorAxis);
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.AbsoluteMagnitude = -4.4;
            bodyInfo.Epoch = epoch;

            bodyInfo = bodyInfos["Earth".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Earth(epoch, out planetSemiMajorAxis);
            bodyInfo.AbsoluteMagnitude = -3.5;
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.Epoch = epoch;

            bodyInfo = bodyInfos["Mars".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Mars(epoch, out planetSemiMajorAxis);
            bodyInfo.AbsoluteMagnitude = -1.52;
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.Epoch = epoch;

            bodyInfo = bodyInfos["Jupiter".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Jupiter(epoch, out planetSemiMajorAxis);
            bodyInfo.AbsoluteMagnitude = -9.3;
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.Epoch = epoch;
            var jupiterSemiMajorAxis = bodyInfo.SemiMajorAxis;

            bodyInfo = bodyInfos["Saturn".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Saturn(epoch, out planetSemiMajorAxis);
            bodyInfo.AbsoluteMagnitude = -9.3;
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.Epoch = epoch;

            bodyInfo = bodyInfos["Uranus".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Uranus(epoch, out planetSemiMajorAxis);
            bodyInfo.AbsoluteMagnitude = -7.19;
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.Epoch = epoch;

            bodyInfo = bodyInfos["Neptune".ToUpperInvariant()];
            bodyInfo.StateVectors = planets.Neptune(epoch, out planetSemiMajorAxis);
            bodyInfo.AbsoluteMagnitude = -6.87;
            bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            bodyInfo.Epoch = epoch;

            bodyInfo = bodyInfos["Pluto".ToUpperInvariant()];
            var plutoMass = bodyInfo.Mass;
            ////bodyInfo.StateVectors = planets.Pluto(epoch, out planetSemiMajorAxis);
            ////bodyInfo.AbsoluteMagnitude = -1.0;
            ////bodyInfo.SemiMajorAxis = planetSemiMajorAxis * AuInKm;
            ////bodyInfo.Epoch = epoch;

            bodyInfos = bodyInfos.Where(x => Math.Abs(x.Value.Epoch - epoch) < 0.00001)
                .ToDictionary(x => x.Key, x => x.Value);

            // add a number of random inner oort cloud bodies
            var random = new Random();
            if (numOort > 0)
            {
                for (var bodyNum = 0; bodyNum < numOort; bodyNum++)
                {
                    const double Mass = 0.0; // = 3.0E25 / 1000000000000;
                    if (bodyNum % 100 == 0)
                    {
                        Console.WriteLine("Oort Cloud {0}", bodyNum);
                    }

                    var distance = (random.NextDouble() * 98000) + 2000;
                    var maxInc = distance < 20000 ? 60 : ((distance - 20000) * (180 - 60) / (100000 - 20000)) + 60;
                    string name;
                    string type;
                    if (distance < 20000)
                    {
                        name = string.Format("InnerOort{0}", bodyNum);
                        type = "InnerOort";
                    }
                    else
                    {
                        name = string.Format("OuterOort{0}", bodyNum);
                        type = "OuterOort";
                    }

                    bodyInfo = new BodyInfo
                                   {
                                       Name = name,
                                       Mass = Mass,
                                       Epoch = epoch,
                                       RelativisticParam = 0.0,
                                       MeanAnomaly = DegreeToRadian(random.NextDouble() * 360),
                                       SemiMajorAxis = AuInKm * distance,
                                       OrbitalEccentricity = random.NextDouble() * 0.3,
                                       InclinationToTheEcliptic = DegreeToRadian((random.NextDouble() * maxInc) - (maxInc / 2)),
                                       ArgumentOfPerihelion = DegreeToRadian(random.NextDouble() * 360),
                                       LongitudeOfTheAscendingNode = DegreeToRadian(random.NextDouble() * 360),
                                       AbsoluteMagnitude = 25.0,
                                       StateVectors = null,
                                       ReadIn = false,
                                       Type = type,
                                       UncertaintyParameter = 0
                                   };
                    bodyInfos.Add(bodyInfo.Name, bodyInfo);
                }
            }

            if (addDuplicateStarSystem)
            {
                // Create the Duplicate system
                var duplicateSystemBodies = new LinkedList<BodyInfo>();
                foreach (var bodyInfoB in bodyInfos.Values)
                {
                    bodyInfo = new BodyInfo
                                   {
                                       Name = bodyInfoB.Name + "B",
                                       Mass = bodyInfoB.Mass,
                                       Epoch = bodyInfoB.Epoch,
                                       RelativisticParam = 0.0,
                                       MeanAnomaly = bodyInfoB.MeanAnomaly,
                                       SemiMajorAxis = bodyInfoB.SemiMajorAxis,
                                       OrbitalEccentricity = bodyInfoB.OrbitalEccentricity,
                                       InclinationToTheEcliptic = bodyInfoB.InclinationToTheEcliptic,
                                       ArgumentOfPerihelion = bodyInfoB.ArgumentOfPerihelion,
                                       LongitudeOfTheAscendingNode = bodyInfoB.LongitudeOfTheAscendingNode,
                                       AbsoluteMagnitude = bodyInfoB.AbsoluteMagnitude,
                                       XOffset = xOffset,
                                       YOffset = yOffset,
                                       VxOffset = vxOffset,
                                       StateVectors = bodyInfoB.StateVectors,
                                       ReadIn = bodyInfoB.ReadIn,
                                       Type = bodyInfoB.Type
                                   };

                    duplicateSystemBodies.AddLast(bodyInfo);
                }

                // Then add it to the bodies
                foreach (var duplicateSystemBody in duplicateSystemBodies)
                {
                    bodyInfos.Add(duplicateSystemBody.Name, duplicateSystemBody);
                }
            }

            Console.WriteLine("Creating list of bodies");
            Console.WriteLine("Adding bodies with mass");

            // Bodys with mass must come first
            var bodiesWithMass =
                bodyInfos.Values.Where(x => x.Mass >= plutoMass).OrderBy(x => x.SemiMajorAxis).ToList();
            bodiesWithMass.AddRange(
                bodyInfos.Values
                    .Where(x => x.Mass > 0.0 && x.Mass < plutoMass && x.SemiMajorAxis < jupiterSemiMajorAxis)
                    .OrderByDescending(x => x.Mass).ToList());
            bodiesWithMass.AddRange(
                bodyInfos.Values
                    .Where(x => x.Mass > 0.0 && x.Mass < plutoMass && x.SemiMajorAxis >= jupiterSemiMajorAxis)
                    .OrderBy(x => x.SemiMajorAxis).ToList());

            Console.WriteLine("{0} bodies with mass", bodiesWithMass.Count);

            var bodiesWithoutMass = bodyInfos.Values.Where(x => x.Mass == 0.0 && x.UncertaintyParameter < 10).ToList();
            Console.WriteLine("Indexing {0} bodies with mass by type", bodiesWithoutMass.Count);

            // index the bodies without mass by type
            var bodysByType = new Dictionary<string, LinkedList<BodyInfo>>();
            foreach (var body in bodiesWithoutMass)
            {
                if (!bodysByType.ContainsKey(body.Type))
                {
                    var bodiesOfSameType = new LinkedList<BodyInfo>();
                    bodiesOfSameType.AddLast(body);
                    bodysByType.Add(body.Type, bodiesOfSameType);
                }
                else
                {
                    bodysByType[body.Type].AddLast(body);
                }
            }

            Console.WriteLine("{0} types", bodysByType.Keys.Count);
            Console.WriteLine("Types are ");
            foreach (var type in bodysByType.Keys)
            {
                Console.WriteLine("{0,32} : {1}", type, bodysByType[type].Count);
            }

            Console.WriteLine();
            Console.WriteLine("Creating a mixure of the bodies without mass");

            // mix them by building a list taking one of each type
            var bodyCount = bodiesWithoutMass.Count;
            var mixedBodyInfos = new LinkedList<BodyInfo>();

            if (neoFirst)
            {
                Console.WriteLine("Adding asteroids of type NEO and NEO! first");

                // Add all NEO and NEO! types
                foreach (var body in bodysByType["NEO"])
                {
                    mixedBodyInfos.AddLast(body);
                }

                bodyCount -= bodysByType["NEO"].Count;
                bodysByType["NEO"] = new LinkedList<BodyInfo>();

                foreach (var body in bodysByType["NEO!"])
                {
                    mixedBodyInfos.AddLast(body);
                }

                bodyCount -= bodysByType["NEO!"].Count;
                bodysByType["NEO!"] = new LinkedList<BodyInfo>();
            }

            while (bodyCount > 0)
            {
                foreach (var type in bodysByType.Keys)
                {
                    if (bodysByType[type].Count <= 0)
                    {
                        continue;
                    }

                    var body = bodysByType[type].First;
                    bodysByType[type].RemoveFirst();
                    mixedBodyInfos.AddLast(body);
                    bodyCount--;
                }

                if (bodyCount % 1000 == 0)
                {
                    Console.WriteLine("{0} left to add", bodyCount);
                }
            }

            var bodiesInOrder = bodiesWithMass;
            bodiesInOrder.AddRange(mixedBodyInfos);
            if (maxBodies > 0)
            {
                bodiesInOrder = bodiesInOrder.Take(maxBodies).ToList();
            }

            Console.WriteLine("Writing to File {0}", fileOutName);
            var count = WriteFile(fileOutName, epoch, bodiesInOrder);
            return count;
        }

        /// <summary>
        ///     The read body details like mass etc from a file.
        /// </summary>
        /// <param name="detailsPathName">
        ///     The details path name.
        /// </param>
        /// <returns>
        ///     The <see cref="Dictionary" />.
        /// </returns>
        private static Dictionary<string, BodyInfo> ReadDetails(string detailsPathName)
        {
            var bodyInfos = new Dictionary<string, BodyInfo>(1000000);
            var detailsFileInfo = new FileInfo(detailsPathName);
            using (var file = new StreamReader(detailsFileInfo.OpenRead()))
            {
                string line;
                while ((line = file.ReadLine()) != null)
                {
                    var bodyInfo = new BodyInfo();

                    var parts = line.Split(',');
                    bodyInfo.Name = parts[1];
                    if (!double.TryParse(parts[2], out bodyInfo.Radius))
                    {
                        bodyInfo.Radius = 0.0;
                    }

                    bodyInfo.Type = string.Empty; // parts[3];

                    if (!double.TryParse(parts[4], out _))
                    {
                    }

                    if (!double.TryParse(parts[5], out bodyInfo.Density))
                    {
                        bodyInfo.Density = 0.0;
                    }

                    if (!double.TryParse(parts[6], out bodyInfo.Mass))
                    {
                        bodyInfo.Mass = 0.0;
                    }

                    bodyInfo.Epoch = 0.0;
                    bodyInfo.RelativisticParam = 0.0;
                    bodyInfo.MeanAnomaly = 0.0;
                    bodyInfo.SemiMajorAxis = 0.0;
                    bodyInfo.OrbitalEccentricity = 0.0;
                    bodyInfo.InclinationToTheEcliptic = 0.0;
                    bodyInfo.ArgumentOfPerihelion = 0.0;
                    bodyInfo.LongitudeOfTheAscendingNode = 0.0;
                    bodyInfo.AbsoluteMagnitude = 0.0;
                    bodyInfo.StateVectors = null;
                    bodyInfo.ReadIn = true;
                    bodyInfo.UncertaintyParameter = 0;

                    if (bodyInfo.Name.ToUpperInvariant() != "NAME")
                    {
                        bodyInfos.Add(bodyInfo.Name.ToUpperInvariant(), bodyInfo);
                    }
                }
            }

            return bodyInfos;
        }

        /// <summary>
        ///     Write out the bodys to a slf file.
        /// </summary>
        /// <param name="fileOutName">
        ///     The file out name.
        /// </param>
        /// <param name="epoch">
        ///     The epoch.
        /// </param>
        /// <param name="bodies">
        ///     The bodies.
        /// </param>
        /// <returns>
        ///     The <see cref="long" />.
        /// </returns>
        private static long WriteFile(string fileOutName, double epoch, IEnumerable<BodyInfo> bodies)
        {
            long count = 0;

            var maxNameLength = 0;

            // 4.5783476938E+09;
            var fileOutStream = File.Create(fileOutName);
            using (var fileOut = new StreamWriter(fileOutStream))
            {
                fileOut.WriteLine(@" {0}", epoch);
                fileOut.Write(" 3");

                foreach (var body in bodies)
                {
                    Double4[] stateVectors;
                    if (body.StateVectors != null)
                    {
                        stateVectors = body.StateVectors;
                    }
                    else if (body.SemiMajorAxis != 0.0)
                    {
                        var ellipticMotion = new EllipticMotion();
                        stateVectors = ellipticMotion.ComputeStateVectors(
                            SunMass + body.Mass,
                            body.MeanAnomaly,
                            body.SemiMajorAxis,
                            body.OrbitalEccentricity,
                            body.InclinationToTheEcliptic,
                            body.ArgumentOfPerihelion,
                            body.LongitudeOfTheAscendingNode);
                    }
                    else
                    {
                        continue;
                    }

                    if (body.SemiMajorAxis > 0)
                    {
                        body.RelativisticParam = -9 * GravitationalConstant * SunMass / (SpeedOfLight * SpeedOfLight * body.SemiMajorAxis);
                    }
                    else
                    {
                        body.RelativisticParam = 0.0;
                    }

                    var name = body.Name + "-" + body.UncertaintyParameter
                               + (body.Type != string.Empty ? "-[" + body.Type + "]" : string.Empty);
                    maxNameLength = name.Length > maxNameLength ? name.Length : maxNameLength;
                    fileOut.WriteLine(
                        " {0} {1} {2} {3}# {4}",
                        body.Mass / 1.0e+24,
                        body.Radius / 1000000,
                        body.AbsoluteMagnitude,
                        body.RelativisticParam,
                        name);
                    fileOut.WriteLine(
                        "{0,23:0.0000000000000000E+00} {1,23:0.0000000000000000E+00} {2,23:0.0000000000000000E+00}",
                        (stateVectors[0].X / Gm) - body.XOffset,
                        (stateVectors[0].Y / Gm) - body.YOffset,
                        stateVectors[0].Z / Gm);
                    fileOut.WriteLine(
                        "{0,23:0.0000000000000000E+00} {1,23:0.0000000000000000E+00} {2,23:0.0000000000000000E+00}",
                        (stateVectors[1].X / 1000) - body.VxOffset,
                        stateVectors[1].Y / 1000,
                        stateVectors[1].Z / 1000);
                    count++;
                }
                Console.WriteLine("Wrote to {0}", fileOutStream.Name);
                Console.WriteLine("Max Name Length is {0}", maxNameLength);
            }

            return count;
        }

        /// <summary>
        ///     The body info.
        /// </summary>
        private class BodyInfo
        {
            /// <summary>
            ///     The absolute magnitude of the body.
            /// </summary>
            public double AbsoluteMagnitude;

            /// <summary>
            ///     The bodies argument of perihelion.
            /// </summary>
            public double ArgumentOfPerihelion;

            /// <summary>
            ///     The density of the body
            /// </summary>
            public double Density;

            /// <summary>
            ///     The epoch of the bodies orbital elements.
            /// </summary>
            public double Epoch;

            /// <summary>
            ///     The bodies inclination to the ecliptic.
            /// </summary>
            public double InclinationToTheEcliptic;

            /// <summary>
            ///     The bodies longitude of the ascending node.
            /// </summary>
            public double LongitudeOfTheAscendingNode;

            /// <summary>
            ///     The bodies mass.
            /// </summary>
            public double Mass;

            /// <summary>
            ///     The bodies mean anomaly.
            /// </summary>
            public double MeanAnomaly;

            /// <summary>
            ///     The name of the body.
            /// </summary>
            public string Name;

            /// <summary>
            ///     The bodies orbital eccentricity.
            /// </summary>
            public double OrbitalEccentricity;

            /// <summary>
            ///     The bodies radius.
            /// </summary>
            public double Radius;

            /// <summary>
            ///     Was the body read in from the details file
            /// </summary>
            public bool ReadIn;

            /// <summary>
            ///     The bodies relativistic parameter.
            /// </summary>
            public double RelativisticParam;

            /// <summary>
            ///     The bodies semi major axis.
            /// </summary>
            public double SemiMajorAxis;

            /// <summary>
            ///     The bodies state vectors.
            /// </summary>
            public Double4[] StateVectors;

            /// <summary>
            ///     The bodies type.
            /// </summary>
            public string Type;

            /// <summary>
            ///     The uncertainty parameter for the bodies orbital parameters
            /// </summary>
            public int UncertaintyParameter;

            /// <summary>
            ///     The bodies vx offset (used to create another copy of the solar system)
            /// </summary>
            public double VxOffset;

            /// <summary>
            ///     The bodies x offset (used to create another copy of the solar system)
            /// </summary>
            public double XOffset;

            /// <summary>
            ///     The bodies y offset (used to create another copy of the solar system)
            /// </summary>
            public double YOffset;
        }
    }
}