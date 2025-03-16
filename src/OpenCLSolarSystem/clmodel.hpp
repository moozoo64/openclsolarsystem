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
#ifndef CLMODEL_H
#define CLMODEL_H

/**
 * CLModel - OpenCL memory buffer and kernel management
 */
class CLModel
{
public:
  // Constructor/Destructor
  CLModel();
  ~CLModel();

  // Public methods
  void CompileProgramAndCreateKernels();
  bool FindDeviceAndCreateContext(cl_uint desiredDeviceVendorId, cl_device_type deviceType, char *desiredPlatformName);
  void CreateBufferObjects(GLuint *vbo, int numParticles, int numGrav);
  void SetInitalState(cl_double4 *initalPositions, cl_double4 *initalVelocities);
  void ReadToInitialState(cl_double4 *initalPositions, cl_double4 *initalVelocities);
  void SetKernelArgumentsAndGroupSize();
  void ExecuteKernels();
  int CleanUpCL();
  void UpdateDisplay();
  void RequestUpdate();
  wxString ErrorMessage(cl_int status);

  // Device/Platform Information
  wxString *deviceName;               /**< Name of selected OpenCL device */
  wxString *deviceCLVersion;          /**< OpenCL version supported by device */
  wxString *platformName;             /**< Name of OpenCL platform */
  wxString *adamsBashforthKernelName; /**< Name of Adams-Bashforth integration kernel */
  wxString *adamsMoultonKernelName;   /**< Name of Adams-Moulton integration kernel */
  wxString *accelerationKernelName;   /**< Name of acceleration computation kernel */
  double deviceCLVersionNumber;       /**< Numeric OpenCL version (e.g., 2.0) */

  // Simulation Parameters
  cl_double delT;         /**< Integration timestep in seconds */
  cl_double espSqr;       /**< Gravitational softening factor squared */
  cl_double julianDate;   /**< Current simulation time in Julian Date */
  cl_double time;         /**< Current simulation time in seconds */
  cl_int numGrav;         /**< Number of gravitational bodies */
  cl_int maxNumGrav;      /**< Maximum allowed gravitational bodies */
  cl_int maxNumParticles; /**< Maximum allowed particles */
  cl_int step;            /**< Current integration step number */
  cl_int centerBody;      /**< Index of central body (usually Sun) */
  cl_uint deviceVendorId; /**< OpenCL device vendor ID */

private:
  // OpenCL Resources
  cl_device_id deviceId;         /**< Selected OpenCL device */
  cl_context context;            /**< OpenCL context */
  cl_device_id *devices;         /**< List of available OpenCL devices */
  cl_command_queue commandQueue; /**< Command queue for kernel execution */
  cl_program program;            /**< Compiled OpenCL program */

  // OpenCL Kernels
  cl_kernel accKernel;            /**< Acceleration computation kernel */
  cl_kernel adamsBashforthKernel; /**< Adams-Bashforth integration kernel */
  cl_kernel adamsMoultonKernel;   /**< Adams-Moulton integration kernel */
  cl_kernel startupKernel;        /**< Initialization kernel */
  cl_kernel copyToDisplayKernel;  /**< Display buffer update kernel */

  // Device Capabilities
  size_t maxWorkGroupSize;        /**< Maximum work-items per work-group */
  cl_uint maxDimensions;          /**< Maximum dimensions for work-groups */
  size_t *maxWorkItemSizes;       /**< Maximum size per dimension */
  cl_ulong totalLocalMemory;      /**< Available local memory per work-group */
  cl_ulong maxConstantBufferSize; /**< Maximum constant buffer size */
  cl_ulong globalMemorySize;      /**< Total available global memory */
  cl_ulong maxMemoryAlloc;        /**< Maximum single allocation size */

  // Kernel Work Group Sizes
  size_t accKernelWorkGroupSize;            /**< Optimal work-group size for acc kernel */
  size_t adamsBashforthKernelWorkGroupSize; /**< Optimal size for Adams-Bashforth */
  size_t adamsMoultonKernelWorkGroupSize;   /**< Optimal size for Adams-Moulton */
  size_t startupKernelWorkGroupSize;        /**< Optimal size for startup kernel */
  size_t copyToDisplayKernelWorkGroupSize;  /**< Optimal size for display copy */
  size_t groupSize;                         /**< Current work-group size in use */

  // Simulation State
  cl_int numParticles; /**< Current number of particles */
  cl_int stage;        /**< Current integration stage */
  cl_int numStages;    /**< Total integration stages */

  // OpenCL memory buffers
  cl_mem dispPos;    // [numParticles][4] - Display positions (GL shared buffer)
  cl_mem currPos;    // [numParticles][4] - Current positions
  cl_mem gravPos;    // [numGrav][4] - Gravitational body positions (constant memory)
  cl_mem currVel;    // [numParticles][4] - Current velocities
  cl_mem newPos;     // [numParticles][4] - Next step positions
  cl_mem newVel;     // [numParticles][4] - Next step velocities
  cl_mem acc;        // [numParticles][4] - Computed accelerations
  cl_mem velHistory; // [numParticles][16][4] - Velocity history ring buffer
  cl_mem accHistory; // [numParticles][16][4] - Acceleration history ring buffer
  cl_mem posLast;    // [numParticles][4] - Previous positions for Adams-Moulton
  cl_mem velLast;    // [numParticles][4] - Previous velocities for Adams-Moulton

  // Dimensions explanation:
  // [numParticles] - Number of bodies in simulation
  // [16] - Ring buffer size for multi-step integration
  // [4] - Vector components (x,y,z,w) where w stores:
  //   - For positions: mass
  //   - For velocities: relativistic factor
  //   - For accelerations: computed acceleration magnitude

  // State Flags
  bool updateDisplay;     /**< Flag to trigger display update */
  bool initialisedOk;     /**< Initialization success status */
  bool gotKhrFp64;        /**< KHR double precision support */
  bool gotAmdFp64;        /**< AMD double precision support */
  bool gotKhrGlSharing;   /**< KHR OpenGL sharing support */
  bool gotAppleGlSharing; /**< Apple OpenGL sharing support */

  // Private methods
  void SetAdamsKernelArgs(cl_kernel adamsKernel);
  bool IsDeviceSuitable(cl_device_id deviceIdToCheck);
};

#endif // CLMODEL_H
