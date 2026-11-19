/*
  Copyright 2013-2026 Michael William Simmons

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

/**
 * CLModel - Manages OpenCL computation resources for N-body solar system simulation
 *
 * This class handles:
 * - OpenCL device selection and context creation
 * - Kernel compilation and management
 * - Buffer allocation and data transfer
 * - Execution of integration kernels for position/velocity updates
 * - Synchronization with OpenGL for visualization
 */
#include "global.hpp"
#include "clmodel.hpp"
#include "kernels.hpp"

CLModel::CLModel()
{
  // Initialize all pointers to NULL to ensure clean state
  this->deviceId = NULL;
  this->context = NULL;
  this->devices = NULL;
  this->commandQueue = NULL;
  this->program = NULL;
  this->accKernel = NULL;
  this->adamsBashforthKernel = NULL;
  this->adamsMoultonKernel = NULL;
  this->startupKernel = NULL;
  this->copyToDisplayKernel = NULL;

  // Initialize numeric values to safe defaults
  this->maxWorkGroupSize = 0;
  this->maxDimensions = 0;
  this->maxWorkItemSizes = NULL;
  this->totalLocalMemory = 0;
  this->accKernelWorkGroupSize = 0;
  this->adamsBashforthKernelWorkGroupSize = 0;
  this->adamsMoultonKernelWorkGroupSize = 0;
  this->startupKernelWorkGroupSize = 0;
  this->groupSize = 64;
  this->maxMemoryAlloc = 0;
  this->globalMemorySize = 0;

  this->dispPos = NULL;
  this->currPos = NULL;
  this->gravPos = NULL;
  this->currVel = NULL;
  this->newPos = NULL;
  this->newVel = NULL;
  this->acc = NULL;
  this->velHistory = NULL;
  this->accHistory = NULL;
  this->posLast = NULL;
  this->velLast = NULL;
  this->rkSumVel = NULL;
  this->rkSumAcc = NULL;

  // Set simulation parameters to initial values
  this->updateDisplay = false;
  this->initialisedOk = false;
  this->gotKhrFp64 = false;
  this->gotAmdFp64 = false;
  this->gotKhrGlSharing = false;
  this->gotAppleGlSharing = false;
  this->delT = 4 * 60 * 60.0f; // 4 hour timestep
  this->espSqr = 0.0;         // Softening length squared (Gm^2). None: the kernels skip self-interaction instead
  this->time = 0.0f;
  this->julianDate = 0.0f;
  this->numGrav = 16;
  this->centerBody = 0;
  this->numStages = 1;
  this->stage = this->numStages;
  this->startupSubSteps = 10; // RK4 sub-steps per step while seeding the Adams history
  this->adamsBashforthKernelName = wxT("adamsBashforth11");
  this->adamsMoultonKernelName = wxT("adamsMoulton10");
  this->accelerationKernelName = wxT("relativistic");
}

CLModel::~CLModel()
{
  this->initialisedOk = false;
  this->CleanUpCL();
  wxLogDebug(wxT("CLModel Destructor"));
}

void CL_CALLBACK PfnNotify(const char *errInfo, const void *private_info, size_t cb, void *user_data)
{
  wxLogError("PfnNotify Error: %s\n", errInfo);
}

/**
 * Finds an OpenCL device matching the specified criteria and creates a context
 *
 * @param desiredDeviceVendorId Preferred vendor ID (0 for any)
 * @param deviceType Type of device to find (GPU/CPU)
 * @param desiredPlatformName Name of preferred platform
 * @return true if suitable device found and context created
 */
