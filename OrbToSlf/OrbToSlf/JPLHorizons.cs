// --------------------------------------------------------------------------------------------------------------------
// <copyright file="JPLHorizons.cs" company="Michael William Simmons">
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
//   The double 4.
// </summary>
// 
namespace OrbToSlf
{
  using Microsoft.Extensions.Configuration;
  using Microsoft.Extensions.Logging;
  using System;
  using System.IO;
  using System.Net.Http;

#if NET48
    using System.Threading.Tasks;
#endif

  /// <summary>
  ///     Fetches the positions of the planets from the JPL Horizons system.
  /// </summary>
  public class JPLHorizons : IPlanets
  {
    private IConfiguration config;
    private ILogger<Utilities> logger;

    public JPLHorizons(IConfiguration config, ILogger<Utilities> logger)
    {
      this.config = config;
      this.logger = logger;
    }


    private Double4[] GetHorizonsData(string target, string time)
    {
      var position = new Double4();
      var velocity = new Double4();
      Uri UriHorizons = new Uri(this.config["UriHorizons"]);
      string url = UriHorizons + "?format=text&COMMAND='" + target + "'&OBJ_DATA='NO'&MAKE_EPHEM='YES'&EPHEM_TYPE='VECTOR'&CENTER='500@0'&VEC_TABLE='2'&TLIST='" + time + "'&TLIST_TYPE='JD'&REF_PLANE='F'";
      /*       string url = $"{UriHorizons}?format=text" +
                $"&COMMAND={Uri.EscapeDataString("'" + target + "'")}" +
                $"&OBJ_DATA={Uri.EscapeDataString("'NO'")}" +
                $"&MAKE_EPHEM={Uri.EscapeDataString("'YES'")}" +
                $"&EPHEM_TYPE={Uri.EscapeDataString("'VECTOR'")}" +
                $"&CENTER={Uri.EscapeDataString("'500@0'")}" +
                $"&VEC_TABLE={Uri.EscapeDataString("'2'")}" +
                $"&TLIST={Uri.EscapeDataString("'" + time + "'")}" +
                $"&TLIST_TYPE={Uri.EscapeDataString("'JD'")}" +
                $"&REF_PLANE={Uri.EscapeDataString("'F'")}"; */
      long lineCount = 0;
      var horizonsHttpClient = new HttpClient();
      var horizonsResponseTask = horizonsHttpClient.GetStreamAsync(url);
      horizonsResponseTask.Wait();
#if NET48
        if (horizonsResponseTask.Status != TaskStatus.RanToCompletion)
        {
            throw new Exception("Failed to fetch data");
        }
#else
      if (!horizonsResponseTask.IsCompletedSuccessfully)
      {
        throw new Exception("Failed to fetch data");
      }
#endif
      using (var horizonsStream = horizonsResponseTask.Result)
      using (var streamReader = new StreamReader(horizonsStream))
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
          if (line.StartsWith("$$SOE"))
          {
            processLine = true;
            continue;
          }

          if (line.StartsWith("$$EOE"))
          {
            processLine = false;
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

          if (line.StartsWith(" X ="))
          {
            position.X = double.Parse(line.Substring(4, 22).Trim()) * 1.0e3;
            position.Y = double.Parse(line.Substring(30, 22).Trim()) * 1.0e3;
            position.Z = double.Parse(line.Substring(56, 22).Trim()) * 1.0e3;
            position.W = 0.0;
            continue;
          }

          if (line.StartsWith(" VX="))
          {
            velocity.X = double.Parse(line.Substring(4, 22).Trim()) * 1.0e3;
            velocity.Y = double.Parse(line.Substring(30, 22).Trim()) * 1.0e3;
            velocity.Z = double.Parse(line.Substring(56, 22).Trim()) * 1.0e3;
            velocity.W = 0.0;
            continue;
          }
        }
      }
      return new[] { position, velocity };
    }

    public bool IsSupported(string planet)
    {
      return planet switch
      {
        "Earth" => true,
        "EMB" => true,
        "Moon" => true,
        "Jupiter" => true,
        "Mars" => true,
        "Mercury" => true,
        "Neptune" => true,
        "Pluto" => true,
        "Saturn" => true,
        "Uranus" => true,
        "Venus" => true,
        _ => false,
      };
    }
    public Double4[] Mercury(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 0.38709927 + 0.00000037 * tc;
      var state = GetHorizonsData("199", t.ToString());
      return state;
    }

    public Double4[] Venus(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 0.72333566 + 0.00000390 * tc;
      var state = GetHorizonsData("299", t.ToString());
      return state;
    }
    public Double4[] Earth(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 1.00000261 + 0.00000562 * tc;
      var state = GetHorizonsData("399", t.ToString());
      return state;
    }

    public Double4[] EMB(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 1.00000261 + 0.00000562 * tc;
      var state = GetHorizonsData("3", t.ToString());
      return state;
    }

    public Double4[] Moon(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 1.00000261 + 0.00000562 * tc;
      var state = GetHorizonsData("301", t.ToString());
      return state;
    }
    public Double4[] Mars(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 1.52371034 + 0.00001847 * tc;
      var state = GetHorizonsData("499", t.ToString());
      return state;
    }

    public Double4[] Jupiter(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 5.20288700 - 0.00011607 * tc;
      var state = GetHorizonsData("599", t.ToString());
      return state;
    }
    public Double4[] Saturn(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 9.53667594 - 0.00125060 * tc;
      var state = GetHorizonsData("699", t.ToString());
      return state;
    }

    public Double4[] Uranus(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 19.18916464 - 0.00196176 * tc;
      var state = GetHorizonsData("799", t.ToString());
      return state;
    }

    public Double4[] Neptune(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 30.06992276 + 0.00026291 * tc;
      var state = GetHorizonsData("899", t.ToString());
      return state;
    }

    public Double4[] Pluto(double t, out double semiMajorAxis)
    {
      var tc = (t - 2451545.0) / 36525;
      semiMajorAxis = 39.48211675 - 0.00031596 * tc;
      var state = GetHorizonsData("999", t.ToString());
      return state;
    }


  }
}