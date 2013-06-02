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
#ifndef FRAME_H_
#define FRAME_H_

#ifndef GLCANVAS_H_
#include "glcanvas.hpp"
#endif // #ifndef GLCANVAS_H_

#ifndef CLMODEL_H
#include "clmodel.hpp"
#endif // #ifndef CLMODEL_H

#ifndef INITIALSTATE_HPP
#include "initialstate.hpp"
#endif

class Frame: public wxFrame
{
	public:
		Frame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE | wxMAXIMIZE);
		virtual ~Frame();
		bool InitFrame(bool doubleBuffer, bool smooth, bool lighting, int numParticles, int numGrav, bool preferCpu, char *desiredPlatform);

	private:
#if wxUSE_GLCANVAS
		GLCanvas *glCanvas;
		CLModel *clModel;
#endif
		wxTimer *timer;
		bool clModelOk;
		char *desiredPlatform;
		bool preferCpu;
		bool runOnIdle;
		int numParticles;
		int numGrav;
		InitialState *initialState;
		double stopDateJdn;
		bool goingToDate;
		wxStopWatch stopWatch;
		void UpdateStatusBar(wxLongLong timeTaken);
		void ResetAll();
		void UpdateMenuItems();
		void Start();
		void Stop();
		void DoStep();

		void OnExit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnStart(wxCommandEvent& event);
		void OnStop(wxCommandEvent& event);
		void OnReset(wxCommandEvent& event);
		void OnSetIntegrator(wxCommandEvent& event);
		void OnSetDeltaTime(wxCommandEvent& event);
		void OnSetNum(wxCommandEvent& event);
		void OnSetGrav(wxCommandEvent& event);
		void OnSetCenter(wxCommandEvent& event);
		void OnSetNewtonian(wxCommandEvent& event);
		void OnImportSlf(wxCommandEvent& event);
		void OnExportSlf(wxCommandEvent& event);
		void OnSetRelativistic(wxCommandEvent& event);
		void OnSaveInitialState(wxCommandEvent& event);
		void OnLoadInitialState(wxCommandEvent& event);
		void OnReadToInitialState(wxCommandEvent& event);
		void OnBlending(wxCommandEvent& event);
		void OnTimer(wxTimerEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnIdle(wxIdleEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif // #ifndef Frame_H_
