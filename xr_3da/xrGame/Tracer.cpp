// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:06 , by user : Oles , from computer : OLES
// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracer.h"

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

void	CTracer::Add	(const Fvector& from, const Fvector& to, float bullet_speed, float trail_speed_factor, float start_length, float width)
{
	if (from.distance_to(to)<.1f)	return;

	bullets.push_back	(Bullet());
	Bullet&	B			= bullets.back();

	Fvector	path;
	path.sub			(to,from);

	B.target.set		(to);
	B.dir.normalize		(path);
	B.pos_head.mad		(from,B.dir,start_length);
	B.pos_trail.set		(from);
	B.speed_head =		bullet_speed;
	B.speed_trail =		bullet_speed*trail_speed_factor;
	B.width =			width;
	B.life_time =		(path.magnitude()-start_length)/bullet_speed;
	if (B.life_time<0)	bullets.pop_back();
}

void	CTracer::Render	()
{
	if (bullets.empty())	return;
	
	u32	vOffset;
	FVF::V	*verts		=	(FVF::V	*) RCache.Vertex.Lock((u32)bullets.size()*4,sh_Geom->vb_stride,vOffset);
	FVF::V	*start		=	verts;
	float	dt			=	Device.fTimeDelta;

//	Fvector&	vTop	=	Device.vCameraTop;
//	Fvector&	vDir	=	Device.vCameraDirection;
	Fvector&	vCenter =	Device.vCameraPosition;
	for (int I=0; I<(int)bullets.size(); I++)
	{
		// General stuff and time control
		Bullet&	B		=	bullets[I];
		B.life_time		-=	dt;
		if (B.life_time<=0)	{
			bullets.erase(bullets.begin()+I);
			I--;
			continue;
		}

		// Physics :)))
		B.pos_head.mad	(B.dir,dt*B.speed_head);
		B.pos_trail.mad	(B.dir,dt*B.speed_trail);

		// Culling
		Fvector sC,lineD;	float sR; 
		sC.sub	(B.pos_head,B.pos_trail);
		lineD.normalize(sC);
		sC.mul	(.5f);
		sR		= sC.magnitude();
		sC.add	(B.pos_trail);
		if (!::Render->ViewBase.testSphere_dirty(sC,sR))	continue;

		// Everything OK - build vertices
		Fvector	P,lineTop,camDir;
		camDir.sub			(sC,vCenter);
		camDir.normalize	();
		lineTop.crossproduct(camDir,lineD);
		float	w = B.width;
		P.mad(B.pos_trail,lineTop,-w);	verts->set(P,0,1);	verts++;
		P.mad(B.pos_trail,lineTop,w);	verts->set(P,0,0);	verts++;
		P.mad(B.pos_head, lineTop,-w);	verts->set(P,1,1);	verts++;
		P.mad(B.pos_head, lineTop,w);	verts->set(P,1,0);	verts++;
	}

	u32 vCount					= (u32)(verts-start);
	RCache.Vertex.Unlock		(vCount,sh_Geom->vb_stride);
	
	if (vCount)	{
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
		RCache.set_xform_world		(Fidentity);
		RCache.set_Shader			(sh_Tracer);
		RCache.set_Geometry			(sh_Geom);
		RCache.Render				(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
	}
}
