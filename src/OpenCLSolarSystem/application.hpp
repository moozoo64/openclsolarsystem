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
#include "global.hpp"

#ifndef FRAME_H_
#include "frame.hpp"
#endif // #ifndef FRAME_H_

#ifndef GLCANVAS_H_
#include "glcanvas.hpp"
#endif // #ifndef GLCANVAS_H_

#ifndef APPLICATION_H_
#define APPLICATION_H_

class Application : public wxApp
{
public:
  virtual bool OnInit(); /**< Initialize the application and create main window */
  virtual int OnExit();  /**< Clean up resources on application exit */
  Application();         /**< Constructor - initializes default settings */

private:
  // OpenGL rendering settings
  bool doubleBuffer; /**< Enable double buffering for smoother rendering */
  bool smooth;       /**< Enable smooth shading (anti-aliasing) */
  bool lighting;     /**< Enable OpenGL lighting effects */
  bool stereo;       /**< Enable stereoscopic 3D rendering */

  // OpenCL device settings
  char *desiredPlatform; /**< Target OpenCL platform name (NVIDIA/AMD/Intel) */
  bool useLastDevice;    /**< Use previously selected OpenCL device */
  bool tryForCPUFirst;   /**< Prefer CPU over GPU for computations */

  // Simulation parameters
  int numParticles; /**< Number of particles in the simulation (default: 2560) */
  int numGrav;      /**< Number of gravitational bodies (default: 16) */

  // Application components
  Frame *frame; /**< Main application window */

  // Command line parsing
  bool Args(int argc, wxChar **argv); /**< Process command line arguments */
};

#endif // #ifndef APPLICATION_H_
