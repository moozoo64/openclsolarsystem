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
#include "frame.hpp"

// UI event Ids
enum
{
    ID_TIMER = 1,
    ID_START,
    ID_STOP,
    ID_RESET,
    ID_RESETCOLOURS,
    ID_IMPORTSLF,
    ID_SETDELTAT15,
    ID_SETDELTATHR,
    ID_SETDELTAT12HR,
    ID_SETDELTATFOURHR,
    ID_SETDELTATDAY,
    ID_SETDELTATMINUS15,
    ID_SETDELTATMINUSHR,
    ID_SETDELTATMINUS12HR,
    ID_SETDELTATMINUSFOURHR,
    ID_SETDELTATMINUSDAY,
    ID_SETNUM2048,
    ID_SETNUM4096,
    ID_SETNUM8192,
    ID_SETNUM16384,
    ID_SETNUM32768,
    ID_SETNUM65535,
    ID_SETNUM131072,
    ID_SETNUM262144,
    ID_SETNUM524288,
    ID_SETNUMMAX,
    ID_SETGRAV16,
    ID_SETGRAV32,
    ID_SETGRAV64,
    ID_SETGRAV128,
    ID_SETGRAV192,
    ID_SETGRAV256,
    ID_SETGRAV384,
    ID_SETGRAV512,
    ID_SETGRAVMAX,
    ID_SAVESTATE,
    ID_LOADSTATE,
    ID_READSTATE,
    ID_EXPORTSLF,
    ID_BLENDING,
    ID_SETADAMS4,
    ID_SETADAMS8,
    ID_SETADAMS10,
    ID_SETADAMS11,
    ID_SETADAMS12,
    ID_SETADAMS16,
    ID_SETNEWTONIAN,
    ID_SETRELATIVISTIC,
    ID_SETRELATIVISTICL,
    ID_SETCENTER0,
    ID_SETCENTER1,
    ID_SETCENTER2,
    ID_SETCENTER3,
    ID_SETCENTER4,
    ID_SETCENTER5,
    ID_SETCENTER6,
    ID_SETCENTER7,
    ID_SETCENTER8,
    ID_SETCENTER9,
    ID_SETCENTER10,
    ID_SETCENTER11,
    ID_SETCENTER12,
    ID_SETCENTER13,
    ID_SETCENTER14,
    ID_SETCENTER15,
    ID_SETCENTER16,
    ID_LOGENCOUNTERS
};

// mapping of UI event ids to functions
BEGIN_EVENT_TABLE( Frame, wxFrame )
	EVT_MENU( wxID_EXIT, Frame::OnExit )
	EVT_MENU( wxID_ABOUT, Frame::OnAbout )
	EVT_MENU( ID_START, Frame::OnStart )
	EVT_MENU( ID_STOP, Frame::OnStop )
	EVT_MENU( ID_RESET, Frame::OnReset )
	EVT_MENU( ID_RESETCOLOURS, Frame::OnResetColours )
	EVT_MENU( ID_IMPORTSLF, Frame::OnImportSlf )
	EVT_MENU( ID_SETADAMS4, Frame::OnSetIntegrator )
	EVT_MENU( ID_SETADAMS8, Frame::OnSetIntegrator )
	EVT_MENU( ID_SETADAMS10, Frame::OnSetIntegrator )
	EVT_MENU( ID_SETADAMS11, Frame::OnSetIntegrator )
	EVT_MENU( ID_SETADAMS12, Frame::OnSetIntegrator )
	EVT_MENU( ID_SETADAMS16, Frame::OnSetIntegrator )
	EVT_MENU( ID_SETDELTATMINUS15, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTATMINUSHR, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTATMINUS12HR, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTATMINUSFOURHR, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTATMINUSDAY, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTAT15, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTATHR, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTAT12HR, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTATFOURHR, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETDELTATDAY, Frame::OnSetDeltaTime )
	EVT_MENU( ID_SETNUM2048, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM4096, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM8192, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM16384, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM32768, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM65535, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM131072, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM262144, Frame::OnSetNum )
	EVT_MENU( ID_SETNUM524288, Frame::OnSetNum )
	EVT_MENU( ID_SETNUMMAX, Frame::OnSetNum )
	EVT_MENU( ID_SETGRAV16, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAV32, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAV64, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAV128, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAV192, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAV256, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAV384, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAV512, Frame::OnSetGrav )
	EVT_MENU( ID_SETGRAVMAX, Frame::OnSetGrav )
	EVT_MENU( ID_SETCENTER0, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER1, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER2, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER3, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER4, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER5, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER6, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER7, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER8, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER9, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER10, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER11, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER12, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER13, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER14, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER15, Frame::OnSetCenter )
	EVT_MENU( ID_SETCENTER16, Frame::OnSetCenter )
	EVT_MENU( ID_SETNEWTONIAN, Frame::OnSetAcceleration )
	EVT_MENU( ID_SETRELATIVISTIC, Frame::OnSetAcceleration )
	EVT_MENU( ID_SETRELATIVISTICL, Frame::OnSetAcceleration )
	EVT_MENU( ID_SAVESTATE, Frame::OnSaveInitialState )
	EVT_MENU( ID_LOADSTATE, Frame::OnLoadInitialState )
	EVT_MENU( ID_READSTATE, Frame::OnReadToInitialState )
	EVT_MENU( ID_EXPORTSLF, Frame::OnExportSlf )
	EVT_MENU( ID_BLENDING, Frame::OnBlending )
	EVT_MENU( ID_LOGENCOUNTERS, Frame::OnLogEncounters )
	EVT_TIMER( ID_TIMER, Frame::OnTimer )
	EVT_IDLE( Frame::OnIdle )
	EVT_CLOSE( Frame::OnClose )
END_EVENT_TABLE()

