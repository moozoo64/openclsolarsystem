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
    ID_IMPORTSLF,
	ID_SETDELTAT15,
	ID_SETDELTATHR,
	ID_SETDELTATFOURHR,
	ID_SETDELTATDAY,
	ID_SETDELTATMINUS15,
	ID_SETDELTATMINUSHR,
	ID_SETDELTATMINUSFOURHR,
	ID_SETDELTATMINUSDAY,
	ID_SETNUMSMALL,
	ID_SETNUMMEDIUM,
	ID_SETNUMLARGE,
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
	ID_SETCENTER10
};

// mapping of UI event ids to functions
BEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_MENU(wxID_EXIT, Frame::OnExit)
	EVT_MENU(wxID_ABOUT, Frame::OnAbout)
	EVT_MENU(ID_START, Frame::Start)
	EVT_MENU(ID_STOP, Frame::Stop)
	EVT_MENU(ID_RESET, Frame::Reset)
	EVT_MENU(ID_IMPORTSLF, Frame::ImportSlf)
	EVT_MENU(ID_SETADAMS4, Frame::SetIntegrator)
	EVT_MENU(ID_SETADAMS8, Frame::SetIntegrator)
	EVT_MENU(ID_SETADAMS10, Frame::SetIntegrator)
	EVT_MENU(ID_SETADAMS11, Frame::SetIntegrator)
	EVT_MENU(ID_SETADAMS12, Frame::SetIntegrator)
	EVT_MENU(ID_SETADAMS16, Frame::SetIntegrator)
	EVT_MENU(ID_SETDELTATMINUS15, Frame::SetDeltaTime)
	EVT_MENU(ID_SETDELTATMINUSHR, Frame::SetDeltaTime)
	EVT_MENU(ID_SETDELTATMINUSFOURHR, Frame::SetDeltaTime)
	EVT_MENU(ID_SETDELTATMINUSDAY, Frame::SetDeltaTime)
	EVT_MENU(ID_SETDELTAT15, Frame::SetDeltaTime)
	EVT_MENU(ID_SETDELTATHR, Frame::SetDeltaTime)
	EVT_MENU(ID_SETDELTATFOURHR, Frame::SetDeltaTime)
	EVT_MENU(ID_SETDELTATDAY, Frame::SetDeltaTime)
	EVT_MENU(ID_SETNUMSMALL, Frame::SetNum)
	EVT_MENU(ID_SETNUMMEDIUM, Frame::SetNum)
	EVT_MENU(ID_SETNUMLARGE, Frame::SetNum)
	EVT_MENU(ID_SETNUMMAX, Frame::SetNum)
	EVT_MENU(ID_SETGRAV16, Frame::SetGrav)
	EVT_MENU(ID_SETGRAV32, Frame::SetGrav)
	EVT_MENU(ID_SETGRAV64, Frame::SetGrav)
	EVT_MENU(ID_SETGRAV128, Frame::SetGrav)
	EVT_MENU(ID_SETGRAV192, Frame::SetGrav)
	EVT_MENU(ID_SETGRAV256, Frame::SetGrav)
	EVT_MENU(ID_SETGRAV384, Frame::SetGrav)
	EVT_MENU(ID_SETGRAV512, Frame::SetGrav)
	EVT_MENU(ID_SETGRAVMAX, Frame::SetGrav)
	EVT_MENU(ID_SETCENTER0, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER1, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER2, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER3, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER4, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER5, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER6, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER7, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER8, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER9, Frame::SetCenter)
	EVT_MENU(ID_SETCENTER10, Frame::SetCenter)
	EVT_MENU(ID_SETNEWTONIAN, Frame::SetNewtonian)
	EVT_MENU(ID_SETRELATIVISTIC, Frame::SetRelativistic)
	EVT_MENU(ID_SAVESTATE, Frame::SaveInitialState)
	EVT_MENU(ID_LOADSTATE, Frame::LoadInitialState)
	EVT_MENU(ID_READSTATE, Frame::ReadToInitialState)
	EVT_MENU(ID_EXPORTSLF, Frame::ExportSlf)
	EVT_MENU(ID_BLENDING, Frame::Blending)
	EVT_TIMER(ID_TIMER, Frame::OnTimer)
	EVT_CLOSE(Frame::OnClose)
