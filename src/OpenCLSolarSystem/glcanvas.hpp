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

/**
 * @brief OpenGL canvas for 3D solar system visualization
 *
 * Handles rendering of particles and gravitational bodies using OpenGL,
 * supports stereoscopic 3D viewing and camera controls.
 */
class GLCanvas : public wxGLCanvas
{
private:
  // OpenGL State
  bool vboCreated;                   /**< Vertex Buffer Objects initialized flag */
  bool active;                       /**< Canvas is active and ready to render */
  bool stereo;                       /**< Stereoscopic rendering enabled */
  bool checkStereo;                  /**< Need to verify stereo support */
  bool glContextInitialized = false; /**< OpenGL context initialized flag */

  // Scene Parameters

  float aspectRatio; /**< Display aspect ratio (width/height) */

  // Rendering Settings
  GLboolean doubleBuffer; /**< Double buffering enabled */
  GLboolean smooth;       /**< Smooth shading enabled */
  GLboolean lighting;     /**< Lighting effects enabled */

  // Camera Position
  GLfloat xrot;  /**< X-axis rotation in degrees */
  GLfloat yrot;  /**< Y-axis rotation in degrees */
  GLfloat xDist; /**< X-axis translation in units */
  GLfloat yDist; /**< Y-axis translation in units */
  GLfloat zDist; /**< Z-axis translation (camera distance) */

  // OpenGL Resources
  GLuint vbo[2];          /**< Vertex Buffer Objects [positions, colors] */
  wxGLContext *glContext; /**< OpenGL rendering context */

  // View Management
  bool updateFrustum;    /**< Flag to recalculate view frustum */
  bool updateViewPort;   /**< Flag to update viewport dimensions */
  bool updateShadeModel; /**< Flag to update shading model */
  bool updateLighting;   /**< Flag to update lighting state */

  // Camera Parameters
  double fieldOfViewYAxis;      /**< Vertical field of view in degrees */
  double nearClippingPlane;     /**< Near clipping plane distance */
  double farClippingPlane;      /**< Far clipping plane distance */
  double screenProjectionPlane; /**< Distance to projection plane */
  double intraocularDistance;   /**< Eye separation for stereo */

  /**
   * @brief Camera frustum parameters
   * Defines view volume for left/right/center eye positions
   */
  struct Camera
  {
    GLdouble leftfrustum;     /**< Left frustum boundary */
    GLdouble rightfrustum;    /**< Right frustum boundary */
    GLdouble bottomfrustum;   /**< Bottom frustum boundary */
    GLdouble topfrustum;      /**< Top frustum boundary */
    GLfloat modeltranslation; /**< Eye position offset */
  } leftCam,                  /**< Left eye camera */
      rightCam,               /**< Right eye camera */
      centerCam;              /**< Center/mono camera */

public:
  int numGrav;      /**< Number of gravitational bodies */
  int numParticles; /**< Total number of particles to render */
  bool blending;    /**< Alpha blending enabled */

  /**
   * @brief Constructor for OpenGL canvas
   * @param parent Parent window
   * @param id Window identifier
   * @param pos Initial position
   * @param size Initial size
   * @param style Window style flags
   * @param name Window name
   * @param doubleBuffer Enable double buffering
   * @param smooth Enable smooth shading
   * @param lighting Enable lighting
   * @param stereo Enable stereoscopic rendering
   */
  GLCanvas(wxWindow *parent,
           wxWindowID id = wxID_ANY,
           const wxPoint &pos = wxDefaultPosition,
           const wxSize &size = wxDefaultSize,
           long style = 0,
           const wxString &name = _T("TestGLCanvas"),
           bool doubleBuffer = true,
           bool smooth = true,
           bool lighting = true,
           bool stereo = false);

  virtual ~GLCanvas();

  bool CreateOpenGlContext(int numParticles, int numGrav);
  bool CleanUpGL();
  void SetColours(GLubyte *colorData);
  void OnPaint(wxPaintEvent &event);
  void OnSize(wxSizeEvent &event);
  void OnEraseBackground(wxEraseEvent &event);
  void OnChar(wxKeyEvent &event);
  void OnMouseEvent(wxMouseEvent &event);
  void SetFrustum();
  void SetupProjectionAndModelView();
  GLuint *getVbo();
  bool VSync(bool vsync);

  DECLARE_EVENT_TABLE()
};

#endif // #if wxUSE_GLCANVAS

#endif // #ifndef GLCANVAS_H_