// My frame constructor
Frame::Frame( wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: wxFrame( frame, wxID_ANY, title, pos, size, style )
{
	wxLogDebug( wxT( "Making Frame" ) );
	this->glCanvas = NULL;
	this->clModel = NULL;
	this->timer = new wxTimer( this,ID_TIMER );
	this->initialState = new InitialState();
	this->stopDateJdn = 2456430.5;
	this->encounterDistance = 5*0.35;
	this->goingToDate = false;
	this->runOnIdle = false;
	this->clModelOk = false;
	this->desiredPlatform = NULL;
	this->useLastDevice = false;
	this->tryForCPUFirst = false;
	this->checkForEncounters = false;
	this->numParticles = 0;
	this->numGrav = 0;
	this->config = NULL;
}

Frame::~Frame()
{
	wxLogDebug( wxT( "Cleaning up Frame" ) );
	this->Stop();
	delete this->clModel;
	delete this->glCanvas;
	delete this->timer;
	delete this->initialState;

#if defined(__WXDEBUG__ )
	delete wxLog::SetActiveTarget( NULL );
#endif

}

void Frame::InitFrame( bool doubleBuffer, bool smooth, bool lighting, bool stereo, int numParticles, int numGrav, bool useLastDevice, char *desiredPlatform, bool tryForCPUFirst )
{
	bool die = false;

#ifdef __WXDEBUG__
	wxLogDebug( wxT( "Frame::InitFrame threadId: %ld" ),wxThread::GetCurrentId() );
#endif
	this->numParticles = numParticles;
	this->numGrav = numGrav;
	this->tryForCPUFirst = tryForCPUFirst;
	this->useLastDevice = useLastDevice;
	this->desiredPlatform = desiredPlatform;

	try
	{
		this->config = wxConfigBase::Get();

		this->initialState->initialNumGrav = this->numGrav;
		if( !this->initialState->LoadInitialState( wxT( "initial.bin" ) ) )
		{
			this->initialState->initialNumParticles = this->numParticles;
			if( !this->initialState->CreateRandomInitialConfig() )
			{
				throw -1;
			}
		}

		this->initialState->SetDefaultBodyColours();
		this->numParticles = this->numParticles > this->initialState->initialNumParticles ? this->initialState->initialNumParticles : this->numParticles;

		// Give it an icon
#ifdef _WIN32
		this->SetIcon( wxICON( sample ) );
#endif

		// Create a menu that lets the user save, load and exit
		wxMenu *menuFile = new wxMenu;
		menuFile->Append( wxID_ABOUT, wxT( "&About..." ) );
		menuFile->AppendSeparator();
		menuFile->Append( ID_SAVESTATE, wxT( "Save Initial" ) );
		menuFile->Append( ID_LOADSTATE, wxT( "Load Initial" ) );
		menuFile->Append( ID_READSTATE, wxT( "Set Initial" ) );
		menuFile->Append( ID_IMPORTSLF, wxT( "&Import .SLF File" ) );
		menuFile->Append( ID_EXPORTSLF, wxT( "&Export .SLF File" ) );
		menuFile->Append( ID_RESETCOLOURS, wxT( "Reset Colours" ) );
		menuFile->Append( wxID_EXIT, wxT( "E&xit" ) );

		// Create a menu that lets the user control the running of the simulation
		wxMenu *menuGo = new wxMenu;
		menuGo->Append( ID_START, wxT( "&Start" ) );
		menuGo->Append( ID_STOP, wxT( "S&top" ) );
		menuGo->Append( ID_RESET, wxT( "&Reset" ) );

		// Create a menu that lets the user choose the menthod used to calculate updated positions and velocities
		// Only one option can be chosen at any time
		wxMenu *menuIntegrator = new wxMenu;
		menuIntegrator->AppendRadioItem( ID_SETADAMS4, wxT( "Adams Bashforth Moulton 4" ) );
		menuIntegrator->AppendRadioItem( ID_SETADAMS8, wxT( "Adams Bashforth Moulton 8" ) );
		menuIntegrator->AppendRadioItem( ID_SETADAMS10, wxT( "Adams Bashforth Moulton 10" ) );
		menuIntegrator->AppendRadioItem( ID_SETADAMS11, wxT( "Adams Bashforth Moulton 11" ) );
		menuIntegrator->AppendRadioItem( ID_SETADAMS12, wxT( "Adams Bashforth Moulton 12" ) );
		menuIntegrator->AppendRadioItem( ID_SETADAMS16, wxT( "Adams Bashforth Moulton 16" ) );

		// Create a menu that lets the user choose the gravity acceleration calculation method
		// Only one option can be chosen at any time
		wxMenu *menuGravity = new wxMenu;
		menuGravity->AppendRadioItem( ID_SETNEWTONIAN, wxT( "Newtonian" ) );
		menuGravity->AppendRadioItem( ID_SETRELATIVISTIC, wxT( "With Relativistic corrections" ) );
		menuGravity->AppendRadioItem( ID_SETRELATIVISTICL, wxT( "With Relativistic corrections using Local Memory" ) );
		// Create a menu that lets the user choose the time step size.
		// Only one option can be chosen at any time
		wxMenu *menuDeltaT = new wxMenu;
		menuDeltaT->AppendRadioItem( ID_SETDELTAT15, wxT( "15 Mins" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATHR, wxT( "1 Hour" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATFOURHR, wxT( "4 Hours" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTAT12HR, wxT( "12 Hours" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATDAY, wxT( "1 Day" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUS15, wxT( "-15 Mins" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUSHR, wxT( "-1 Hour" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUSFOURHR, wxT( "-4 Hours" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUS12HR, wxT( "-12 Hours" ) );
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUSDAY, wxT( "-1 Day" ) );

		// Create a menu that lets the user choose the number of bodies/particles.
		// Only one option can be chosen at any time
		wxMenu *menuNum = new wxMenu;
		menuNum->AppendRadioItem( ID_SETNUM2048, wxT( "2048" ) );
		menuNum->AppendRadioItem( ID_SETNUM4096, wxT( "4096" ) );
		menuNum->AppendRadioItem( ID_SETNUM8192, wxT( "8192" ) );
		menuNum->AppendRadioItem( ID_SETNUM16384, wxT( "16384" ) );
		menuNum->AppendRadioItem( ID_SETNUM32768, wxT( "32768" ) );
		menuNum->AppendRadioItem( ID_SETNUM65535, wxT( "65536" ) );
		menuNum->AppendRadioItem( ID_SETNUM131072, wxT( "131072" ) );
		menuNum->AppendRadioItem( ID_SETNUM262144, wxT( "262144" ) );
		menuNum->AppendRadioItem( ID_SETNUM524288, wxT( "524288" ) );
		menuNum->AppendRadioItem( ID_SETNUMMAX, wxT( "Maximum" ) );

		// Create a menu that lets the user choose the number of bodies/particles with gravitional effects
		// Only one option can be chosen at any time
		wxMenu *menuGrav = new wxMenu;
		menuGrav->AppendRadioItem( ID_SETGRAV16, wxT( "16" ) );
		menuGrav->AppendRadioItem( ID_SETGRAV32, wxT( "32" ) );
		menuGrav->AppendRadioItem( ID_SETGRAV64, wxT( "64" ) );
		menuGrav->AppendRadioItem( ID_SETGRAV128, wxT( "128" ) );
		menuGrav->AppendRadioItem( ID_SETGRAV192, wxT( "192" ) );
		menuGrav->AppendRadioItem( ID_SETGRAV256, wxT( "256" ) );
		menuGrav->AppendRadioItem( ID_SETGRAV384, wxT( "384" ) );
		menuGrav->AppendRadioItem( ID_SETGRAV512, wxT( "512" ) );
		menuGrav->AppendRadioItem( ID_SETGRAVMAX, wxT( "Maximum" ) );

		// Create a menu that lets the user choose a body to center the display on.
		// Only one option can be chosen at any time
		wxMenu *menuCenterOn = new wxMenu;
		menuCenterOn->AppendRadioItem( ID_SETCENTER0, this->initialState->physicalProperties[0].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER1, this->initialState->physicalProperties[1].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER2, this->initialState->physicalProperties[2].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER3, this->initialState->physicalProperties[3].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER4, this->initialState->physicalProperties[4].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER5, this->initialState->physicalProperties[5].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER6, this->initialState->physicalProperties[6].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER7, this->initialState->physicalProperties[7].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER8, this->initialState->physicalProperties[8].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER9, this->initialState->physicalProperties[9].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER10, this->initialState->physicalProperties[10].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER11, this->initialState->physicalProperties[11].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER12, this->initialState->physicalProperties[12].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER13, this->initialState->physicalProperties[13].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER14, this->initialState->physicalProperties[14].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER15, this->initialState->physicalProperties[15].Name );
		menuCenterOn->AppendRadioItem( ID_SETCENTER16, this->initialState->physicalProperties[16].Name );


		// Create a menu that lets the user choose how asteroids are displayed.
		wxMenu *menuOptions = new wxMenu;
		menuOptions->AppendCheckItem( ID_BLENDING, wxT( "Blending" ) );
		menuOptions->AppendCheckItem( ID_LOGENCOUNTERS, wxT( "Detect Close Encounters" ) );

		// Add the menus to the windows menu bar
		wxMenuBar *menuBar = new wxMenuBar;
		menuBar->Append( menuFile, wxT( "&File" ) );
		menuBar->Append( menuGo, wxT( "&Go" ) );
		menuBar->Append( menuIntegrator, wxT( "Integrator" ) );
		menuBar->Append( menuGravity, wxT( "Gravity" ) );
		menuBar->Append( menuDeltaT, wxT( "Time Delta" ) );
		menuBar->Append( menuNum, wxT( "Number Bodies" ) );
		menuBar->Append( menuGrav, wxT( "Number with Mass" ) );
		menuBar->Append( menuCenterOn, wxT( "Center on Body" ) );
		menuBar->Append( menuOptions, wxT( "Options" ) );
		this->SetMenuBar( menuBar );
		wxLogDebug( wxT( "Created Menu" ) );

		// Create the status bar to display messages and the current simulated date and step count
		this->CreateStatusBar();
		this->SetStatusText( wxT( "Nbody" ) );
		this->Show( true );

		// Create the OpenGL canvas on which the simulation will be displayed
		this->glCanvas = new GLCanvas( this, wxID_ANY, wxDefaultPosition,wxDefaultSize, 0, wxT( "GLCanvas" ), doubleBuffer, smooth, lighting, stereo );
		this->glCanvas->blending = true;
		this->glCanvas->CreateOpenGlContext( this->numParticles, this->numGrav );

		// Create an openCL model to run the simulation and initialise it
		this->clModel = new CLModel();
		this->ChooseDevice( this->config );
		this->clModel->CreateBufferObjects( this->glCanvas->getVbo(), this->numParticles, this->numGrav );
		this->clModel->CompileProgramAndCreateKernels();
		this->glCanvas->SetColours( this->initialState->initialColorData );
		this->clModel->SetInitalState( this->initialState->initialPositions,this->initialState->initialVelocities );
		this->clModel->julianDate = this->initialState->initialJulianDate;
		this->clModel->time = 0.0f;
		this->clModel->SetKernelArgumentsAndGroupSize();
		this->clModel->UpdateDisplay();

		// set the keyboard focus to the simmulation display so that the keyboard functions work
		this->glCanvas->SetFocus();

		// Update the menu items to reflect the current state
		this->UpdateMenuItems();

		menuBar = this->GetMenuBar();
		wxMenuItem *menuItem;
		menuItem = menuBar->FindItem( ID_SETADAMS11 );
		menuItem->Check( true );
		menuItem = menuBar->FindItem( ID_SETRELATIVISTIC );
		menuItem->Check( true );

		wxLogDebug( wxT( "Init Frame Succeeded" ) );
	}
	catch ( int ex )
	{
		wxLogError( wxT( "Fatal Exception %d" ),ex );
		die = true;
	}

	if( die )
	{
		this->Close( true );
	}
}

// Take the simulation date, time and step count and display it in the status bar
// Computes a modified moving average for the frame time see https://en.wikipedia.org/wiki/Moving_average
// timeTaken is the number of microseconds since it was last called or 0 if called while not running
void Frame::UpdateStatusBar( wxLongLong timeTaken )
{
	static double movingAverageTimeTaken;

	if( timeTaken != 0 )
	{
		movingAverageTimeTaken = ( 19*movingAverageTimeTaken + timeTaken.ToDouble() )/20.0;
	}

	double frameRate = 1000000.0/movingAverageTimeTaken;

	// compute the Julian day Number
	double jdn = this->clModel->julianDate + ( this->clModel->time )*1/( 60*60*24 );
	wxDateTime dateTime;
	dateTime.Set( jdn );

	wxString message;
	message.Printf( "Center: %s Julian Day: %f Date: %10s %8s step: %d fps: %.2f",this->initialState->physicalProperties[this->clModel->centerBody].Name,jdn,dateTime.FormatISODate().c_str(),dateTime.FormatISOTime().c_str(), this->clModel->step, frameRate );
	this->SetStatusText( message );
}

void Frame::ChooseDevice( wxConfigBase  *config )
{
	// See if a device vendor id was set last time we ran
	// if so use it
	wxString lastDeviceVendorIdConfigName = wxT( "LastDeviceVendorId" );
	int lastDeviceVendorId = 0;
	if( this->useLastDevice )
	{
		if( !config->Read( lastDeviceVendorIdConfigName, &lastDeviceVendorId ) )
		{
			lastDeviceVendorId = 0;
		}
	}

	if( lastDeviceVendorId != 0 )
	{
		wxLogDebug( wxT( "looking for Vendor Id 0x%X" ),( unsigned int )lastDeviceVendorId );
		if( this->clModel->FindDeviceAndCreateContext( ( cl_uint ) lastDeviceVendorId, CL_DEVICE_TYPE_ALL , NULL ) )
		{
			lastDeviceVendorId = this->clModel->deviceVendorId;
		}
		else
		{
			this->clModel->CleanUpCL();
			lastDeviceVendorId = 0;
		}
	}

	if( this->tryForCPUFirst )
	{
		// if no vendor device id was set try looking for a CPU
		if( lastDeviceVendorId == 0 )
		{
			if( this->clModel->FindDeviceAndCreateContext( 0, CL_DEVICE_TYPE_CPU , this->desiredPlatform ) )
			{
				lastDeviceVendorId = this->clModel->deviceVendorId;
			}
			else
			{
				this->clModel->CleanUpCL();
			}
		}
	}

	// if no vendor device id was set try looking for a GPU
	if( lastDeviceVendorId == 0 )
	{
		if( this->clModel->FindDeviceAndCreateContext( 0, CL_DEVICE_TYPE_GPU , this->desiredPlatform ) )
		{
			lastDeviceVendorId = this->clModel->deviceVendorId;
		}
		else
		{
			this->clModel->CleanUpCL();
		}
	}

	if( !this->tryForCPUFirst )
	{
		// if no vendor device id was set try looking for a CPU
		if( lastDeviceVendorId == 0 )
		{
			if( this->clModel->FindDeviceAndCreateContext( 0, CL_DEVICE_TYPE_CPU , this->desiredPlatform ) )
			{
				lastDeviceVendorId = this->clModel->deviceVendorId;
			}
			else
			{
				this->clModel->CleanUpCL();
			}
		}
	}

	// if no vendor device id was set try looking for anything
	if( lastDeviceVendorId == 0 )
	{
		if( this->clModel->FindDeviceAndCreateContext( 0, CL_DEVICE_TYPE_ALL , this->desiredPlatform ) )
		{
			lastDeviceVendorId = this->clModel->deviceVendorId;
		}
		else
		{
			this->clModel->CleanUpCL();
		}
	}

	if( lastDeviceVendorId == 0 )
	{
		throw -1;
	}

	config->Write( lastDeviceVendorIdConfigName, this->clModel->deviceVendorId );
	wxLogDebug( wxT( "Found Vendor Id 0x%X" ),( unsigned int )this->clModel->deviceVendorId );
}

// Advances the simulation one time step
void Frame::DoStep()
{
	try
	{
		// Adams-Bashforth
		this->clModel->ExecuteKernels();

		// Request that dispPos vbo be updated after the Adams-Moulton
		this->clModel->RequestUpdate();
		// Adams-Moulton
		this->clModel->ExecuteKernels();
	}
	catch( int e )
	{
		this->Stop();
	}

	this->UpdateStatusBar( this->stopWatch.TimeInMicro() );

	if( this->checkForEncounters )
	{
		static int moonIndex = -1;
		this->Stop();
		this->clModel->ReadToInitialState( this->initialState->initialPositions,this->initialState->initialVelocities );
		this->initialState->initialJulianDate = this->clModel->julianDate + ( this->clModel->time )*1/( 60*60*24 );
		this->initialState->initialNumParticles = this->numParticles;

		double bodyX = this->initialState->initialPositions[this->clModel->centerBody].x;
		double bodyY = this->initialState->initialPositions[this->clModel->centerBody].y;
		double bodyZ = this->initialState->initialPositions[this->clModel->centerBody].z;
		double encounterDistanceSquared = this->encounterDistance * this->encounterDistance;
		bool hadEncounter = false;
		wxString message;
		for( int index = 0; index < this->numParticles; index++ )
		{
			if( index == this->clModel->centerBody || index == moonIndex )
			{
				continue;
			}

			double distanceFromCenterSquared = ( this->initialState->initialPositions[index].x - bodyX ) * ( this->initialState->initialPositions[index].x - bodyX );
			distanceFromCenterSquared += ( this->initialState->initialPositions[index].y - bodyY ) * ( this->initialState->initialPositions[index].y - bodyY );
			distanceFromCenterSquared += ( this->initialState->initialPositions[index].z - bodyZ ) * ( this->initialState->initialPositions[index].z - bodyZ );
			if( distanceFromCenterSquared < encounterDistanceSquared )
			{
				hadEncounter = true;
				if( moonIndex == -1 )
				{
					wxString name = wxString( this->initialState->physicalProperties[index].Name );
					if( name.Contains( wxT( "Moon" ) ) )
					{
						moonIndex = index;
					}
				}
				wxDateTime dateTime;
				dateTime.Set( this->initialState->initialJulianDate );
				wxLogMessage( wxT( "Encounter %s %s %f" ), this->initialState->physicalProperties[index].Name,dateTime.FormatISOCombined(), sqrt( distanceFromCenterSquared ) );
			}
		}
		this->Start();
	}

	this->stopWatch.Start( 0 );

	// check if going a date
	if( this->goingToDate )
	{
		double currentJdn = this->clModel->julianDate + ( this->clModel->time )*1/( 60*60*24 );
		if( this->clModel->delT > 0 )
		{
			if( currentJdn >= this->stopDateJdn )
			{
				this->Stop();
				this->goingToDate = false;
			}
		}
		else
		{
			if( currentJdn <= this->stopDateJdn )
			{
				this->Stop();
				this->goingToDate = false;
			}
		}
	}


}

// Run when Idle
void Frame::OnIdle( wxIdleEvent& event )
{
	if( runOnIdle )
	{

#ifdef __WXDEBUG__
		wxLogDebug( wxT( "Frame::OnIdle threadId: %ld" ),wxThread::GetCurrentId() );
#endif
		this->DoStep();
		this->Refresh( false );
	}

	event.RequestMore( true );
}

// Run on Interval timer. This is used to Run while them menu or about dialog is being displayed
void Frame::OnTimer( wxTimerEvent& event )
{
	if( !IsShown() ) return;
	static bool InOnTimer = false;

#ifdef __WXDEBUG__
	wxLogDebug( wxT( "Timer Fired threadId: %ld" ),wxThread::GetCurrentId() );
#endif

	if( InOnTimer )
	{
		wxLogDebug( wxT( "Already in OnTimer" ) );
		return;
	}

	InOnTimer = true;
	this->DoStep();
	this->Refresh( false );
	InOnTimer = false;
}

void Frame::Stop()
{
	this->runOnIdle = false;
	this->timer->Stop();
	this->UpdateStatusBar( 0 );
}

void Frame::Start()
{
	this->runOnIdle = true;

#ifdef _WIN32
	this->timer->Start( 1,false );
#endif

	this->stopWatch.Start( 0 );
}

void Frame::OnStart( wxCommandEvent& WXUNUSED( event ) )
{
	wxLogDebug( wxT( "Frame Start" ) );
	this->Start();
}

void Frame::OnStop( wxCommandEvent& WXUNUSED( event ) )
{
	wxLogDebug( wxT( "Frame Stop" ) );
	this->Stop();
}

void Frame::OnReset( wxCommandEvent& WXUNUSED( event ) )
{
	this->numParticles = this->numParticles > this->initialState->initialNumParticles ? this->initialState->initialNumParticles : this->numParticles;
	this->ResetAll();
}

// sets the time delta between steps
void Frame::OnSetDeltaTime( wxCommandEvent& event )
{
	this->Stop();
	int id = event.GetId();
	switch( id )
	{
	case ID_SETDELTAT15:
		this->clModel->delT = 15*60.0f;
		break;
	case ID_SETDELTATHR:
		this->clModel->delT = 1*60*60.0f;
		break;
	case ID_SETDELTATFOURHR:
		this->clModel->delT = 4*60*60.0f;
		break;
	case ID_SETDELTATDAY:
		this->clModel->delT = 24*60*60.0f;
		break;
	case ID_SETDELTAT12HR:
		this->clModel->delT = 12*60*60.0f;
		break;
	case ID_SETDELTATMINUS15:
		this->clModel->delT = -15*60.0f;
		break;
	case ID_SETDELTATMINUSHR:
		this->clModel->delT = -60*60.0f;
		break;
	case ID_SETDELTATMINUSFOURHR:
		this->clModel->delT = -4*60*60.0f;
		break;
	case ID_SETDELTATMINUS12HR:
		this->clModel->delT = -12*60*60.0f;
		break;
	case ID_SETDELTATMINUSDAY:
		this->clModel->delT = -24*60*60.0f;
		break;
	default:
		this->clModel->delT = 4*60*60.0f;
		break;
	}
	this->clModel->step = 0;
	this->clModel->SetKernelArgumentsAndGroupSize();
}

// sets the body to center the display on
void Frame::OnSetCenter( wxCommandEvent& event )
{
	int id = event.GetId();

	switch( id )
	{
	case ID_SETCENTER0:
		this->clModel->centerBody = 0;
		break;
	case ID_SETCENTER1:
		this->clModel->centerBody = 1;
		break;
	case ID_SETCENTER2:
		this->clModel->centerBody = 2;
		break;
	case ID_SETCENTER3:
		this->clModel->centerBody = 3;
		break;
	case ID_SETCENTER4:
		this->clModel->centerBody = 4;
		break;
	case ID_SETCENTER5:
		this->clModel->centerBody = 5;
		break;
	case ID_SETCENTER6:
		this->clModel->centerBody = 6;
		break;
	case ID_SETCENTER7:
		this->clModel->centerBody = 7;
		break;
	case ID_SETCENTER8:
		this->clModel->centerBody = 8;
		break;
	case ID_SETCENTER9:
		this->clModel->centerBody = 9;
		break;
	case ID_SETCENTER10:
		this->clModel->centerBody = 10;
		break;
	case ID_SETCENTER11:
		this->clModel->centerBody = 11;
		break;
	case ID_SETCENTER12:
		this->clModel->centerBody = 12;
		break;
	case ID_SETCENTER13:
		this->clModel->centerBody = 13;
		break;
	case ID_SETCENTER14:
		this->clModel->centerBody = 14;
		break;
	case ID_SETCENTER15:
		this->clModel->centerBody = 15;
		break;
	case ID_SETCENTER16:
		this->clModel->centerBody = 16;
		break;
	default:
		this->clModel->centerBody = 0;
		break;
	}
	wxLogDebug( wxT( "set centerBody to %d day" ),this->clModel->centerBody );
	this->clModel->UpdateDisplay();
	this->UpdateStatusBar( 0 );
	this->Refresh( true );
}

void Frame::OnSaveInitialState( wxCommandEvent& event )
{
	wxLogDebug( wxT( "Saving" ) );
	wxFileDialog fileDialog( this, wxT( "Choose Save file" ), wxT( "" ), wxT( "" ), wxT( "*.bin;*.BIN" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
	if( fileDialog.ShowModal() == wxID_OK )
	{
		this->initialState->SaveInitialState( fileDialog.GetPath() );
	}
}

void Frame::OnExportSlf( wxCommandEvent& event )
{
	wxLogDebug( wxT( "Exporting to SLF file" ) );
	wxFileDialog fileDialog( this, wxT( "Choose Save file" ), wxT( "" ), wxT( "" ), wxT( "*.slf;*.SLF" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
	if( fileDialog.ShowModal() == wxID_OK )
	{
		this->initialState->ExportSLF( fileDialog.GetPath() );
	}
}

// displays a dialog for the user to chose and .bin file of initial conditions and then loads it
void Frame::OnLoadInitialState( wxCommandEvent& event )
{
	wxLogDebug( wxT( "Frame::LoadInitialState" ) );
	this->Stop();
	wxLogDebug( wxT( "Loading" ) );
	wxFileDialog fileDialog( this, wxT( "Choose file" ), wxT( "" ), wxT( "" ), wxT( "*.bin;*.BIN" ), wxFD_OPEN || wxFD_FILE_MUST_EXIST );
	if( fileDialog.ShowModal() == wxID_OK )
	{
		if( this->initialState->LoadInitialState( fileDialog.GetPath() ) )
		{
			if( this->numParticles > this->initialState->initialNumParticles )
			{
				this->numParticles = this->initialState->initialNumParticles;
			}
			this->numGrav = this->initialState->initialNumGrav > this->clModel->maxNumGrav ? this->clModel->maxNumGrav : this->initialState->initialNumGrav;
			this->ResetAll();
		}
	}
}

// copys the currently displayed positions and velocities to the initial state
// All future resets will set the simulation back to this state
void Frame::OnReadToInitialState( wxCommandEvent& event )
{
	wxLogDebug( wxT( "Frame::ReadToInitialState" ) );
	this->Stop();
	this->clModel->ReadToInitialState( this->initialState->initialPositions,this->initialState->initialVelocities );
	this->initialState->initialJulianDate = this->clModel->julianDate + ( this->clModel->time )*1/( 60*60*24 );
	this->initialState->initialNumParticles = this->numParticles;
}

// updates the Menu after loading in a new initial.bin or slf file
void Frame::UpdateMenuItems()
{
	wxMenuBar *menuBar = this->GetMenuBar();
	if( menuBar != NULL )
	{
		wxMenuItem *menuItem;
		switch( this->numGrav )
		{
		case 16:
			menuItem = menuBar->FindItem( ID_SETGRAV16 );
			menuItem->Check( true );
			break;
		case 32:
			menuItem = menuBar->FindItem( ID_SETGRAV32 );
			menuItem->Check( true );
			break;
		case 64:
			menuItem = menuBar->FindItem( ID_SETGRAV64 );
			menuItem->Check( true );
			break;
		case 128:
			menuItem = menuBar->FindItem( ID_SETGRAV128 );
			menuItem->Check( true );
			break;
		case 192:
			menuItem = menuBar->FindItem( ID_SETGRAV192 );
			menuItem->Check( true );
			break;
		case 256:
			menuItem = menuBar->FindItem( ID_SETGRAV256 );
			menuItem->Check( true );
			break;
		case 384:
			menuItem = menuBar->FindItem( ID_SETGRAV384 );
			menuItem->Check( true );
			break;
		case 512:
			menuItem = menuBar->FindItem( ID_SETGRAV512 );
			menuItem->Check( true );
			break;
		default:
			menuItem = menuBar->FindItem( ID_SETGRAVMAX );
			menuItem->Check( true );
			break;
		}

		switch( this->numParticles )
		{
		case 2*1024:
			menuItem = menuBar->FindItem( ID_SETNUM2048 );
			menuItem->Check( true );
			break;
		case 4*1024:
			menuItem = menuBar->FindItem( ID_SETNUM4096 );
			menuItem->Check( true );
			break;
		case 8*1024:
			menuItem = menuBar->FindItem( ID_SETNUM8192 );
			menuItem->Check( true );
			break;
		case 16*1024:
			menuItem = menuBar->FindItem( ID_SETNUM16384 );
			menuItem->Check( true );
			break;
		case 32*1024:
			menuItem = menuBar->FindItem( ID_SETNUM32768 );
			menuItem->Check( true );
			break;
		case 64*1024:
			menuItem = menuBar->FindItem( ID_SETNUM65535 );
			menuItem->Check( true );
			break;
		case 128*1024:
			menuItem = menuBar->FindItem( ID_SETNUM131072 );
			menuItem->Check( true );
			break;
		case 256*1024:
			menuItem = menuBar->FindItem( ID_SETNUM262144 );
			menuItem->Check( true );
			break;
		case 512*1024:
			menuItem = menuBar->FindItem( ID_SETNUM524288 );
			menuItem->Check( true );
			break;
		default:
			menuItem = menuBar->FindItem( ID_SETNUMMAX );
			menuItem->Check( true );
			break;
		}

		switch( ( int )this->clModel->delT )
		{
		case 15*60:
			menuItem = menuBar->FindItem( ID_SETDELTAT15 );
			menuItem->Check( true );
			break;
		case 60*60:
			menuItem = menuBar->FindItem( ID_SETDELTATHR );
			menuItem->Check( true );
			break;
		case 4*60*60:
			menuItem = menuBar->FindItem( ID_SETDELTATFOURHR );
			menuItem->Check( true );
			break;
		case 12*60*60:
			menuItem = menuBar->FindItem( ID_SETDELTAT12HR );
			menuItem->Check( true );
			break;
		case 24*60*60:
			menuItem = menuBar->FindItem( ID_SETDELTATDAY );
			menuItem->Check( true );
			break;
		case -15*60:
			menuItem = menuBar->FindItem( ID_SETDELTATMINUS15 );
			menuItem->Check( true );
			break;
		case -60*60:
			menuItem = menuBar->FindItem( ID_SETDELTATMINUSHR );
			menuItem->Check( true );
			break;
		case -4*60*60:
			menuItem = menuBar->FindItem( ID_SETDELTATMINUSFOURHR );
			menuItem->Check( true );
			break;
		case -12*60*60:
			menuItem = menuBar->FindItem( ID_SETDELTATMINUS12HR );
			menuItem->Check( true );
			break;
		case -24*60*60:
			menuItem = menuBar->FindItem( ID_SETDELTATMINUSDAY );
			menuItem->Check( true );
			break;
		}

		switch( this->glCanvas->blending )
		{
		case true:
			menuItem = menuBar->FindItem( ID_BLENDING );
			menuItem->Check( true );
			break;
		default:
			menuItem = menuBar->FindItem( ID_BLENDING );
			menuItem->Check( false );
			break;
		}

		menuItem = menuBar->FindItem( ID_LOGENCOUNTERS );
		menuItem->Check( this->checkForEncounters );
	}
}

void Frame::OnImportSlf( wxCommandEvent& WXUNUSED( event ) )
{
	this->Stop();
	wxLogDebug( wxT( "Importing Solex SFL file" ) );
	wxFileDialog fileDialog( this, wxT( "Choose Solex SLF file to Import" ), wxT( "" ), wxT( "" ), wxT( "*.SLF;*.slf" ), wxFD_OPEN );
	if( fileDialog.ShowModal() == wxID_OK )
	{
		if( this->initialState->ImportSLF( fileDialog.GetPath() ) )
		{
			this->numParticles = this->initialState->initialNumParticles;
			this->numGrav = this->initialState->initialNumGrav > this->clModel->maxNumGrav ? this->clModel->maxNumGrav : this->initialState->initialNumGrav;
			this->ResetAll();
		}
	}
}

void Frame::ResetAll()
{
	bool die = false;
#ifdef __WXDEBUG__
	wxLogDebug( wxT( "Frame::ResetAll threadId: %ld" ),wxThread::GetCurrentId() );
#endif

	this->Stop();
	try
	{
		this->clModel->CleanUpCL();
		this->glCanvas->CleanUpGL();
		this->glCanvas->CreateOpenGlContext( this->numParticles, this->numGrav );
		this->ChooseDevice( this->config );
		this->clModel->CreateBufferObjects( this->glCanvas->getVbo(), this->numParticles, this->numGrav );
		this->clModel->CompileProgramAndCreateKernels();
		this->clModel->SetInitalState( this->initialState->initialPositions,this->initialState->initialVelocities );
		this->glCanvas->SetColours( this->initialState->initialColorData );
		this->clModel->julianDate = this->initialState->initialJulianDate;
		this->clModel->time = 0.0f;
		this->UpdateStatusBar( 0 );
		this->clModel->SetKernelArgumentsAndGroupSize();
		this->clModel->UpdateDisplay();
		this->Refresh( false );
		this->UpdateMenuItems();
	}
	catch ( int ex )
	{
		wxLogError( wxT( "Fatal Exception %d" ),ex );
		die = true;
	}

	if( die )
	{
		this->Close( true );
	}
}

// set the number of bodies (particles)
void Frame::OnSetNum( wxCommandEvent& event )
{
	switch( event.GetId() )
	{
	case ID_SETNUM2048:
		this->numParticles = 2*1024 < this->initialState->initialNumParticles ? 2*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM4096:
		this->numParticles = 4*1024 < this->initialState->initialNumParticles ? 4*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM8192:
		this->numParticles = 8*1024 < this->initialState->initialNumParticles ? 8*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM16384:
		this->numParticles = 16*1024 < this->initialState->initialNumParticles ? 16*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM32768:
		this->numParticles = 32*1024 < this->initialState->initialNumParticles ? 32*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM65535:
		this->numParticles = 64*1024 < this->initialState->initialNumParticles ? 64*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM131072:
		this->numParticles = 128*1024 < this->initialState->initialNumParticles ? 128*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM262144:
		this->numParticles = 256*1024 < this->initialState->initialNumParticles ? 256*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUM524288:
		this->numParticles = 512*1024 < this->initialState->initialNumParticles ? 512*1024 : this->initialState->initialNumParticles;
		break;
	case ID_SETNUMMAX:
		this->numParticles = this->initialState->initialNumParticles;
		break;
	default:
		this->numParticles = 256*32 < this->initialState->initialNumParticles ? 256*32 : this->initialState->initialNumParticles;
		break;
	}
	this->ResetAll();
}

// set the number of bodies which have a gravitation effect
void Frame::OnSetGrav( wxCommandEvent& event )
{
	switch( event.GetId() )
	{
	case ID_SETGRAV16:
		this->numGrav = 16;
		break;
	case ID_SETGRAV32:
		this->numGrav = 32;
		break;
	case ID_SETGRAV64:
		this->numGrav = 64;
		break;
	case ID_SETGRAV128:
		this->numGrav = 128;
		break;
	case ID_SETGRAV192:
		this->numGrav = 192;
		break;
	case ID_SETGRAV256:
		this->numGrav = 256;
		break;
	case ID_SETGRAV384:
		this->numGrav = 384;
		break;
	case ID_SETGRAV512:
		this->numGrav = 512;
		break;
	case ID_SETGRAVMAX:
		this->numGrav = this->clModel->maxNumGrav;
		break;
	default:
		this->numGrav = 16;
		break;
	}
	this->initialState->initialNumGrav = this->numGrav;
	this->initialState->SetDefaultBodyColours();
	this->ResetAll();
}

// Set blending for the asteroids.
// Makes them semi transparent
void Frame::OnBlending( wxCommandEvent& event )
{
	this->glCanvas->blending = ! this->glCanvas->blending;
	this->Refresh( true );
}

// Sets the Integrator used to compute new positions and velocities
void Frame::OnSetIntegrator( wxCommandEvent& event )
{
	switch( event.GetId() )
	{
	case ID_SETADAMS4:
		this->clModel->adamsBashforthKernelName = new wxString( "adamsBashforth4" );
		this->clModel->adamsMoultonKernelName = new wxString( "adamsMoulton3" );
		break;
	case ID_SETADAMS8:
		this->clModel->adamsBashforthKernelName = new wxString( "adamsBashforth8" );
		this->clModel->adamsMoultonKernelName = new wxString( "adamsMoulton7" );
		break;
	case ID_SETADAMS10:
		this->clModel->adamsBashforthKernelName = new wxString( "adamsBashforth10" );
		this->clModel->adamsMoultonKernelName = new wxString( "adamsMoulton9" );
		break;
	case ID_SETADAMS11:
		this->clModel->adamsBashforthKernelName = new wxString( "adamsBashforth11" );
		this->clModel->adamsMoultonKernelName = new wxString( "adamsMoulton10" );
		break;
	case ID_SETADAMS12:
		this->clModel->adamsBashforthKernelName = new wxString( "adamsBashforth12" );
		this->clModel->adamsMoultonKernelName = new wxString( "adamsMoulton11" );
		break;
	case ID_SETADAMS16:
		this->clModel->adamsBashforthKernelName = new wxString( "adamsBashforth16" );
		this->clModel->adamsMoultonKernelName = new wxString( "adamsMoulton15" );
		break;
	default:
		this->clModel->adamsBashforthKernelName = new wxString( "adamsBashforth11" );
		this->clModel->adamsMoultonKernelName = new wxString( "adamsMoulton10" );
		break;
	}
	this->ResetAll();
}

// sets the Acceleration calculation kernel to use
void Frame::OnSetAcceleration( wxCommandEvent& event )
{
	switch( event.GetId() )
	{
	case ID_SETNEWTONIAN:
		this->clModel->accelerationKernelName = new wxString( "newtonian" );
		break;
	case ID_SETRELATIVISTIC:
		this->clModel->accelerationKernelName = new wxString( "relativistic" );
		break;
	case ID_SETRELATIVISTICL:
		this->clModel->accelerationKernelName = new wxString( "relativisticLocal" );
		break;
	default:
		this->clModel->accelerationKernelName = new wxString( "relativistic" );
		break;
	}
	this->ResetAll();
}

// Intercept menu commands
void Frame::OnExit( wxCommandEvent& WXUNUSED( event ) )
{
	// true is to force the frame to close
	wxLogDebug( wxT( "Frame OnExit" ) );
	this->Close( true );
}

void Frame::OnClose( wxCloseEvent& event )
{
	wxLogDebug( wxT( "Frame OnClose" ) );
	this->Stop();
	this->Destroy();
}

void Frame::OnAbout( wxCommandEvent& WXUNUSED( event ) )
{
	wxString message;
	message.Printf( wxT( "Solar System Simulation\n (c) 2013 Michael Simmons\nbody count:%d\nWith Mass:%d\nMax Count Possible:%d\nPredictor %s\nCorrector %s\nPlatform: %s\nDevice: %s\nCL Version: %s\nWeb: https://sourceforge.net/projects/openclsolarsyst/" ),
	                this->numParticles, this->numGrav, this->clModel->maxNumParticles, this->clModel->adamsBashforthKernelName->c_str(),this->clModel->adamsMoultonKernelName->c_str(), this->clModel->platformName->c_str(), this->clModel->deviceName->c_str(),this->clModel->deviceCLVersion->c_str() );
	wxMessageBox( message, wxT( "About NBody" ), wxOK | wxICON_INFORMATION );
}
void Frame::OnLogEncounters( wxCommandEvent& event )
{
	this->checkForEncounters = !this->checkForEncounters;
	this->Refresh( true );
}
void Frame::OnResetColours( wxCommandEvent& event )
{
	this->initialState->SetDefaultBodyColours();
	this->glCanvas->SetColours( this->initialState->initialColorData );
	this->Refresh( true );
}
