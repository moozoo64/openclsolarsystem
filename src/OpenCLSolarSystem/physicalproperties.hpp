/*
  Copyright 2013-2025 Michael William Simmons

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#ifndef PHYSICALPROPERTIES_HPP
#define PHYSICALPROPERTIES_HPP

/**
 * @brief Physical properties of celestial bodies in the solar system
 *
 * Stores key physical characteristics used for:
 * - Gravitational calculations
 * - Visual representation
 * - Body identification
 */
class PhysicalProperties
{
public:
  /**
   * @brief Constructor - initializes properties to default values
   */
  PhysicalProperties();

  /**
   * @brief Destructor
   */
  ~PhysicalProperties();

  int Index; /**< Unique identifier for the celestial body */

  wxChar Name[32]; /**< Body name (planet, asteroid, etc.) */
                   /**< Maximum 31 characters + null terminator */

  double Mass; /**< Mass in solar masses */
               /**< 1.0 = one solar mass */

  double Radius; /**< Mean radius in astronomical units (AU) */
                 /**< 1.0 = distance from Earth to Sun */

  double AbsoluteMagnitude; /**< Absolute visual magnitude */
                            /**< Lower values indicate brighter objects */

  double RelativisticParameter; /**< Relativistic correction factor */
                                /**< Used for general relativity calculations */
};

#endif // PHYSICALPROPERTIES_HPP
