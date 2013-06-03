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
#include "glcanvas.hpp"

static int *makeGLAttrib(bool doubleBuffer)
{

#ifdef __WXMSW__
	int *gl_attrib = NULL;
#else
	static int gl_attrib[20] = { WX_GL_RGBA, WX_GL_MIN_RED, 1, WX_GL_MIN_GREEN, 1,
	                             WX_GL_MIN_BLUE, 1, WX_GL_DEPTH_SIZE, 1,
	                             WX_GL_DOUBLEBUFFER,
#  if defined(__WXMAC__) || defined(__WXCOCOA__)
	                             GL_NONE
	                           };
#  else
	None
};
#  endif
#endif //__WXMSW__

#ifdef __WXGTK__
	if(!doubleBuffer)
	{
		gl_attrib[9] = None;
	}
#endif

	return gl_attrib;
}

/*
 * GLCanvas implementation
 */

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
	EVT_SIZE(GLCanvas::OnSize)
	EVT_PAINT(GLCanvas::OnPaint)
	EVT_CHAR(GLCanvas::OnChar)
	EVT_MOUSE_EVENTS(GLCanvas::OnMouseEvent)
	EVT_ERASE_BACKGROUND(GLCanvas::OnEraseBackground)
END_EVENT_TABLE()

GLCanvas::GLCanvas(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos, const wxSize& size, long style,
                   const wxString& name,
                   bool doubleBuffer, bool smooth, bool lighting)
	: wxGLCanvas(parent, id,makeGLAttrib(doubleBuffer), pos, size, style|wxFULL_REPAINT_ON_RESIZE, name, wxNullPalette)

{
#ifdef __WXDEBUG__
	wxLogDebug(wxT("GLCanvas Create threadId: %ld"),wxThread::GetCurrentId());
#endif

	this->glContext = new wxGLContext(this);
	
	this->numParticles = 0;
	this->xrot = 0.0f;
	this->yrot = 0.0f;
	this->xDist = 0.0f;
	this->yDist = 0.0f;
	this->zDist = 0.0f;
	this->vbo[0] = 0;
	this->vbo[1] = 0;
	this->vboCreated = false;
	this->updateProjectionAndModelView = true;
	this->updateShadeModel = true;
	this->updateLighting = true;
	this->active = false;
	this->doubleBuffer = doubleBuffer?GL_TRUE:GL_FALSE;
	this->smooth = smooth?GL_TRUE:GL_FALSE;
	this->lighting = lighting?GL_TRUE:GL_FALSE;
}

GLCanvas::~GLCanvas()
{
	wxLogDebug(wxT("Cleaning up GLCanvas"));
	GLCanvas::CleanUpGL();
	delete glContext;
}

void GLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	
#ifdef __WXDEBUG__
	wxLogDebug(wxT("GLCanvas::OnPaint Start threadId: %ld"),wxThread::GetCurrentId());
#endif
	
	if(!IsShown() || !this->active)
	{
		return;
	}
	
    if(!this->vboCreated)
	{
		wxLogError(wxT("OnPaint before vboCreated"));
	}
	
    wxGLCanvas::SetCurrent(*this->glContext);
	
	// check if projection and model view need to be set
	// This will happen on the first OnPaint after creation or a resize
	if(this->updateProjectionAndModelView)
	{
		this->SetupProjectionAndModelView();
	}
	
	if(this->updateShadeModel)
	{
		if (this->smooth)
		{
			glShadeModel(GL_SMOOTH);
		}
		else
		{
			glShadeModel(GL_FLAT);
		}
		this->updateShadeModel = false;
	}
	
	if(this->updateLighting)
	{
		if (this->lighting)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
		this->updateLighting = false;
	}

    wxPaintDC(this);
		
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glPushMatrix();
	glTranslatef( this->xDist, this->yDist, this->zDist);
	glRotatef( this->yrot, 0.0f, 1.0f, 0.0f );
	glRotatef( this->xrot, 1.0f, 0.0f, 0.0f );

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
	glVertexPointer(3, GL_FLOAT, 4*sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
	
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Draw the Sun as a large point
	glPointSize(3);
	glDrawArrays(GL_POINTS, 0, 1);
	
	// Draw the first 16 "planets"
	glPointSize(2);
	glDrawArrays(GL_POINTS, 1, 16);
	
	// Draw the rest as single pixels
	if(!this->blending){
		glDisable(GL_BLEND);
	}
	glPointSize(1);
	glDisable(GL_POINT_SMOOTH);
	glDrawArrays(GL_POINTS, 16, this->numParticles-16);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();
	glFinish();
	wxLogDebug(wxT("GLCanvas::OnPaint glFinish()"));
	this->SwapBuffers();
	
	wxLogDebug(wxT("GLCanvas::OnPaint Done"));
}

