#pragma once

#include "vert.h"

struct st_FACE;
void RegisterFACE(st_FACE* f);

struct st_FACE {
	st_VERT*	v[3];
	DWORD		m;

	st_FACE(){
		RegisterFACE(this);
	}
	void	VSet(int id, st_VERT* V){
		v[id]=V;
	}
	void	SetVerts(st_VERT* v1, st_VERT* v2, st_VERT* v3)
	{
		VSet(0,v1);
		VSet(1,v2);
		VSet(2,v3);
	}
	void	ReplaceVert(st_VERT* from, st_VERT* to)
	{
		if (v[0]==from) VSet(0,to);
		if (v[1]==from) VSet(1,to);
		if (v[2]==from) VSet(2,to);
	}
};

DEF_VECTOR(vFACE,st_FACE*);

