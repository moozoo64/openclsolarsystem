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
  using System.Globalization;
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
    ///     Converts a GM in km^3/s^2 to the SLF mass in kg, the mass that OpenCLSolarSystem multiplies
    ///     back by the same G (G_SLF_SCALED) to get exactly this GM. Only GM matters to the simulation,
    ///     and it is known far more precisely than G, so a source's own G and quoted mass are not used.
    ///     The result is not the body's true mass, because Constants.GravitationalConstant is not CODATA.
    /// </summary>
    /// <param name="gm">GM in km^3/s^2.</param>
    /// <returns>The SLF mass in kg.</returns>
    public static double MassFromGM(double gm)
    {
      return gm * 1e9 / Constants.GravitationalConstant;
    }

    /// <summary>
    ///     The read body details like mass etc from a file.
    ///     Columns: Number, Name, Radius (km), Type, GM (km^3/s^2), Density, Mass (kg), Mass 1e24.
    ///     When a row has a GM, the mass is computed from it with MassFromGM and the Mass column is
    ///     ignored. A warning is logged if they disagree by more than 0.1%, which allows for a Mass
    ///     computed with another G (CODATA differs by 2.6e-4) but catches a wrong value. Rows without a
    ///     GM use Mass. Spaces are removed from names, as MpcHandler and AstroHandler do with the MPC
    ///     and astorb designations they are matched against. Only the first 8 columns are read and
    ///     they must not contain quoted commas; later columns (e.g. notes) may.
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
          bodyInfo.Name = parts[1].Trim().Replace(" ", string.Empty);
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

          // GM (km^3/s^2) takes precedence over Mass: see MassFromGM.
          double gm;
          bool hasGM = double.TryParse(parts[4], NumberStyles.Float, CultureInfo.InvariantCulture, out gm) && gm > 0.0;

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
          bool hasMass = double.TryParse(parts[6], NumberStyles.Float, CultureInfo.InvariantCulture, out mass);
          if (hasGM)
          {
            bodyInfo.Mass = MassFromGM(gm);
            if (hasMass && mass > 0.0 && Math.Abs(bodyInfo.Mass / mass - 1.0) > 1e-3)
            {
              this.logger?.LogWarning(
                "Info.csv: {0} GM {1} km^3/s^2 and Mass {2} kg differ by {3:E2}; using the GM",
                bodyInfo.Name, gm, mass, bodyInfo.Mass / mass - 1.0);
            }
          }
          else if (hasMass)
          {
            bodyInfo.Mass = mass;
          }
          else
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

          // Vitagliano's -9 G M_sun / (c^2 a) correction is only defined for an elliptic orbit.
          // Hyperbolic orbits (a <= 0) get 0, so only the 6 G M_sun / (c^2 r) term applies to them.
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