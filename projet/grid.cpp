#include "grid.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;

Grid::Grid(unsigned int size,float minval,float maxval) {
  const float w = maxval-minval;
  const float h = w;

  const float stepW  = w/(float)size;
  const float stepH  = h/(float)size;
  const float startx = minval;
  const float starty = minval;

  float r;

  int i;

  float c[3] = {0.0,0.0,0.0};

  for(unsigned int i=0;i<size;++i) {
    for(unsigned int j=0;j<size;++j) {
     
      const float currentx = startx+stepW*(float)j;
      const float currenty = starty+stepH*(float)i;
    
      _vertices.push_back(currentx);
      _vertices.push_back(currenty);
      _vertices.push_back(0.0f);

      if(i>0 && j>0) {
    int i1 = i*size+j;
    int i2 = (i-1)*size+j;
    int i3 = (i-1)*size+j-1;
    int i4 = i*size+j-1;
   
    _faces.push_back(i1);
    _faces.push_back(i2);
    _faces.push_back(i3);
    _faces.push_back(i3);
    _faces.push_back(i4);
    _faces.push_back(i1);
      }
    }
  }


  _nbVertices = _vertices.size()/3;
  _nbFaces    = _faces.size()/3;

  // computing center
  for(i=0;i<_nbVertices*3;i+=3) {
    c[0] += _vertices[i  ];
    c[1] += _vertices[i+1];
    c[2] += _vertices[i+2];
  }
  center[0] = c[0]/(float)_nbVertices;
  center[1] = c[1]/(float)_nbVertices;
  center[2] = c[2]/(float)_nbVertices;

  // computing radius
  radius = 0.0;
  for(i=0;i<_nbVertices*3;i+=3) {
    c[0] = _vertices[i  ]-center[0];
    c[1] = _vertices[i+1]-center[1];
    c[2] = _vertices[i+2]-center[2];

    r = sqrt(c[0]*c[0]+c[1]*c[1]+c[2]*c[2]);
    radius = r>radius ? r : radius;
  }
}

Grid::~Grid() {
  _vertices.clear();
  _faces.clear();
}