bool CLModel::FindDeviceAndCreateContext(cl_uint desiredDeviceVendorId, cl_device_type deviceType, char *desiredPlatformName)
{
#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::FindDeviceAndCreateContext threadId: %ld"), wxThread::GetCurrentId());
#endif
  bool success = false;
  cl_int status = CL_SUCCESS;
  cl_uint numPlatforms;
  cl_platform_id platform = NULL;
  char *thePlatformName = NULL;
  cl_platform_id *platforms = NULL;
  cl_device_id *deviceIds = NULL;
  char *extensions = NULL;
  char *deviceName = NULL;
  char *deviceCLVersion = NULL;
  cl_device_id *contextDeviceIds = NULL;

  try
  {
    // Get a list of platforms
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetPlatformIDs failed to get number of platforms %s"), this->ErrorMessage(status));
      throw status;
    }

    if (numPlatforms <= 0)
    {
      wxLogError(wxT("number of platforms is %d"), numPlatforms);
      throw CL_DEVICE_NOT_FOUND;
    }

    wxLogDebug(wxT("Found %d platforms"), numPlatforms);

    platforms = new cl_platform_id[numPlatforms];

    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetPlatformIDs failed to get platforms %s"), this->ErrorMessage(status));
      throw status;
    }

    // Search every platform for a suitable device.
    // If desiredDeviceVendorId is set, the device must also have that vendor id.
    // Otherwise, if desiredPlatformName is set, only that platform (CL_PLATFORM_VENDOR) is searched.
    bool foundDevice = false;
    for (unsigned i = 0; i < numPlatforms && !foundDevice; ++i)
    {
      platform = platforms[i];

      size_t platformNameSize;
      status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 0, NULL, &platformNameSize);
      if (status != CL_SUCCESS)
      {
        wxLogError(wxT("clGetPlatformInfo failed to get CL_PLATFORM_VENDOR size %s"), this->ErrorMessage(status));
        continue;
      }

      delete[] thePlatformName;
      thePlatformName = new char[platformNameSize / sizeof(char)];
      status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, platformNameSize, thePlatformName, NULL);
      if (status != CL_SUCCESS)
      {
        wxLogError(wxT("clGetPlatformInfo failed to get CL_PLATFORM_VENDOR %s"), this->ErrorMessage(status));
        continue;
      }

      wxString platformVendor(thePlatformName, wxConvUTF8);
      wxLogDebug(wxT("Platform %u: %s"), i, platformVendor);

      // is this the platform we are looking for
      if (desiredDeviceVendorId == 0 && desiredPlatformName != NULL)
      {
        if (strcmp(thePlatformName, desiredPlatformName) != 0)
        {
          wxLogDebug(wxT("  skipped, not the requested platform"));
          continue;
        }
      }

      // Get the id's of the devices under this platform
      cl_uint numberOfDevices = 0;
      status = clGetDeviceIDs(platform, deviceType, 0, NULL, &numberOfDevices);
      if (status != CL_SUCCESS || numberOfDevices == 0)
      {
        if (status != CL_SUCCESS && status != CL_DEVICE_NOT_FOUND)
        {
          wxLogError(wxT("clGetDeviceIDs failed to get number of devices %s"), this->ErrorMessage(status));
        }
        wxLogDebug(wxT("  no devices of the requested type"));
        status = CL_SUCCESS;
        continue;
      }

      wxLogDebug(wxT("  %u devices of the requested type"), numberOfDevices);

      deviceIds = new cl_device_id[numberOfDevices];
      status = clGetDeviceIDs(platform, deviceType, numberOfDevices, deviceIds, NULL);
      if (status != CL_SUCCESS)
      {
        wxLogError(wxT("clGetDeviceIDs failed to get devices %s"), this->ErrorMessage(status));
        delete[] deviceIds;
        deviceIds = NULL;
        continue;
      }

      // Now search for the device
      for (unsigned j = 0; j < numberOfDevices; ++j)
      {
        cl_uint deviceVendorId;
        status = clGetDeviceInfo(deviceIds[j], CL_DEVICE_VENDOR_ID, sizeof(deviceVendorId), &deviceVendorId, NULL);
        if (status != CL_SUCCESS)
        {
          wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_VENDOR_ID %s"), this->ErrorMessage(status));
          continue;
        }

        wxLogDebug(wxT("  Device %u: %s vendor id 0x%X"), j, GetDeviceName(deviceIds[j]), (unsigned int)deviceVendorId);

        if (desiredDeviceVendorId != 0 && deviceVendorId != desiredDeviceVendorId)
        {
          wxLogDebug(wxT("    skipped, not vendor id 0x%X"), (unsigned int)desiredDeviceVendorId);
          continue;
        }

        if (!this->IsDeviceSuitable(deviceIds[j]))
        {
          continue;
        }

        wxLogDebug(wxT("    selected"));
        this->deviceId = deviceIds[j];
        this->deviceVendorId = deviceVendorId;
        this->platformName = platformVendor;
        foundDevice = true;
        break;
      }

      delete[] deviceIds;
      deviceIds = NULL;
    }

    delete[] thePlatformName;
    thePlatformName = NULL;

    delete[] platforms;
    platforms = NULL;

    if (!foundDevice)
    {
      throw -1;
    }

    size_t deviceNameSize;
    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_NAME, 0, NULL, &deviceNameSize);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_NAME %s"), this->ErrorMessage(status));
      throw status;
    }

    // Get the device name
    deviceName = new char[deviceNameSize / sizeof(char)];
    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_NAME, deviceNameSize, deviceName, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_NAME %s"), this->ErrorMessage(status));
      throw status;
    }

    this->deviceName = wxString(deviceName, wxConvUTF8);
    delete[] deviceName;
    deviceName = NULL;

    size_t deviceCLVersionSize;
    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_VERSION, 0, NULL, &deviceCLVersionSize);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_VERSION %s"), this->ErrorMessage(status));
      throw status;
    }

    // Get the device name
    deviceCLVersion = new char[deviceCLVersionSize / sizeof(char)];
    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_VERSION, deviceCLVersionSize, deviceCLVersion, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_VERSION %s"), this->ErrorMessage(status));
      throw status;
    }

    this->deviceCLVersion = wxString(deviceCLVersion, wxConvUTF8);
    delete[] deviceCLVersion;
    deviceCLVersion = NULL;

    // Format of CL_DEVICE_VERSION is OpenCL<space><major_version.minor_version><space><vendor-specific information>
    // Parse it and extract the major.minor version number as a double
    if (this->deviceCLVersion.length() > 0)
    {
      wxStringTokenizer lineTokenizer(this->deviceCLVersion);
      if (lineTokenizer.HasMoreTokens())
      {
        // Skip "OpenCL"
        lineTokenizer.GetNextToken();
        if (lineTokenizer.HasMoreTokens())
        {
          wxString version = lineTokenizer.GetNextToken();

          if (!version.ToDouble(&(this->deviceCLVersionNumber)))
          {
            wxLogDebug(wxT("Invalid version number %s"), version);
            this->deviceCLVersionNumber = 1.2;
          }
        }
        else
        {
          wxLogDebug(wxT("Expected <major_version.minor_version> in CL_DEVICE_VERSION"));
        }
      }
      else
      {
        wxLogDebug(wxT("Expected OpenCL<space> in CL_DEVICE_VERSION"));
      }
    }

    wxLogDebug(wxT("OpenCL version read as:%f"), this->deviceCLVersionNumber);

    // this->deviceCLVersionNumber = 1.1;

    // Get Extensions
    this->gotKhrFp64 = false;
    this->gotAmdFp64 = false;
    this->gotKhrGlSharing = false;
    this->gotAppleGlSharing = false;

    size_t extensionsSize;
    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionsSize);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_EXTENSIONS size %s"), this->ErrorMessage(status));
      throw status;
    }

    extensions = new char[extensionsSize / sizeof(char)];
    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_EXTENSIONS, extensionsSize, extensions, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_EXTENSIONS %s"), this->ErrorMessage(status));
      throw status;
    }

    wxString deviceExtensions = wxString(extensions);

    if (deviceExtensions.Contains(wxT("cl_khr_gl_sharing")))
    {
      this->gotKhrGlSharing = true;
    }

    if (deviceExtensions.Contains(wxT("cl_APPLE_gl_sharing")))
    {
      this->gotAppleGlSharing = true;
    }

    if (deviceExtensions.Contains(wxT("cl_khr_fp64")))
    {
      this->gotKhrFp64 = true;
    }

    if (deviceExtensions.Contains(wxT("cl_amd_fp64")))
    {
      this->gotAmdFp64 = true;
    }

    delete[] extensions;
    extensions = NULL;

    // see http://www.dyn-lab.com/articles/cl-gl.html
#ifdef _WIN32
    cl_context_properties properties[] =
        {
            CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
            0};
#endif

#ifdef __linux__
    cl_context_properties properties[] =
        {
            CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
            CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
            0};
#endif

#ifdef __APPLE__
    CGLContextObj glContext = CGLGetCurrentContext();
    CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);

    cl_context_properties properties[] =
        {
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)shareGroup,
            0};
#endif

    this->context = clCreateContext(properties, 1, &(this->deviceId), &PfnNotify, NULL, &status);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clCreateContext failed %s"), this->ErrorMessage(status));
      throw status;
    }

    size_t contextDeviceListSize;
    status = clGetContextInfo(this->context, CL_CONTEXT_DEVICES, 0, NULL, &contextDeviceListSize);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetContextInfo failed to get CL_CONTEXT_DEVICES size %s"), this->ErrorMessage(status));
      throw status;
    }

    unsigned int contextNumberOfDevices = (int)(contextDeviceListSize / sizeof(cl_device_id));
    contextDeviceIds = new cl_device_id[contextNumberOfDevices];
    if (contextDeviceIds == NULL)
    {
      wxLogError(wxT("Failed to allocate memory for devices %d"), contextNumberOfDevices);
      throw -1;
    }

    status = clGetContextInfo(this->context, CL_CONTEXT_DEVICES, contextDeviceListSize, contextDeviceIds, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetContextInfo failed to get CL_CONTEXT_DEVICES %s"), this->ErrorMessage(status));
      throw status;
    }

    bool deviceOk = false;
    for (unsigned j = 0; j < contextNumberOfDevices; ++j)
    {
      if (this->deviceId == contextDeviceIds[j])
      {
        deviceOk = true;
        break;
      }
    }

    delete[] contextDeviceIds;
    contextDeviceIds = NULL;

    if (!deviceOk)
    {
      wxLogError(wxT("desired device is not suitable"));
      throw -1;
    }

    if (this->deviceCLVersionNumber >= 2.0)
    {
      cl_queue_properties properties[] = {
          CL_QUEUE_PROPERTIES, 0, // No special properties
          0                       // Terminating zero
      };
      this->commandQueue = clCreateCommandQueueWithProperties(this->context, this->deviceId, properties, &status);
    }
    else
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
      this->commandQueue = clCreateCommandQueue(this->context, this->deviceId, 0, &status);
