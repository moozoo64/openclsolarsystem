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
	this->gotKhrFp64 = false;
	this->gotAmdFp64 =false;
	this->gotKhrGlSharing = false;
	this->gotAppleGlSharing = false;
	this->delT = 4*60*60.0f;
	this->espSqr = 0.000001f;
	this->time =0.0f;
	this->julianDate =0.0f;
	this->numGrav = 16;
	this->centerBody = 0;
	this->numStages = 1;
	this->stage = this->numStages;
	this->adamsBashforthKernelName = new wxString("adamsBashforth11");
	this->adamsMoultonKernelName = new wxString("adamsMoulton10");
	this->accelerationKernelName = new wxString("relativistic");
}

CLModel::~CLModel()
{
	this->initialisedOk=false;
	this->CleanUpCL();
	wxLogDebug(wxT("CLModel Destructor"));
}

void CL_CALLBACK PfnNotify(const char * errInfo, const void * private_info, size_t cb, void * user_data)
{
	wxLogError("PfnNotify Error: %s\n", errInfo);
}

int CLModel::ChooseAndCreateContext(char *desiredPlatformName,bool preferCpu)
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::ChooseAndCreateContext threadId: %ld"),wxThread::GetCurrentId());
#endif

	cl_int status = CL_SUCCESS;
	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	
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
	
	// We attempt twice to find a device that supports double precision and cl_khr_gl_sharing
	// On the first attempt we consider if the user prefered a cpu device.
	// On the second attempt we try and find any device cpu or not.
	bool foundDevice = false;
	bool ignorePreferCpu = false;
	for(int i =0; i<2;i++)
	{
		for (unsigned j = 0; j < numberOfDevices; ++j)
		{
			// Get Extensions
			this->gotKhrFp64 = false;
			this->gotAmdFp64 =false;
			this->gotKhrGlSharing = false;
			this->gotAppleGlSharing = false;

			char extensions[4096];
			status = clGetDeviceInfo(deviceIds[j], CL_DEVICE_EXTENSIONS, sizeof(extensions), extensions, NULL);
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_EXTENSIONS %s"),this->ErrorMessage(status));
				return status;
			}
			
			wxString deviceExtensions = wxString(extensions);
		
			if(deviceExtensions.Contains(wxT("cl_khr_gl_sharing")))
			{
				this->gotKhrGlSharing = true;
			}
			
			if(deviceExtensions.Contains(wxT("cl_APPLE_gl_sharing")))
			{
				this->gotAppleGlSharing = true;
			}
			
			if(deviceExtensions.Contains(wxT("cl_khr_fp64")))
			{
				this->gotKhrFp64 = true;
			}
			
			if(deviceExtensions.Contains(wxT("cl_amd_fp64")))
			{
				this->gotAmdFp64 = true;
			}
			
			// Check for double precision
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
				wxLogDebug(wxT("skipping device %d it does not support double precision"),j);
				continue;
			}
			
			// skip device if it does not support gl sharing
			if(!this->gotKhrGlSharing && !this->gotAppleGlSharing)
			{
				wxLogDebug(wxT("skipping device %d it does not support cl_khr_gl_sharing"),j);
				continue;
			}
			
			cl_device_type deviceType;
			status = clGetDeviceInfo(deviceIds[j], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, NULL);
			if( status != CL_SUCCESS)
			{
				wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_TYPE %s"),this->ErrorMessage(status));
				return status;
			}
			
			if(!ignorePreferCpu && preferCpu)
			{
				if(deviceType != CL_DEVICE_TYPE_CPU)
				{
					wxLogDebug(wxT("Skipping device %d while looking for CPU device"),j);
					continue;
				}
				else
				{
					wxLogDebug(wxT("Found CPU Device as requested"));
				}
			}
			
			this->deviceId = deviceIds[j];
			foundDevice = true;
			break;
		}
		
		if(foundDevice)
		{
			break;
		}
		ignorePreferCpu = true;
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

	cl_device_id* interopDeviceIds = NULL;