void GLCanvas::SetColours(GLubyte *colorData)
{
	if(!this->vboCreated)
	{
		wxLogError(wxT("SetColours before vboCreated"));
	}
	
	unsigned int colorSize = this->numParticles * 4 * sizeof(GLubyte);
	wxGLCanvas::SetCurrent(*this->glContext);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, colorSize, colorData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glFinish();
	wxLogDebug(wxT("GLCanvas::SetColours glFinish()"));
}

// Called when the window is resized
void GLCanvas::OnSize(wxSizeEvent& event)
{
	if(!IsShown())
	{
		return;
	}
	
	if(!this->vboCreated)
	{
		wxLogError(wxT("OnSize before vboCreated"));
	}
	
	this->updateProjectionAndModelView = true;
}

// sets up the Projection and Model View
void GLCanvas::SetupProjectionAndModelView()
{
	wxLogDebug(wxT("GLCanvas::SetupProjectionAndModelView"));
	int w, h;
	this->GetClientSize(&w, &h);
	this->SetCurrent(*this->glContext);
	glViewport(0, 0, (GLint) w, (GLint) h);
	
	float width,height;
	
	if (w > h)
    {
		width = (float)w/(float)h;
		height = 1.0;
    }
    else
    {
        height = (float)h/(float)w;
        width = 1.0;
    }
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum( -width/10.0, width/10.0, -height/10.0, height/10.0, 5.0, 600000.0 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, -30000.0f );
	
	this->updateProjectionAndModelView = false;
}

// Handle Key press events
void GLCanvas::OnChar(wxKeyEvent& event)
{
	if(!IsShown())
	{
		return;
	}
	
	if(!this->vboCreated)
	{
		wxLogError(wxT("OnChar before vboCreated"));
	}
	
	switch( event.GetKeyCode() )
	{
		case WXK_ESCAPE:
			wxTheApp->ExitMainLoop();
			return;

		case WXK_LEFT:
			this->xDist -= 0.01*1000.0f;
			//this->yrot -= 15.0;
			break;

		case WXK_RIGHT:
			this->xDist += 0.01*1000.0f;
			//this->yrot += 15.0;
			break;

		case WXK_UP:
			this->yDist += 0.01*1000.0f;
			//this->xrot += 15.0;
			break;

		case WXK_DOWN:
			this->yDist -= 0.01*1000.0f;
			//this->xrot -= 15.0;
			break;

		case 's':
		case 'S':
			this->smooth = !this->smooth;
			this->updateShadeModel = true;
			break;

		case 'l':
		case 'L':
			this->lighting = !this->lighting;
			this->updateLighting = true;
			break;

		case 'z':
			this->zDist += 1000.0f;
			break;
			
		case 'Z':
			this->zDist += -1000.0f;
			break;

		default:
			event.Skip();
			return;
	}

	this->Refresh(false);
}

// Handle Mouse events
void GLCanvas::OnMouseEvent(wxMouseEvent& event)
{
	static int dragging = 0;
	static float last_x, last_y;

	if(!this->vboCreated)
	{
		wxLogError(wxT("OnMouseEvent before vboCreated"));
	}
	
	//printf("%f %f %d\n", event.GetX(), event.GetY(), (int)event.LeftIsDown());
	if(event.LeftIsDown())
	{
		if(!dragging)
		{
			dragging = 1;
		}
		else
		{
			this->yrot += (event.GetX() - last_x)*1.0;
			this->xrot += (event.GetY() - last_y)*1.0;
			this->Refresh(false);
		}
		last_x = event.GetX();
		last_y = event.GetY();
	}
	else
		dragging = 0;
	
	if(event.GetWheelRotation() > 0)
	{
		this->zDist += 0.1*1000.0f;
		this->Refresh(false);
	}
	else if(event.GetWheelRotation()<0)
	{
		this->zDist += -0.1*1000.0f;
		this->Refresh(false);
	}
}

