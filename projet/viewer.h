#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h> 

// OpenGL library 
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "meshLoader.h"
#include "shader.h"
#include "grid.h"

class Viewer : public QGLWidget {
 public:
  Viewer(char *filename,
	 const QGLFormat &format=QGLFormat::defaultFormat());
  Viewer(const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();
  
 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  void createVAO();
  void deleteVAO();
  void drawObject(const glm::vec3 &pos,const glm::vec3 &col);
  void drawQuad();

  void createShaders();
  void deleteShaders();
  void disableShader();

  void createFBO();
  void deleteFBO();
  void initFBO();

  QTimer        *_timer;    // timer that controls the animation
  unsigned int   _currentshader; // current shader index

  Mesh   *_mesh;   // the mesh
  Camera *_cam;    // the camera
  Grid *_grid;

  glm::vec3 _light; // light direction
  bool      _mode;  // camera motion or light motion

  Shader *_shaderNoise; // shader used to draw Noise
  Shader *_shaderNormal; // shader used to draw Normal
  Shader *_shaderMountain; // shader used to create the Mountain geometry
  Shader *_shaderShadow; //shader used to create the shadowMap of the mountain
  Shader *_shaderPost; //shader dedicated to the Post process

  // vao/vbo ids (1 for the object, 1 for the viewport quad)
  GLuint _vaoObject;
  GLuint _vaoQuad;
  GLuint _buffers[5];
  GLuint _quad;

  // render texture ids 
  GLuint _rendNormalId;
  GLuint _rendShadowsId;
  GLuint _rendDepthId;

  // fbo id
  GLuint _fboNoise;
  GLuint _fboShadow;
  GLuint _fboOther;
};

#endif // VIEWER_H