END_EVENT_TABLE()

// My frame constructor
Frame::Frame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: wxFrame(frame, wxID_ANY, title, pos, size, style)
{
	wxLogDebug(wxT("Making Frame"));
	this->glCanvas = NULL;
	this->clModel = NULL;
	this->timer = new wxTimer(this,ID_TIMER);
	this->initialState = new InitialState();
	this->stopDateJdn = 2456430.5;
	this->goingToDate = false;
}

Frame::~Frame()
{
	wxLogDebug(wxT("Cleaning up Frame"));
	this->timer->Stop();
	delete this->clModel;
	delete this->glCanvas;
	delete this->timer;
	delete this->initialState;
}

bool Frame::InitFrame(bool doubleBuffer, bool smooth, bool lighting, int numParticles, int numGrav, bool preferCpu, char *desiredPlatform)
{
	this->numParticles = numParticles;
	this->numGrav = numGrav;
	this->preferCpu = preferCpu;
	this->desiredPlatform = desiredPlatform;

	try
	{
		this->initialState->initialNumGrav = this->numGrav;
		if(!this->initialState->LoadInitialState(wxT("initial.bin")))
		{
			this->initialState->initialNumParticles = this->numParticles;
			if(!this->initialState->CreateRandomInitialConfig())
			{
				throw -1;
			}
		}
		
		this->initialState->SetDefaultBodyColours();
		this->numParticles = this->numParticles > this->initialState->initialNumParticles ? this->initialState->initialNumParticles : this->numParticles;
		
		// Give it an icon
		this->SetIcon(wxICON(sample));
	
		// Create a menu that lets the user save, load and exit
		wxMenu *menuFile = new wxMenu;
		menuFile->Append( wxID_ABOUT, wxT("&About..." ));
		menuFile->AppendSeparator();
		menuFile->Append( ID_SAVESTATE, wxT("Save Initial" ));
		menuFile->Append( ID_LOADSTATE, wxT("Load Initial" ));
		menuFile->Append( ID_READSTATE, wxT("Set Initial" ));
		menuFile->Append( ID_IMPORTSLF, wxT("&Import .SLF File" ));
		menuFile->Append( ID_EXPORTSLF, wxT("&Export .SLF File" ));
		menuFile->Append( wxID_EXIT, wxT("E&xit" ));
		
		// Create a menu that lets the user control the running of the simulation
		wxMenu *menuGo = new wxMenu;
		menuGo->Append( ID_START, wxT("&Start" ));
		menuGo->Append( ID_STOP, wxT("S&top" ));
		menuGo->Append( ID_RESET, wxT("&Reset" ));

		// Create a menu that lets the user choose the menthod used to calculate updated positions and velocities
		// Only one option can be chosen at any time
		wxMenu *menuIntegrator = new wxMenu;
		menuIntegrator->AppendRadioItem( ID_SETADAMS4, wxT("Adams Bashforth Moulton 4" ));
		menuIntegrator->AppendRadioItem( ID_SETADAMS8, wxT("Adams Bashforth Moulton 8" ));
		menuIntegrator->AppendRadioItem( ID_SETADAMS10, wxT("Adams Bashforth Moulton 10" ));
		menuIntegrator->AppendRadioItem( ID_SETADAMS11, wxT("Adams Bashforth Moulton 11" ));
		menuIntegrator->AppendRadioItem( ID_SETADAMS12, wxT("Adams Bashforth Moulton 12" ));
		menuIntegrator->AppendRadioItem( ID_SETADAMS16, wxT("Adams Bashforth Moulton 16" ));
		
		// Create a menu that lets the user choose the gravity acceleration calculation method
		// Only one option can be chosen at any time
		wxMenu *menuGravity = new wxMenu;
		menuGravity->AppendRadioItem( ID_SETNEWTONIAN, wxT("Newtonian"));
		menuGravity->AppendRadioItem( ID_SETRELATIVISTIC, wxT("With Relativistic corrections"));
	
		// Create a menu that lets the user choose the time step size.
		// Only one option can be chosen at any time
		wxMenu *menuDeltaT = new wxMenu;
		menuDeltaT->AppendRadioItem( ID_SETDELTAT15, wxT("15 Mins" ));
		menuDeltaT->AppendRadioItem( ID_SETDELTATHR, wxT("1 Hour" ));
		menuDeltaT->AppendRadioItem( ID_SETDELTATFOURHR, wxT("4 Hours" ));
		menuDeltaT->AppendRadioItem( ID_SETDELTATDAY, wxT("1 Day" ));
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUS15, wxT("-15 Mins" ));
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUSHR, wxT("-1 Hour" ));
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUSFOURHR, wxT("-4 Hours" ));
		menuDeltaT->AppendRadioItem( ID_SETDELTATMINUSDAY, wxT("-1 Day" ));
		
		// Create a menu that lets the user choose the number of bodies/particles.
		// Only one option can be chosen at any time
		wxMenu *menuNum = new wxMenu;
		menuNum->AppendRadioItem( ID_SETNUMSMALL, wxT("8192" ));
		menuNum->AppendRadioItem( ID_SETNUMMEDIUM, wxT("309760" ));
		menuNum->AppendRadioItem( ID_SETNUMLARGE, wxT("594688" ));
		menuNum->AppendRadioItem( ID_SETNUMMAX, wxT("Maximum" ));
		
		// Create a menu that lets the user choose the number of bodies/particles with gravitional effects 
		// Only one option can be chosen at any time
		wxMenu *menuGrav = new wxMenu;
		menuGrav->AppendRadioItem( ID_SETGRAV16, wxT("16" ));
		menuGrav->AppendRadioItem( ID_SETGRAV32, wxT("32" ));
		menuGrav->AppendRadioItem( ID_SETGRAV64, wxT("64" ));
		menuGrav->AppendRadioItem( ID_SETGRAV128, wxT("128" ));
		menuGrav->AppendRadioItem( ID_SETGRAV192, wxT("192" ));
		menuGrav->AppendRadioItem( ID_SETGRAV256, wxT("256" ));
		menuGrav->AppendRadioItem( ID_SETGRAV384, wxT("384" ));
		menuGrav->AppendRadioItem( ID_SETGRAV512, wxT("512" ));
		menuGrav->AppendRadioItem( ID_SETGRAVMAX, wxT("Maximum" ));
		
		// Create a menu that lets the user choose a body to center the display on.
		// Only one option can be chosen at any time
		wxMenu *menuCenterOn = new wxMenu;
		menuCenterOn->AppendRadioItem( ID_SETCENTER0, wxT("0" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER1, wxT("1" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER2, wxT("2" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER3, wxT("3" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER4, wxT("4" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER5, wxT("5" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER6, wxT("6" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER7, wxT("7" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER8, wxT("8" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER9, wxT("9" ));
		menuCenterOn->AppendRadioItem( ID_SETCENTER10, wxT("10" ));
		
		// Create a menu that lets the user choose how asteroids are displayed.
		wxMenu *menuOptions = new wxMenu;
		menuOptions->AppendCheckItem( ID_BLENDING, wxT("Blending"));
		
		// Add the menus to the windows menu bar
		wxMenuBar *menuBar = new wxMenuBar;
		menuBar->Append( menuFile, wxT("&File" ));
		menuBar->Append( menuGo, wxT("&Go" ));
		menuBar->Append( menuIntegrator, wxT("Integrator" ));
		menuBar->Append( menuGravity, wxT("Gravity" ));
		menuBar->Append( menuDeltaT, wxT("Time Delta"));
		menuBar->Append( menuNum, wxT("Number Bodies"));
		menuBar->Append( menuGrav, wxT("Number with Mass"));
		menuBar->Append( menuCenterOn, wxT("Center on Body"));
		menuBar->Append( menuOptions, wxT("Options"));
		this->SetMenuBar( menuBar );
		wxLogDebug(wxT("Created Menu"));

		// Create the status bar to display messages and the current simulated date and step count
		this->CreateStatusBar();
		this->SetStatusText( wxT("Nbody"));
		this->Show(true);

		// Create the OpenGL canvas on which the simulation will be displayed
		this->glCanvas = new GLCanvas(this, wxID_ANY, wxDefaultPosition,wxDefaultSize, 0, wxT("GLCanvas"));
		this->glCanvas->blending = true;
		this->glCanvas->InitGL(this->numParticles, this->numGrav);
		
		// Create an openCL model to run the simulation and initialise it
		this->clModel = new CLModel();
		this->clModel->InitCL(this->glCanvas->getVbo(),(char *)desiredPlatform, this->preferCpu, this->numParticles, this->numGrav);
		this->glCanvas->SetColours(this->initialState->initialColorData);
		this->clModel->SetInitalState(this->initialState->initialPositions,this->initialState->initialVelocities);
		this->clModel->julianDate = this->initialState->initialJulianDate;
		this->clModel->time = 0.0f;
		this->clModel->InitKernels();
		this->clModel->UpdateDisplay();
		
		// set the keyboard focus to the simmulation display so that the keyboard functions work
		this->glCanvas->SetFocus();
	}
	catch (int ex)
	{
		wxLogError(wxT("Exception %d"),ex);
	}
	wxLogDebug(wxT("Init Frame Succeeded"));

	return true;
}

// Take the simmulation date, time and step count and display it in the status bar
void Frame::DisplayDate()
{
	double jdn = this->clModel->julianDate + (this->clModel->time)*1/(60*60*24);// (11574.07407f*this->clModel->time); // 0.000001 = 1000 seconds
	wxDateTime dateTime;
	dateTime.Set(jdn);
	wxString message;
	message.Printf("Julian Day: %f Date: %10s %8s step: %d",jdn,dateTime.FormatISODate().c_str(),dateTime.FormatISOTime().c_str(), this->clModel->step);
	this->SetStatusText(message);
}

// The interval timer triggers the advancement to the next step
void Frame::OnTimer(wxTimerEvent& event)
{
	if(!IsShown()) return;
	static bool InOnTimer = false;

	wxLogDebug(wxT("Timer Fired"));
	if(InOnTimer)
	{
		wxLogDebug(wxT("Already in OnTimer"));
		return;
	}

	InOnTimer = true;

	try{
		this->clModel->ExecuteKernel();
		this->clModel->RequestUpdate();
		this->clModel->ExecuteKernel();
	}catch( int e)
	{
		this->timer->Stop();
	}
	
	this->DisplayDate();
	
	// check if going a date
	if(this->goingToDate)
	{
		double currentJdn = this->clModel->julianDate + (this->clModel->time)*1/(60*60*24);
		if(this->clModel->delT > 0)
		{
			if(currentJdn >= this->stopDateJdn)
			{
				this->timer->Stop();
				this->goingToDate = false;
			}
		}
		else
		{
			if(currentJdn <= this->stopDateJdn)
			{
				this->timer->Stop();
				this->goingToDate = false;
			}
		}
	}

	this->Refresh(false);
	InOnTimer = false;
}

void Frame::Start( wxCommandEvent& WXUNUSED(event) )
{
	wxLogDebug(wxT("Frame Start"));
	this->timer->Start(1,false);
}

void Frame::Stop( wxCommandEvent& WXUNUSED(event) )
{
	wxLogDebug(wxT("Frame Stop"));
	this->timer->Stop();
	this->DisplayDate();
}

void Frame::Reset( wxCommandEvent& WXUNUSED(event) )
{
	this->numParticles = this->numParticles > this->initialState->initialNumParticles ? this->initialState->initialNumParticles : this->numParticles;
	this->ResetAll();
}

// sets the time delta between steps
void Frame::SetDeltaTime(wxCommandEvent& event)
{
	this->timer->Stop();
	int id = event.GetId();
	switch(id)
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
		case ID_SETDELTATMINUS15:
			this->clModel->delT = -15*60.0f;
			break;
		case ID_SETDELTATMINUSHR:
			this->clModel->delT = -60*60.0f;
			break;
		case ID_SETDELTATMINUSFOURHR:
			this->clModel->delT = -4*60*60.0f;
			break;
		case ID_SETDELTATMINUSDAY:
			this->clModel->delT = -24*60*60.0f;
			break;
		default:
			this->clModel->delT = 4*60*60.0f;
			break;
	}
	this->clModel->step = 0;
	this->clModel->InitKernels();
}

// sets the body to center the display on
void Frame::SetCenter(wxCommandEvent& event)
{
	int id = event.GetId();

	switch(id)
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
		default:
			this->clModel->centerBody = 0;
			break;
	}
	wxLogDebug(wxT("set centerBody to %d day"),this->clModel->centerBody);
	this->clModel->UpdateDisplay();
	this->Refresh(true);
}

void Frame::SaveInitialState(wxCommandEvent& event)
{
	wxLogDebug(wxT("Saving"));
	wxFileDialog fileDialog(this, wxT("Choose Save file"), wxT(""), wxT(""), wxT("*.bin"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if(fileDialog.ShowModal() == wxID_OK)
	{
		this->initialState->SaveInitialState(fileDialog.GetPath());
	}
}

void Frame::ExportSlf(wxCommandEvent& event)
{
	wxLogDebug(wxT("Exporting to SLF file"));
	wxFileDialog fileDialog(this, wxT("Choose Save file"), wxT(""), wxT(""), wxT("*.slf"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if(fileDialog.ShowModal() == wxID_OK)
	{
		this->initialState->ExportSLF(fileDialog.GetPath());
	}
}

// displays a dialog for the user to chose and .bin file of initial conditions and then loads it
void Frame::LoadInitialState(wxCommandEvent& event)
{
	wxLogDebug(wxT("Frame Stop"));
	this->timer->Stop();
	wxLogDebug(wxT("Loading"));
	wxFileDialog fileDialog(this, wxT("Choose file"), wxT(""), wxT(""), wxT("*.bin"), wxFD_OPEN || wxFD_FILE_MUST_EXIST);
	if(fileDialog.ShowModal() == wxID_OK)
	{		
		if(this->initialState->LoadInitialState(fileDialog.GetPath()))
		{
			if(this->numParticles > this->initialState->initialNumParticles)
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
void Frame::ReadToInitialState(wxCommandEvent& event)
{
	wxLogDebug(wxT("Frame Stop"));
	this->timer->Stop();
	this->clModel->ReadToInitialState(this->initialState->initialPositions,this->initialState->initialVelocities);
	this->initialState->initialJulianDate = this->clModel->julianDate + (this->clModel->time)*1/(60*60*24);
	this->initialState->initialNumParticles = this->numParticles;
}

// updates the Menu after loading in a new initial.bin or slf file 
void Frame::UpdateMenuItems()
{
	wxMenuBar *menuBar = this->GetMenuBar();
	if(menuBar != NULL)
	{
		wxMenuItem *menuItem;
		switch(this->numGrav)
		{
			case 16:
				menuItem = menuBar->FindItem(ID_SETGRAV16);
				menuItem->Check(true);
				break;
			case 32:
				menuItem = menuBar->FindItem(ID_SETGRAV32);
				menuItem->Check(true);
				break;
			case 64:
				menuItem = menuBar->FindItem(ID_SETGRAV64);
				menuItem->Check(true);
				break;
			case 128:
				menuItem = menuBar->FindItem(ID_SETGRAV128);
				menuItem->Check(true);
				break;
			case 192:
				menuItem = menuBar->FindItem(ID_SETGRAV192);
				menuItem->Check(true);
				break;
			case 256:
				menuItem = menuBar->FindItem(ID_SETGRAV256);
				menuItem->Check(true);
				break;
			case 384:
				menuItem = menuBar->FindItem(ID_SETGRAV384);
				menuItem->Check(true);
				break;
			case 512:
				menuItem = menuBar->FindItem(ID_SETGRAV512);
				menuItem->Check(true);
				break;
			default:
				menuItem = menuBar->FindItem(ID_SETGRAVMAX);
				menuItem->Check(true);
				break;
		}

		switch(this->numParticles)
		{
			case 256*32:
				menuItem = menuBar->FindItem(ID_SETNUMSMALL);
				menuItem->Check(true);
				break;
			case 256*1210:
				menuItem = menuBar->FindItem(ID_SETNUMMEDIUM);
				menuItem->Check(true);
				break;
			case 256*2323:
				menuItem = menuBar->FindItem(ID_SETNUMLARGE);
				menuItem->Check(true);
				break;
			default:
				menuItem = menuBar->FindItem(ID_SETNUMMAX);
				menuItem->Check(true);
				break;
		}

		switch((int)this->clModel->delT)
		{
			case 15*60:
				menuItem = menuBar->FindItem(ID_SETDELTAT15);
				menuItem->Check(true);
				break;
			case 60*60:
				menuItem = menuBar->FindItem(ID_SETDELTATHR);
				menuItem->Check(true);
				break;
			case 4*60*60:
				menuItem = menuBar->FindItem(ID_SETDELTATFOURHR);
				menuItem->Check(true);
				break;
			case 24*60*60:
				menuItem = menuBar->FindItem(ID_SETDELTATDAY);
				menuItem->Check(true);
				break;
		}
		
		switch(this->glCanvas->blending)
		{
			case true:
				menuItem = menuBar->FindItem(ID_BLENDING);
				menuItem->Check(true);
				break;
			default:
				menuItem = menuBar->FindItem(ID_BLENDING);
				menuItem->Check(false);
				break;
		}
	}
}
		
void Frame::ImportSlf( wxCommandEvent& WXUNUSED(event) )
{
	wxLogDebug(wxT("Importing Solex SFL file"));
	wxFileDialog fileDialog(this, wxT("Choose Solex SLF file to Import"), wxT(""), wxT(""), wxT("*.SLF"), wxFD_OPEN);
	if(fileDialog.ShowModal() == wxID_OK)
	{
		if(this->initialState->ImportSLF(fileDialog.GetPath()))
		{
			if(this->numParticles > this->initialState->initialNumParticles)
			{
				this->numParticles = this->initialState->initialNumParticles;
			}
			this->ResetAll();
		}
	}
}

void Frame::ResetAll()
{
	wxLogDebug(wxT("Frame Stop"));
	this->timer->Stop();
	this->clModel->CleanUpCL();
	this->glCanvas->CleanUpGL();
	this->glCanvas->InitGL(this->numParticles, this->numGrav);
	this->glCanvas->SetColours(this->initialState->initialColorData);
	this->clModel->InitCL(this->glCanvas->getVbo(),(char *)this->desiredPlatform, this->preferCpu, this->numParticles, this->numGrav);
	this->clModel->SetInitalState(this->initialState->initialPositions,this->initialState->initialVelocities);
	this->clModel->julianDate = this->initialState->initialJulianDate;
	this->clModel->time = 0.0f;
	this->DisplayDate();
	this->clModel->InitKernels();
	this->clModel->UpdateDisplay();
	this->Refresh(false);
	this->UpdateMenuItems();
}

// set the number of bodies (particles)
void Frame::SetNum(wxCommandEvent& event)
{
	switch(event.GetId())
	{
		case ID_SETNUMSMALL:
			this->numParticles = 256*32 < this->initialState->initialNumParticles ? 256*32 : this->initialState->initialNumParticles;
			break;
		case ID_SETNUMMEDIUM:
			this->numParticles = 256*1210 < this->initialState->initialNumParticles ? 256*1210 : this->initialState->initialNumParticles;
			break;
		case ID_SETNUMLARGE:
			this->numParticles = 256*2323 < this->initialState->initialNumParticles ? 256*2323 : this->initialState->initialNumParticles;
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
void Frame::SetGrav(wxCommandEvent& event)
{
	switch(event.GetId())
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
void Frame::Blending(wxCommandEvent& event)
{
	this->glCanvas->blending = ! this->glCanvas->blending;
	this->Refresh(true);
}

// Sets the Integrator used to compute new positions and velocities
void Frame::SetIntegrator(wxCommandEvent& event)
{
	switch(event.GetId())
	{
		case ID_SETADAMS4:
			this->clModel->adamsBashforthKernelName = new wxString("adamsBashforth4");
			this->clModel->adamsMoultonKernelName = new wxString("adamsMoulton3");
			break;
		case ID_SETADAMS8:
			this->clModel->adamsBashforthKernelName = new wxString("adamsBashforth8");
			this->clModel->adamsMoultonKernelName = new wxString("adamsMoulton7");
			break;
		case ID_SETADAMS10:
			this->clModel->adamsBashforthKernelName = new wxString("adamsBashforth10");
			this->clModel->adamsMoultonKernelName = new wxString("adamsMoulton9");
			break;
		case ID_SETADAMS11:
			this->clModel->adamsBashforthKernelName = new wxString("adamsBashforth11");
			this->clModel->adamsMoultonKernelName = new wxString("adamsMoulton10");
			break;
		case ID_SETADAMS12:
			this->clModel->adamsBashforthKernelName = new wxString("adamsBashforth12");
			this->clModel->adamsMoultonKernelName = new wxString("adamsMoulton11");
			break;
		case ID_SETADAMS16:
			this->clModel->adamsBashforthKernelName = new wxString("adamsBashforth16");
			this->clModel->adamsMoultonKernelName = new wxString("adamsMoulton15");
			break;
		default:
			this->clModel->adamsBashforthKernelName = new wxString("adamsBashforth11");
			this->clModel->adamsMoultonKernelName = new wxString("adamsMoulton10");
			break;
	}
	this->ResetAll();
}

// sets the Acceleration calculation to use the Newtonian Gravitaton kernel
void Frame::SetNewtonian(wxCommandEvent& event)
{
	this->clModel->accelerationKernelName = new wxString("newtonian");
	this->ResetAll();
}

// sets the Acceleration calculation to use the Newtonian Gravitaton with Relativistic corrections kernel
void Frame::SetRelativistic(wxCommandEvent& event)
{
	this->clModel->accelerationKernelName = new wxString("relativistic");
	this->ResetAll();
}
		
// Intercept menu commands
void Frame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
	// true is to force the frame to close
	wxLogDebug(wxT("Frame OnExit"));
	this->Close(true);
}

void Frame::OnClose(wxCloseEvent& event)
{
	wxLogDebug(wxT("Frame OnClose"));
	this->timer->Stop();
	this->Destroy();
}

void Frame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString message;
	message.Printf(wxT("OpenCL/GL Solar System\n (c) 2013 Michael Simmons\nbody count:%d\nWith Mass:%d\nPredictor %s\nCorrector %s\nOpenCL Platform: %s\nOpenCL Device: %s"),this->numParticles, this->numGrav,this->clModel->adamsBashforthKernelName->c_str(),this->clModel->adamsMoultonKernelName->c_str(), this->clModel->platformName->c_str(), this->clModel->deviceName->c_str());
	wxMessageBox( message, wxT("About NBody"), wxOK | wxICON_INFORMATION );
}
