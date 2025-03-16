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

class Frame : public wxFrame
{
public:
  /**
   * Frame constructor - creates main application window
   * @param frame Parent frame pointer
   * @param title Window title
   * @param pos Window position
   * @param size Window size
   * @param style Window style flags
   */
  Frame(wxFrame *frame, const wxString &title, const wxPoint &pos,
        const wxSize &size, long style = wxDEFAULT_FRAME_STYLE | wxMAXIMIZE);

  virtual ~Frame(); /**< Destructor - cleans up resources */

  /**
   * Initializes frame with simulation parameters
   * @param doubleBuffer Enable double buffering for smoother rendering
   * @param smooth Enable smooth shading
   * @param lighting Enable OpenGL lighting
   * @param stereo Enable stereoscopic rendering
   * @param numParticles Number of particles to simulate
   * @param numGrav Number of gravitational bodies
   * @param useLastDevice Use previously selected OpenCL device
   * @param desiredPlatform Preferred OpenCL platform name
   * @param tryForCPUFirst Try CPU before GPU for computation
   */
  void InitFrame(bool doubleBuffer, bool smooth, bool lighting, bool stereo,
                 int numParticles, int numGrav, bool useLastDevice,
                 char *desiredPlatform, bool tryForCPUFirst);

private:
  // OpenGL/OpenCL Components
  GLCanvas *glCanvas; /**< OpenGL rendering canvas */
  CLModel *clModel;   /**< OpenCL computation model */
  wxTimer *timer;     /**< Animation timer */

  // System State
  bool clModelOk;          /**< OpenCL initialization status */
  char *desiredPlatform;   /**< Preferred OpenCL platform name */
  bool useLastDevice;      /**< Use previously selected device */
  bool tryForCPUFirst;     /**< Prefer CPU over GPU */
  bool runOnIdle;          /**< Run simulation during idle time */
  bool checkForEncounters; /**< Check for close encounters between bodies */

  // Simulation Parameters
  int numParticles;           /**< Current number of particles */
  int numGrav;                /**< Number of gravitational bodies */
  InitialState *initialState; /**< Initial simulation state */
  double stopDateJdn;         /**< Julian date to stop simulation */
  double encounterDistance;   /**< Distance threshold for encounters */
  bool goingToDate;           /**< Flag for time-targeted simulation */

  // System Components
  wxStopWatch stopWatch; /**< Performance timing */
  wxConfigBase *config;  /**< Application configuration */

  // Utility Methods
  /**
   * Updates status bar with simulation stats
   * @param timeTaken Time since last update in microseconds
   */
  void UpdateStatusBar(wxLongLong timeTaken);

  void ResetAll();        /**< Reset simulation to initial state */
  void UpdateMenuItems(); /**< Update menu checkmarks/labels */
  void Start();           /**< Start simulation */
  void Stop();            /**< Stop simulation */
  void DoStep();          /**< Execute one simulation step */

  /**
   * Select OpenCL compute device
   * @param config Application configuration for device persistence
   */
  void ChooseDevice(wxConfigBase *config);

  // Event Handlers
  void OnExit(wxCommandEvent &event);               /**< Handle exit command */
  void OnAbout(wxCommandEvent &event);              /**< Show about dialog */
  void OnStart(wxCommandEvent &event);              /**< Start simulation */
  void OnStop(wxCommandEvent &event);               /**< Stop simulation */
  void OnReset(wxCommandEvent &event);              /**< Reset simulation */
  void OnResetColours(wxCommandEvent &event);       /**< Reset body colors */
  void OnSetIntegrator(wxCommandEvent &event);      /**< Change integration method */
  void OnSetDeltaTime(wxCommandEvent &event);       /**< Change timestep */
  void OnSetNum(wxCommandEvent &event);             /**< Change particle count */
  void OnSetGrav(wxCommandEvent &event);            /**< Change gravity body count */
  void OnSetCenter(wxCommandEvent &event);          /**< Change center body */
  void OnImportSlf(wxCommandEvent &event);          /**< Import Solex file */
  void OnExportSlf(wxCommandEvent &event);          /**< Export Solex file */
  void OnSetAcceleration(wxCommandEvent &event);    /**< Change acceleration calc method */
  void OnSaveInitialState(wxCommandEvent &event);   /**< Save initial conditions */
  void OnLoadInitialState(wxCommandEvent &event);   /**< Load initial conditions */
  void OnReadToInitialState(wxCommandEvent &event); /**< Set current as initial */
  void OnBlending(wxCommandEvent &event);           /**< Toggle transparency */
  void OnLogEncounters(wxCommandEvent &event);      /**< Toggle encounter logging */
  void OnTimer(wxTimerEvent &event);                /**< Handle timer updates */
  void OnClose(wxCloseEvent &event);                /**< Handle window close */
  void OnIdle(wxIdleEvent &event);                  /**< Handle idle updates */

  DECLARE_EVENT_TABLE() /**< wxWidgets event table */
};

#endif // #ifndef Frame_H_
