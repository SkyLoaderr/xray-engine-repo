// GlowManager.cpp: implementation of the CGlowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlowManager.h"
#include "xr_creator.h"
#include "fstaticrender.h"
#include "xrlevel.h"

// font
#include "x_ray.h"
#include "xr_smallfont.h"

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
}

CGlowManager::~CGlowManager()
{
	Unload	();
}

void CGlowManager::Load(CStream* fs)
{
	// stream
	Stream = Device.Streams.Create(FVF::F_TL,MAX_GlowsPerFrame*4);

	// glows itself
	DWORD size  = fs->Length();
	R_ASSERT(size);
	DWORD one	= 4*sizeof(float)+2*sizeof(DWORD);
	R_ASSERT(size%one == 0);
	DWORD count	= size/one;
	Glows.reserve(count);

	for (;count;count--)
	{
		CGlow G;
		fs->Read(&G.C,3*sizeof(float));
		fs->Read(&G.R,1*sizeof(float));

		DWORD T		= fs->Rdword();
		DWORD S		= fs->Rdword();
		G.hShader	= pCreator->LL_CreateShader(S,T);

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
	// shaders
	for(DWORD i=0; i<Glows.size(); i++) 
		Device.Shader.Delete(Glows[i].hShader);

	// glows
	Glows.clear		();
	Selected_Count	= 0;

	// stream
	Stream = 0;
}

IC bool glow_compare(CGlow* g1, CGlow *g2)
{	return g1->hShader < g2->hShader; }

void CGlowManager::add(vector<WORD> &V)
{
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
			if (::Render.View->visibleSphereNC(G.C,G.R)) {
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
		std::sort(Selected,Selected+Selected_Count,glow_compare);
		
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
			FVF::TL	*	pvs		= pv = (FVF::TL*) Stream->Lock(count*4,vOffset);
			for (; pos<end; pos++)
			{
				CGlow&	G			= *Selected[pos];
				if (G.fade<=1.f)	continue;
				
				TL.transform	(G.C,Device.mFullTransform);
				float size		= Device.dwWidth * G.R /TL.p.w;
				
				// Convert to screen coords
				float cx        = Device._x2real(TL.p.x);
				float cy        = Device._y2real(TL.p.y);
				float dist		= Device.vCameraPosition.distance_to_sqr(G.C);
				DWORD C			= iFloor(G.fade*(1-(dist/dlim2)));
				DWORD clr		= D3DCOLOR_RGBA(C,C,C,C);
				
				pv->set(cx - size, cy + size, TL.p.z, TL.p.w, clr, 0, 1); pv++;
				pv->set(cx - size, cy - size, TL.p.z, TL.p.w, clr, 0, 0); pv++;
				pv->set(cx + size, cy + size, TL.p.z, TL.p.w, clr, 1, 1); pv++;
				pv->set(cx + size, cy - size, TL.p.z, TL.p.w, clr, 1, 0); pv++;
			}
			int verts				= pv-pvs;
			Stream->Unlock			(verts);
			if (verts) {
				Device.Shader.Set		(T);
				Device.Primitive.Draw	(Stream,verts,verts/2,vOffset,Device.Streams_QuadIB);
			}
		}
	}
	Device.Statistic.RenderDUMP_Glows.End();
	Selected_Count = 0;
}
