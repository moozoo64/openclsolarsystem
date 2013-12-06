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
#include "wx/wxprec.h"
#ifndef WX_PRECOMP

//#define GLEW_STATIC
#include <GL/glew.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef __linux__
#include <GL/glxew.h>
#endif 

#include "wx/wx.h"
#include "wx/timer.h"
#include "wx/glcanvas.h"
#include "wx/math.h"
#include "wx/log.h"
#include "wx/ffile.h"
#include "wx/string.h"
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/progdlg.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/filedlg.h>
#include <wx/config.h>

#ifdef _WIN32
#include <GL/wglew.h>
#endif


#endif

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <CL/cl_gl.h>


