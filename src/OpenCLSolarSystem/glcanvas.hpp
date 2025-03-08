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
#ifndef GLCANVAS_H_
#define GLCANVAS_H_

#if wxUSE_GLCANVAS


#define DTR 0.0174532925

class GLCanvas: public wxGLCanvas
{
private:
	bool vboCreated;
	bool active;
	bool stereo;
	bool checkStereo;
	int numParticles;
	float aspectRatio;
	GLboolean doubleBuffer;
	GLboolean smooth;
	GLboolean lighting;
	GLfloat xrot;
	GLfloat yrot;
	GLfloat xDist;
	GLfloat yDist;
	GLfloat zDist;
	GLuint vbo[2];
	wxGLContext*	glContext;
	void SetupProjectionAndModelView();
	void SetFrustum( void );
	bool updateFrustum;
	bool updateViewPort;
	bool updateShadeModel;
	bool updateLighting;
	double fieldOfViewYAxis;
	double nearClippingPlane;
	double farClippingPlane;
	double screenProjectionPlane;
	double intraocularDistance;

	struct Camera
	{
		GLdouble leftfrustum;
		GLdouble rightfrustum;
		GLdouble bottomfrustum;
		GLdouble topfrustum;
		GLfloat modeltranslation;
	} leftCam, rightCam, centerCam;

public:
	int numGrav;
	bool blending;
	GLCanvas( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
	          const wxSize& size = wxDefaultSize, long style = 0,
	          const wxString& name = _T( "TestGLCanvas" ),
	          bool doubleBuffer = true, bool smooth = true, bool lighting = true, bool stereo = false
	        );
	~GLCanvas();

	bool CreateOpenGlContext( int numParticles, int numGrav );
	bool CleanUpGL();
	void SetColours( GLubyte *colorData );
	void OnPaint( wxPaintEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnEraseBackground( wxEraseEvent& event );
	void OnChar( wxKeyEvent& event );
	void OnMouseEvent( wxMouseEvent& event );
	GLuint* getVbo();
	bool VSync( bool vsync );

	DECLARE_EVENT_TABLE()
};

#endif // #if wxUSE_GLCANVAS

#endif // #ifndef GLCANVAS_H_
