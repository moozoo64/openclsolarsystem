/*
	Copyright 2013 Michael William Simmons

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

class CLModel
{
	public:
		CLModel();
		~CLModel();
		int CompileProgramAndCreateKernels();
		int ChooseAndCreateContext(char *desiredPlatformName,bool preferCpu);
		int CreateBufferObjects(GLuint *vbo,int numParticles, int numGrav);
		int SetInitalState(cl_double4 *initalPositions, cl_double4 *initalVelocities);
		int ReadToInitialState(cl_double4 *initalPositions, cl_double4 *initalVelocities);
		int SetKernelArgumentsAndGroupSize();
		int ExecuteKernels();
		int CleanUpCL();
		int UpdateDisplay();
		void RequestUpdate();
		wxString ErrorMessage(cl_int status);
		wxString *deviceName;
		wxString *platformName;
		wxString *adamsBashforthKernelName; //Bashforth
		wxString *adamsMoultonKernelName;
		wxString *accelerationKernelName;
		cl_double delT;                      /**< dT (timestep) */
		cl_double espSqr;                    /**< Softening Factor*/
		cl_double julianDate;
		cl_double time;
		cl_int numGrav;
		cl_int maxNumGrav;
		cl_int step;
		cl_int centerBody;

	private:
		cl_device_id deviceId;
		cl_context context;                 /**< CL context */
		cl_device_id *devices;              /**< CL device list */
		cl_command_queue commandQueue;      /**< CL command queue */
		cl_program program;                 /**< CL program */
		cl_kernel accKernel;                /**< CL kernel */
		cl_kernel adamsBashforthKernel;
		cl_kernel adamsMoultonKernel;
		cl_kernel startupKernel;
		cl_kernel copyToDisplayKernel;
		
		size_t maxWorkGroupSize;            /**< Max allowed work-items in a group */
		cl_uint maxDimensions;              /**< Max group dimensions allowed */
		size_t* maxWorkItemSizes;           /**< Max work-items sizes in each dimensions */
		cl_ulong totalLocalMemory;          /**< Max local memory allowed */
		cl_ulong maxConstantBufferSize;
		size_t accKernelWorkGroupSize;      /**< Group size returned by kernel */
		size_t adamsBashforthKernelWorkGroupSize;    /**< Group size returned by kernel */
		size_t adamsMoultonKernelWorkGroupSize; 
		size_t startupKernelWorkGroupSize;    /**< Group size returned by kernel */
		size_t copyToDisplayKernelWorkGroupSize;    /**< Group size returned by kernel */
		size_t groupSize;                   /**< Work-Group size */

		cl_int numParticles;
		cl_int stage;
		cl_int numStages;

		cl_mem dispPos;
		cl_mem currPos;						/**< Position of partciles */
		cl_mem currVel;						/**< Velocity of partciles */
		cl_mem gravPos;
		cl_mem acc;
		cl_mem newPos;						/**< Position of partciles */
		cl_mem newVel;						/**< Velocity of partciles */
		cl_mem posLast;
		cl_mem velLast;
		cl_mem velHistory;
		cl_mem accHistory;

		bool updateDisplay;
		bool initialisedOk;
		bool gotKhrFp64;
		bool gotAmdFp64;
		bool gotKhrGlSharing;
		bool gotAppleGlSharing;
		int SetAdamsKernelArgs(cl_kernel adamsKernel);
};

#endif // CLMODEL_H
