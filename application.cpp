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
			
	for (int i = 1; i < argc; i++)
	{
		if (wxStrcmp(argv[i], wxT("-sb")) == 0)
		{
			this->doubleBuffer = false;
		}
		else if (wxStrcmp(argv[i], wxT("-db")) == 0)
		{
			this->doubleBuffer = true;
		}else if (wxStrcmp(argv[i], wxT("-cpu")) == 0)
		{
			this->preferCpu = true;
		}
		else if (wxStrcmp(argv[i], wxT("-nvidia")) == 0)
		{
			desiredPlatform=(char*)"NVIDIA Corporation";
			this->numParticles = 2560;
			this->numGrav=16;
		}
		else if (wxStrcmp(argv[i], wxT("-amd")) == 0)
		{
			desiredPlatform=(char*)"Advanced Micro Devices, Inc.";
			this->numParticles = 2560;
			this->numGrav=16;
		}
		else if (wxStrcmp(argv[i], wxT("-intel")) == 0)
		{
			desiredPlatform=(char*)"Intel(R) Corporation";
			this->numParticles = 2560;
			this->numGrav=16;
		}
		else if (wxStrcmp(argv[i], wxT("-nsmall")) == 0)
		{
			this->numParticles = 256*32;
		}
		else if (wxStrcmp(argv[i], wxT("-nlarge")) == 0)
		{
			this->numParticles = 256*1210;
		}
		else if (wxStrcmp(argv[i], wxT("-nhuge")) == 0)
		{
			this->numParticles = 256*2323;
		}
		else if (wxStrcmp(argv[i], wxT("-nsuperhuge")) == 0)
		{
			this->numParticles = 256*4000;
		}
		else if (wxStrcmp(argv[i], wxT("-ngravlarge")) == 0)
		{
			this->numGrav = 32;
		}
		else if (wxStrcmp(argv[i], wxT("-ngravhuge")) == 0)
		{
			this->numGrav = 64;
		}
		else if (wxStrcmp(argv[i], wxT("-ngravsuperhuge")) == 0)
		{
			this->numGrav = 128;
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
	this->logFile = NULL;
	this->lighting = false;
	this->smooth = false;
	this->doubleBuffer = true;
	this->preferCpu = false;
	this->numParticles = 2560*8;
	this->numGrav=16;
	this->desiredPlatform = (char*)"Advanced Micro Devices, Inc.";
	AttachConsole((DWORD)-1);
}

// `Main program' equivalent, creating windows and returning main app frame
bool Application::OnInit()
{

#if defined(__WXDEBUG__ )
	//If debugging then setup logging to a file
	this->logFile = new wxFFile();
	if( logFile->Open(wxT("log.log"),wxT("w+") ))
	{
		wxLogStderr *logStderr = new wxLogStderr(logFile->fp());
		wxLog::SetActiveTarget(logStderr);
	}
#endif

	this->frame = new Frame(NULL, wxT("OpenGL/CL Solar System fp64 (c) Michael Simmons 2013"), wxDefaultPosition, wxDefaultSize);

#if defined(__WXDEBUG__ )
	//If debugging also send log to a windows
	//This is attached to the Frame so that it closes with it.
	wxLogWindow *logWindow = new wxLogWindow(this->frame, wxT(" Debug Log"),true,true);
#endif

	// Process the command line arguments
	this->Args(argc, argv);
	this->frame->InitFrame(this->doubleBuffer, this->smooth, this->lighting,this->numParticles, this->numGrav,this->preferCpu,this->desiredPlatform);

	wxLogDebug(wxT("Application::OnInit Done"));
	return true;
}

int Application::OnExit()
{
	//this->logFile->Flush();
	//wxLogDebug(wxT("Application::OnExit Start"));
	//delete this->logWindow;
	//delete this->frame;
	//wxLogDebug(wxT("Application::OnExit Done"));
	return 0;
}
