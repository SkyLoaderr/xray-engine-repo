/*
 *   Polygon Reduction Demo by Stan Melax (c) 1998
 *  Permission to use any of this code wherever you want is granted..
 *  Although, please do acknowledge authorship if appropriate.
 *
 *  This module initializes the bunny model data and calls
 *  the polygon reduction routine.  At each frame the RenderModel()
 *  routine is called to draw the model.  This module also
 *  animates the parameters (such as number of vertices to
 *  use) to show the model at various levels of detail.
 */

#include "stdafx.h"
#include "list.h"
#include "progmesh.h"

List<Vector> vert;       // global list of vertices
List<tridata> tri;       // global list of triangles

// Note that the use of the Map() function and the collapse_map
// list isn't part of the polygon reduction algorithm.
// We just set up this system here in this module
// so that we could retrieve the model at any desired vertex count.
// Therefore if this part of the program confuses you, then
// dont worry about it.  It might help to look over the progmesh.cpp
// module first.

//       Map()
//
// When the model is rendered using a maximum of mx vertices
// then it is vertices 0 through mx-1 that are used.  
// We are able to do this because the vertex list 
// gets sorted according to the collapse order.
// The Map() routine takes a vertex number 'a' and the
// maximum number of vertices 'mx' and returns the 
// appropriate vertex in the range 0 to mx-1.
// When 'a' is greater than 'mx' the Map() routine
// follows the chain of edge collapses until a vertex
// within the limit is reached.
//   An example to make this clear: assume there is
//   a triangle with vertices 1, 3 and 12.  But when
//   rendering the model we limit ourselves to 10 vertices.
//   In that case we find out how vertex 12 was removed
//   by the polygon reduction algorithm.  i.e. which
//   edge was collapsed.  Lets say that vertex 12 was collapsed
//   to vertex number 7.  This number would have been stored
//   in the collapse_map array (i.e. collapse_map[12]==7).
//   Since vertex 7 is in range (less than max of 10) we
//   will want to render the triangle 1,3,7.  
//   Pretend now that we want to limit ourselves to 5 vertices.
//   and vertex 7 was collapsed to vertex 3 
//   (i.e. collapse_map[7]==3).  Then triangle 1,3,12 would now be
//   triangle 1,3,3.  i.e. this polygon was removed by the
//   progressive mesh polygon reduction algorithm by the time
//   it had gotten down to 5 vertices.
//   No need to draw a one dimensional polygon. :-)

/*
int Map(int a,int mx) {
	if(mx<=0) return 0;
	while(a>=mx) {  
		a=collapse_map[a];
	}
	return a;
}

void DrawModelTriangles() {
	VERIFY(collapse_map.num);
	renderpolycount=0;
	int i=0;
	for(i=0;i<tri.num;i++) {
		int p0= Map(tri[i].v[0],render_num);
		int p1= Map(tri[i].v[1],render_num);
		int p2= Map(tri[i].v[2],render_num);
		// note:  serious optimization opportunity here,
		//  by sorting the triangles the following "continue" 
		//  could have been made into a "break" statement.
		if(p0==p1 || p1==p2 || p2==p0) continue;
		renderpolycount++;
		// if we are not currenly morphing between 2 levels of detail
		// (i.e. if morph=1.0) then q0,q1, and q2 are not necessary.
		int q0= Map(p0,(int)(render_num*lodbase));
		int q1= Map(p1,(int)(render_num*lodbase));
		int q2= Map(p2,(int)(render_num*lodbase));
		Vector v0,v1,v2; 
		v0 = vert[p0]*morph + vert[q0]*(1-morph);
		v1 = vert[p1]*morph + vert[q1]*(1-morph);
		v2 = vert[p2]*morph + vert[q2]*(1-morph);
		glBegin(GL_POLYGON);
			// the purpose of the demo is to show polygons
			// therefore just use 1 face normal (flat shading)
			Vector nrml = (v1-v0) * (v2-v1);  // cross product
			if(0<magnitude(nrml)) {
				glNormal3fv(normalize(nrml));
			}
			glVertex3fv(v0);
			glVertex3fv(v1);
			glVertex3fv(v2);
		glEnd();
	}
}
*/