#ifdef __WXDEBUG__
	cl_uint numInteropDeviceIds = 0;
	clGetGLContextInfoKHR_fn clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddressForPlatform(platform, "clGetGLContextInfoKHR");
	if(clGetGLContextInfoKHR == NULL)
	{
		wxLogError(wxT("clGetExtensionFunctionAddressForPlatform failed to find clGetGLContextInfoKHR %s"),this->ErrorMessage(status));
		return -1;
	}
	
	size_t deviceSize = 0;
	status =  clGetGLContextInfoKHR(properties,CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,0,NULL,&deviceSize);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetGLContextInfoKHR CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	if(deviceSize == 0)
	{
		wxLogDebug(wxT("clGetGLContextInfoKHR reports no CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR"));
	}
	
	//size_t interopDeviceSize = 0;
	status =  clGetGLContextInfoKHR(properties,CL_DEVICES_FOR_GL_CONTEXT_KHR,0,NULL,&numInteropDeviceIds);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clGetGLContextInfoKHR CL_DEVICES_FOR_GL_CONTEXT_KHR failed %s"),this->ErrorMessage(status));
		return status;
	}	
	
	if(numInteropDeviceIds == 0)
	{
		wxLogDebug(wxT("clGetGLContextInfoKHR reports no CL_DEVICES_FOR_GL_CONTEXT_KHR"));
	}
	else
	{
		wxLogDebug(wxT("clGetGLContextInfoKHR reports %d devices"),numInteropDeviceIds);
		interopDeviceIds = new cl_device_id[numInteropDeviceIds];
		status =  clGetGLContextInfoKHR(properties,CL_DEVICES_FOR_GL_CONTEXT_KHR,numInteropDeviceIds,interopDeviceIds,NULL);
		if( status != CL_SUCCESS)
		{
			wxLogError(wxT("clGetGLContextInfoKHR CL_DEVICES_FOR_GL_CONTEXT_KHR failed %s"),this->ErrorMessage(status));
			return status;
		}
	}
#endif
							
	this->context = clCreateContext( properties,numberOfDevices,deviceIds,&PfnNotify,NULL,&status);
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

#ifdef __WXDEBUG__
	bool deviceOnGLInteropList = false;
	if(interopDeviceIds != NULL)
	{
		for (unsigned j = 0; j < numInteropDeviceIds; ++j)
		{
			if(this->deviceId == interopDeviceIds[j])
			{
				deviceOnGLInteropList = true;
				break;
			}
		}
	}
	
	if(deviceOnGLInteropList)
	{
		wxLogDebug(wxT("Device is on GL interop list"));
	}
	else
	{
		wxLogDebug(wxT("Device is not on GL interop list"));
	}
#endif

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
	
	delete[] contextDeviceIds;
	delete[] deviceIds;
	delete[] platforms;
	if(interopDeviceIds != NULL)
	{
		delete[] interopDeviceIds;
	}
	
	wxLogDebug(wxT("Finished CLModel::ChooseAndCreateContext"));
	return status;
}

int CLModel::CreateBufferObjects(GLuint *vbo,int numParticles, int numGrav)
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::CreateBufferObjects threadId: %ld"),wxThread::GetCurrentId());
#endif

	cl_int status = CL_SUCCESS;
	this->step = 0;
	this->numGrav = numGrav;
	this->numParticles = (cl_int)((numParticles / this->groupSize) * this->groupSize);;

	// Create cl_mem objects
	// Get an openCL buffer to the openGL Vertex Array of points.
	// We aquire this and then copy the simulation positions to it to update the on screen positions.
	this->dispPos = clCreateFromGLBuffer(this->context, CL_MEM_WRITE_ONLY, vbo[0], &status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateFromGLBuffer failed to create cl_mem object for GL vertex buffer object %s"),this->ErrorMessage(status));
		return status;
	}
	
	// The current positions of the solar system bodies.
	this->currPos = clCreateBuffer(this->context,CL_MEM_READ_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for currPos %s"),this->ErrorMessage(status));
		return status;
	}
	
	// Contains the new positions computed by the integration from current positions.
	this->newPos = clCreateBuffer(this->context,CL_MEM_WRITE_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for newPos %s"),this->ErrorMessage(status));
		return status;
	}

	// This buffer contains a copy of the current positions for the bodys for which we are including gravitatoinal effects.
	// This buffer is small enough to fit into the on chip memory cache of the GPU
	// It is marked as read only for the kernels and of memory type __constant
	// My understanding is that after the first numGrav positions are read the should all be cached on chip.
	this->gravPos = clCreateBuffer(this->context,CL_MEM_READ_ONLY,this->numGrav * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for gravPos %s"),this->ErrorMessage(status));
		return status;
	}
	
	// The current velocities of the solar system bodies.
	this->currVel = clCreateBuffer(this->context,CL_MEM_READ_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for currVel %s"),this->ErrorMessage(status));
		return status;
	}

	// Contains the new velocities computed by the integration from current velocities.
	this->newVel = clCreateBuffer(this->context,CL_MEM_WRITE_ONLY,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for newVel %s"),this->ErrorMessage(status));
		return status;
	}
	
	// contains the gravitational accerations computed from the current positions by the acceleration kernel
	this->acc = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for acc %s"),this->ErrorMessage(status));
		return status;
	}
	
	// This is used to hold the current position at the start of the Adams Bashforth Intgration for use by the Adams Moulton Inegrator.
	// i.e. between the AB and AM integrations the current position holds the estimated position. But the AM still needs the position from the start 
	this->posLast = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 1 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for posLast %s"),this->ErrorMessage(status));
		return status;
	}

	// This is used to hold the current velocities at the start of the Adams Bashforth Intgration for use by the Adams Moulton Integrator. 
	this->velLast = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 1 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for velLast %s"),this->ErrorMessage(status));
		return status;
	}
	
	// 16 element ring buffer used to store the previous steps velocities.
	// e.g the velocity for the previous step is stored at index (step-1)&0xf
	this->velHistory = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 16 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for velHistory %s"),this->ErrorMessage(status));
		return status;
	}

	// 16 element ring buffer used to store the previous steps accerlerations.
	// e.g the accerleration for the previous step is stored at index (step-1)&0xf
	this->accHistory = clCreateBuffer(this->context,CL_MEM_READ_WRITE,this->numParticles * 16 * sizeof(cl_double4),0,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateBuffer failed to create cl_mem object for accHistory %s"),this->ErrorMessage(status));
		return status;
	}
	
	wxLogDebug(wxT("Finished CLModel::CreateBufferObjects"));
	return status;
}

