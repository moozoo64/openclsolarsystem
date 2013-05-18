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
#include "global.hpp"
#include "clmodel.hpp"

CLModel::CLModel()
{
	this->deviceId = NULL;
	this->context = NULL;
	this->devices = NULL;
	this->commandQueue = NULL;
	this->program = NULL;
	this->accKernel = NULL;
	this->adamsBashforthKernel = NULL;
	this->adamsMoultonKernel = NULL;
	this->startupKernel = NULL;

	this->maxWorkGroupSize = 0;
	this->maxDimensions = 0;
	this->maxWorkItemSizes = NULL;
	this->totalLocalMemory = 0;
	this->accKernelWorkGroupSize = 0;
	this->adamsBashforthKernelWorkGroupSize = 0;
	this->adamsMoultonKernelWorkGroupSize = 0;
	this->startupKernelWorkGroupSize = 0;
	this->groupSize = 64;

	this->dispPos = NULL;
	this->currPos = NULL;
	this->gravPos = NULL;
	this->currVel = NULL;
	this->newPos = NULL;
	this->newVel= NULL;
	this->acc= NULL;
	this->velHistory = NULL;
	this->accHistory = NULL;

	this->updateDisplay = false;
	this->initialisedOk = false;
	this->delT = 15*60;
	this->espSqr = 0.000001f;
	this->time =0.0f;
	this->julianDate =0.0f;
	this->numGrav = 16;
	this->centerBody = 0;
	this->numStages = 1;
	this->stage = this->numStages;
	this->adamsBashforthKernelName = new wxString("adamsBashforth4");
	this->adamsMoultonKernelName = new wxString("adamsMoulton3");
	this->accelerationKernelName = new wxString("newtonian");
}

CLModel::~CLModel()
{
	this->initialisedOk=false;
	this->CleanUpCL();
	wxLogDebug(wxT("CLModel Destructor"));
}

int CLModel::InitCL(GLuint *vbo,char *desiredPlatformName,bool preferCpu,int numParticles, int numGrav)
{
	wxLogDebug(wxT("CLModel::InitCL"));
	cl_int status = CL_SUCCESS;
	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	this->step = 0;
	this->numGrav = numGrav;
	this->numParticles = (cl_int)((numParticles / this->groupSize) * this->groupSize);;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetPlatformIDs failed to get number of platforms %s"),this->ErrorMessage(status));
		return status;
	}

	if (numPlatforms <= 0)
	{
		wxLogError(wxT("number of platforms is %d"),numPlatforms);
		return CL_DEVICE_NOT_FOUND;
	}

	wxLogDebug(wxT("Found %d platforms"),numPlatforms);

	cl_platform_id* platforms = new cl_platform_id[numPlatforms];

	status = clGetPlatformIDs(numPlatforms, platforms, NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetPlatformIDs failed to get platforms %s"),this->ErrorMessage(status));
		return status;
	}

	char platformName[1024];
	for (unsigned i = 0; i < numPlatforms; ++i)
	{
		status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformName), platformName, NULL);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clGetPlatformInfo failed to get CL_PLATFORM_VENDOR %s"),this->ErrorMessage(status));
			return status;
		}

		//wxLogDebug(wxT("  %s"),platformName);

		if (!strcmp(platformName, desiredPlatformName))
		{
			platform = platforms[i];
			wxLogDebug(wxT("Found Desired platform"));
			wxLogDebug(wxT("Using platform %s"),platformName);
			this->platformName = new wxString(platformName,wxConvUTF8);
			break;
		}
	}

	if(platform == NULL)
	{
		wxLogError(wxT("Desired Platform not found"));
		return CL_DEVICE_NOT_FOUND;
	}

	cl_uint numberOfDevices = 0;
	status = clGetDeviceIDs(platform,CL_DEVICE_TYPE_ALL,0,NULL,&numberOfDevices);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetDeviceIDs failed to get number of devices %s"),this->ErrorMessage(status));
		return status;
	}

	wxLogDebug(wxT("platform has %d devices"),numberOfDevices);

// Now find the device
	cl_device_id* deviceIds = new cl_device_id[numberOfDevices];
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numberOfDevices, deviceIds, NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetDeviceIDs failed to get devices %s"),this->ErrorMessage(status));
		return status;
	}
	
	bool foundDevice = false;
	for (unsigned j = 0; j < numberOfDevices; ++j)
	{
		cl_uint preferedVectorWidthDouble;
		status = clGetDeviceInfo(deviceIds[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(preferedVectorWidthDouble), &preferedVectorWidthDouble, NULL);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE %s"),this->ErrorMessage(status));
			return status;
		}
		// skip device if it does not support double precision
		if(preferedVectorWidthDouble == 0)
		{
			continue;
		}
		
		cl_device_type deviceType;
		status = clGetDeviceInfo(deviceIds[j], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, NULL);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_TYPE %s"),this->ErrorMessage(status));
			return status;
		}
		
		if(preferCpu && deviceType != CL_DEVICE_TYPE_CPU)
		{
			continue;
		}
		wxLogDebug(wxT("Found CPU Device as requested"));
		
		this->deviceId = deviceIds[j];
		foundDevice = true;
		break;
	}
	
	if(!foundDevice)
	{
		// ignore preferCpu
		for (unsigned j = 0; j < numberOfDevices; ++j)
		{
			//check if device supports double precision
			cl_uint preferedVectorWidthDouble;
			status = clGetDeviceInfo(deviceIds[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(preferedVectorWidthDouble), &preferedVectorWidthDouble, NULL);
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_TYPE %s"),this->ErrorMessage(status));
				return status;
			}
			// skip device if it does not support double precision
			if(preferedVectorWidthDouble == 0)
			{
				continue;
			}
			
			this->deviceId = deviceIds[j];
			foundDevice = true;
			break;
		}
	}
	
	if(foundDevice)
	{
		char deviceName[1024];
		status = clGetDeviceInfo(this->deviceId, CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_NAME %s"),this->ErrorMessage(status));
			return status;
		}
		this->deviceName = new wxString(deviceName,wxConvUTF8);
	}
	else
	{
		wxLogError(wxT("Failed to find a suitable OpenCL device"));
		return -1;
	}

