// --------------------------------------------------------------------------------------------------------------------
// <copyright file="MpcHandler.cs" company="Michael William Simmons">
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

  using System.Collections.Generic;
  using System.Globalization;
  using System.IO;
  using System.Net.Http;
  using System.Threading.Tasks;
  using System;
  using Microsoft.Extensions.Configuration;
  using Microsoft.Extensions.Logging;
  using System.Security.Policy;

  public class MpcHandler : IRemoteBodyInfosSource
  {

    private IConfiguration config;
    private ILogger<MpcHandler> logger;
    private IUtilities utilities;

    public MpcHandler(IConfiguration config, ILogger<MpcHandler> logger, IUtilities utilities)
    {
      this.config = config;
      this.logger = logger;
      this.utilities = utilities;
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
    public Dictionary<string, BodyInfo> FetchBodyInfos(string detailsPathName)
    {
      var bodyInfos = utilities.ReadDetails(detailsPathName);
      long lineCount = 0;
      var uri = new Uri(this.config["UriMpcOrb"]);
      this.logger.LogInformation("Fetching data from {0}", uri);
      var mpcorbHttpClient = new HttpClient();
      var mpcorbResponseTask = mpcorbHttpClient.GetStreamAsync(uri);
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
              this.logger.LogInformation("{0}", lineCount);
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
              this.logger.LogError("Bad Year {0}", epochString);
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
              this.logger.LogError("Bad meanAnomaly {0}", meanAnomalyString);
              throw new Exception(string.Format("Bad meanAnomaly {0}", meanAnomalyString));
            }

            meanAnomaly = utilities.DegreeToRadian(meanAnomaly);

            var argumentOfPerihelionString = line.Substring(38 - 1, 46 - 38 + 1);
            if (!double.TryParse(argumentOfPerihelionString, out var argumentOfPerihelion))
            {
              this.logger.LogError("Bad argumentOfPerihelion {0}", argumentOfPerihelionString);
              throw new Exception(string.Format("Bad argumentOfPerihelion {0}", argumentOfPerihelionString));
            }

            argumentOfPerihelion = utilities.DegreeToRadian(argumentOfPerihelion);

            var longitudeOfTheAscendingNodeString = line.Substring(49 - 1, 57 - 49 + 1);
            if (!double.TryParse(longitudeOfTheAscendingNodeString, out var longitudeOfTheAscendingNode))
            {
              this.logger.LogError("Bad longitudeOfTheAscendingNode {0}", longitudeOfTheAscendingNodeString);
              throw new Exception(string.Format("Bad longitudeOfTheAscendingNode {0}", longitudeOfTheAscendingNodeString));
            }

            longitudeOfTheAscendingNode = utilities.DegreeToRadian(longitudeOfTheAscendingNode);

            var inclinationToTheEclipticString = line.Substring(60 - 1, 68 - 60 + 1);
            if (!double.TryParse(inclinationToTheEclipticString, out var inclinationToTheEcliptic))
            {
              this.logger.LogError("Bad inclinationToTheEcliptic {0}", inclinationToTheEclipticString);
              throw new Exception(string.Format("Bad inclinationToTheEcliptic {0}", inclinationToTheEclipticString));
            }

            inclinationToTheEcliptic = utilities.DegreeToRadian(inclinationToTheEcliptic);

            var orbitalEccentricityString = line.Substring(71 - 1, 79 - 71 + 1);
            if (!double.TryParse(orbitalEccentricityString, out var orbitalEccentricity))
            {
              this.logger.LogError("Bad orbitalEccentricity {0}", orbitalEccentricityString);
              throw new Exception(string.Format("Bad orbitalEccentricity {0}", orbitalEccentricityString));
            }

            //// var meanDailyMotionString = line.Substring(81 - 1, 91 - 81 + 1);
            //// var meanDailyMotionInRadiansPerDay = DegreeToRadian(double.Parse(meanDailyMotionString));

            var semiMajorAxisString = line.Substring(93 - 1, 103 - 93 + 1);
            if (!double.TryParse(semiMajorAxisString, out var semiMajorAxis))
            {
              this.logger.LogError("Bad semiMajorAxis {0}", semiMajorAxisString);
              throw new Exception(string.Format("Bad semiMajorAxis {0}", semiMajorAxisString));
            }

            semiMajorAxis = semiMajorAxis * Constants.AuInKm;

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
                this.logger.LogInformation("Body {0} already added", readableDesignation);
              }
            }
          }
        }
      }

      return bodyInfos;
    }
  }
}

