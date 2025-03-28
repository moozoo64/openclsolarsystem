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
#include "glcanvas.hpp"

extern const int ID_GL_CONTEXT_READY;

void GLCanvas::SetFrustum(void)
{
  double top = this->nearClippingPlane * tan(DTR * this->fieldOfViewYAxis / 2);
  double right = this->aspectRatio * top;
  double frustumshift = (this->intraocularDistance / 2) * this->nearClippingPlane / this->screenProjectionPlane;

  this->centerCam.topfrustum = top;
  this->centerCam.bottomfrustum = -top;
  this->centerCam.leftfrustum = -right;
  this->centerCam.rightfrustum = right;
  this->centerCam.modeltranslation = 0.0f;

  this->leftCam.topfrustum = top;
  this->leftCam.bottomfrustum = -top;
  this->leftCam.leftfrustum = -right + frustumshift;
  this->leftCam.rightfrustum = right + frustumshift;
  this->leftCam.modeltranslation = this->intraocularDistance / 2;

  this->rightCam.topfrustum = top;
  this->rightCam.bottomfrustum = -top;
  this->rightCam.leftfrustum = -right - frustumshift;
  this->rightCam.rightfrustum = right - frustumshift;
  this->rightCam.modeltranslation = -this->intraocularDistance / 2;

  this->updateFrustum = false;
}

static int *makeGLAttrib(bool doubleBuffer, bool stereo)
{
  int index = 0;
  static int gl_attrib[32];

  gl_attrib[index++] = WX_GL_RGBA;
  gl_attrib[index++] = WX_GL_MIN_RED;
  gl_attrib[index++] = 1;
  gl_attrib[index++] = WX_GL_MIN_GREEN;
  gl_attrib[index++] = 1;
  gl_attrib[index++] = WX_GL_MIN_BLUE;
  gl_attrib[index++] = 1;
  gl_attrib[index++] = WX_GL_DEPTH_SIZE;
  gl_attrib[index++] = 0;

  // gl_attrib[index++] = WX_GL_SAMPLE_BUFFERS;
  // gl_attrib[index++] = GL_TRUE;

  if (doubleBuffer)
  {
    gl_attrib[index++] = WX_GL_DOUBLEBUFFER;
  }

  if (stereo)
  {
    gl_attrib[index++] = WX_GL_STEREO;
  }

  gl_attrib[index++] = GL_NONE;

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
                   const wxPoint &pos, const wxSize &size, long style,
                   const wxString &name,
                   bool doubleBuffer, bool smooth, bool lighting, bool stereo)
    : wxGLCanvas(parent, id, makeGLAttrib(doubleBuffer, stereo), pos, size, style | wxFULL_REPAINT_ON_RESIZE, name, wxNullPalette)

{
#ifdef __WXDEBUG__
  wxLogDebug(wxT("GLCanvas Create threadId: %ld"), wxThread::GetCurrentId());
#endif

  this->glContext = new wxGLContext(this);

  this->numParticles = 0;
  this->xrot = 0.0f;
  this->yrot = 0.0f;
  this->xDist = 0.0f;
  this->yDist = 0.0f;
  this->zDist = -2610.0f;
  this->vbo[0] = 0;
  this->vbo[1] = 0;
  this->vboCreated = false;
  this->updateViewPort = true;
  this->updateFrustum = true;
  this->updateShadeModel = true;
  this->updateLighting = true;
  this->active = false;
  this->checkStereo = true;
  this->doubleBuffer = doubleBuffer ? GL_TRUE : GL_FALSE;
  this->smooth = smooth ? GL_TRUE : GL_FALSE;
  this->lighting = lighting ? GL_TRUE : GL_FALSE;
  this->stereo = stereo;
  this->aspectRatio = 16.0f / 9.0f;
  this->fieldOfViewYAxis = 45;
  this->nearClippingPlane = 0.5;
  this->farClippingPlane = 600000.0;
  this->screenProjectionPlane = 2600.0;
  this->intraocularDistance = 400.0f;
}