void GLCanvas::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{
	// Do nothing, to avoid flashing.
}

// Gets the vbos. If they don't exist yet it creates them.
// This should be called for the first time after the CL context has been made
GLuint* GLCanvas::getVbo()
{
	if(!this->vboCreated)
	{
		wxLogDebug(wxT("GLCanvas::getVbo creating vbo's"));
		wxGLCanvas::SetCurrent(*this->glContext);
		
		// use float4 for 128 bit alignment
		unsigned int size = this->numParticles * 4 * sizeof(GLfloat);
		GLfloat *data = (GLfloat *)malloc(size);
		memset(data,0x1,size);

		unsigned int colorSize = this->numParticles * 4 * sizeof(GLubyte);
		GLubyte *colorData = (GLubyte *) malloc(colorSize);

		wxLogDebug(wxT("Initialising %d particles"),this->numParticles);

		for (int i = 0; i<this->numParticles; i++)
		{
			colorData[i*4+0] = 64;
			colorData[i*4+1] = 64;
			colorData[i*4+2] = 92;
			colorData[i*4+3] = 255;
		}
		
		// Create the vbos and initialise them
		glGenBuffers(2, &(this->vbo[0]));
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, colorSize, colorData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glFinish();
		wxLogDebug(wxT("GLCanvas::getVbo glFinish()"));

		free(data);
		free(colorData);
		this->vboCreated = true;
	}
	
	wxLogDebug(wxT("GLCanvas::getVbo returning vbo's"));
	return this->vbo;
}

bool GLCanvas::VSync(bool vsync)
{
	int interval = vsync ? 1 : 0;
	bool success = false;
	
	if(!this->active)
	{
		wxLogError(wxT("GLCanvas::VSync before opengl is setup"));
		return success;
	}
	
#ifdef _WIN32	
	if(wglewIsSupported("WGL_EXT_swap_control"))
	{
		if(!wglSwapIntervalEXT(interval))
		{
			wxLogDebug(wxT("Failed to set vsync"));
		}
		else
		{
			success =true;
		}
	}
	else
	{
		wxLogDebug(wxT("WGL_EXT_swap_control not supported"));
	}
#endif

#ifdef __linux__
	if(glxewIsSupported ("GLX_SGI_swap_control"))
	{
		if(glXSwapIntervalSGI(interval) != 0)
		{
			wxLogDebug(wxT("Failed to set vsync"));
		}
		else
		{
			success =true;
		}
	}
	else
	{
		wxLogDebug(wxT("GLX_SGI_swap_control not supported"));
	}
#endif

#ifdef __APPLE__
	if(CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &interval) !=0)
	{
			wxLogDebug(wxT("Failed to set vsync"));
	}
	else
	{
		success =true;
	}
#endif

#ifdef __WXDEBUG__
	if(success)
	{
		wxLogDebug(wxT("Set vsync %s"), vsync ? "true" : "false");
	}
#endif

	return success;
}

// Creates the OpenGL Context
bool GLCanvas::CreateOpenGlContext(int numParticles, int numGrav)
{

#ifdef __WXDEBUG__
	wxLogDebug(wxT("GLCanvas::CreateOpenGlContext Start threadId: %ld"),wxThread::GetCurrentId());
#endif

	this->numParticles = numParticles;
	this->numGrav = numGrav;

	if(!IsShown())
	{
		return false;
	} 
    
    wxGLCanvas::SetCurrent(*this->glContext);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		wxLogError(wxT("Problem with glew Error:%s\n"),glewGetErrorString(err));
		return false;
	}
	
	if (!glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object"))
	{
		wxLogError(wxT("Problem with GL version\n"));
		return false;
	}

	this->active=true;
	this->VSync(false);
	this->vboCreated = false;
	this->updateProjectionAndModelView = true;
	//glFinish();
	wxLogDebug(wxT("GLCanvas::CreateOpenGlContext Done"));
	return true;
}

bool GLCanvas::CleanUpGL()
{
	wxLogDebug(wxT("GLCanvas::CleanUpGL"));
	this->active = false;
	glDeleteBuffers(2, &(this->vbo[0]));
	glFinish();
	wxLogDebug(wxT("GLCanvas::CleanUpGL glFinish()"));
	this->vboCreated = false;
	wxLogDebug(wxT("GLCanvas::CleanUpGL Done"));
	return true;
}

