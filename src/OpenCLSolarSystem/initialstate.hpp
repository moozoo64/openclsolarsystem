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
#ifndef INITIALSTATE_HPP
#define INITIALSTATE_HPP

#ifndef GLCANVAS_H_
#include "glcanvas.hpp"
#endif // #ifndef GLCANVAS_H_

#ifndef CLMODEL_H
#include "clmodel.hpp"
#endif // #ifndef CLMODEL_H

#include "physicalproperties.hpp"

/**
 * @brief Manages initial configuration and state for solar system simulation
 *
 * Handles:
 * - Particle positions and velocities
 * - Physical properties of celestial bodies
 * - Color data for visualization
 * - Import/export of simulation states
 * - Random initial configuration generation
 */
class InitialState
{
private:
  /**
   * @brief XORShift random number generator
   * @return Pseudo-random unsigned long value
   * Implementation from George Marsaglia's Xorshift RNG
   */
  unsigned long xor128();

public:
  // Simulation Parameters
  int initialNumParticles; /**< Total number of particles in simulation */
  int initialNumGrav;      /**< Number of gravitational bodies */

  // State Vectors
  cl_double4 *initialVelocities; /**< [numParticles][4] Initial velocity vectors (x,y,z,relativisticFactor) */
  cl_double4 *initialPositions;  /**< [numParticles][4] Initial position vectors (x,y,z,mass) */

  // Visualization Data
  GLubyte *initialColorData; /**< [numParticles][4] RGBA color values for each particle */

  // Physical Properties
  PhysicalProperties *physicalProperties; /**< [numParticles] Physical parameters for each body */

  // Time Reference
  cl_double initialJulianDate; /**< Initial simulation time in Julian Date */

  /**
   * @brief Default constructor - initializes empty state
   */
  InitialState();

  /**
   * @brief Destructor - releases allocated memory
   */
  ~InitialState();

  /**
   * @brief Deallocates all dynamic memory
   */
  void DeAllocate();

  /**
   * @brief Allocates memory for simulation state
   * @return true if allocation successful
   */
  bool Allocate();

  /**
   * @brief Sets default colors based on celestial body types
   * Maps different categories (planets, asteroids, comets, etc.) to specific colors
   */
  void SetDefaultBodyColours();

  /**
   * @brief Imports state from Solex SLF format file
   * @param fileName Path to SLF file
   * @return true if import successful
   */
  bool ImportSLF(wxString fileName);

  /**
   * @brief Exports current state to Solex SLF format
   * @param fileName Target file path
   * @return true if export successful
   */
  bool ExportSLF(wxString fileName);

  /**
   * @brief Saves state to binary file format
   * @param fileName Target file path
   * @return true if save successful
   */
  bool SaveInitialState(wxString fileName);

  /**
   * @brief Loads state from binary file format
   * @param fileName Source file path
   * @return true if load successful
   */
  bool LoadInitialState(wxString fileName);

  /**
   * @brief Generates random initial configuration
   * Creates test data when no initial state file exists
   * @return true if generation successful
   */
  bool CreateRandomInitialConfig();
};

#endif // INITIALSTATE_HPP