int CLModel::CompileProgramAndCreateKernels()
{
	
#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::CompileProgramAndCreateKernels threadId: %ld"),wxThread::GetCurrentId());
#endif

	cl_int status = CL_SUCCESS;
	// load the contents of the kernel file into a in memory string
	wxString nbodySource;
	wxFFile nbodyFile("adams.cl", "r");
	if(!nbodyFile.IsOpened())
	{
		wxLogError(wxT("Failed to read adams.cl"));
		return -1;
	}

	nbodyFile.ReadAll(&nbodySource,wxConvISO8859_1);
	
	wxString programSource;
	if(this->gotKhrGlSharing && !this->gotAmdFp64)
	{
		programSource.Append(wxT("#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable \r\n"));
	}
	
	if(this->gotAppleGlSharing)
	{
		programSource.Append(wxT("#pragma OPENCL EXTENSION cl_APPLE_gl_sharing : enable \r\n"));
	}
	
	if(this->gotKhrFp64)
	{
		programSource.Append(wxT("#pragma OPENCL EXTENSION cl_khr_fp64 : enable \r\n"));
	}
	else if(this->gotAmdFp64)
	{
		programSource.Append(wxT("#pragma OPENCL EXTENSION cl_amd_fp64 : enable \r\n"));
	}
	
	programSource.Append(nbodySource);
	
	const char * source = programSource.c_str();
	size_t sourceSize[] = {strlen(source)};

	// setup a openCL program to hold the program
	this->program = clCreateProgramWithSource(this->context,1,&source,sourceSize,&status);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clCreateProgramWithSource failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	// compile the program (kernels)
	const char * options = "-cl-mad-enable";// -cl-fast-relaxed-math";// "-cl-mad-enable -cl-fast-relaxed-math -cl-nv-verbose ";
	status = clBuildProgram(this->program,1,&this->deviceId,options,NULL,NULL);
	if( status != CL_SUCCESS)
	{
		// if it failed to compile then obtain the compile log and display it in an error dialog
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
	
	// if we are debugging then include the compile log
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
	
	// setup kernels (pointers?) to required compiled kernels
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

	this->initialisedOk = true;
	wxLogDebug(wxT("Finished CLModel:CompileProgramAndCreateKernels"));

	return status;
}

// Excutes the kernels to advance the simulation to the next time step
int CLModel::ExecuteKernels()
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::ExecuteKernel threadId: %ld"),wxThread::GetCurrentId());
#endif

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

	wxLogDebug(wxT("CLModel::ExecuteKernels clFinish()"));

	// for the first 16 steps we call the startupKernel. This populates the 16 element ring buffer
	// It is current very inaccurate and uses a 1st order Adams Bashford Moulton followed by a 2nd order
	// follows by several 4th orders steps until all 16 elements in the history ring buffer have been set.
	// It needs to be replaced with a Runge Kutta 7th order with a smaller step size (1/10th?)
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
		// The Integration has two stages
		// The first, Adams Bashford Integration, computes an initial estimate of the next position and velocity
		// The second,Adams Moulton Integration, takes the estimates, combined with the acceration at estimate position
		// and compute a more exact position and velocity
		// The first stage is also called the Predictor stage, the second stage the corrector stage.
		// Note that it is possible to iterate the corrector stage to increase the accurarcy.
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
	
	wxLogDebug(wxT("CLModel::ExecuteKernels clFinish()"));
	
	// Copy new positions to current position
	status = clEnqueueCopyBuffer(commandQueue,this->newPos,this->currPos,0,0,sizeof(cl_double4) * this->numParticles,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueCopyBuffer newPos to currPos failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	// Copy new positions of the bodies with mass to gravPos
	status = clEnqueueCopyBuffer(commandQueue,this->newPos,this->gravPos,0,0,sizeof(cl_double4) * this->numGrav,0,0,0);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueCopyBuffer newPos to currPos failed %s"),this->ErrorMessage(status));
		return status;
	}
    // Copy new velocities to current velocities
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
	
	wxLogDebug(wxT("CLModel::ExecuteKernels clFinish()"));
		
	this->stage = this->stage - 1;
	if(this->stage < 0)
	{
		// if we just finished the corrector stage then advance to the next step (time)
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

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::UpdateDisplay threadId: %ld"),wxThread::GetCurrentId());
#endif

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

	wxLogDebug(wxT("CLModel::UpdateDisplay clEnqueueAcquireGLObjects()"));

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
	
	// update the copyToDisplayKernel's centerBody argument so it knows which body to offset the posistions against
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
	status = clEnqueueReleaseGLObjects(this->commandQueue, 1, &this->dispPos, 0, 0, eventND);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clEnqueueReleaseGLObjects failed to release dispPos %s"),this->ErrorMessage(status));
		return status;
	}

	status = clWaitForEvents(1, eventND);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clWaitForEvents clEnqueueReleaseGLObjects failed %s"),this->ErrorMessage(status));
		return status;
	}
	clReleaseEvent(eventND[0]);
	
	status = clFinish(this->commandQueue);
	if( status != CL_SUCCESS)
	{
		wxLogError(wxT("clFinish failed %s"),this->ErrorMessage(status));
		return status;
	}
	
	wxLogDebug(wxT("CLModel::UpdateDisplay clFinish()"));
	wxLogDebug(wxT("CLModel:UpdateDisplay Done"));
	return status;
}

