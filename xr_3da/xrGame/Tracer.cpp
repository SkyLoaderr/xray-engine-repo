// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracer.h"

const DWORD	MAX_TRACERS	= 1024;
const DWORD TRACER_SIZE = 0.1f;

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

void	CTracer::Add	(const Fvector& from, const Fvector& to, float bullet_speed, float trail_speed_factor, float start_length, DWORD color)
{
	bullets.push_back	(Bullet());
	Bullet&	B			= bullets.back();

	Fvector	path;
	path.sub			(from,to);

	B.target.set		(to);
	B.dir.normalize		(path);
	B.pos_head.direct	(from,B.dir,start_length);
	B.pos_trail.set		(from);
	B.speed_head =		bullet_speed;
	B.speed_trail =		bullet_speed*trail_speed_factor;
	B.color.set			(color);
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
		Fvector sC;	float sR;
		sC.sub	(B.pos_head,B.pos_trail);
		sC.mul	(.5f);
		sR		= sC.magnitude();
		sC.add	(B.pos_trail);
		if (!::Render.ViewBase->testSphereDirty(sC,sR))	continue;

		// Everything OK - build vertices
		Fvector	P;
		DWORD	C	= B.color.get	();
		P.mad(B.pos_trail,vTop,-TRACER_SIZE);	verts->set(P,C,0,1);	verts++;
		P.mad(B.pos_trail,vTop,TRACER_SIZE);	verts->set(P,C,0,0);	verts++;
		P.mad(B.pos_head, vTop,-TRACER_SIZE);	verts->set(P,C,1,1);	verts++;
		P.mad(B.pos_head, vTop,TRACER_SIZE);	verts->set(P,C,1,0);	verts++;
	}

	DWORD vCount = verts-start;
	VS->Unlock(vCount);
	Device.Primitive.Draw(VS,vCount,vCount/2,vOffset,Device.Streams_QuadIB);
}
