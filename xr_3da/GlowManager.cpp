// GlowManager.cpp: implementation of the CGlowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlowManager.h"
#include "xr_creator.h"
#include "render.h"
#include "xrlevel.h"

// font
#include "x_ray.h"
#include "GameFont.h"

// breakpoints
#include "xr_input.h"

#define FADE_SCALE			1024.f
#define MAX_GlowsDist1		float(pCreator->Environment.Current.Far)
#define MAX_GlowsDist2		float(MAX_GlowsDist1*MAX_GlowsDist1)

int psGlowsPerFrame		= 7;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlowManager::CGlowManager()
{
	VS		= 0;
}

CGlowManager::~CGlowManager()
{
	Unload	();
}

void CGlowManager::OnDeviceCreate()
{
	VS	= Device.Shader._CreateVS	(FVF::F_TL);
}

void CGlowManager::OnDeviceDestroy()
{
	Device.Shader._DeleteVS			(VS);
}

void CGlowManager::Load(CStream* fs)
{
	OnDeviceCreate		();

	// glows itself
	DWORD size  = fs->Length();
	R_ASSERT	(size);
	DWORD one	= 4*sizeof(float)+2*sizeof(DWORD);
	R_ASSERT	(size%one == 0);
	DWORD count	= size/one;
	Glows.reserve(count);

	for (;count;count--)
	{
		CGlow G;
		fs->Read(&G.C,3*sizeof(float));
		fs->Read(&G.R,1*sizeof(float));

		DWORD T		= fs->Rdword();
		DWORD S		= fs->Rdword();
		G.hShader	= pCreator->LL_CreateShader(S,T,-1,-1);

		G.fade		= 255.f;
		G.dwFrame	= 0x0;
		G.bTestResult=TRUE;

		Glows.push_back(G);
	}
	Selected_Count	= 0;
	dwTestID		= 0;
}

void CGlowManager::Unload()
{
	OnDeviceDestroy		();

	// shaders
	for(DWORD i=0; i<Glows.size(); i++) 
		Device.Shader.Delete(Glows[i].hShader);

	// glows
	Glows.clear		();
	Selected_Count	= 0;

	// stream
	VS				= 0;
}

IC bool glow_compare(CGlow* g1, CGlow *g2)
{	return g1->hShader < g2->hShader; }

void CGlowManager::add(vector<WORD> &V)
{
	if (! (psEnvFlags & effGlows))		return;

	Device.Statistic.RenderDUMP_Glows.Begin();

	Fvector &start	= Device.vCameraPosition;
	DWORD	mark	= Device.dwFrame;
	float	dt		= Device.fTimeDelta;
	float	dlim2	= MAX_GlowsDist2;

	for (vector<WORD>::iterator I=V.begin(); I!=V.end(); I++)
	{
		CGlow&	G	 = Glows[*I];
		if (G.dwFrame==mark) continue;

		float	range = start.distance_to_sqr(G.C);
		if (range < dlim2) {
			if (::Render->View->testSphere_dirty(G.C,G.R)) {
				G.dwFrame = mark;

				// 2. Use result of test
				if (G.bTestResult) {
					G.fade -= dt*FADE_SCALE;
					if (G.fade<1.) G.fade = 1;
				} else {
					G.fade += dt*FADE_SCALE;
					if (G.fade>255.f) G.fade = 255.f;
				}

				/*if (G.fade>1.f)*/ Selected[Selected_Count++] = &G;
				continue;
			}
		} else {
			G.dwFrame = mark;
		}
		G.fade -= dt*FADE_SCALE;
		if (G.fade<1.) G.fade = 1;
	}

	Device.Statistic.RenderDUMP_Glows.End();
}

void CGlowManager::Render()
{
	if (!Selected_Count) return;

	Device.Statistic.RenderDUMP_Glows.Begin();
	float	_width		= float(::Render->getTarget()->get_width());
	float	_width_2	= _width / 2.f;
	float	_height_2	= float(::Render->getTarget()->get_height())/2.f;
	float	fov_scale	= _width / (Device.fFOV/90.f);
	{
		// 1. Test some number of glows
		Fvector &start	= Device.vCameraPosition;
		for (int i=0; i<psGlowsPerFrame; i++,dwTestID++)
		{
			DWORD	ID	= dwTestID%Selected_Count;
			CGlow&	G	= *Selected[ID];
			if (G.dwFrame=='test')	break;

			G.dwFrame	=	'test';
			Fvector dir;
			dir.sub(G.C,start); float range = dir.magnitude();
			dir.div(range);
			G.bTestResult = pCreator->ObjectSpace.RayTest(start,dir,range,TRUE,&G.RayCache);
		}

		// 2. Sort by shader
		std::sort	(Selected,Selected+Selected_Count,glow_compare);
		
		FVF::TL		*pv;
		FVF::TL		TL;
		
		DWORD		pos = 0, count;
		Shader*		T;

		float		dlim2	= MAX_GlowsDist2;
		for (;pos<Selected_Count;) 
		{
			T		= Selected[pos]->hShader;
			count	= 0;
			while	((pos+count<Selected_Count) && (Selected[pos+count]->hShader==T)) count++;
			
			DWORD		vOffset;
			DWORD		end		= pos+count;
			FVF::TL	*	pvs		= pv = (FVF::TL*) Device.Streams.Vertex.Lock(count*4,VS->dwStride,vOffset);
			for (; pos<end; pos++)
			{
				CGlow&	G			= *Selected[pos];
				if (G.fade<=1.f)	continue;
				
				TL.transform	(G.C,Device.mFullTransform);
				float size		= fov_scale * G.R /TL.p.w;
				
				// Convert to screen coords
				float cx        = (1+TL.p.x)*_width_2;
				float cy        = (1+TL.p.y)*_height_2;
				float dist		= Device.vCameraPosition.distance_to_sqr(G.C);
				DWORD C			= iFloor(G.fade*(1-(dist/dlim2)));
				DWORD clr		= D3DCOLOR_RGBA(C,C,C,C);
				
				pv->set(cx - size, cy + size, TL.p.z, TL.p.w, clr, 0, 1); pv++;
				pv->set(cx - size, cy - size, TL.p.z, TL.p.w, clr, 0, 0); pv++;
				pv->set(cx + size, cy + size, TL.p.z, TL.p.w, clr, 1, 1); pv++;
				pv->set(cx + size, cy - size, TL.p.z, TL.p.w, clr, 1, 0); pv++;
			}
			int vCount				= pv-pvs;
			Device.Streams.Vertex.Unlock		(vCount,VS->dwStride);
			if (vCount) {
				Device.Shader.set_Shader		(T);
				Device.Primitive.setVertices	(VS->dwHandle,VS->dwStride,Device.Streams.Vertex.Buffer());
				Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);
				Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount,0,vCount/2);
			}
		}
	}
	Device.Statistic.RenderDUMP_Glows.End();
	Selected_Count = 0;
}