#pragma GCC diagnostic pop
    }

    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("Command queue creation failed failed %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), (void *)&this->maxWorkGroupSize, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_WORK_GROUP_SIZE %s"), this->ErrorMessage(status));
      throw status;
    }

    if (this->groupSize > this->maxWorkGroupSize)
    {
      this->groupSize = this->maxWorkGroupSize;
    }

    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), (void *)&this->maxDimensions, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS %s"), this->ErrorMessage(status));
      throw status;
    }

    size_t maxWorkItemSizesSize = maxDimensions * sizeof(size_t);

    delete[] this->maxWorkItemSizes;
    this->maxWorkItemSizes = new size_t[maxDimensions];

    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, maxWorkItemSizesSize, (void *)this->maxWorkItemSizes, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), (void *)&this->totalLocalMemory, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_LOCAL_MEM_SIZE %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), (void *)&this->maxConstantBufferSize, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE %s"), this->ErrorMessage(status));
      throw status;
    }

    this->maxNumGrav = this->maxConstantBufferSize / sizeof(cl_double4);
    wxLogDebug(wxT("max grav particles %d"), this->maxNumGrav);
    if (this->numGrav > this->maxNumGrav)
    {
      this->numGrav = this->maxNumGrav;
    }

    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), (void *)&this->globalMemorySize, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_GLOBAL_MEM_SIZE %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clGetDeviceInfo(this->deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), (void *)&this->maxMemoryAlloc, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_MAX_MEM_ALLOC_SIZE %s"), this->ErrorMessage(status));
      throw status;
    }

    int maxHistory = this->maxMemoryAlloc / (16 * sizeof(cl_double4));
    int maxGlobal = this->globalMemorySize / ((2 * 16 * sizeof(cl_double4)) + (4 * sizeof(cl_double4)));
    this->maxNumParticles = maxGlobal < maxHistory ? maxGlobal : maxHistory;
    wxLogDebug(wxT("max history particles %d"), maxHistory);
    wxLogDebug(wxT("max global particles %d"), maxGlobal);

    success = true;
  }
  catch (int ex)
  {
    delete[] thePlatformName;
    delete[] platforms;
    delete[] deviceIds;
    delete[] extensions;
    delete[] deviceName;
    delete[] deviceCLVersion;
    delete[] contextDeviceIds;
    success = false;
  }

  wxLogDebug(wxT("Finished CLModel::FindDeviceAndCreateContext"));
  return success;
}

bool CLModel::IsDeviceSuitable(cl_device_id deviceIdToCheck)
{
  // Get Extensions
  bool deviceHasKhrFp64 = false;
  bool deviceHasAmdFp64 = false;
  bool deviceHasKhrGlSharing = false;
  bool deviceHasAppleGlSharing = false;
  cl_int status = CL_SUCCESS;
  char *extensions = NULL;
  bool isSuitable = false;

  try
  {
    size_t extensionsSize;
    status = clGetDeviceInfo(deviceIdToCheck, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionsSize);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_EXTENSIONS size %s"), this->ErrorMessage(status));
      throw status;
    }

    extensions = new char[extensionsSize / sizeof(char)];
    status = clGetDeviceInfo(deviceIdToCheck, CL_DEVICE_EXTENSIONS, extensionsSize, extensions, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_EXTENSIONS %s"), this->ErrorMessage(status));
      throw status;
    }

    wxString deviceExtensions = wxString(extensions);

    if (deviceExtensions.Contains(wxT("cl_khr_gl_sharing")))
    {
      deviceHasKhrGlSharing = true;
    }

    if (deviceExtensions.Contains(wxT("cl_APPLE_gl_sharing")))
    {
      deviceHasAppleGlSharing = true;
    }

    if (deviceExtensions.Contains(wxT("cl_khr_fp64")))
    {
      deviceHasKhrFp64 = true;
    }

    if (deviceExtensions.Contains(wxT("cl_amd_fp64")))
    {
      deviceHasAmdFp64 = true;
    }

    delete[] extensions;
    extensions = NULL;

    // Check for double precision
    cl_uint preferedVectorWidthDouble;
    status = clGetDeviceInfo(deviceIdToCheck, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(preferedVectorWidthDouble), &preferedVectorWidthDouble, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clGetDeviceInfo failed to get CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE %s"), this->ErrorMessage(status));
      throw status;
    }

    bool hasGlSharing = deviceHasKhrGlSharing || deviceHasAppleGlSharing;
    bool hasFp64 = preferedVectorWidthDouble != 0 || deviceHasAmdFp64 || deviceHasKhrFp64;
    if (hasGlSharing && hasFp64)
    {
      isSuitable = true;
    }
    else
    {
      wxLogDebug(wxT("    skipped, missing%s%s"), hasGlSharing ? wxT("") : wxT(" OpenGL sharing"), hasFp64 ? wxT("") : wxT(" double precision"));
    }
  }
  catch (int ex)
  {
    // A device that can't be queried is skipped rather than ending the search
    delete[] extensions;
    wxLogDebug(wxT("    skipped, device query failed"));
    isSuitable = false;
  }

  return isSuitable;
}

// The device name (CL_DEVICE_NAME), for logging
wxString CLModel::GetDeviceName(cl_device_id deviceIdToName)
{
  size_t nameSize = 0;
  if (clGetDeviceInfo(deviceIdToName, CL_DEVICE_NAME, 0, NULL, &nameSize) != CL_SUCCESS || nameSize == 0)
  {
    return wxT("(unknown)");
  }

  char *name = new char[nameSize];
  wxString result = wxT("(unknown)");
  if (clGetDeviceInfo(deviceIdToName, CL_DEVICE_NAME, nameSize, name, NULL) == CL_SUCCESS)
  {
    result = wxString(name, wxConvUTF8);
  }
  delete[] name;
  return result.Trim().Trim(false);
}

void CLModel::CreateBufferObjects(GLuint *vbo, int numParticles, int numGrav)
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::CreateBufferObjects threadId: %ld"), wxThread::GetCurrentId());
#endif

  cl_int status = CL_SUCCESS;
  this->step = 0;
  this->stage = this->numStages;
  this->numParticles = (cl_int)numParticles;
  this->numGrav = numGrav;
  if (this->numGrav > this->numParticles)
  {
    wxLogDebug(wxT("numGrav %d reduced to the number of bodies %d"), this->numGrav, this->numParticles);
    this->numGrav = this->numParticles;
  }

  // Create cl_mem objects
  // Get an openCL buffer to the openGL Vertex Array of points.
  // We aquire this and then copy the simulation positions to it to update the on screen positions.
  this->dispPos = clCreateFromGLBuffer(this->context, CL_MEM_WRITE_ONLY, vbo[0], &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateFromGLBuffer failed to create cl_mem object for GL vertex buffer object %s"), this->ErrorMessage(status));
    throw status;
  }

  // The current positions of the solar system bodies.
  this->currPos = clCreateBuffer(this->context, CL_MEM_READ_ONLY, this->numParticles * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for currPos %s"), this->ErrorMessage(status));
    throw status;
  }

  // Contains the new positions computed by the integration from current positions.
  this->newPos = clCreateBuffer(this->context, CL_MEM_WRITE_ONLY, this->numParticles * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for newPos %s"), this->ErrorMessage(status));
    throw status;
  }

  // This buffer contains a copy of the current positions for the bodys for which we are including gravitatoinal effects.
  // This buffer is small enough to fit into the on chip memory cache of the GPU
  // It is marked as read only for the kernels and of memory type __constant
  // My understanding is that after the first numGrav positions are read the should all be cached on chip.
  this->gravPos = clCreateBuffer(this->context, CL_MEM_READ_ONLY, this->numGrav * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for gravPos %s"), this->ErrorMessage(status));
    throw status;
  }

  // The current velocities of the solar system bodies.
  this->currVel = clCreateBuffer(this->context, CL_MEM_READ_ONLY, this->numParticles * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for currVel %s"), this->ErrorMessage(status));
    throw status;
  }

  // Contains the new velocities computed by the integration from current velocities.
  this->newVel = clCreateBuffer(this->context, CL_MEM_WRITE_ONLY, this->numParticles * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for newVel %s"), this->ErrorMessage(status));
    throw status;
  }

  // contains the gravitational accerations computed from the current positions by the acceleration kernel
  this->acc = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->numParticles * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for acc %s"), this->ErrorMessage(status));
    throw status;
  }

  // This is used to hold the current position at the start of the Adams Bashforth Intgration for use by the Adams Moulton Inegrator.
  // i.e. between the AB and AM integrations the current position holds the estimated position. But the AM still needs the position from the start
  this->posLast = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->numParticles * 1 * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for posLast %s"), this->ErrorMessage(status));
    throw status;
  }

  // This is used to hold the current velocities at the start of the Adams Bashforth Intgration for use by the Adams Moulton Integrator.
  this->velLast = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->numParticles * 1 * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for velLast %s"), this->ErrorMessage(status));
    throw status;
  }

  // 16 element ring buffer used to store the previous steps velocities.
  // e.g the velocity for the previous step is stored at index (step-1)&0xf
  this->velHistory = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->numParticles * 16 * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for velHistory %s"), this->ErrorMessage(status));
    throw status;
  }

  // 16 element ring buffer used to store the previous steps accerlerations.
  // e.g the accerleration for the previous step is stored at index (step-1)&0xf
  this->accHistory = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->numParticles * 16 * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for accHistory %s"), this->ErrorMessage(status));
    throw status;
  }

  // RK4 startup accumulators for k1 + 2k2 + 2k3 (position and velocity derivatives)
  this->rkSumVel = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->numParticles * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for rkSumVel %s"), this->ErrorMessage(status));
    throw status;
  }

  this->rkSumAcc = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->numParticles * sizeof(cl_double4), 0, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateBuffer failed to create cl_mem object for rkSumAcc %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("Finished CLModel::CreateBufferObjects"));
}

