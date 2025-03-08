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
#include "application.hpp"


// Parses the arguments passed on the command line
bool Application::Args( int argc, wxChar **argv )
{
	this->numParticles = 2560;
	this->numGrav=16;
	this->useLastDevice = true;
	this->tryForCPUFirst = false;
	this->desiredPlatform = NULL;

	for ( int i = 1; i < argc; i++ )
	{
		if ( wxStrcmp( argv[i], wxT( "-sb" ) ) == 0 )
		{
			this->doubleBuffer = false;
		}
		else if ( wxStrcmp( argv[i], wxT( "-db" ) ) == 0 )
		{
			this->doubleBuffer = true;
		}
		else if ( wxStrcmp( argv[i], wxT( "-stereo" ) ) == 0 )
		{
			this->stereo = true;
		}
		else if ( wxStrcmp( argv[i], wxT( "-cpu" ) ) == 0 )
		{
			this->tryForCPUFirst = true;
			this->useLastDevice = false;
		}
		else if ( wxStrcmp( argv[i], wxT( "-nvidia" ) ) == 0 )
		{
			this->desiredPlatform =( char* )"NVIDIA Corporation";
			this->useLastDevice = false;
		}
		else if ( wxStrcmp( argv[i], wxT( "-amd" ) ) == 0 )
		{
			this->desiredPlatform =( char* )"Advanced Micro Devices, Inc.";
			this->useLastDevice = false;
		}
		else if ( wxStrcmp( argv[i], wxT( "-intel" ) ) == 0 )
		{
			this->desiredPlatform =( char* )"Intel(R) Corporation";
			this->useLastDevice = false;
		}
		else
		{
			wxLogError( wxT( "Bad option: %s" ),argv[i] );
			return false;
		}
	}

	return true;
}

IMPLEMENT_APP( Application )

Application::Application()
{
	this->frame = NULL;
	this->lighting = false;
	this->smooth = false;
	this->doubleBuffer = true;
	this->stereo = false;
	this->numParticles = 2560;
	this->numGrav=16;
	this->useLastDevice = true;
	this->tryForCPUFirst = false;
	this->desiredPlatform = NULL;

#ifdef _WIN32
	// Attach the Console so that opencl printf's will go to it.
	AttachConsole( ( DWORD )-1 );
#endif

}

// `Main program' equivalent, creating windows and returning main app frame
bool Application::OnInit()
{
	bool success = false;
	this->frame = NULL;

	try
	{
		this->frame = new Frame( NULL, wxT( "Solar System Simulation" ), wxDefaultPosition, wxDefaultSize );

//#if defined(__WXDEBUG__ )
		//If debugging send log to a windows
		//This is attached to the Frame so that it closes with it.
		wxLogWindow *logWindow = new wxLogWindow( this->frame, wxT( "OpenCL Solar System Log" ),true,false );
		wxLog::SetActiveTarget( logWindow );
		wxLogDebug( wxT( "Application::OnInit threadId: %ld" ),wxThread::GetCurrentId() );
//#endif

		// Process the command line arguments
		this->Args( argc, argv );
		this->frame->InitFrame( this->doubleBuffer, this->smooth, this->lighting,this->stereo, this->numParticles, this->numGrav,this->useLastDevice,this->desiredPlatform, this->tryForCPUFirst );
		success = true;
		wxLogDebug( wxT( "Application::OnInit Done" ) );
	}
	catch( int ex )
	{
		success = false;
		if( this->frame != NULL )
		{
			this->frame->Destroy();
		}
	}

	return success;
}

int Application::OnExit()
{
	return 0;
}
