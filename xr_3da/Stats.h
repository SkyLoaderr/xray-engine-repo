// Stats.h: interface for the CStats class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "stats_timer.h"

class ENGINE_API CFontHUD;

class ENGINE_API CStats
{
public:
	CFontHUD*	font;

	float		fFPS,fRFPS,fTPS;	// FPS, RenderFPS, TPS
	DWORD		dwPoly, dwVert;
	DWORD		dwCalls;			// Number of primitive-render calls
	DWORD		dwShader_Codes;		// Number of times the shader-code changes
	DWORD		dwShader_Textures;	// Number of times the shader-tex changes
	DWORD		dwShader_Matrices;	// Number of times the shader-xform changes
	DWORD		dwShader_Constants;	// Number of times the shader-consts changes
	DWORD		dwXFORMs;
	DWORD		dwSND_Played,dwSND_Allocated;	// Play/Alloc

	CStatTimer	EngineTOTAL;		// 
	CStatTimer	Sheduler;			// 
	CStatTimer	UpdateClient;		// 
	CStatTimer	Physics;			// movement+collision
	CStatTimer	AI_Think;			// thinking
	CStatTimer	AI_Range;			// query: range
	CStatTimer	AI_Path;			// query: path
	CStatTimer	AI_Node;			// query: node
	CStatTimer	AI_Vis;				// visibility detection - total
	CStatTimer	AI_Vis_Query;		// visibility detection - portal traversal and frustum culling
	CStatTimer	AI_Vis_RayTests;	// visibility detection - ray casting

	CStatTimer	RenderTOTAL;		// 
	CStatTimer	RenderTOTAL_Real;	
	CStatTimer	RenderCALC;			// portal traversal, frustum culling, entities "OnVisible"
	CStatTimer	RenderCALC_HOM;		// HOM rendering
	CStatTimer	Animation;			// skeleton calculation
	CStatTimer	RenderDUMP;			// actual primitive rendering
	CStatTimer	RenderDUMP_RT;		// ...render-targets
	CStatTimer	RenderDUMP_SKIN;	// ...skinning
	CStatTimer	RenderDUMP_Cached;	// ...cached rendering
	CStatTimer	RenderDUMP_HUD;		// ...hud rendering
	CStatTimer	RenderDUMP_Glows;	// ...glows vis-testing,sorting,render
	CStatTimer	RenderDUMP_Lights;	// ...d-lights building/rendering
	CStatTimer	RenderDUMP_WM;		// ...walmark sorting, rendering
	CStatTimer	RenderDUMP_DT_VIS;	// ...details visibility detection
	CStatTimer	RenderDUMP_DT_Render;// ...details rendering
	CStatTimer	RenderDUMP_DT_Cache;// ...details slot cache access
	CStatTimer	RenderDUMP_Pcalc;	// ...projectors	building
	CStatTimer	RenderDUMP_Scalc;	// ...shadows		building
	CStatTimer	RenderDUMP_Srender;	// ...shadows		render
	
	CStatTimer	Sound;				// total time taken by sound subsystem (accurate only in single-threaded mode)
	CStatTimer	Input;				// total time taken by input subsystem (accurate only in single-threaded mode)
	CStatTimer	clRAY;				// total: ray-testing
	CStatTimer	clBOX;				// total: box query
	CStatTimer	clFRUSTUM;			// total: frustum query
	
	CStatTimer	netClient;
	CStatTimer	netServer;
	
	CStatTimer	TEST0;				// debug counter
	CStatTimer	TEST1;				// debug counter
	CStatTimer	TEST2;				// debug counter
	CStatTimer	TEST3;				// debug counter

	void		Show			(void);
	void		OnDeviceCreate	(void);
	void		OnDeviceDestroy (void);

	CStats	();
	~CStats	();
};

#define UPDATEC(vert,poly,pass)		{ Device.Statistic.dwVert+=(vert)*(pass); Device.Statistic.dwPoly+=(poly)*pass; Device.Statistic.dwCalls+=pass; }

#endif // !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