void CLModel::CompileProgramAndCreateKernels()
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::CompileProgramAndCreateKernels threadId: %ld"), wxThread::GetCurrentId());
#endif

  cl_int status = CL_SUCCESS;
  // Replace file loading with embedded source
  wxString nbodySource = wxString(Kernels::adamsfma, wxConvUTF8);

  wxString programSource;
  if (this->gotKhrGlSharing && !this->gotAmdFp64)
  {
    programSource.Append(wxT("#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable \r\n"));
  }

  if (this->gotAppleGlSharing)
  {
    programSource.Append(wxT("#pragma OPENCL EXTENSION cl_APPLE_gl_sharing : enable \r\n"));
  }

  if (this->gotKhrFp64)
  {
    programSource.Append(wxT("#pragma OPENCL EXTENSION cl_khr_fp64 : enable \r\n"));
  }
  else if (this->gotAmdFp64)
  {
    programSource.Append(wxT("#pragma OPENCL EXTENSION cl_amd_fp64 : enable \r\n"));
  }

  programSource.Append(nbodySource);

  const char *source = programSource.c_str();
  size_t sourceSize[] = {strlen(source)};

  // setup a openCL program to hold the program
  this->program = clCreateProgramWithSource(this->context, 1, &source, sourceSize, &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateProgramWithSource failed %s"), this->ErrorMessage(status));
    throw status;
  }

  // compile the program (kernels)
  const char *options = "-cl-mad-enable"; // -cl-fast-relaxed-math";// "-cl-mad-enable -cl-fast-relaxed-math -cl-nv-verbose ";
  status = clBuildProgram(this->program, 1, &this->deviceId, options, NULL, NULL);
  if (status != CL_SUCCESS)
  {
    // if it failed to compile then obtain the compile log and display it in an error dialog
    wxLogError(wxT("clBuildProgram failed %s"), this->ErrorMessage(status));
    if (status == CL_BUILD_PROGRAM_FAILURE)
    {
      // Determine the size of the log
      size_t log_size;
      clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

      // Allocate memory for the log
      char *log = new char[log_size + 1];

      // Get the log
      clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
      log[log_size] = 0;
      wxLogError(wxT("%s"), wxString(log, wxConvUTF8));
      delete[] log;
    }
    throw status;
  }

  // if we are debugging then include the compile log
#ifdef __WXDEBUG__
  size_t log_size;
  status = clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clGetProgramBuildInfo failed %s"), this->ErrorMessage(status));
    throw status;
  }
  // Allocate memory for the log
  char *log = new char[log_size + 1];

  // Get the log
  status = clGetProgramBuildInfo(this->program, this->deviceId, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
  if (status != CL_SUCCESS)
  {
    delete[] log;
    wxLogError(wxT("clGetProgramBuildInfo failed %s"), this->ErrorMessage(status));
    throw status;
  }
  log[log_size] = 0;
  wxLogDebug(wxT("%s"), wxString(log, wxConvUTF8));
  delete[] log;
#endif

  // setup kernels (pointers?) to required compiled kernels
  this->accKernel = clCreateKernel(this->program, this->accelerationKernelName.c_str(), &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateKernel failed %s"), this->ErrorMessage(status));
    throw status;
  }

  this->startupKernel = clCreateKernel(this->program, "rungeKutta4Startup", &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateKernel rungeKutta4Startup failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("Using adamsBashforthKernel %s"), this->adamsBashforthKernelName.c_str());
  this->adamsBashforthKernel = clCreateKernel(this->program, this->adamsBashforthKernelName.c_str(), &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateKernel adamsBashforthKernel failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("Using adamsKernel %s"), this->adamsMoultonKernelName.c_str());
  this->adamsMoultonKernel = clCreateKernel(this->program, this->adamsMoultonKernelName.c_str(), &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateKernel adamsMoultonKernel failed %s"), this->ErrorMessage(status));
    throw status;
  }

  this->copyToDisplayKernel = clCreateKernel(this->program, "copyToDisplay", &status);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clCreateKernel copyToDisplay failed %s"), this->ErrorMessage(status));
    throw status;
  }

  this->initialisedOk = true;
  wxLogDebug(wxT("Finished CLModel:CompileProgramAndCreateKernels"));
}

// Advances the simulation.
// For the first 16 steps each call takes one full step of delT with the RK4 startup.
// After that each call runs one stage (Adams-Bashforth predictor or Adams-Moulton corrector)
// of the current step.
void CLModel::ExecuteKernels()
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::ExecuteKernel threadId: %ld"), wxThread::GetCurrentId());
#endif

  cl_int status = CL_SUCCESS;

  if (!this->initialisedOk)
  {
    wxLogDebug(wxT("Aborted CLModel failed to Initialise"));
    throw -1;
  }

  if (this->groupSize > this->maxWorkItemSizes[0] || this->groupSize > this->maxWorkGroupSize)
  {
    wxLogError(wxT("groupSize %llu is greater than maxWorkItemSizes %llu or maxWorkGroupSize %llu"), (unsigned long long)this->groupSize, (unsigned long long)this->maxWorkItemSizes[0], (unsigned long long)this->maxWorkGroupSize);
    throw -1;
  }

  status = clFinish(this->commandQueue);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clFinish failed %s"), this->ErrorMessage(status));
    throw status;
  }

  bool stepCompleted = false;

  if (this->step < 16)
  {
    // The 16 element Adams history ring buffer is not full yet.
    // Take this whole step with sub-stepped RK4, which also records its history entry.
    wxLogDebug(wxT("CLModel:Using startupKernel"));
    this->ExecuteStartupStep();
    this->stage = this->numStages;
    stepCompleted = true;
  }
  else
  {
    // Acceleration at the current (start of step, or predicted) positions
    this->EnqueueKernel(this->accKernel, wxT("acceleration"));
    this->EnqueueBarrier();

    // The Integration has two stages
    // The first, Adams Bashford Integration, computes an initial estimate of the next position and velocity
    // The second,Adams Moulton Integration, takes the estimates, combined with the acceration at estimate position
    // and compute a more exact position and velocity
    // The first stage is also called the Predictor stage, the second stage the corrector stage.
    // Note that it is possible to iterate the corrector stage to increase the accurarcy.
    cl_kernel adamsKernel = (this->stage == 1) ? this->adamsBashforthKernel : this->adamsMoultonKernel;

    status = clSetKernelArg(adamsKernel, 6, sizeof(cl_int), (void *)&this->stage);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clSetKernelArg 6 adams kernel failed for stage %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clSetKernelArg(adamsKernel, 7, sizeof(cl_int), (void *)&this->step);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clSetKernelArg 7 adams kernel failed for step %s"), this->ErrorMessage(status));
      throw status;
    }

    this->EnqueueKernel(adamsKernel, (this->stage == 1) ? wxT("adamsBashforth") : wxT("adamsMoulton"));
    this->EnqueueBarrier();
    this->EnqueueCopyNewToCurrent();
    this->EnqueueBarrier();

    this->stage = this->stage - 1;
    if (this->stage < 0)
    {
      // if we just finished the corrector stage then advance to the next step (time)
      this->stage = this->numStages;
      stepCompleted = true;
    }
  }

  if (stepCompleted)
  {
    this->time += this->delT;
    this->step++;

    if (this->updateDisplay)
    {
      this->updateDisplay = !this->updateDisplay;
      this->UpdateDisplay();
    }
  }

  wxLogDebug(wxT("CLModel:ExecuteKernel Done"));
}

