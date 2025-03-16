// --------------------------------------------------------------------------------------------------------------------
// <copyright file="CommandLineArgumentsParser.cs" company="Michael William Simmons">
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

namespace OrbToSlfConsole
{
    public class CommandLineArgumentsParser
    {
        public Dictionary<string, string> ParseCommandLineArguments(string[] args)
        {
            var commandLineConfig = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            var astorb = "False";
            var addDuplicate = "False";
            var neoFirst = "False";
            var maxBodies = "0";
            var numOort = "0";
            var xOffset = "0.0";
            var yOffset = "0.0";
            var vxOffset = "0.0";
            var vyOffset = "0.0";
            foreach (var arg in args)
            {
                switch (arg)
                {
                    case "ASTORB":
                        astorb = true.ToString();
                        break;
                    case "NEOFirst":
                        neoFirst = true.ToString();
                        break;
                    case "AddDuplicate":
                        addDuplicate = true.ToString();
                        break;
                    default:
                        if (arg.ToLowerInvariant().StartsWith("astrob"))
                        {
                          astorb = arg.Split('=')[1];
                        }
                        if (arg.ToLowerInvariant().StartsWith("neofirst"))
                        {
                          neoFirst = arg.Split('=')[1];
                        }
                        if (arg.ToLowerInvariant().StartsWith("addduplicate"))
                        {
                          addDuplicate = arg.Split('=')[1];
                        }
                        if (arg.ToLowerInvariant().StartsWith("maxbodies"))
                        {
                            maxBodies = arg.Split('=')[1];
                        }
                        else if (arg.ToLowerInvariant().StartsWith("numoort"))
                        {
                            numOort = arg.Split('=')[1];
                        }
                        else if (arg.ToLowerInvariant().StartsWith("xoffset"))
                        {
                            xOffset = arg.Split('=')[1];
                        }
                        else if (arg.ToLowerInvariant().StartsWith("yoffset"))
                        {
                            yOffset = arg.Split('=')[1];
                        }
                        else if (arg.ToLowerInvariant().StartsWith("vxoffset"))
                        {
                            vxOffset = arg.Split('=')[1];
                        }
                        else if (arg.ToLowerInvariant().StartsWith("vyoffset"))
                        {
                            vyOffset = arg.Split('=')[1];
                        }
                        else
                        {
                             Console.WriteLine("Unknown argument {0}", arg);
                        }
                        break;
                }
            }

            commandLineConfig.Add("MaxBodies", maxBodies);
            commandLineConfig.Add("NumOort", numOort);
            commandLineConfig.Add("Astorb", astorb);
            commandLineConfig.Add("NEOFirst", neoFirst.ToString());
            commandLineConfig.Add("AddDuplicate", addDuplicate.ToString());
            commandLineConfig.Add("XOffset", xOffset);
            commandLineConfig.Add("YOffset", yOffset);
            commandLineConfig.Add("VXOffset", vxOffset);
            commandLineConfig.Add("VYOffset", vyOffset);

            return commandLineConfig;
        }
    }
}
