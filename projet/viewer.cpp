#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _mode(false) {

  setlocale(LC_ALL,"C");

  _grid = new Grid();
  _cam = new Camera(_grid->radius, glm::vec3(_grid->center[0],_grid->center[1],_grid->center[2]));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _mesh;
  delete _cam;
  delete _grid;

  for(unsigned int i=0;i<_shaders.size();++i) {
    delete _shaders[i];
  }

  deleteVAO();
  deleteTextures();
}

void Viewer::createTextures() {
  QImage image = QGLWidget::convertToGLFormat(QImage("/home/b/baillema/Desktop/Cours/SIM/TP5/TP05/textures/ice_cold.png"));

  // enable the use of 2D textures 
  glEnable(GL_TEXTURE_2D);

  // (see exercice 2)
  glGenTextures(1,&(_texIds[0]));
  glBindTexture(GL_TEXTURE_2D,_texIds[0]);

  //glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image.width(),image.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,image.bits());
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image.width(),image.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,image.bits());

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // choose the filtering mode for minification
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // choose the filtering mode for magnification
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT); // choose the border mode in the U direction
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT); // choose the border mode in the V direction

  glGenerateMipmap(GL_TEXTURE_2D);
}

void Viewer::deleteTextures() {
  // (see exercice 2)
  glDeleteTextures(1,&(_texIds[0]));
}

void Viewer::createVAO() {
  // create some buffers inside the GPU memory
  glGenVertexArrays(1,&_vao);
  glGenBuffers(4,_buffers);

  // activate VAO
  glBindVertexArray(_vao);

  // store grid positions into buffer 0 inside the GPU memory
  glBindBuffer(GL_ARRAY_BUFFER,_buffers[0]);
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);

  // store mesh normals into buffer 1 inside the GPU memory
  /*
  glBindBuffer(GL_ARRAY_BUFFER,_buffers[1]);
  glBufferData(GL_ARRAY_BUFFER,_mesh->nb_vertices*3*sizeof(float),_mesh->normals,GL_STATIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_TRUE,0,(void *)0);
  glEnableVertexAttribArray(1);

  // store coordinates
  glBindBuffer(GL_ARRAY_BUFFER,_buffers[2]);
  glBufferData(GL_ARRAY_BUFFER,_mesh->nb_vertices*2*sizeof(float),_mesh->coords,GL_STATIC_DRAW);
  glVertexAttribPointer(2,2,GL_FLOAT,GL_TRUE,0,(void *)0);
  glEnableVertexAttribArray(2);
  */
  // store grid indices into buffer 3 inside the GPU memory
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_buffers[3]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(unsigned int),_grid->faces(),GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void Viewer::deleteVAO() {
  // delete your VAO here (function called in destructor)
  glDeleteBuffers(4,_buffers);
  glDeleteVertexArrays(1,&_vao);
}

void Viewer::drawVAO() {
  // activate the VAO, draw the associated triangles and desactivate the VAO
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
  glBindVertexArray(0);
}

void Viewer::createShaders() {
  // add your own shader files here 

  // *** Phong shader *** 
  _vertexFilenames.push_back("shaders/noise.vert");
  _fragmentFilenames.push_back("shaders/noise.frag");

}

void Viewer::enableShader(unsigned int shader) {
  // current shader ID 
  GLuint id = _shaders[shader]->id(); 

  // activate the current shader 
  glUseProgram(id);

  // send the model-view matrix 
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));

  // send the projection matrix 
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));

  // send the normal matrix (inverse( transpose( top-left 3x3(MDV))) 
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));

  // send a light direction (defined in camera space)
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));

  // send the link to your texture here
  // (see exercice 3)
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texIds[0]);
  glGenerateMipmap(GL_TEXTURE_2D);
  glUniform1i(glGetUniformLocation(id,"texturing"),0);

}

void Viewer::disableShader() {
  // desactivate all shaders 
  glUseProgram(0);
}

void Viewer::paintGL() {
  // clear the color and depth buffers 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // tell the GPU to use this specified shader and send custom variables (matrices and others)
  enableShader(_currentshader);

  // actually draw the scene 
  drawVAO();

  // tell the GPU to stop using this shader 
  disableShader(); 
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  
  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    for(unsigned int i=0;i<_vertexFilenames.size();++i) {
      _shaders[i]->reload(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
    }
  }

  // space: next shader
  if(ke->key()==Qt::Key_Space) {
    _currentshader = (_currentshader+1)%_shaders.size();
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  glewExperimental = GL_TRUE;

  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init and load all shader files
  for(unsigned int i=0;i<_vertexFilenames.size();++i) {
    _shaders.push_back(new Shader());
    _shaders[i]->load(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
  }

  // init all 
  createVAO();
  createTextures();

  // starts the timer 
  _timer->start();
}