// Takes one full step of delT as startupSubSteps sub-steps of classic 4th order Runge-Kutta.
// Used for steps 0..15 to seed the Adams history ring buffer: rungeKutta4Startup records the
// velocity and acceleration at the start of the step in velHistory/accHistory[step & 0xF].
// Each RK4 stage needs the acceleration at an intermediate state, so for every stage the
// intermediate state is copied to currPos/gravPos/currVel and the acceleration kernel re-run.
void CLModel::ExecuteStartupStep()
{
  cl_int status = clSetKernelArg(this->startupKernel, 7, sizeof(cl_int), (void *)&this->step);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 7 startupKernel failed for step %s"), this->ErrorMessage(status));
    throw status;
  }

  for (cl_int subStep = 0; subStep < this->startupSubSteps; subStep++)
  {
    status = clSetKernelArg(this->startupKernel, 13, sizeof(cl_int), (void *)&subStep);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clSetKernelArg 13 startupKernel failed for subStep %s"), this->ErrorMessage(status));
      throw status;
    }

    for (cl_int rkStage = 0; rkStage < 4; rkStage++)
    {
      this->EnqueueKernel(this->accKernel, wxT("acceleration"));
      this->EnqueueBarrier();

      status = clSetKernelArg(this->startupKernel, 6, sizeof(cl_int), (void *)&rkStage);
      if (status != CL_SUCCESS)
      {
        wxLogError(wxT("clSetKernelArg 6 startupKernel failed for rkStage %s"), this->ErrorMessage(status));
        throw status;
      }

      this->EnqueueKernel(this->startupKernel, wxT("rungeKutta4Startup"));
      this->EnqueueBarrier();
      this->EnqueueCopyNewToCurrent();
      this->EnqueueBarrier();
    }
  }
}

// The number of work-items to launch: numParticles rounded up to a whole number of
// work-groups. The kernels ignore the work-items past the last body.
size_t CLModel::GlobalWorkSize()
{
  return ((size_t)this->numParticles + this->groupSize - 1) / this->groupSize * this->groupSize;
}

// Enqueues a kernel over all the particles and flushes the queue
void CLModel::EnqueueKernel(cl_kernel kernel, const wxChar *kernelName)
{
  size_t globalThreads[] = {this->GlobalWorkSize()};
  size_t localThreads[] = {this->groupSize};

  cl_int status = clEnqueueNDRangeKernel(this->commandQueue, kernel, 1, NULL, globalThreads, localThreads, 0, 0, NULL);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueNDRangeKernel %s failed %s"), kernelName, this->ErrorMessage(status));
    throw status;
  }

  status = clFlush(this->commandQueue);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clFlush failed %s"), this->ErrorMessage(status));
    throw status;
  }
}

void CLModel::EnqueueBarrier()
{
  cl_int status;
  if (this->deviceCLVersionNumber >= 1.2)
  {
    status = clEnqueueBarrierWithWaitList(this->commandQueue, 0, NULL, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clEnqueueBarrierWithWaitList failed %s"), this->ErrorMessage(status));
      throw status;
    }
  }
  else
  {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    status = clEnqueueBarrier(this->commandQueue);
#pragma GCC diagnostic pop
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clEnqueueBarrier failed %s"), this->ErrorMessage(status));
      throw status;
    }
  }
}

// Makes the integrator output (newPos/newVel) the current state
void CLModel::EnqueueCopyNewToCurrent()
{
  // Copy new positions to current position
  cl_int status = clEnqueueCopyBuffer(commandQueue, this->newPos, this->currPos, 0, 0, sizeof(cl_double4) * this->numParticles, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueCopyBuffer newPos to currPos failed %s"), this->ErrorMessage(status));
    throw status;
  }

  // Copy new positions of the bodies with mass to gravPos
  status = clEnqueueCopyBuffer(commandQueue, this->newPos, this->gravPos, 0, 0, sizeof(cl_double4) * this->numGrav, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueCopyBuffer newPos to gravPos failed %s"), this->ErrorMessage(status));
    throw status;
  }

  // Copy new velocities to current velocities
  status = clEnqueueCopyBuffer(commandQueue, this->newVel, this->currVel, 0, 0, sizeof(cl_double4) * this->numParticles, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueCopyBuffer newVel to currVel failed %s"), this->ErrorMessage(status));
    throw status;
  }
}

// Restarts the integration from the current state, e.g. after the time step has changed.
// The Adams history is then rebuilt by the RK4 startup over the next 16 steps.
// If called between the predictor and corrector stages, currPos/currVel hold the
// uncorrected predicted state, so the state at the start of the step (saved in
// posLast/velLast by the Adams-Bashforth kernel) is restored first.
// time only advances once a step completes, so it still matches the restored state.
void CLModel::RestartIntegration()
{
  if (this->initialisedOk && this->step >= 16 && this->stage != this->numStages)
  {
    wxLogDebug(wxT("CLModel::RestartIntegration restoring start of step state"));

    cl_int status = clFinish(this->commandQueue);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clFinish failed %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clEnqueueCopyBuffer(commandQueue, this->posLast, this->currPos, 0, 0, sizeof(cl_double4) * this->numParticles, 0, 0, 0);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clEnqueueCopyBuffer posLast to currPos failed %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clEnqueueCopyBuffer(commandQueue, this->posLast, this->gravPos, 0, 0, sizeof(cl_double4) * this->numGrav, 0, 0, 0);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clEnqueueCopyBuffer posLast to gravPos failed %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clEnqueueCopyBuffer(commandQueue, this->velLast, this->currVel, 0, 0, sizeof(cl_double4) * this->numParticles, 0, 0, 0);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clEnqueueCopyBuffer velLast to currVel failed %s"), this->ErrorMessage(status));
      throw status;
    }

    status = clFinish(this->commandQueue);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clFinish failed %s"), this->ErrorMessage(status));
      throw status;
    }
  }

  this->step = 0;
  this->stage = this->numStages;
}