// see http://www.dyn-lab.com/articles/cl-gl.html
#ifdef _WIN32
	cl_context_properties properties[] = {
	   CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
	   CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
	   CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
	   0};
#endif

#ifdef __linux__
	cl_context_properties properties[] = {
	   CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
	   CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
	   CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
	   0};
#endif

#ifdef __APPLE__
	CGLContextObj glContext = CGLGetCurrentContext();
	CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);

	cl_context_properties properties[] = {
	   CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,(cl_context_properties)shareGroup,
	   0};
#endif
									
	this->context = clCreateContext( properties,numberOfDevices,deviceIds,NULL,NULL,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateContext failed %s"),this->ErrorMessage(status));
		return status;
	}

	size_t contextDeviceListSize;

	status = clGetContextInfo(this->context,CL_CONTEXT_DEVICES,0,NULL,&contextDeviceListSize);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetContextInfo failed to get CL_CONTEXT_DEVICES size %s"),this->ErrorMessage(status));
		return status;
	}

	int contextNumberOfDevices = (int)(contextDeviceListSize / sizeof(cl_device_id));
	cl_device_id* contextDeviceIds = new cl_device_id[contextNumberOfDevices];
	if(contextDeviceIds == NULL)
	{
		wxLogError(wxT("Failed to allocate memory for devices %d"),contextNumberOfDevices);
		return -1;
	}

	status = clGetContextInfo(this->context, CL_CONTEXT_DEVICES, contextDeviceListSize,contextDeviceIds,NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetContextInfo failed to get CL_CONTEXT_DEVICES %s"),this->ErrorMessage(status));
		return status;
	}

	bool deviceOk = false;
	for (unsigned j = 0; j < numberOfDevices; ++j)
	{
		if(this->deviceId == contextDeviceIds[j])
		{
			deviceOk = true;
			break;
		}
	}

	if(!deviceOk)
	{
		wxLogError(wxT("desired device is not suitable"));
		return -1;
	}

	this->commandQueue = clCreateCommandQueue(this->context,this->deviceId,0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateCommandQueue failed %s"),this->ErrorMessage(status));
		return status;
	}

	status = clGetDeviceInfo(this->deviceId,CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),(void*)&this->maxWorkGroupSize,NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_WORK_GROUP_SIZE %s"),this->ErrorMessage(status));
		return status;
	}
	
	if(this->groupSize > this->maxWorkGroupSize)
	{
		this->groupSize = this->maxWorkGroupSize;
	}

	status = clGetDeviceInfo(this->deviceId,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,sizeof(cl_uint),(void*)&this->maxDimensions,NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS %s"),this->ErrorMessage(status));
		return status;
	}

	this->maxWorkItemSizes = (size_t*)malloc(maxDimensions * sizeof(size_t));

	status = clGetDeviceInfo(this->deviceId,CL_DEVICE_MAX_WORK_ITEM_SIZES,sizeof(size_t) * maxDimensions,(void*)this->maxWorkItemSizes,NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS %s"),this->ErrorMessage(status));
		return status;
	}

	status = clGetDeviceInfo(this->deviceId,CL_DEVICE_LOCAL_MEM_SIZE,sizeof(cl_ulong),(void *)&this->totalLocalMemory,	NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_LOCAL_MEM_SIZE %s"),this->ErrorMessage(status));
		return status;
	}

	status = clGetDeviceInfo(this->deviceId,CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,sizeof(cl_ulong),(void *)&this->maxConstantBufferSize,	NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->maxNumGrav = this->maxConstantBufferSize/sizeof(cl_double4);
	wxLogDebug(wxT("max grav particles %d"),this->maxNumGrav);
	if(this->numGrav > this->maxNumGrav)
	{
		this->numGrav = this->maxNumGrav;
	}

	// Create cl_mem objects
	this->dispPos = clCreateFromGLBuffer(this->context, CL_MEM_WRITE_ONLY, vbo[0], &status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateFromGLBuffer failed to create cl_mem object for GL vertex buffer object %s"),this->ErrorMessage(status));
		return status;
	}

	this->currPos = clCreateBuffer(this->context,CL_MEM_READ_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for currPos %s"),this->ErrorMessage(status));
		return status;
	}

	this->newPos = clCreateBuffer(this->context,CL_MEM_WRITE_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for newPos %s"),this->ErrorMessage(status));
		return status;
	}

	this->gravPos = clCreateBuffer(this->context,CL_MEM_READ_ONLY,this->numGrav * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for gravPos %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->currVel = clCreateBuffer(this->context,CL_MEM_READ_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for currVel %s"),this->ErrorMessage(status));
		return status;
	}

	this->newVel = clCreateBuffer(this->context,CL_MEM_WRITE_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for newVel %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->acc = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for acc %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->posLast = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 1 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for posLast %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->velLast = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 1 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for velLast %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->velHistory = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 16 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for velHistory %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->accHistory = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 16 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for accHistory %s"),this->ErrorMessage(status));
		return status;
	}
   
	wxString nbodySource;
	wxFFile nbodyFile("Adams.cl", "r");
	if(!nbodyFile.IsOpened())
	{
		wxLogError(wxT("Failed to read Adams.cl"));
		return -1;
	}
	
	nbodyFile.ReadAll(&nbodySource,wxConvISO8859_1);
	const char * source = (const char *)nbodySource;
	size_t sourceSize[] = {strlen(source)};

	this->program = clCreateProgramWithSource(this->context,1,&source,sourceSize,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateProgramWithSource failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	const char * options = "-cl-mad-enable -cl-fast-relaxed-math";// "-cl-mad-enable -cl-fast-relaxed-math -cl-nv-verbose ";
	status = clBuildProgram(this->program,1,&this->deviceId,options,NULL,NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clBuildProgram failed %s"),this->ErrorMessage(status));
		if (status == CL_BUILD_PROGRAM_FAILURE) {
			// Determine the size of the log
			size_t log_size;
			clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

			// Allocate memory for the log
			char *log = (char *) malloc(log_size);

			// Get the log
			clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
			wxLogError(log);
		}
		return status;
	}
	
#ifdef __WXDEBUG__	
	size_t log_size;
	status = clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetProgramBuildInfo failed %s"),this->ErrorMessage(status));
		return status;
	}
	// Allocate memory for the log
	char *log = (char *) malloc(log_size);

	// Get the log
	status = clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetProgramBuildInfo failed %s"),this->ErrorMessage(status));
		return status;
	}
	wxLogDebug(log);
#endif
	
	this->accKernel = clCreateKernel(this->program,this->accelerationKernelName->c_str(),&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateKernel failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->startupKernel = clCreateKernel(this->program,"adamsStartup",&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateKernel adamsStartup failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	wxLogDebug(wxT("Using adamsBashforthKernel %s"),this->adamsBashforthKernelName->c_str());
	this->adamsBashforthKernel = clCreateKernel(this->program,this->adamsBashforthKernelName->c_str(),&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateKernel adamsBashforthKernel failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	wxLogDebug(wxT("Using adamsKernel %s"),this->adamsMoultonKernelName->c_str());
	this->adamsMoultonKernel = clCreateKernel(this->program,this->adamsMoultonKernelName->c_str(),&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateKernel adamsMoultonKernel failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->copyToDisplayKernel = clCreateKernel(this->program,"copyToDisplay",&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateKernel copyToDisplay failed %s"),this->ErrorMessage(status));
		return status;
	}

	delete[] contextDeviceIds;
	delete[] deviceIds;
	delete[] platforms;
	wxLogDebug(wxT("CLModel:InitCL Done"));

	this->initialisedOk = true;
	return CL_SUCCESS;
}

int CLModel::ExecuteKernel()
{
	wxLogDebug(wxT("CLModel:ExecuteKernel Start"));
	cl_int status = CL_SUCCESS;

	if(!this->initialisedOk)
	{
		wxLogDebug(wxT("Aborted CLModel failed to Initialise"));
		return -1;
	}

	//TODO this should be per kernel. Not the lowest that works for all of them
	size_t globalThreads[] = {(size_t)this->numParticles};
	size_t localThreads[] = {this->groupSize};

	if(localThreads[0] > maxWorkItemSizes[0] || localThreads[0] > maxWorkGroupSize)
	{
		wxLogError(wxT("groupSize %d is greater than maxWorkItemSizes %d or maxWorkGroupSize %d"),this->groupSize,maxWorkItemSizes[0],maxWorkGroupSize);
		return -1;
	}

	// Execute acceleration kernel on given device
	cl_event  eventND[1];
	status = clEnqueueNDRangeKernel(this->commandQueue, this->accKernel, 1, NULL, globalThreads, localThreads, 0, 0, eventND );
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueNDRangeKernel failed %s"),this->ErrorMessage(status));
		return status;
	}

	status = clWaitForEvents(1, eventND);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clWaitForEvents failed %s"),this->ErrorMessage(status));
		return status;
	}
	clReleaseEvent(eventND[0]);

	status = clFinish(this->commandQueue);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clFinish failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	// for the first 16 steps we call the startupKernel. This populates the 16 element ring buffer
	if (this->step < 16)
	{	
		wxLogDebug(wxT("CLModel:Using startupKernel"));
		
		status = clSetKernelArg(this->startupKernel,6,sizeof(cl_int),(void *)&this->stage);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clSetKernelArg 6 startupKernel failed for stage %s"),this->ErrorMessage(status));
			return status;
		}
		
		status = clSetKernelArg(this->startupKernel,7,sizeof(cl_int),(void *)&this->step);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clSetKernelArg 7 startupKernel failed for step %s"),this->ErrorMessage(status));
			return status;
		}
		
		// use reuse event?
		status = clEnqueueNDRangeKernel(this->commandQueue, this->startupKernel, 1, NULL, globalThreads, localThreads, 0, 0, eventND );
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clEnqueueNDRangeKernel startupKernel failed %s"),this->ErrorMessage(status));
			return status;
		}
	}
	else
	{
		if(stage == 1)
		{
			// Update arguments for the AdamsBashfordKernel then Execute it
			status = clSetKernelArg(this->adamsBashforthKernel,6,sizeof(cl_int),(void *)&this->stage);
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clSetKernelArg 6 adamsBashforthKernel failed for stage %s"),this->ErrorMessage(status));
				return status;
			}
			
			status = clSetKernelArg(this->adamsBashforthKernel,7,sizeof(cl_int),(void *)&this->step);
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clSetKernelArg 7 adamsBashforthKernel failed for step %s"),this->ErrorMessage(status));
				return status;
			}
			
			// use reuse event?
			status = clEnqueueNDRangeKernel(this->commandQueue, this->adamsBashforthKernel, 1, NULL, globalThreads, localThreads, 0, 0, eventND );
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clEnqueueNDRangeKernel adamsBashforthKernel failed %s"),this->ErrorMessage(status));
				return status;
			}
		}
		else
		{
			// Update arguments for the adamsMoultonKernel then Execute it
			status = clSetKernelArg(this->adamsMoultonKernel,6,sizeof(cl_int),(void *)&this->stage);
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clSetKernelArg 6 adamsMoultonKernel failed for stage %s"),this->ErrorMessage(status));
				return status;
			}
			
			status = clSetKernelArg(this->adamsMoultonKernel,7,sizeof(cl_int),(void *)&this->step);
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clSetKernelArg 7 adamsMoultonKernel failed for step %s"),this->ErrorMessage(status));
				return status;
			}
			
			// use reuse event?
			status = clEnqueueNDRangeKernel(this->commandQueue, this->adamsMoultonKernel, 1, NULL, globalThreads, localThreads, 0, 0, eventND );
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clEnqueueNDRangeKernel failed %s"),this->ErrorMessage(status));
				return status;
			}
		}
	}

	status = clWaitForEvents(1, eventND);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clWaitForEvents failed %s"),this->ErrorMessage(status));
		return status;
	}
	clReleaseEvent(eventND[0]);
		
	status = clFinish(this->commandQueue);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clFinish failed %s"),this->ErrorMessage(status));
		return status;
	}

	// Copy new positions to old
	status = clEnqueueCopyBuffer(commandQueue,this->newPos,this->currPos,0,0,sizeof(cl_double4) * this->numParticles,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueCopyBuffer newPos to currPos failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	// Copy new positions to gravPos
	status = clEnqueueCopyBuffer(commandQueue,this->newPos,this->gravPos,0,0,sizeof(cl_double4) * this->numGrav,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueCopyBuffer newPos to currPos failed %s"),this->ErrorMessage(status));
		return status;
	}

	status = clEnqueueCopyBuffer(commandQueue,this->newVel,this->currVel,0,0,sizeof(cl_double4) * this->numParticles,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueCopyBuffer newVel to currVel failed %s"),this->ErrorMessage(status));
		return status;
	}

	status = clFinish(this->commandQueue);
	if ( status != CL_SUCCESS)
	{
		wxLogError(wxT("clFinish failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	this->stage = this->stage - 1;
	if(this->stage < 0)
	{
		this->stage = this->numStages;
		this->time += this->delT;
		this->step++;
		
		if ( this->updateDisplay)
		{
			this->updateDisplay = !this->updateDisplay;
			this->UpdateDisplay();
		}
	}
	wxLogDebug(wxT("CLModel:ExecuteKernel Done"));
	return 1;
}

// Aquire the GL points buffer and then copy the positions to it
int CLModel::UpdateDisplay()
{
	wxLogDebug(wxT("CLModel:UpdateDisplay Started"));
	cl_int status = CL_SUCCESS;

	if(!this->initialisedOk)
	{
		wxLogDebug(wxT("Aborted CLModel failed to Initialise"));
		return -1;
	}

	//TODO this should be per kernel. Not the lowest that works for all of them
	size_t globalThreads[] = {(size_t)this->numParticles};
	size_t localThreads[] = {this->groupSize};

	if(localThreads[0] > maxWorkItemSizes[0] || localThreads[0] > maxWorkGroupSize)
	{
		wxLogError(wxT("groupSize %d is greater than maxWorkItemSizes %d or maxWorkGroupSize %d"),this->groupSize,maxWorkItemSizes[0],maxWorkGroupSize);
		return -1;
	}

	// Execute acceleration kernel on given device
	//glFinish();

	cl_event  eventND[1];
	// Acquire GL buffer
	status = clEnqueueAcquireGLObjects(this->commandQueue, 1, &this->dispPos, 0, 0, eventND);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueAcquireGLObjects failed to acquire dispPos %s"),this->ErrorMessage(status));
		return status;
	}

	status = clWaitForEvents(1, eventND);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clWaitForEvents failed %s"),this->ErrorMessage(status));
		return status;
	}
	clReleaseEvent(eventND[0]);

	// Copy new positions to the display vertex buffer
	/*status = clEnqueueCopyBuffer(commandQueue,this->currPos,this->dispPos,0,0,sizeof(cl_double4) * this->numParticles,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueCopyBuffer newPos to dispPos failed %s"),this->ErrorMessage(status));
		return status;
	}
	 * */
	
	status = clSetKernelArg(this->copyToDisplayKernel,3,sizeof(cl_int),(void*)&this->centerBody);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 3 failed for centerBody %s"),this->ErrorMessage(status));
		return status;
	}
	
	// use reuse event?
	status = clEnqueueNDRangeKernel(this->commandQueue, this->copyToDisplayKernel, 1, NULL, globalThreads, localThreads, 0, 0, eventND );
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueNDRangeKernel copyToDisplayKernel failed %s"),this->ErrorMessage(status));
		return status;
	}

	status = clWaitForEvents(1, eventND);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clWaitForEvents failed %s"),this->ErrorMessage(status));
		return status;
	}
	clReleaseEvent(eventND[0]);
	
	// Release GL buffer
	status = clEnqueueReleaseGLObjects(this->commandQueue, 1, &this->dispPos, 0, 0, 0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueReleaseGLObjects failed to release dispPos %s"),this->ErrorMessage(status));
		return status;
	}

	status = clFinish(this->commandQueue);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clFinish failed %s"),this->ErrorMessage(status));
		return status;
	}

	wxLogDebug(wxT("CLModel:UpdateDisplay Done"));
	return status;
}