// initialise the kernels so they are ready to be called.
int CLModel::SetKernelArgumentsAndGroupSize()
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::SetKernelArgumentsAndGroupSize threadId: %ld"),wxThread::GetCurrentId());
#endif

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
	
	// the acceleration kernel that includes relativistic corrections need the relativistic parameter stored
	// in .w. We pass the whole velocity vector in case it can be used in a more complicated relativistic on MOND type kernel
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
	
	// I'm a bit paranoid here. I set the group size to the smallest that will work for all the kernels
	// perhap each kernel should use the largest group size possible for it. But I don't do that.
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
	
	wxLogDebug(wxT("Finished CLModel:SetKernelArgumentsAndGroupSize"));
	return success=status;;
}

// the Adams Bashforth and Adams Moulton integration kernels all have the same signature
/*
__kernel
void adamsMoulton11( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
*/
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

// Release and/or de-allocates all opencl resources.
// A cleanup and initialise sequence is done every time something major changes.
// e.g. number of bodies or the kernels changes.
int CLModel::CleanUpCL()
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::CleanUpCL threadId: %ld"),wxThread::GetCurrentId());
#endif

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

// Copies the initial positions and velocities into the opencl buffers
int CLModel::SetInitalState(cl_double4 *initalPositions, cl_double4 *initalVelocities)
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::SetInitalState threadId: %ld"),wxThread::GetCurrentId());
#endif
	
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

// snapshots the current positions and velocities and makes them the initial start conditions.
// From there they can be saved to disk as either binary or slf format for later
int CLModel::ReadToInitialState(cl_double4 *initalPositions, cl_double4 *initalVelocities)
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("CLModel::ReadToInitialState threadId: %ld"),wxThread::GetCurrentId());
#endif

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
// Because the error numbers are to hard to remember
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
		case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR: return wxT("CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR");
		default: return wxT("UnKnown Error");
	}
}
