// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:06 , by user : Oles , from computer : OLES
// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracer.h"
#include "../render.h"

const u32	MAX_TRACERS	= (1024*5);
const float TRACER_SIZE = 0.13f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracer::CTracer()
{
	sh_Tracer.create("effects\\bullet_tracer","effects\\bullet_tracer");
	sh_Geom.create	(FVF::F_V, RCache.Vertex.Buffer(), RCache.QuadIB);
}

CTracer::~CTracer()
{
}


IC void FillSprite      (FVF::V*& pv, const Fvector& pos, const float r1, float r2)
{
	const Fvector& T        = Device.vCameraTop;
	const Fvector& R        = Device.vCameraRight;
	Fvector Vr, Vt;
	Vr.x            = R.x*r1;
	Vr.y            = R.y*r1;
	Vr.z            = R.z*r1;
	Vt.x            = T.x*r2;
	Vt.y            = T.y*r2;
	Vt.z            = T.z*r2;

	Fvector         a,b,c,d;
	a.sub           (Vt,Vr);
	b.add           (Vt,Vr);
	c.invert        (a);
	d.invert        (b);
	pv->set         (d.x+pos.x,d.y+pos.y,d.z+pos.z, 0.f,1.f);        pv++;
	pv->set         (a.x+pos.x,a.y+pos.y,a.z+pos.z, 0.f,0.f);        pv++;
	pv->set         (c.x+pos.x,c.y+pos.y,c.z+pos.z, 1.f,1.f);        pv++;
	pv->set         (b.x+pos.x,b.y+pos.y,b.z+pos.z, 1.f,0.f);        pv++;
}

IC void FillSprite	(FVF::V*& pv, const Fvector& pos, const Fvector& dir, float r1, float r2)
{
    const Fvector& T        = dir;
    Fvector R;      R.crossproduct(T,Device.vCameraDirection).normalize_safe();
    Fvector Vr, Vt;
    Vr.x            = R.x*r1;
    Vr.y            = R.y*r1;
    Vr.z            = R.z*r1;
    Vt.x            = T.x*r2;
    Vt.y            = T.y*r2;
    Vt.z            = T.z*r2;

    Fvector         a,b,c,d;
    a.sub           (Vt,Vr);
    b.add           (Vt,Vr);
    c.invert        (a);
    d.invert        (b);
    pv->set         (d.x+pos.x,d.y+pos.y,d.z+pos.z, 0.f,1.f);        pv++;
    pv->set         (a.x+pos.x,a.y+pos.y,a.z+pos.z, 0.f,0.f);        pv++;
    pv->set         (c.x+pos.x,c.y+pos.y,c.z+pos.z, 1.f,1.f);        pv++;
    pv->set         (b.x+pos.x,b.y+pos.y,b.z+pos.z, 1.f,0.f);        pv++;
}

void  CTracer::Render	(FVF::V*&verts,  
						 const Fvector& center, const Fvector& dir,
						 float length, float width)
{
	if (::Render->ViewBase.testSphere_dirty((Fvector&)center,length*.5f))
	{
		FillSprite(verts,center,width,width);
		FillSprite(verts,center,dir,width,length);
	}
}