int CLModel::InitKernels()
{
	wxLogDebug(wxT("CLModel:InitKernels Start"));
	if(!this->initialisedOk)
	{
		wxLogDebug(wxT("Aborted CLModel failed to Initialise"));
		return -1;
	}

	int success = CL_SUCCESS;
	cl_int status;
	int paramNumber = 0;

/*
	__kernel
	void copyToDisplay( 
		__constant double4* pos,
		__global double4* dispPos,
		int centerBodyIndex
	) 
*/
	status = clSetKernelArg(this->copyToDisplayKernel,paramNumber++,sizeof(cl_mem),(void*)&this->gravPos);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for gravPos %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(this->copyToDisplayKernel,paramNumber++,sizeof(cl_mem),(void*)&this->currPos);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for currPos %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(this->copyToDisplayKernel,paramNumber++,sizeof(cl_mem),(void*)&this->dispPos);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for dispPos %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(this->copyToDisplayKernel,paramNumber++,sizeof(cl_int),(void*)&this->centerBody);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for centerBody %s"),this->ErrorMessage(status));
		success=status;
	}
	
	/* Set appropriate arguments to the kernel 
	__kernel void computeAcc( 
	   __constant double4* gravPos,
	   __global double4* pos, 
	   int numGrav, 
	   double epsSqr, 
	   __global double4* acc)
   */
	paramNumber = 0;
	status = clSetKernelArg(this->accKernel,paramNumber++,sizeof(cl_mem),(void*)&this->gravPos);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for gravPos %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(this->accKernel,paramNumber++,sizeof(cl_mem),(void*)&this->currPos);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for currPos %s"),this->ErrorMessage(status));
		success=status;
	}

	if(this->accelerationKernelName->IsSameAs(wxT("relativistic"),false))
	{
		status = clSetKernelArg(this->accKernel,paramNumber++,sizeof(cl_mem),(void*)&this->currVel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clSetKernelArg failed for currVel %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	status = clSetKernelArg(this->accKernel,paramNumber++,sizeof(cl_int),(void *)&this->numGrav);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for numGrav %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(this->accKernel,paramNumber++,sizeof(cl_double),(void *)&this->espSqr);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for espSqr %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(this->accKernel,paramNumber++,sizeof(cl_mem),(void *)&this->acc);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for acc %s"),this->ErrorMessage(status));
		success=status;
	}

	// set integration kernel args
	status = SetAdamsKernelArgs(this->startupKernel);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("SetAdamsKernelArgs failed for  startupKernel %s"),this->ErrorMessage(status));
		success=status;
	}
	status = SetAdamsKernelArgs(this->adamsBashforthKernel);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("SetAdamsKernelArgs failed for adamsBashforthKernel %s"),this->ErrorMessage(status));
		success=status;
	}
	status = SetAdamsKernelArgs(this->adamsMoultonKernel);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("SetAdamsKernelArgs failed for adamsMoultonKernel %s"),this->ErrorMessage(status));
		success=status;
	}
	
	/* Check group size against group size returned by kernel */
	status = clGetKernelWorkGroupInfo(this->accKernel, this->deviceId,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&this->accKernelWorkGroupSize,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("getting accKernel kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"),this->ErrorMessage(status));
		success=status;
	}
	
	wxLogDebug(wxT("accKernel Work Group Size %lu"),this->accKernelWorkGroupSize);
	
	if(this->groupSize > this->accKernelWorkGroupSize)
	{
		wxLogDebug(wxT("Group size larger than max acc kernel group size"));
		this->groupSize = this->accKernelWorkGroupSize;
	}
	
	status = clGetKernelWorkGroupInfo(this->startupKernel, this->deviceId,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&this->startupKernelWorkGroupSize,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("getting startupKernel kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"),this->ErrorMessage(status));
		success=status;
	}

	wxLogDebug(wxT("startupKernel Work Group Size %lu"),this->startupKernelWorkGroupSize);

	if(this->groupSize > this->startupKernelWorkGroupSize)
	{
		wxLogDebug(wxT("Group size larger than max startup kernel group size"));
		this->groupSize = this->startupKernelWorkGroupSize;
	}
	
	status = clGetKernelWorkGroupInfo(this->adamsBashforthKernel, this->deviceId,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&this->adamsBashforthKernelWorkGroupSize,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("getting adamsBashforth kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"),this->ErrorMessage(status));
		success=status;
	}

	wxLogDebug(wxT("adamsBashford Work Group Size %lu"),this->adamsBashforthKernelWorkGroupSize);

	if(this->groupSize > this->adamsBashforthKernelWorkGroupSize)
	{
		wxLogDebug(wxT("Group size larger than max adamsBashforth group size"));
		this->groupSize = this->adamsBashforthKernelWorkGroupSize;
	}
	
	status = clGetKernelWorkGroupInfo(this->adamsMoultonKernel, this->deviceId,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&this->adamsMoultonKernelWorkGroupSize,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("getting adamsMoulton kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"),this->ErrorMessage(status));
		success=status;
	}

	wxLogDebug(wxT("adamsKernel Work Group Size %lu"),this->adamsMoultonKernelWorkGroupSize);

	if(this->groupSize > this->adamsMoultonKernelWorkGroupSize)
	{
		wxLogDebug(wxT("Group size larger than max adams Moulton group size"));
		this->groupSize = this->adamsMoultonKernelWorkGroupSize;
	}
	
	status = clGetKernelWorkGroupInfo(this->copyToDisplayKernel, this->deviceId,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&this->copyToDisplayKernelWorkGroupSize,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("getting copyToDisplay kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"),this->ErrorMessage(status));
		success=status;
	}

	wxLogDebug(wxT("copyToDisplayKernel Work Group Size %lu"),this->copyToDisplayKernelWorkGroupSize);

	if(this->groupSize > this->copyToDisplayKernelWorkGroupSize)
	{
		wxLogDebug(wxT("Group size larger than max group size"));
		this->groupSize = this->copyToDisplayKernelWorkGroupSize;
	}
	
	wxLogDebug(wxT("CLModel:InitKernels Done"));
	return success=status;;
}

