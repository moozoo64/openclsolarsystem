// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Process.cs" company="Michael William Simmons">
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
  using Microsoft.Extensions.Configuration;
  using Microsoft.Extensions.Logging;
  using System;
  using System.Collections.Generic;
  using System.Linq;

  public interface IProcess
  {
    void DoProcess();
    long ProcessUri();
  }
  public class Process : IProcess
  {
    private IConfiguration config;
    private ILogger<Process> logger;
    private IUtilities utilities;
    private IPlanets planets;
    private IRemoteBodyInfosSource remoteBodyInfosSource;

    private bool neoFirst;
    private bool addDuplicateStarSystem;
    private double xOffset;
    private double yOffset;
    private double zOffset;
    private double vxOffset;
    private double vyOffset;
    private double vzOffset;
    private int maxBodies;
    private int numOort;
    private string planetInfoFileName;
    private string slfOutFileName;

    public Process(IConfiguration config, ILogger<Process> logger, IUtilities utilities,IPlanets planets, IRemoteBodyInfosSource remoteBodyInfosSource)
    {
      this.config = config;
      this.logger = logger;
      this.utilities = utilities;
      this.planets = planets;
      this.remoteBodyInfosSource = remoteBodyInfosSource;
      this.neoFirst = config.GetValue<bool>("NeoFirst");
      this.addDuplicateStarSystem = config.GetValue<bool>("AddDuplicate");
      this.xOffset = config.GetValue<double>("XOffset");
      this.yOffset = config.GetValue<double>("YOffset");
      this.zOffset = config.GetValue<double>("ZOffset");
      this.vxOffset = config.GetValue<double>("VxOffset");
      this.vyOffset = config.GetValue<double>("VyOffset");
      this.vzOffset = config.GetValue<double>("VzOffset");
      this.maxBodies = config.GetValue<int>("MaxBodies");
      this.numOort = config.GetValue<int>("NumOort");
      this.planetInfoFileName = config.GetValue<string>("PlanetInfoFileName");
      this.slfOutFileName = config.GetValue<string>("SlfOutFileName");
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
    public void DoProcess()
    {
      this.logger.LogInformation("Starting Process");
      var count = ProcessUri();
      this.logger.LogInformation(" outputed {0}", count);
      this.logger.LogInformation("Finished Processing");
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
    public long ProcessUri()
    {
      var detailsPathName = this.planetInfoFileName;
      var fileOutName = this.slfOutFileName;
      Dictionary<string, BodyInfo> bodyInfos;
      bodyInfos = this.remoteBodyInfosSource.FetchBodyInfos(detailsPathName);

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
      this.logger.LogInformation("Using epoch {0}", epoch);

      // Add the Sun
      var bodyInfo = bodyInfos["SUN"];
      var pos = new Double4 { X = 0.0, Y = 0.0, Z = 0.0 };
      var vel = new Double4 { X = 0.0, Y = 0.0, Z = 0.0 };
      bodyInfo.StateVectors = new[] { pos, vel };
      bodyInfo.Epoch = epoch;

      // Add the planets
      bodyInfo = bodyInfos["Mercury".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Mercury(epoch, out var planetSemiMajorAxis);
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
      bodyInfo.Epoch = epoch;

      bodyInfo = bodyInfos["Venus".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Venus(epoch, out planetSemiMajorAxis);
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
      bodyInfo.AbsoluteMagnitude = -4.4;
      bodyInfo.Epoch = epoch;

      bodyInfo = bodyInfos["Earth".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Earth(epoch, out planetSemiMajorAxis);
      bodyInfo.AbsoluteMagnitude = -3.5;
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
      bodyInfo.Epoch = epoch;

      if (planets.IsSupported("Moon"))
      {
        bodyInfo = bodyInfos["Moon".ToUpperInvariant()];
        bodyInfo.StateVectors = planets.Moon(epoch, out planetSemiMajorAxis);
        bodyInfo.AbsoluteMagnitude = -3.5;
        bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
        bodyInfo.Epoch = epoch;
      }

      bodyInfo = bodyInfos["Mars".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Mars(epoch, out planetSemiMajorAxis);
      bodyInfo.AbsoluteMagnitude = -1.52;
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
      bodyInfo.Epoch = epoch;

      bodyInfo = bodyInfos["Jupiter".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Jupiter(epoch, out planetSemiMajorAxis);
      bodyInfo.AbsoluteMagnitude = -9.3;
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
      bodyInfo.Epoch = epoch;
      var jupiterSemiMajorAxis = bodyInfo.SemiMajorAxis;

      bodyInfo = bodyInfos["Saturn".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Saturn(epoch, out planetSemiMajorAxis);
      bodyInfo.AbsoluteMagnitude = -9.3;
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
      bodyInfo.Epoch = epoch;

      bodyInfo = bodyInfos["Uranus".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Uranus(epoch, out planetSemiMajorAxis);
      bodyInfo.AbsoluteMagnitude = -7.19;
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
      bodyInfo.Epoch = epoch;

      bodyInfo = bodyInfos["Neptune".ToUpperInvariant()];
      bodyInfo.StateVectors = planets.Neptune(epoch, out planetSemiMajorAxis);
      bodyInfo.AbsoluteMagnitude = -6.87;
      bodyInfo.SemiMajorAxis = planetSemiMajorAxis * Constants.AuInKm;
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
            this.logger.LogInformation("Oort Cloud {0}", bodyNum);
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
            MeanAnomaly = utilities.DegreeToRadian(random.NextDouble() * 360),
            SemiMajorAxis = Constants.AuInKm * distance,
            OrbitalEccentricity = random.NextDouble() * 0.3,
            InclinationToTheEcliptic = utilities.DegreeToRadian((random.NextDouble() * maxInc) - (maxInc / 2)),
            ArgumentOfPerihelion = utilities.DegreeToRadian(random.NextDouble() * 360),
            LongitudeOfTheAscendingNode = utilities.DegreeToRadian(random.NextDouble() * 360),
            AbsoluteMagnitude = 25.0,
            StateVectors = null,
            ReadIn = false,
            Type = type,
            UncertaintyParameter = 0
          };
          bodyInfos.Add(bodyInfo.Name, bodyInfo);
        }
      }

      if (this.addDuplicateStarSystem)
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
            XOffset = this.xOffset,
            YOffset = this.yOffset,
            ZOffset = this.zOffset,
            VxOffset = this.vxOffset,
            VyOffset = this.vyOffset,
            VzOffset = this.vzOffset,
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

      this.logger.LogInformation("Creating list of bodies");
      this.logger.LogInformation("Adding bodies with mass");

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

      this.logger.LogInformation("{0} bodies with mass", bodiesWithMass.Count);

      var bodiesWithoutMass = bodyInfos.Values.Where(x => x.Mass == 0.0 && x.UncertaintyParameter < 10).ToList();
      this.logger.LogInformation("Indexing {0} bodies with mass by type", bodiesWithoutMass.Count);

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

      this.logger.LogInformation("{0} types", bodysByType.Keys.Count);
      this.logger.LogInformation("Types are ");
      foreach (var type in bodysByType.Keys)
      {
        this.logger.LogInformation("{0,32} : {1}", type, bodysByType[type].Count);
      }

      this.logger.LogInformation("Creating a mixure of the bodies without mass");

      // mix them by building a list taking one of each type
      var bodyCount = bodiesWithoutMass.Count;
      var mixedBodyInfos = new LinkedList<BodyInfo>();

      if (this.neoFirst)
      {
        this.logger.LogInformation("Adding asteroids of type NEO and NEO! first");

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
          this.logger.LogInformation("{0} left to add", bodyCount);
        }
      }

      var bodiesInOrder = bodiesWithMass;
      bodiesInOrder.AddRange(mixedBodyInfos);
      if (maxBodies > 0)
      {
        bodiesInOrder = bodiesInOrder.Take(maxBodies).ToList();
      }

      this.logger.LogInformation("Writing to File {0}", fileOutName);
      var count = utilities.WriteFile(fileOutName, epoch, bodiesInOrder);
      return count;
    }
  }
}