GLCanvas::~GLCanvas()
{
  wxLogDebug(wxT("Cleaning up GLCanvas"));
  GLCanvas::CleanUpGL();
  delete glContext;
}

void GLCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("GLCanvas::OnPaint Start threadId: %ld"), wxThread::GetCurrentId());
#endif

  GLenum errCode;
  const GLubyte *errString;
  if (!IsShown() || !this->active)
  {
    return;
  }

  if (!this->vboCreated)
  {
    wxLogError(wxT("OnPaint before vboCreated"));
    return;
  }

  wxGLCanvas::SetCurrent(*this->glContext);

  // Check if got stereo
  if (this->stereo && this->checkStereo)
  {
    GLboolean supportsStereo = GL_FALSE;
    glGetBooleanv(GL_STEREO, &supportsStereo);
    this->stereo = (supportsStereo == GL_TRUE);
    wxLogDebug(wxT("Stereo %s"), this->stereo ? "enabled" : "disabled");
    this->checkStereo = false;
  }

  // check if projection and model view need to be set
  // This will happen on the first OnPaint after creation or a resize
  if (this->updateViewPort)
  {
    this->SetupProjectionAndModelView();
  }

  if (this->updateFrustum)
  {
    this->SetFrustum();
  }

  if (this->updateShadeModel)
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

  if (this->updateLighting)
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
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (int eye = 0; eye < (this->stereo ? 2 : 1); eye++)
  {
    if (this->stereo)
    {
      if (eye == 0)
      {
        wxLogDebug(wxT("Rendering Left Eye Camera"));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(this->leftCam.leftfrustum, this->leftCam.rightfrustum, this->leftCam.bottomfrustum, this->leftCam.topfrustum, this->nearClippingPlane, this->farClippingPlane);
        glTranslatef(this->leftCam.modeltranslation, 0.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDrawBuffer(GL_BACK_LEFT);
      }
      else
      {
        wxLogDebug(wxT("Rendering Right Eye Camera"));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(this->rightCam.leftfrustum, this->rightCam.rightfrustum, this->rightCam.bottomfrustum, this->rightCam.topfrustum, this->nearClippingPlane, this->farClippingPlane);
        glTranslatef(this->rightCam.modeltranslation, 0.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDrawBuffer(GL_BACK_RIGHT);
      }
    }
    else
    {
      wxLogDebug(wxT("Center Camera"));
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(this->centerCam.leftfrustum, this->centerCam.rightfrustum, this->centerCam.bottomfrustum, this->centerCam.topfrustum, this->nearClippingPlane, this->farClippingPlane);
      glTranslatef(this->centerCam.modeltranslation, 0.0, 0.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
    }

    glPushMatrix();
    glTranslatef(this->xDist, this->yDist, this->zDist);
    glRotatef(this->yrot, 0.0f, 1.0f, 0.0f);
    glRotatef(this->xrot, 1.0f, 0.0f, 0.0f);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glVertexPointer(3, GL_FLOAT, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Draw the Sun as a large point
    glPointSize(4);
    glDrawArrays(GL_POINTS, 0, 1);

    // Draw the first 16 "planets"
    glPointSize(3);
    glDrawArrays(GL_POINTS, 1, 16);

    // Draw the rest as single pixels
    if (!this->blending)
    {
      glDisable(GL_BLEND);
    }
    glPointSize(1);
    glDisable(GL_POINT_SMOOTH);
    glDrawArrays(GL_POINTS, 16, this->numParticles - 16);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glPopMatrix();
  }

  wxLogDebug(wxT("GLCanvas::OnPaint glFinish()"));
  glFinish();
  this->SwapBuffers();

  if ((errCode = glGetError()) != GL_NO_ERROR)
  {
    errString = gluErrorString(errCode);
    wxLogError(wxT("GLCanvas::OnPaint Fatal OpenGL Error: %d %s"), errCode, wxString(errString));
    this->Close(true);
  }

  wxLogDebug(wxT("GLCanvas::OnPaint Done"));
}

void GLCanvas::SetColours(GLubyte *colorData)
{
  GLenum errCode;
  const GLubyte *errString;

  if (!this->vboCreated)
  {
    wxLogError(wxT("SetColours before vboCreated"));
  }

  unsigned int colorSize = this->numParticles * 4 * sizeof(GLubyte);
  wxGLCanvas::SetCurrent(*this->glContext);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, colorSize, colorData, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glFinish();

  if ((errCode = glGetError()) != GL_NO_ERROR)
  {
    errString = gluErrorString(errCode);
    wxLogError(wxT("GLCanvas::SetColours Fatal OpenGL Error: %s"), wxString(errString));
    this->Close(true);
  }

  wxLogDebug(wxT("GLCanvas::SetColours glFinish()"));
}

// Called when the window is resized
void GLCanvas::OnSize(wxSizeEvent &event)
{
  // Call the base class OnSize
  event.Skip();

  // Only attempt initialization if not already done and window is shown
  if (!this->glContextInitialized && IsShown())
  {
    this->glContextInitialized = CreateOpenGlContext(this->numParticles, this->numGrav);

    if (this->glContextInitialized)
    {
      // Signal to parent that GL is ready
      wxCommandEvent readyEvent(wxEVT_COMMAND_MENU_SELECTED, ID_GL_CONTEXT_READY);
      GetParent()->GetEventHandler()->AddPendingEvent(readyEvent);
    }
  }

  if (this->glContextInitialized)
  {
    this->updateViewPort = true;
  }
}

// sets up the Projection and Model View
void GLCanvas::SetupProjectionAndModelView()
{
  wxLogDebug(wxT("GLCanvas::SetupProjectionAndModelView"));
  GLenum errCode;
  const GLubyte *errString;

  int w, h;
  this->GetClientSize(&w, &h);
  this->SetCurrent(*this->glContext);
  glViewport(0, 0, (GLint)w, (GLint)h);

  float width, height;

  if (w > h)
  {
    width = (float)w / (float)h;
    height = 1.0;
  }
  else
  {
    height = (float)h / (float)w;
    width = 1.0;
  }
  this->aspectRatio = width / height;
  this->SetFrustum();
  this->updateViewPort = false;

  if ((errCode = glGetError()) != GL_NO_ERROR)
  {
    errString = gluErrorString(errCode);
    wxLogError(wxT("GLCanvas::SetupProjectionAndModelView Fatal OpenGL Error: %s"), wxString(errString));
    this->Close(true);
  }
}

// Handle Key press events
void GLCanvas::OnChar(wxKeyEvent &event)
{
  if (!IsShown())
  {
    return;
  }

  if (!this->vboCreated)
  {
    wxLogError(wxT("OnChar before vboCreated"));
  }

  switch (event.GetKeyCode())
  {
  case WXK_ESCAPE:
    wxTheApp->ExitMainLoop();
    return;

  case WXK_LEFT:
    this->xDist -= 0.01 * 1000.0f;
    // this->yrot -= 15.0;
    break;

  case WXK_RIGHT:
    this->xDist += 0.01 * 1000.0f;
    // this->yrot += 15.0;
    break;

  case WXK_UP:
    this->yDist += 0.01 * 1000.0f;
    // this->xrot += 15.0;
    break;

  case WXK_DOWN:
    this->yDist -= 0.01 * 1000.0f;
    // this->xrot -= 15.0;
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
    this->zDist += 10.0f;
    wxLogDebug(wxT("GLCanvas::OnChar zDist %f"), this->zDist);
    break;

  case 'Z':
    this->zDist += -10.0f;
    wxLogDebug(wxT("GLCanvas::OnChar zDist %f"), this->zDist);
    break;

  case 'c':
  case 'C':
    this->yDist = 0.0f;
    this->xDist = 0.0f;
    break;

  case 'r':
  case 'R':
    this->yDist = 0.0f;
    this->xDist = 0.0f;
    this->xrot = 0.0f;
    this->yrot = 0.0f;
    this->zDist = -2600.0f;
    this->fieldOfViewYAxis = 45;
    this->nearClippingPlane = 1000.0;
    this->farClippingPlane = 600000.0;
    this->screenProjectionPlane = 2600.0;
    this->intraocularDistance = 400.0f;
    break;

  case 't':
    this->zDist = this->zDist * 0.1;
    // this->screenProjectionPlane = - this->zDist;
    this->nearClippingPlane = -this->zDist / 2.0;
    wxLogDebug(wxT("GLCanvas::OnChar zDist %f, nearClippingPlane %f, screenProjectionPlane %f"), this->zDist, this->nearClippingPlane, this->screenProjectionPlane);
    break;

  case 'T':
    this->zDist = this->zDist * 10.0;
    // this->screenProjectionPlane = -this->zDist;
    this->nearClippingPlane = -this->zDist / 2.0;
    wxLogDebug(wxT("GLCanvas::OnChar zDist %f, nearClippingPlane %f, screenProjectionPlane %f"), this->zDist, this->nearClippingPlane, this->screenProjectionPlane);
    break;

  case 'i':
    this->intraocularDistance += 100.0f;
    this->updateFrustum = true;
    break;

  case 'I':
    this->intraocularDistance += -100.0f;
    this->intraocularDistance = (this->intraocularDistance < 0) ? 0 : this->intraocularDistance;
    this->updateFrustum = true;
    break;

  case 'n':
    this->nearClippingPlane = this->nearClippingPlane * 0.1f;
    this->updateFrustum = true;
    wxLogDebug(wxT("GLCanvas::OnChar nearClippingPlane %f"), this->nearClippingPlane);
    break;

  case 'N':
    this->nearClippingPlane = this->nearClippingPlane * 10.0f;
    this->nearClippingPlane = (this->nearClippingPlane < 0) ? 0 : this->nearClippingPlane;
    this->updateFrustum = true;
    wxLogDebug(wxT("GLCanvas::OnChar nearClippingPlane %f"), this->nearClippingPlane);
    break;

  default:
    event.Skip();
    return;
  }

  this->Refresh(false);
}

// Handle Mouse events
void GLCanvas::OnMouseEvent(wxMouseEvent &event)
{
  static int dragging = 0;
  static float last_x, last_y;

  if (!this->vboCreated)
  {
    wxLogError(wxT("OnMouseEvent before vboCreated"));
  }

  // printf("%f %f %d\n", event.GetX(), event.GetY(), (int)event.LeftIsDown());
  if (event.LeftIsDown())
  {
    if (!dragging)
    {
      dragging = 1;
    }
    else
    {
      this->yrot += (event.GetX() - last_x) * 1.0;
      this->xrot += (event.GetY() - last_y) * 1.0;
      this->Refresh(false);
    }
    last_x = event.GetX();
    last_y = event.GetY();
  }
  else
    dragging = 0;

  if (event.GetWheelRotation() > 0)
  {
    // this->zDist += 0.1*1000.0f;
    this->zDist *= 0.6666666666667;
    this->nearClippingPlane *= 0.6666666666667;
    this->farClippingPlane *= 0.6666666666667;
    this->updateFrustum = true;
    this->Refresh(false);
  }
  else if (event.GetWheelRotation() < 0)
  {
    // this->zDist += -0.1*1000.0f;
    this->zDist *= 1.5;
    this->nearClippingPlane *= 1.5;
    this->farClippingPlane *= 1.5;
    this->updateFrustum = true;
    this->Refresh(false);
  }
}

void GLCanvas::OnEraseBackground(wxEraseEvent &WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

// Gets the vbos. If they don't exist yet it creates them.
// This should be called for the first time after the CL context has been made
GLuint *GLCanvas::getVbo()
{
  GLenum errCode;
  const GLubyte *errString;

  if (!this->vboCreated)
  {
    wxLogDebug(wxT("GLCanvas::getVbo creating vbo's"));
    wxGLCanvas::SetCurrent(*this->glContext);
    unsigned int size = this->numParticles * 4 * sizeof(GLfloat);
    unsigned int colorSize = this->numParticles * 4 * sizeof(GLubyte);

    // Create the vbos
    glGenBuffers(2, &(this->vbo[0]));
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colorSize, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glFinish();
    wxLogDebug(wxT("GLCanvas::getVbo glFinish()"));

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
      errString = gluErrorString(errCode);
      wxLogError(wxT("GLCanvas::getVbo Fatal OpenGL Error: %s"), wxString(errString));
      this->Close(true);
    }

    this->vboCreated = true;
  }

  wxLogDebug(wxT("GLCanvas::getVbo returning vbo's"));
  return this->vbo;
}

bool GLCanvas::VSync(bool vsync)
{
  int interval = vsync ? 1 : 0;
  bool success = false;

  if (!this->active)
  {
    wxLogError(wxT("GLCanvas::VSync before opengl is setup"));
    return success;
  }

#ifdef _WIN32
  if (wglewIsSupported("WGL_EXT_swap_control"))
  {
    if (!wglSwapIntervalEXT(interval))
    {
      wxLogDebug(wxT("Failed to set vsync"));
    }
    else
    {
      success = true;
    }
  }
  else
  {
    wxLogDebug(wxT("WGL_EXT_swap_control not supported"));
  }
#endif

#ifdef __linux__
  if (glxewIsSupported("GLX_SGI_swap_control"))
  {
    if (glXSwapIntervalSGI(interval) != 0)
    {
      wxLogDebug(wxT("Failed to set vsync"));
    }
    else
    {
      success = true;
    }
  }
  else
  {
    wxLogDebug(wxT("GLX_SGI_swap_control not supported"));
  }
#endif

#ifdef __APPLE__
  if (CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &interval) != 0)
  {
    wxLogDebug(wxT("Failed to set vsync"));
  }
  else
  {
    success = true;
  }
#endif

#ifdef __WXDEBUG__
  if (success)
  {
    wxLogDebug(wxT("Set vsync %s"), vsync ? "true" : "false");
  }
#endif

  return success;
}

// Creates the OpenGL Context
bool GLCanvas::CreateOpenGlContext(int numParticlesIn, int numGravIn)
{

#ifdef __WXDEBUG__
  wxLogDebug(wxT("GLCanvas::CreateOpenGlContext Start threadId: %ld"), wxThread::GetCurrentId());
#endif

  GLenum errCode;
  const GLubyte *errString;

  this->numParticles = numParticlesIn;
  this->numGrav = numGravIn;

  if (!IsShown())
  {
    return false;
  }

  wxGLCanvas::SetCurrent(*this->glContext);
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    wxLogError(wxT("Problem with glew Error:%s\n"), glewGetErrorString(err));
    return false;
  }

  if (!glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object"))
  {
    wxLogError(wxT("Problem with GL version\n"));
    return false;
  }

  this->active = true;
  this->VSync(false);
  this->vboCreated = false;
  this->updateViewPort = true;

  if ((errCode = glGetError()) != GL_NO_ERROR)
  {
    errString = gluErrorString(errCode);
    wxLogError(wxT("GLCanvas::CreateOpenGlContext Fatal OpenGL Error: %s"), wxString(errString));
    this->Close(true);
  }

  wxLogDebug(wxT("GLCanvas::CreateOpenGlContext Done"));
  return true;
}

bool GLCanvas::CleanUpGL()
{
  wxLogDebug(wxT("GLCanvas::CleanUpGL"));
  GLenum errCode;
  const GLubyte *errString;

  this->active = false;
  glDeleteBuffers(2, &(this->vbo[0]));
  glFinish();
  wxLogDebug(wxT("GLCanvas::CleanUpGL glFinish()"));
  this->vboCreated = false;
  wxLogDebug(wxT("GLCanvas::CleanUpGL Done"));
  if ((errCode = glGetError()) != GL_NO_ERROR)
  {
    errString = gluErrorString(errCode);
    wxLogError(wxT("GLCanvas::CleanUpGL Fatal OpenGL Error: %s"), wxString(errString));
    this->Close(true);
    return false;
  }
  return true;
}
