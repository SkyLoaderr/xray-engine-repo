// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:06 , by user : Oles , from computer : OLES
// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracer.h"

const DWORD	MAX_TRACERS	= (1024*5);
const float TRACER_SIZE = 0.2f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracer::CTracer()
{
	Device.seqDevCreate.Add(this);
	Device.seqDevDestroy.Add(this);
	OnDeviceCreate();
}

CTracer::~CTracer()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	OnDeviceDestroy();
}

void	CTracer::OnDeviceCreate()
{
	REQ_CREATE	();
	sh_Tracer	= Device.Shader.Create		("effects\\bullet_tracer","effects\\bullet_tracer");
	VS			= Device.Shader._CreateVS	(FVF::F_V);
}
void	CTracer::OnDeviceDestroy()
{
	Device.Shader.Delete	(sh_Tracer);
	Device.Shader._DeleteVS	(VS);
}

void	CTracer::Add	(const Fvector& from, const Fvector& to, float bullet_speed, float trail_speed_factor, float start_length, float width)
{
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
	
	DWORD	vOffset;
	FVF::V	*verts		=	(FVF::V	*) Device.Streams.Vertex.Lock(bullets.size()*4,VS->dwStride,vOffset);
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

	DWORD vCount					= verts-start;
	Device.Streams.Vertex.Unlock	(vCount,VS->dwStride);
	
	if (vCount)	{
		Device.set_xform_world		(Fidentity);
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
		Device.Shader.set_Shader	(sh_Tracer);
		Device.Primitive.setVertices(VS->dwHandle,VS->dwStride,Device.Streams.Vertex.Buffer());
		Device.Primitive.setIndices	(vOffset,Device.Streams.QuadIB);;
		Device.Primitive.Render		(D3DPT_TRIANGLELIST,0,vCount,0,vCount/2);
		UPDATEC						(vCount,vCount/2,1);
		HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
	}
}
