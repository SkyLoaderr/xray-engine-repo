/*
 *  Progressive Mesh type Polygon Reduction Algorithm
 *  by Stan Melax (c) 1998
 *
 *  The function ProgressiveMesh() takes a model in an "indexed face 
 *  set" sort of way.  i.e. list of vertices and list of triangles.
 *  The function then does the polygon reduction algorithm
 *  internally and reduces the model all the way down to 0
 *  vertices and then returns the order in which the
 *  vertices are collapsed and to which neighbor each vertex
 *  is collapsed to.  
 *  The method used here now is slightly different than done previously
 *  we are experimenting in the hopes of eventually incorporating
 *  Hoppe's siggraph 97 technique of view dependant simplification.
 */

#ifndef PROGRESSIVE_MESH_H
#define PROGRESSIVE_MESH_H

#include "..\OGF_Face.h"
#include "list.h"

class PMarg{
public:
	BOOL	useedgelength;
	BOOL	usecurvature;
	BOOL	protecttexture;
	BOOL	protectsmooth;
	BOOL	lockborder;
	BOOL	lockselected;
	DWORD	dwUVMASK;
	DWORD	dwUVCount;
};

void ComputeProgressiveMesh	(OGF* pOGF, List<int> &order, List<int> &PM, PMarg _pmarg);

/*
void DoProgressiveMesh		(OGF* pOGF, List<int> &order, List<int> &PM, float ratio,
							int gentexture,int maintainsmooth);
*/

#endif
