// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracer.h"
#include "..\fstaticrender.h"

const DWORD	MAX_TRACERS	= (1024*6);
const float TRACER_SIZE = 0.2f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracer::CTracer()
{
	sh_Tracer	= Device.Shader.Create	("bullet_tracer","bullet_tracer");
	VS			= Device.Streams.Create	(FVF::F_LIT,MAX_TRACERS*4);
}

CTracer::~CTracer()
{
	Device.Shader.Delete	(sh_Tracer);
}

void	CTracer::Add	(const Fvector& from, const Fvector& to, float bullet_speed, float trail_speed_factor, float start_length, float width)
{
	bullets.push_back	(Bullet());
	Bullet&	B			= bullets.back();

	Fvector	path;
	path.sub			(to,from);

	B.target.set		(to);
	B.dir.normalize		(path);
//	B.pos_head.direct	(from,B.dir,start_length/2);
	B.pos_head.direct	(from,B.dir,start_length);
//	B.pos_trail.direct	(from,B.dir,-start_length/2);
	B.pos_trail.set		(from);
	B.speed_head =		bullet_speed;
	B.speed_trail =		bullet_speed*trail_speed_factor;
	B.width =			width;
//	B.life_time =		(path.magnitude()-start_length/2)/bullet_speed;
	B.life_time =		(path.magnitude()-start_length)/bullet_speed;
	if (B.life_time<0)	bullets.pop_back();
}

void	CTracer::Render	()
{
	if (bullets.empty())	return;
	
	DWORD	vOffset;
	FVF::LIT	*verts	=	(FVF::LIT	*) VS->Lock(bullets.size()*4,vOffset);
	FVF::LIT	*start	=	verts;
	float	dt			=	Device.fTimeDelta;

	Fvector&	vTop	=	Device.vCameraTop;
	Fvector&	vDir	=	Device.vCameraDirection;
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
		if (!::Render.ViewBase.testSphereDirty(sC,sR))	continue;

		// Everything OK - build vertices
		Fvector	P,lineTop,camDir;
		camDir.sub			(sC,vCenter);
		camDir.normalize	();
		lineTop.crossproduct(camDir,lineD);
		float	w = B.width;
		P.mad(B.pos_trail,lineTop,-w);	verts->set(P,0,0,1);	verts++;
		P.mad(B.pos_trail,lineTop,w);	verts->set(P,0,0,0);	verts++;
		P.mad(B.pos_head, lineTop,-w);	verts->set(P,0,1,1);	verts++;
		P.mad(B.pos_head, lineTop,w);	verts->set(P,0,1,0);	verts++;
	}

	DWORD vCount			= verts-start;
	VS->Unlock				(vCount);
	
	if (vCount)	{
		HW.pDevice->SetTransform	(D3DTS_WORLD, precalc_identity.d3d());
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
		Device.Shader.Set			(sh_Tracer);
		Device.Primitive.Draw		(VS,vCount,vCount/2,vOffset,Device.Streams_QuadIB);
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
	}
}
