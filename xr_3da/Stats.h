// Stats.h: interface for the CStats class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_
#pragma once

class ENGINE_API CGameFont;

DECLARE_MESSAGE(Stats);

class ENGINE_API CStats
{
public:
	CGameFont*	pFont;

	float		fFPS,fRFPS,fTPS;				// FPS, RenderFPS, TPS
	float		fMem_calls;
	u32			dwMem_calls;
	u32			dwSND_Played,dwSND_Allocated;	// Play/Alloc

	CStatTimer	EngineTOTAL;		// 
	CStatTimer	Sheduler;			// 
	CStatTimer	UpdateClient;		// 
	CStatTimer	Scripting;			// 
	CStatTimer	Physics;			// movement+collision
	CStatTimer	ph_collision;		// collision
	CStatTimer	ph_core;			// collision
	CStatTimer	AI_Think;			// thinking
	CStatTimer	AI_Range;			// query: range
	CStatTimer	AI_Path;			// query: path
	CStatTimer	AI_Node;			// query: node
	CStatTimer	AI_Vis;				// visibility detection - total
	CStatTimer	AI_Vis_Query;		// visibility detection - portal traversal and frustum culling
	CStatTimer	AI_Vis_RayTests;	// visibility detection - ray casting

	CStatTimer	RenderTOTAL;		// 
	CStatTimer	RenderTOTAL_Real;	
	CStatTimer	RenderCALC;			// portal traversal, frustum culling, entities "renderable_Render"
	CStatTimer	RenderCALC_HOM;		// HOM rendering
	CStatTimer	Animation;			// skeleton calculation
	CStatTimer	RenderDUMP;			// actual primitive rendering
	CStatTimer	RenderDUMP_Wait;	// ...waiting something back (queries results, etc.)
	CStatTimer	RenderDUMP_RT;		// ...render-targets
	CStatTimer	RenderDUMP_SKIN;	// ...skinning
	CStatTimer	RenderDUMP_HUD;		// ...hud rendering
	CStatTimer	RenderDUMP_Glows;	// ...glows vis-testing,sorting,render
	CStatTimer	RenderDUMP_Lights;	// ...d-lights building/rendering
	CStatTimer	RenderDUMP_WM;		// ...wallmark sorting, rendering
	u32			RenderDUMP_WMS_Count;// ...number of static wallmark
	u32			RenderDUMP_WMD_Count;// ...number of dynamic wallmark
	u32			RenderDUMP_WMT_Count;// ...number of wallmark tri
	CStatTimer	RenderDUMP_DT_VIS;	// ...details visibility detection
	CStatTimer	RenderDUMP_DT_Render;// ...details rendering
	CStatTimer	RenderDUMP_DT_Cache;// ...details slot cache access
	u32			RenderDUMP_DT_Count;// ...number of DT-elements
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

	shared_str	eval_line_1;
	shared_str	eval_line_2;
	shared_str	eval_line_3;

	void		Show				(void);
	void		OnDeviceCreate		(void);
	void		OnDeviceDestroy		(void);
public:
	xr_vector		<shared_str>	errors;
	CRegistrator	<pureStats>		seqStats;
public:
					CStats			();
					~CStats			();

	IC CGameFont*	Font			(){return pFont;}
};

#endif // !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
