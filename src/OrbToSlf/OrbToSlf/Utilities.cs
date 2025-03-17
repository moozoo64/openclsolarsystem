// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Utilities.cs" company="Michael William Simmons">
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
  using System;
  using System.Collections.Generic;
  using System.IO;

#if NET48
  using System.Threading.Tasks;
#endif

  using Microsoft.Extensions.Configuration;
  using Microsoft.Extensions.Logging;

  public interface IUtilities
  {
    Dictionary<string, BodyInfo> ReadDetails(string detailsPathName);
    long WriteFile(string fileOutName, double epoch, IEnumerable<BodyInfo> bodies);
    double DegreeToRadian(double angle);

  }

  /// <summary>
  ///     A program that reads asteroid data and creates a .SLF file of the solar system
  ///     It computes the positions of the planets using the method detailed in
  ///     "Keplerian Elements for Approximate Positions of the Major Planets"
  ///     by EM Standish, Solar System Dynamics Group" JPL/Caltech
  ///     It can also add random Oort cloud objects
  /// </summary>
  public class Utilities : IUtilities
  {

    private IConfiguration config;
    private ILogger<Utilities> logger;
    private IPlanets planets;

    public Utilities(IConfiguration configuration, ILogger<Utilities> logger, IPlanets planets)
    {
      this.config = configuration;
      this.logger = logger;
      this.planets = planets;
    }

    /// <summary>
    ///     The converts and angle in degrees to radians.
    /// </summary>
    /// <param name="angle">
    ///     The angle in degrees
    /// </param>
    /// <returns>
    ///     The angle in Radians
    /// </returns>
    public double DegreeToRadian(double angle)
    {
      return Math.PI * angle / 180.0;
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
    public Dictionary<string, BodyInfo> ReadDetails(string detailsPathName)
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
          double radius;
          if (!double.TryParse(parts[2], out radius))
          {
            bodyInfo.Radius = 0.0;
          }
          else
          {
            bodyInfo.Radius = radius;
          }

          bodyInfo.Type = string.Empty; // parts[3];

          if (!double.TryParse(parts[4], out _))
          {
          }

          double density;
          if (!double.TryParse(parts[5], out density))
          {
            bodyInfo.Density = 0.0;
          }
          else
          {
            bodyInfo.Density = density;
          }

          double mass;
          if (!double.TryParse(parts[6], out mass))
          {
            bodyInfo.Mass = 0.0;
          }
          else
          {
            bodyInfo.Mass = mass;
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
    public long WriteFile(string fileOutName, double epoch, IEnumerable<BodyInfo> bodies)
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
                Constants.SunMass + body.Mass,
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
            body.RelativisticParam = -9 * Constants.GravitationalConstant * Constants.SunMass / (Constants.SpeedOfLight * Constants.SpeedOfLight * body.SemiMajorAxis);
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
              (stateVectors[0].X / Constants.Gm) - body.XOffset,
              (stateVectors[0].Y / Constants.Gm) - body.YOffset,
              stateVectors[0].Z / Constants.Gm - body.ZOffset);
          fileOut.WriteLine(
              "{0,23:0.0000000000000000E+00} {1,23:0.0000000000000000E+00} {2,23:0.0000000000000000E+00}",
              (stateVectors[1].X / 1000) - body.VxOffset,
              stateVectors[1].Y / 1000 - body.VyOffset,
              stateVectors[1].Z / 1000 - body.VzOffset);
          count++;
        }
        this.logger.LogInformation("Wrote to {0}", fileOutStream.Name);
        this.logger.LogInformation("Max Name Length is {0}", maxNameLength);
      }

      return count;
    }
  }
}