int CLModel::SetAdamsKernelArgs(cl_kernel adamsKernel)
{
	cl_int status;
	int success = CL_SUCCESS;
	//adamskernel
	status = clSetKernelArg(adamsKernel,0,sizeof(cl_mem),(void *)&this->currPos);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 0 failed for currPos %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,1,sizeof(cl_mem),(void *)&this->currVel);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 1 failed for currVel %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,2,sizeof(cl_mem),(void *)&this->acc);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 2 failed for acc %s"),this->ErrorMessage(status));
		success=status;
	}

	status = clSetKernelArg(adamsKernel,3,sizeof(cl_double),(void *)&this->delT);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 3 failed for delT %s"),this->ErrorMessage(status));
		success=status;
	}

	status = clSetKernelArg(adamsKernel,4,sizeof(cl_mem),(void*)&this->newPos);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 4 failed for newPos %s"),this->ErrorMessage(status));
		success=status;
	}

	status = clSetKernelArg(adamsKernel,5,sizeof(cl_mem),(void *)&this->newVel);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 5 failed for newVel %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,6,sizeof(cl_int),(void *)&this->stage);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg failed for stage %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,7,sizeof(cl_int),(void *)&this->step);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 7 failed for step %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,8,sizeof(cl_int),(void *)&this->numParticles);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 8 failed for step %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,9,sizeof(cl_mem),(void *)&this->posLast);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 9 failed for posLast %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,10,sizeof(cl_mem),(void *)&this->velLast);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 10 failed for velLast %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,11,sizeof(cl_mem),(void *)&this->velHistory);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 11 failed for velHistory %s"),this->ErrorMessage(status));
		success=status;
	}
	
	status = clSetKernelArg(adamsKernel,12,sizeof(cl_mem),(void *)&this->accHistory);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clSetKernelArg 12 failed for accHistory %s"),this->ErrorMessage(status));
		success=status;
	}
	
	return success;
}
int CLModel::CleanUpCL()
{
	wxLogDebug(wxT("CLModel:CleanUpCL"));
	cl_int status;
	int success = CL_SUCCESS;

	if(this->currPos != NULL)
	{
		status = clReleaseMemObject(this->currPos);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject currPos failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->newPos != NULL)
	{
		status = clReleaseMemObject(this->newPos);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject newPos failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->currVel != NULL)
	{
		status = clReleaseMemObject(this->currVel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject currVel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->newVel != NULL)
	{
		status = clReleaseMemObject(this->newVel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject newVel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->gravPos != NULL)
	{
		status = clReleaseMemObject(this->gravPos);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject gravPos failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->acc != NULL)
	{
		status = clReleaseMemObject(this->acc);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject acc failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->posLast != NULL)
	{
		status = clReleaseMemObject(this->posLast);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject posLast failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->velLast != NULL)
	{
		status = clReleaseMemObject(this->velLast);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject velLast failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->velHistory != NULL)
	{
		status = clReleaseMemObject(this->velHistory);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject velHistory failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->accHistory != NULL)
	{
		status = clReleaseMemObject(this->accHistory);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject accHistory failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->dispPos != NULL)
	{
		status = clReleaseMemObject(this->dispPos);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseMemObject newVel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->accKernel != NULL)
	{
		status = clReleaseKernel(this->accKernel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseKernel accKernel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->startupKernel != NULL)
	{
		status = clReleaseKernel(this->startupKernel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseKernel startupKernel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->adamsBashforthKernel != NULL)
	{
		status = clReleaseKernel(this->adamsBashforthKernel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseKernel adamsBashforthKernel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->adamsMoultonKernel != NULL)
	{
		status = clReleaseKernel(this->adamsMoultonKernel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseKernel adamsMoultonKernel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}
	
	if(this->copyToDisplayKernel != NULL)
	{
		status = clReleaseKernel(this->copyToDisplayKernel);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseKernel copyToDisplayKernel failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->program != NULL)
	{
		status = clReleaseProgram(this->program);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseProgram failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->commandQueue != NULL)
	{
		status = clReleaseCommandQueue(this->commandQueue);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseCommandQueue failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	if(this->context != NULL)
	{
		status = clReleaseContext(this->context);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clReleaseContext failed %s"),this->ErrorMessage(status));
			success=status;
		}
	}

	wxLogDebug(wxT("CLModel:CleanUpCL Done"));
	return success;
}

void CLModel::RequestUpdate()
{
	this->updateDisplay = true;
}

int CLModel::SetInitalState(cl_double4 *initalPositions, cl_double4 *initalVelocities)
{
	cl_int status = CL_SUCCESS;
	status = clEnqueueWriteBuffer(this->commandQueue,this->currPos,CL_TRUE,0,this->numParticles * sizeof(cl_double4),initalPositions,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueWriteBuffer write inital Positions to currPos %s"),this->ErrorMessage(status));
		return status;
	}
	
	status = clEnqueueWriteBuffer(this->commandQueue,this->gravPos,CL_TRUE,0,this->numGrav * sizeof(cl_double4),initalPositions,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueWriteBuffer write inital Positions to gravPos %s"),this->ErrorMessage(status));
		return status;
	}

	status = clEnqueueWriteBuffer(this->commandQueue,this->currVel,CL_TRUE,0,this->numParticles * sizeof(cl_double4),initalVelocities,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueWriteBuffer write inital Velocity to currVel %s"),this->ErrorMessage(status));
	}
	this->step = 0;
	return status;
}

int CLModel::ReadToInitialState(cl_double4 *initalPositions, cl_double4 *initalVelocities)
{
	cl_int status = CL_SUCCESS;
	status = clEnqueueReadBuffer(this->commandQueue,this->currPos,CL_TRUE,0,this->numParticles * sizeof(cl_double4),initalPositions,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueWriteBuffer write inital Positions to currPos %s"),this->ErrorMessage(status));
		return status;
	}
	
	status = clEnqueueReadBuffer(this->commandQueue,this->currVel,CL_TRUE,0,this->numParticles * sizeof(cl_double4),initalVelocities,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueWriteBuffer write inital Velocity to currVel %s"),this->ErrorMessage(status));
	}
	return status;
}

// convert the openCL status code to text
wxString CLModel::ErrorMessage(cl_int status)
{
	switch(status)
	{
		case CL_SUCCESS: return wxT("CL_SUCCESS");
		case CL_DEVICE_NOT_FOUND: return wxT("CL_DEVICE_NOT_FOUND");
		case CL_DEVICE_NOT_AVAILABLE: return wxT("CL_DEVICE_NOT_AVAILABLE");                     
		case CL_COMPILER_NOT_AVAILABLE: return wxT("CL_COMPILER_NOT_AVAILABLE");                   
		case CL_MEM_OBJECT_ALLOCATION_FAILURE: return wxT("CL_MEM_OBJECT_ALLOCATION_FAILURE");           
		case CL_OUT_OF_RESOURCES: return wxT("CL_OUT_OF_RESOURCES");                         
		case CL_OUT_OF_HOST_MEMORY: return wxT("CL_OUT_OF_HOST_MEMORY");                       
		case CL_PROFILING_INFO_NOT_AVAILABLE: return wxT("CL_PROFILING_INFO_NOT_AVAILABLE");             
		case CL_MEM_COPY_OVERLAP: return wxT("CL_MEM_COPY_OVERLAP");                         
		case CL_IMAGE_FORMAT_MISMATCH: return wxT("CL_IMAGE_FORMAT_MISMATCH");                    
		case CL_IMAGE_FORMAT_NOT_SUPPORTED: return wxT("CL_IMAGE_FORMAT_NOT_SUPPORTED");               
		case CL_BUILD_PROGRAM_FAILURE: return wxT("CL_BUILD_PROGRAM_FAILURE");                    
		case CL_MAP_FAILURE: return wxT("CL_MAP_FAILURE");                              
		case CL_MISALIGNED_SUB_BUFFER_OFFSET: return wxT("CL_MISALIGNED_SUB_BUFFER_OFFSET");             
		case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return wxT("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST");
		case CL_COMPILE_PROGRAM_FAILURE: return wxT("CL_COMPILE_PROGRAM_FAILURE");                  
		case CL_LINKER_NOT_AVAILABLE: return wxT("CL_LINKER_NOT_AVAILABLE");                     
		case CL_LINK_PROGRAM_FAILURE: return wxT("CL_LINK_PROGRAM_FAILURE");                     
		case CL_DEVICE_PARTITION_FAILED: return wxT("CL_DEVICE_PARTITION_FAILED");                  
		case CL_KERNEL_ARG_INFO_NOT_AVAILABLE: return wxT("CL_KERNEL_ARG_INFO_NOT_AVAILABLE");            
		case CL_INVALID_VALUE: return wxT("CL_INVALID_VALUE");                            
		case CL_INVALID_DEVICE_TYPE: return wxT("CL_INVALID_DEVICE_TYPE");                      
		case CL_INVALID_PLATFORM: return wxT("CL_INVALID_PLATFORM");                         
		case CL_INVALID_DEVICE: return wxT("CL_INVALID_DEVICE");                           
		case CL_INVALID_CONTEXT: return wxT("CL_INVALID_CONTEXT");                          
		case CL_INVALID_QUEUE_PROPERTIES: return wxT("CL_INVALID_QUEUE_PROPERTIES");                 
		case CL_INVALID_COMMAND_QUEUE: return wxT("CL_INVALID_COMMAND_QUEUE");                    
		case CL_INVALID_HOST_PTR: return wxT("CL_INVALID_HOST_PTR");                         
		case CL_INVALID_MEM_OBJECT: return wxT("CL_INVALID_MEM_OBJECT");                       
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return wxT("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR");          
		case CL_INVALID_IMAGE_SIZE: return wxT("CL_INVALID_IMAGE_SIZE");                       
		case CL_INVALID_SAMPLER: return wxT("CL_INVALID_SAMPLER");                          
		case CL_INVALID_BINARY: return wxT("CL_INVALID_BINARY");                           
		case CL_INVALID_BUILD_OPTIONS: return wxT("CL_INVALID_BUILD_OPTIONS");                   
		case CL_INVALID_PROGRAM: return wxT("CL_INVALID_PROGRAM");                          
		case CL_INVALID_PROGRAM_EXECUTABLE: return wxT("CL_INVALID_PROGRAM_EXECUTABLE");               
		case CL_INVALID_KERNEL_NAME: return wxT("CL_INVALID_KERNEL_NAME");                      
		case CL_INVALID_KERNEL_DEFINITION: return wxT("CL_INVALID_KERNEL_DEFINITION");                
		case CL_INVALID_KERNEL: return wxT("CL_INVALID_KERNEL");                           
		case CL_INVALID_ARG_INDEX: return wxT("CL_INVALID_ARG_INDEX");                        
		case CL_INVALID_ARG_VALUE: return wxT("CL_INVALID_ARG_VALUE");                        
		case CL_INVALID_ARG_SIZE: return wxT("CL_INVALID_ARG_SIZE");                         
		case CL_INVALID_KERNEL_ARGS: return wxT("CL_INVALID_KERNEL_ARGS");                      
		case CL_INVALID_WORK_DIMENSION: return wxT("CL_INVALID_WORK_DIMENSION");                   
		case CL_INVALID_WORK_GROUP_SIZE: return wxT("CL_INVALID_WORK_GROUP_SIZE");                  
		case CL_INVALID_WORK_ITEM_SIZE: return wxT("CL_INVALID_WORK_ITEM_SIZE");                   
		case CL_INVALID_GLOBAL_OFFSET: return wxT("CL_INVALID_GLOBAL_OFFSET");                    
		case CL_INVALID_EVENT_WAIT_LIST: return wxT("CL_INVALID_EVENT_WAIT_LIST");                  
		case CL_INVALID_EVENT: return wxT("CL_INVALID_EVENT");                            
		case CL_INVALID_OPERATION: return wxT("CL_INVALID_OPERATION");                       
		case CL_INVALID_GL_OBJECT: return wxT("CL_INVALID_GL_OBJECT");                        
		case CL_INVALID_BUFFER_SIZE: return wxT("CL_INVALID_BUFFER_SIZE");                      
		case CL_INVALID_MIP_LEVEL: return wxT("CL_INVALID_MIP_LEVEL");                        
		case CL_INVALID_GLOBAL_WORK_SIZE: return wxT("CL_INVALID_GLOBAL_WORK_SIZE");                 
		case CL_INVALID_PROPERTY: return wxT("CL_INVALID_PROPERTY");                         
		case CL_INVALID_IMAGE_DESCRIPTOR: return wxT("CL_INVALID_IMAGE_DESCRIPTOR");                 
		case CL_INVALID_COMPILER_OPTIONS: return wxT("CL_INVALID_COMPILER_OPTIONS");                 
		case CL_INVALID_LINKER_OPTIONS: return wxT("CL_INVALID_LINKER_OPTIONS");                   
		case CL_INVALID_DEVICE_PARTITION_COUNT: return wxT("CL_INVALID_DEVICE_PARTITION_COUNT");
		default: return wxT("UnKnown Error");
	}
}