// Aquire the GL points buffer and then copy the positions to it
void CLModel::UpdateDisplay()
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::UpdateDisplay threadId: %ld"), wxThread::GetCurrentId());
#endif

  cl_int status = CL_SUCCESS;

  if (!this->initialisedOk)
  {
    wxLogDebug(wxT("Aborted CLModel failed to Initialise"));
    throw -1;
  }

  // TODO this should be per kernel. Not the lowest that works for all of them
  size_t globalThreads[] = {this->GlobalWorkSize()};
  size_t localThreads[] = {this->groupSize};

  if (localThreads[0] > this->maxWorkItemSizes[0] || localThreads[0] > this->maxWorkGroupSize)
  {
    wxLogError(wxT("groupSize %llu is greater than maxWorkItemSizes %llu or maxWorkGroupSize %llu"), (unsigned long long)this->groupSize, (unsigned long long)this->maxWorkItemSizes[0], (unsigned long long)this->maxWorkGroupSize);
    throw -1;
  }

  // Execute acceleration kernel on given device
  // glFinish();

  wxLogDebug(wxT("CLModel::UpdateDisplay clEnqueueAcquireGLObjects()"));

  // cl_event  eventND[1];
  //  Acquire GL buffer
  status = clEnqueueAcquireGLObjects(this->commandQueue, 1, &this->dispPos, 0, 0, NULL);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueAcquireGLObjects failed to acquire dispPos %s"), this->ErrorMessage(status));
    throw status;
  }

  // Copy new positions to the display vertex buffer
  // update the copyToDisplayKernel's centerBody argument so it knows which body to offset the posistions against
  status = clSetKernelArg(this->copyToDisplayKernel, 3, sizeof(cl_int), (void *)&this->centerBody);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 3 failed for centerBody %s"), this->ErrorMessage(status));
    throw status;
  }

  // use reuse event?
  status = clEnqueueNDRangeKernel(this->commandQueue, this->copyToDisplayKernel, 1, NULL, globalThreads, localThreads, 0, 0, NULL);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueNDRangeKernel copyToDisplayKernel failed %s"), this->ErrorMessage(status));
    throw status;
  }

  // Release GL buffer
  status = clEnqueueReleaseGLObjects(this->commandQueue, 1, &this->dispPos, 0, 0, NULL);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueReleaseGLObjects failed to release dispPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clFinish(this->commandQueue);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clFinish failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("CLModel::UpdateDisplay clFinish()"));
  wxLogDebug(wxT("CLModel:UpdateDisplay Done"));
}

// initialise the kernels so they are ready to be called.
void CLModel::SetKernelArgumentsAndGroupSize()
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::SetKernelArgumentsAndGroupSize threadId: %ld"), wxThread::GetCurrentId());
#endif

  if (!this->initialisedOk)
  {
    wxLogDebug(wxT("Aborted CLModel failed to Initialise"));
    throw -1;
  }

  cl_int status;
  int paramNumber = 0;

  /*
    __kernel
    void copyToDisplay(
      __constant double4* pos,
      __global double4* dispPos,
      int centerBodyIndex,
      int numParticles
    )
  */
  status = clSetKernelArg(this->copyToDisplayKernel, paramNumber++, sizeof(cl_mem), (void *)&this->gravPos);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for gravPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->copyToDisplayKernel, paramNumber++, sizeof(cl_mem), (void *)&this->currPos);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for currPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->copyToDisplayKernel, paramNumber++, sizeof(cl_mem), (void *)&this->dispPos);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for dispPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->copyToDisplayKernel, paramNumber++, sizeof(cl_int), (void *)&this->centerBody);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for centerBody %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->copyToDisplayKernel, paramNumber++, sizeof(cl_int), (void *)&this->numParticles);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for numParticles %s"), this->ErrorMessage(status));
    throw status;
  }

  /* Set appropriate arguments to the kernel
  __kernel void computeAcc(
     __constant double4* gravPos,
     __global double4* pos,
     int numGrav,
     double epsSqr,
     __global double4* acc,
     [__local double4* localGravPos,]  (relativisticLocal only)
     int numParticles)
  */
  paramNumber = 0;
  status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_mem), (void *)&this->gravPos);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for gravPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_mem), (void *)&this->currPos);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for currPos %s"), this->ErrorMessage(status));
    throw status;
  }

  // the acceleration kernel that includes relativistic corrections need the relativistic parameter stored
  // in .w. We pass the whole velocity vector in case it can be used in a more complicated relativistic on MOND type kernel
  if (!this->accelerationKernelName.IsSameAs(wxT("newtonian"), false))
  {
    status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_mem), (void *)&this->currVel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clSetKernelArg failed for currVel %s"), this->ErrorMessage(status));
      throw status;
    }
  }

  status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_int), (void *)&this->numGrav);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for numGrav %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_double), (void *)&this->espSqr);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for espSqr %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_mem), (void *)&this->acc);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for acc %s"), this->ErrorMessage(status));
    throw status;
  }

  if (this->accelerationKernelName.IsSameAs(wxT("relativisticLocal"), false))
  {
    status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_double4) * this->groupSize, NULL);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clSetKernelArg failed for localGravPos %s"), this->ErrorMessage(status));
      throw status;
    }
  }

  status = clSetKernelArg(this->accKernel, paramNumber++, sizeof(cl_int), (void *)&this->numParticles);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg failed for numParticles %s"), this->ErrorMessage(status));
    throw status;
  }

  // set integration kernel args
  SetAdamsKernelArgs(this->startupKernel);
  SetStartupKernelArgs();
  SetAdamsKernelArgs(this->adamsBashforthKernel);
  SetAdamsKernelArgs(this->adamsMoultonKernel);

  // I'm a bit paranoid here. I set the group size to the smallest that will work for all the kernels
  // perhap each kernel should use the largest group size possible for it. But I don't do that.
  /* Check group size against group size returned by kernel */
  status = clGetKernelWorkGroupInfo(this->accKernel, this->deviceId, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &this->accKernelWorkGroupSize, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("getting accKernel kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("accKernel Work Group Size %llu"), (unsigned long long)this->accKernelWorkGroupSize);

  if (this->groupSize > this->accKernelWorkGroupSize)
  {
    wxLogDebug(wxT("Group size larger than max acc kernel group size"));
    this->groupSize = this->accKernelWorkGroupSize;
  }

  status = clGetKernelWorkGroupInfo(this->startupKernel, this->deviceId, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &this->startupKernelWorkGroupSize, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("getting startupKernel kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("startupKernel Work Group Size %llu"), (unsigned long long)this->startupKernelWorkGroupSize);

  if (this->groupSize > this->startupKernelWorkGroupSize)
  {
    wxLogDebug(wxT("Group size larger than max startup kernel group size"));
    this->groupSize = this->startupKernelWorkGroupSize;
  }

  status = clGetKernelWorkGroupInfo(this->adamsBashforthKernel, this->deviceId, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &this->adamsBashforthKernelWorkGroupSize, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("getting adamsBashforth kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("adamsBashford Work Group Size %llu"), (unsigned long long)this->adamsBashforthKernelWorkGroupSize);

  if (this->groupSize > this->adamsBashforthKernelWorkGroupSize)
  {
    wxLogDebug(wxT("Group size larger than max adamsBashforth group size"));
    this->groupSize = this->adamsBashforthKernelWorkGroupSize;
  }

  status = clGetKernelWorkGroupInfo(this->adamsMoultonKernel, this->deviceId, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &this->adamsMoultonKernelWorkGroupSize, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("getting adamsMoulton kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("adamsKernel Work Group Size %llu"), (unsigned long long)this->adamsMoultonKernelWorkGroupSize);

  if (this->groupSize > this->adamsMoultonKernelWorkGroupSize)
  {
    wxLogDebug(wxT("Group size larger than max adams Moulton group size"));
    this->groupSize = this->adamsMoultonKernelWorkGroupSize;
  }

  status = clGetKernelWorkGroupInfo(this->copyToDisplayKernel, this->deviceId, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &this->copyToDisplayKernelWorkGroupSize, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("getting copyToDisplay kernel CL_KERNEL_WORK_GROUP_SIZE failed %s"), this->ErrorMessage(status));
    throw status;
  }

  wxLogDebug(wxT("copyToDisplayKernel Work Group Size %llu"), (unsigned long long)this->copyToDisplayKernelWorkGroupSize);

  if (this->groupSize > this->copyToDisplayKernelWorkGroupSize)
  {
    wxLogDebug(wxT("Group size larger than max group size"));
    this->groupSize = this->copyToDisplayKernelWorkGroupSize;
  }

  wxLogDebug(wxT("Finished CLModel:SetKernelArgumentsAndGroupSize"));
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
void CLModel::SetAdamsKernelArgs(cl_kernel adamsKernel)
{
  cl_int status;

  // adamskernel
  status = clSetKernelArg(adamsKernel, 0, sizeof(cl_mem), (void *)&this->currPos);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 0 failed for currPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 1, sizeof(cl_mem), (void *)&this->currVel);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 1 failed for currVel %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 2, sizeof(cl_mem), (void *)&this->acc);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 2 failed for acc %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 3, sizeof(cl_double), (void *)&this->delT);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 3 failed for delT %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 4, sizeof(cl_mem), (void *)&this->newPos);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 4 failed for newPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 5, sizeof(cl_mem), (void *)&this->newVel);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 5 failed for newVel %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 6, sizeof(cl_int), (void *)&this->stage);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 6 failed for stage %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 7, sizeof(cl_int), (void *)&this->step);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 7 failed for step %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 8, sizeof(cl_int), (void *)&this->numParticles);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 8 failed for step %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 9, sizeof(cl_mem), (void *)&this->posLast);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 9 failed for posLast %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 10, sizeof(cl_mem), (void *)&this->velLast);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 10 failed for velLast %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 11, sizeof(cl_mem), (void *)&this->velHistory);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 11 failed for velHistory %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(adamsKernel, 12, sizeof(cl_mem), (void *)&this->accHistory);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 12 failed for accHistory %s"), this->ErrorMessage(status));
    throw status;
  }
}

