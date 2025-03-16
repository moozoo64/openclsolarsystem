// --------------------------------------------------------------------------------------------------------------------
// <copyright file="IPlanets.cs" company="Michael William Simmons">
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
//   The elliptic motion.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace OrbToSlf
{
  public interface IPlanets
  {
    bool IsSupported(string planet);
    Double4[] Earth(double t, out double semiMajorAxis);
    Double4[] EMB(double t, out double semiMajorAxis);
    Double4[] Moon(double t, out double semiMajorAxis);
    Double4[] Jupiter(double t, out double semiMajorAxis);
    Double4[] Mars(double t, out double semiMajorAxis);
    Double4[] Mercury(double t, out double semiMajorAxis);
    Double4[] Neptune(double t, out double semiMajorAxis);
    Double4[] Pluto(double t, out double semiMajorAxis);
    Double4[] Saturn(double t, out double semiMajorAxis);
    Double4[] Uranus(double t, out double semiMajorAxis);
    Double4[] Venus(double t, out double semiMajorAxis);
  }
}
