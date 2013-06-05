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
#include "application.hpp"

bool Application::Args(int argc, wxChar **argv)
{
	this->numParticles = 2560;
	this->numGrav=16;
	this->useLastDevice = true;
			
	for (int i = 1; i < argc; i++)
	{
		if (wxStrcmp(argv[i], wxT("-sb")) == 0)
		{
			this->doubleBuffer = false;
		}
		else if (wxStrcmp(argv[i], wxT("-db")) == 0)
		{
			this->doubleBuffer = true;
		}
		else if (wxStrcmp(argv[i], wxT("-nvidia")) == 0)
		{
			desiredPlatform=(char*)"NVIDIA Corporation";
			this->useLastDevice = false;
		}
		else if (wxStrcmp(argv[i], wxT("-amd")) == 0)
		{
			desiredPlatform=(char*)"Advanced Micro Devices, Inc.";
			this->useLastDevice = false;
		}
		else if (wxStrcmp(argv[i], wxT("-intel")) == 0)
		{
			desiredPlatform=(char*)"Intel(R) Corporation";
			this->useLastDevice = false;
		}
		else
		{
			wxString msg = wxT("Bad option: ");
			msg += argv[i];
			wxMessageBox(msg);
			return false;
		}
	}

	return true;
}

IMPLEMENT_APP(Application)

Application::Application()
{
	this->frame = NULL;
	this->lighting = false;
	this->smooth = false;
	this->doubleBuffer = true;
	this->useLastDevice = true;
	this->numParticles = 2560*8;
	this->numGrav=16;
	this->desiredPlatform = (char*)"Advanced Micro Devices, Inc.";
	
	// Attach the Console so that opencl printf's will go to it. 
	AttachConsole((DWORD)-1);
}

// `Main program' equivalent, creating windows and returning main app frame
bool Application::OnInit()
{
	this->frame = new Frame(NULL, wxT("OpenGL/CL Solar System fp64"), wxDefaultPosition, wxDefaultSize);

#if defined(__WXDEBUG__ )
	//If debugging send log to a windows
	//This is attached to the Frame so that it closes with it.
	wxLogWindow *logWindow = new wxLogWindow(this->frame, wxT("Debug Log"),true,true);
	wxLog::SetActiveTarget(logWindow);
	wxLogDebug(wxT("Application::OnInit threadId: %ld"),wxThread::GetCurrentId());
#endif

	// Process the command line arguments
	this->Args(argc, argv);
	if(!this->frame->InitFrame(this->doubleBuffer, this->smooth, this->lighting,this->numParticles, this->numGrav,this->useLastDevice,this->desiredPlatform))
	{
		this->frame->Destroy();
		return false;
	}

	wxLogDebug(wxT("Application::OnInit Done"));
	return true;
}

int Application::OnExit()
{
	return 0;
}