// The RK4 startup kernel has the Adams kernel arguments (set by SetAdamsKernelArgs) followed by
/*
int subStep,
int numSubSteps,
__global double4* sumVel,
__global double4* sumAcc)
*/
void CLModel::SetStartupKernelArgs()
{
  cl_int subStep = 0;
  cl_int status = clSetKernelArg(this->startupKernel, 13, sizeof(cl_int), (void *)&subStep);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 13 failed for subStep %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->startupKernel, 14, sizeof(cl_int), (void *)&this->startupSubSteps);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 14 failed for numSubSteps %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->startupKernel, 15, sizeof(cl_mem), (void *)&this->rkSumVel);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 15 failed for rkSumVel %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clSetKernelArg(this->startupKernel, 16, sizeof(cl_mem), (void *)&this->rkSumAcc);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clSetKernelArg 16 failed for rkSumAcc %s"), this->ErrorMessage(status));
    throw status;
  }
}

// Release and/or de-allocates all opencl resources.
// A cleanup and initialise sequence is done every time something major changes.
// e.g. number of bodies or the kernels changes.
int CLModel::CleanUpCL()
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::CleanUpCL threadId: %ld"), wxThread::GetCurrentId());
#endif

  cl_int status;
  int success = CL_SUCCESS;

  if (this->commandQueue != NULL)
  {
    status = clFinish(this->commandQueue);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clFinish failed %s"), this->ErrorMessage(status));
      success = status;
    }
  }

  if (this->currPos != NULL)
  {
    status = clReleaseMemObject(this->currPos);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject currPos failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->currPos = NULL;
    }
  }

  if (this->newPos != NULL)
  {
    status = clReleaseMemObject(this->newPos);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject newPos failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->newPos = NULL;
    }
  }

  if (this->currVel != NULL)
  {
    status = clReleaseMemObject(this->currVel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject currVel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->currVel = NULL;
    }
  }

  if (this->newVel != NULL)
  {
    status = clReleaseMemObject(this->newVel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject newVel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->newVel = NULL;
    }
  }

  if (this->gravPos != NULL)
  {
    status = clReleaseMemObject(this->gravPos);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject gravPos failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->gravPos = NULL;
    }
  }

  if (this->acc != NULL)
  {
    status = clReleaseMemObject(this->acc);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject acc failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->acc = NULL;
    }
  }

  if (this->posLast != NULL)
  {
    status = clReleaseMemObject(this->posLast);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject posLast failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->posLast = NULL;
    }
  }

  if (this->velLast != NULL)
  {
    status = clReleaseMemObject(this->velLast);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject velLast failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->velLast = NULL;
    }
  }

  if (this->velHistory != NULL)
  {
    status = clReleaseMemObject(this->velHistory);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject velHistory failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->velHistory = NULL;
    }
  }

  if (this->accHistory != NULL)
  {
    status = clReleaseMemObject(this->accHistory);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject accHistory failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->accHistory = NULL;
    }
  }

  if (this->rkSumVel != NULL)
  {
    status = clReleaseMemObject(this->rkSumVel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject rkSumVel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->rkSumVel = NULL;
    }
  }

  if (this->rkSumAcc != NULL)
  {
    status = clReleaseMemObject(this->rkSumAcc);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject rkSumAcc failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->rkSumAcc = NULL;
    }
  }

  if (this->dispPos != NULL)
  {
    status = clReleaseMemObject(this->dispPos);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseMemObject dispPos failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->dispPos = NULL;
    }
  }

  if (this->accKernel != NULL)
  {
    status = clReleaseKernel(this->accKernel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseKernel accKernel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->accKernel = NULL;
    }
  }

  if (this->startupKernel != NULL)
  {
    status = clReleaseKernel(this->startupKernel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseKernel startupKernel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->startupKernel = NULL;
    }
  }

  if (this->adamsBashforthKernel != NULL)
  {
    status = clReleaseKernel(this->adamsBashforthKernel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseKernel adamsBashforthKernel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->adamsBashforthKernel = NULL;
    }
  }

  if (this->adamsMoultonKernel != NULL)
  {
    status = clReleaseKernel(this->adamsMoultonKernel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseKernel adamsMoultonKernel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->adamsMoultonKernel = NULL;
    }
  }

  if (this->copyToDisplayKernel != NULL)
  {
    status = clReleaseKernel(this->copyToDisplayKernel);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseKernel copyToDisplayKernel failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->copyToDisplayKernel = NULL;
    }
  }

  if (this->program != NULL)
  {
    status = clReleaseProgram(this->program);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseProgram failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->program = NULL;
    }
  }

  if (this->commandQueue != NULL)
  {
    status = clReleaseCommandQueue(this->commandQueue);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseCommandQueue failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->commandQueue = NULL;
    }
  }

  if (this->context != NULL)
  {
    status = clReleaseContext(this->context);
    if (status != CL_SUCCESS)
    {
      wxLogError(wxT("clReleaseContext failed %s"), this->ErrorMessage(status));
      success = status;
    }
    else
    {
      this->context = NULL;
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
void CLModel::SetInitalState(cl_double4 *initalPositions, cl_double4 *initalVelocities)
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::SetInitalState threadId: %ld"), wxThread::GetCurrentId());
#endif

  cl_int status = CL_SUCCESS;
  status = clEnqueueWriteBuffer(this->commandQueue, this->currPos, CL_FALSE, 0, this->numParticles * sizeof(cl_double4), initalPositions, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueWriteBuffer write inital Positions to currPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clEnqueueWriteBuffer(this->commandQueue, this->gravPos, CL_FALSE, 0, this->numGrav * sizeof(cl_double4), initalPositions, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueWriteBuffer write inital Positions to gravPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clEnqueueWriteBuffer(this->commandQueue, this->currVel, CL_FALSE, 0, this->numParticles * sizeof(cl_double4), initalVelocities, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueWriteBuffer write inital Velocity to currVel %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clFinish(this->commandQueue);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clFinish failed %s"), this->ErrorMessage(status));
    throw status;
  }

  // New state, so restart the integration (RK4 startup) from the beginning of a step
  this->step = 0;
  this->stage = this->numStages;
}

// snapshots the current positions and velocities and makes them the initial start conditions.
// From there they can be saved to disk as either binary or slf format for later
void CLModel::ReadToInitialState(cl_double4 *initalPositions, cl_double4 *initalVelocities)
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("CLModel::ReadToInitialState threadId: %ld"), wxThread::GetCurrentId());
#endif

  cl_int status = CL_SUCCESS;
  status = clFinish(this->commandQueue);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clFinish failed %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clEnqueueReadBuffer(this->commandQueue, this->currPos, CL_TRUE, 0, this->numParticles * sizeof(cl_double4), initalPositions, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueWriteBuffer write inital Positions to currPos %s"), this->ErrorMessage(status));
    throw status;
  }

  status = clEnqueueReadBuffer(this->commandQueue, this->currVel, CL_TRUE, 0, this->numParticles * sizeof(cl_double4), initalVelocities, 0, 0, 0);
  if (status != CL_SUCCESS)
  {
    wxLogError(wxT("clEnqueueWriteBuffer write inital Velocity to currVel %s"), this->ErrorMessage(status));
    throw status;
  }
}

// convert the openCL status code to text
// Because the error numbers are to hard to remember
wxString CLModel::ErrorMessage(cl_int status)
{
  switch (status)
  {
  case CL_SUCCESS:
    return wxT("CL_SUCCESS");
  case CL_DEVICE_NOT_FOUND:
    return wxT("CL_DEVICE_NOT_FOUND");
  case CL_DEVICE_NOT_AVAILABLE:
    return wxT("CL_DEVICE_NOT_AVAILABLE");
  case CL_COMPILER_NOT_AVAILABLE:
    return wxT("CL_COMPILER_NOT_AVAILABLE");
  case CL_MEM_OBJECT_ALLOCATION_FAILURE:
    return wxT("CL_MEM_OBJECT_ALLOCATION_FAILURE");
  case CL_OUT_OF_RESOURCES:
    return wxT("CL_OUT_OF_RESOURCES");
  case CL_OUT_OF_HOST_MEMORY:
    return wxT("CL_OUT_OF_HOST_MEMORY");
  case CL_PROFILING_INFO_NOT_AVAILABLE:
    return wxT("CL_PROFILING_INFO_NOT_AVAILABLE");
  case CL_MEM_COPY_OVERLAP:
    return wxT("CL_MEM_COPY_OVERLAP");
  case CL_IMAGE_FORMAT_MISMATCH:
    return wxT("CL_IMAGE_FORMAT_MISMATCH");
  case CL_IMAGE_FORMAT_NOT_SUPPORTED:
    return wxT("CL_IMAGE_FORMAT_NOT_SUPPORTED");
  case CL_BUILD_PROGRAM_FAILURE:
    return wxT("CL_BUILD_PROGRAM_FAILURE");
  case CL_MAP_FAILURE:
    return wxT("CL_MAP_FAILURE");
  case CL_MISALIGNED_SUB_BUFFER_OFFSET:
    return wxT("CL_MISALIGNED_SUB_BUFFER_OFFSET");
  case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
    return wxT("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST");
  case CL_COMPILE_PROGRAM_FAILURE:
    return wxT("CL_COMPILE_PROGRAM_FAILURE");
  case CL_LINKER_NOT_AVAILABLE:
    return wxT("CL_LINKER_NOT_AVAILABLE");
  case CL_LINK_PROGRAM_FAILURE:
    return wxT("CL_LINK_PROGRAM_FAILURE");
  case CL_DEVICE_PARTITION_FAILED:
    return wxT("CL_DEVICE_PARTITION_FAILED");
  case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
    return wxT("CL_KERNEL_ARG_INFO_NOT_AVAILABLE");
  case CL_INVALID_VALUE:
    return wxT("CL_INVALID_VALUE");
  case CL_INVALID_DEVICE_TYPE:
    return wxT("CL_INVALID_DEVICE_TYPE");
  case CL_INVALID_PLATFORM:
    return wxT("CL_INVALID_PLATFORM");
  case CL_INVALID_DEVICE:
    return wxT("CL_INVALID_DEVICE");
  case CL_INVALID_CONTEXT:
    return wxT("CL_INVALID_CONTEXT");
  case CL_INVALID_QUEUE_PROPERTIES:
    return wxT("CL_INVALID_QUEUE_PROPERTIES");
  case CL_INVALID_COMMAND_QUEUE:
    return wxT("CL_INVALID_COMMAND_QUEUE");
  case CL_INVALID_HOST_PTR:
    return wxT("CL_INVALID_HOST_PTR");
  case CL_INVALID_MEM_OBJECT:
    return wxT("CL_INVALID_MEM_OBJECT");
  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
    return wxT("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR");
  case CL_INVALID_IMAGE_SIZE:
    return wxT("CL_INVALID_IMAGE_SIZE");
  case CL_INVALID_SAMPLER:
    return wxT("CL_INVALID_SAMPLER");
  case CL_INVALID_BINARY:
    return wxT("CL_INVALID_BINARY");
  case CL_INVALID_BUILD_OPTIONS:
    return wxT("CL_INVALID_BUILD_OPTIONS");
  case CL_INVALID_PROGRAM:
    return wxT("CL_INVALID_PROGRAM");
  case CL_INVALID_PROGRAM_EXECUTABLE:
    return wxT("CL_INVALID_PROGRAM_EXECUTABLE");
  case CL_INVALID_KERNEL_NAME:
    return wxT("CL_INVALID_KERNEL_NAME");
  case CL_INVALID_KERNEL_DEFINITION:
    return wxT("CL_INVALID_KERNEL_DEFINITION");
  case CL_INVALID_KERNEL:
    return wxT("CL_INVALID_KERNEL");
  case CL_INVALID_ARG_INDEX:
    return wxT("CL_INVALID_ARG_INDEX");
  case CL_INVALID_ARG_VALUE:
    return wxT("CL_INVALID_ARG_VALUE");
  case CL_INVALID_ARG_SIZE:
    return wxT("CL_INVALID_ARG_SIZE");
  case CL_INVALID_KERNEL_ARGS:
    return wxT("CL_INVALID_KERNEL_ARGS");
  case CL_INVALID_WORK_DIMENSION:
    return wxT("CL_INVALID_WORK_DIMENSION");
  case CL_INVALID_WORK_GROUP_SIZE:
    return wxT("CL_INVALID_WORK_GROUP_SIZE");
  case CL_INVALID_WORK_ITEM_SIZE:
    return wxT("CL_INVALID_WORK_ITEM_SIZE");
  case CL_INVALID_GLOBAL_OFFSET:
    return wxT("CL_INVALID_GLOBAL_OFFSET");
  case CL_INVALID_EVENT_WAIT_LIST:
    return wxT("CL_INVALID_EVENT_WAIT_LIST");
  case CL_INVALID_EVENT:
    return wxT("CL_INVALID_EVENT");
  case CL_INVALID_OPERATION:
    return wxT("CL_INVALID_OPERATION");
  case CL_INVALID_GL_OBJECT:
    return wxT("CL_INVALID_GL_OBJECT");
  case CL_INVALID_BUFFER_SIZE:
    return wxT("CL_INVALID_BUFFER_SIZE");
  case CL_INVALID_MIP_LEVEL:
    return wxT("CL_INVALID_MIP_LEVEL");
  case CL_INVALID_GLOBAL_WORK_SIZE:
    return wxT("CL_INVALID_GLOBAL_WORK_SIZE");
  case CL_INVALID_PROPERTY:
    return wxT("CL_INVALID_PROPERTY");
  case CL_INVALID_IMAGE_DESCRIPTOR:
    return wxT("CL_INVALID_IMAGE_DESCRIPTOR");
  case CL_INVALID_COMPILER_OPTIONS:
    return wxT("CL_INVALID_COMPILER_OPTIONS");
  case CL_INVALID_LINKER_OPTIONS:
    return wxT("CL_INVALID_LINKER_OPTIONS");
  case CL_INVALID_DEVICE_PARTITION_COUNT:
    return wxT("CL_INVALID_DEVICE_PARTITION_COUNT");
  case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR:
    return wxT("CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR");
  default:
    return wxString::Format(wxT("Unknown Error Code: %d"), status);
  }
}
