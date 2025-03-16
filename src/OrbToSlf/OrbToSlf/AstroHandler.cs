// --------------------------------------------------------------------------------------------------------------------
// <copyright file="AstroHandler.cs" company="Michael William Simmons">
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
  using System.IO;
  using System.Net.Http;
  using System.Threading.Tasks;
  using System;
  using Microsoft.Extensions.Configuration;
  using Microsoft.Extensions.Logging;
  using System.Security.Policy;

  public class AstroHandler : IRemoteBodyInfosSource
  {
    private IConfiguration config;
    private ILogger<AstroHandler> logger;
    private IUtilities utilities;

    public AstroHandler(IConfiguration config, ILogger<AstroHandler> logger, IUtilities utilities)
    {
      this.config = config;
      this.logger = logger;
      this.utilities = utilities;
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
    public Dictionary<string, BodyInfo> FetchBodyInfos(string detailsPathName)
    {
      var bodyInfos = utilities.ReadDetails(detailsPathName);
      long lineCount = 0;
      var uri = new Uri(this.config["UriAstOrb"]);
      this.logger.LogInformation("Fetching data from {0}", uri);
      var astrorbHttpClient = new HttpClient();
      var astrorbResponseTask = astrorbHttpClient.GetStreamAsync(uri);
      astrorbResponseTask.Wait();
#if NET48
      if (astrorbResponseTask.Status != TaskStatus.RanToCompletion)
      {
        this.logger.LogError("Failed to fetch data");
        throw new Exception("Failed to fetch data");
      }
#else
      if (!astrorbResponseTask.IsCompletedSuccessfully)
      {
        this.logger.LogError("Failed to fetch data");
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
              this.logger.LogInformation("{0}", lineCount);
            }

            var epochString = line.Substring(107 - 1, 8);
            if (!int.TryParse(epochString.Substring(0, 4), out var epochYear))
            {
              this.logger.LogError("Bad Year {0}", epochString);
              throw new Exception(string.Format("Bad Year {0}", epochString));
            }

            if (!int.TryParse(epochString.Substring(4, 2), out var epochMonth))
            {
              this.logger.LogError("Bad Month {0}", epochString);
              throw new Exception(string.Format("Bad Month {0}", epochString));
            }

            if (!int.TryParse(epochString.Substring(6, 2), out var epochDay))
            { 
              this.logger.LogError("Bad Day {0}", epochString);
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
              this.logger.LogError("Bad meanAnomaly {0}", meanAnomalyString);
              throw new Exception(string.Format("Bad meanAnomaly {0}", meanAnomalyString));
            }

            meanAnomaly = utilities.DegreeToRadian(meanAnomaly);

            var argumentOfPerihelionString = line.Substring(127 - 1, 10);
            if (!double.TryParse(argumentOfPerihelionString, out var argumentOfPerihelion))
            {
              this.logger.LogError("Bad argumentOfPerihelion {0}", argumentOfPerihelionString);
              throw new Exception(
                  string.Format("Bad argumentOfPerihelion {0}", argumentOfPerihelionString));
            }

            argumentOfPerihelion = utilities.DegreeToRadian(argumentOfPerihelion);

            var longitudeOfTheAscendingNodeString = line.Substring(138 - 1, 10);
            if (!double.TryParse(longitudeOfTheAscendingNodeString, out var longitudeOfTheAscendingNode))
            {
              this.logger.LogError("Bad longitudeOfTheAscendingNode {0}", longitudeOfTheAscendingNodeString);
              throw new Exception(
                  string.Format(
                      "Bad longitudeOfTheAscendingNode {0}",
                      longitudeOfTheAscendingNodeString));
            }

            longitudeOfTheAscendingNode = utilities.DegreeToRadian(longitudeOfTheAscendingNode);

            var inclinationToTheEclipticString = line.Substring(148 - 1, 10);
            if (!double.TryParse(inclinationToTheEclipticString, out var inclinationToTheEcliptic))
            {
              this.logger.LogError("Bad inclinationToTheEcliptic {0}", inclinationToTheEclipticString);
              throw new Exception(
                  string.Format("Bad inclinationToTheEcliptic {0}", inclinationToTheEclipticString));
            }

            inclinationToTheEcliptic = utilities.DegreeToRadian(inclinationToTheEcliptic);

            var orbitalEccentricityString = line.Substring(159 - 1, 10);
            if (!double.TryParse(orbitalEccentricityString, out var orbitalEccentricity))
            {
              this.logger.LogError("Bad orbitalEccentricity {0}", orbitalEccentricityString);
              throw new Exception(
                  string.Format("Bad orbitalEccentricity {0}", orbitalEccentricityString));
            }

            //// var meanDailyMotion = line.Substring(81 - 1, 91 - 81 + 1);
            //// meanDailyMotionInRadiansPerDay = 0.0f; // double.Parse(meanDailyMotion);

            var semiMajorAxisString = line.Substring(170 - 1, 12);
            if (!double.TryParse(semiMajorAxisString, out var semiMajorAxis))
            {
              this.logger.LogError("Bad semiMajorAxis {0}", semiMajorAxisString);
              throw new Exception(string.Format("Bad semiMajorAxis {0}", semiMajorAxisString));
            }

            semiMajorAxis = semiMajorAxis * Constants.AuInKm;

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
  }
}