// Stats.h: interface for the CStats class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_
#pragma once

class ENGINE_API CStatTimer
{
	friend class CStats;
	friend class CRenderDevice;
private:
	CTimer		T;
	__int64		accum;
	float		result;
	DWORD		count;
public:
	CStatTimer()
	{
		accum	= 0;
		result	= 0;
		count	= 0;
	}

	IC void		FrameStart	()
	{
		accum	= 0;
		count	= 0;
	}
	IC void		FrameEnd	()
	{
		result	= 0.97f*result + 0.03f*float(accum)*CPU::cycles2milisec;
	}
	IC void		Begin()
	{	count++; T.Start(); }
	IC void		End()
	{	accum += T.GetElapsed(); }
};

class ENGINE_API CStats
{
public:
	float		fFPS,fRFPS,fTPS;	// FPS, RenderFPS, TPS
	DWORD		dwPoly, dwVert;
	DWORD		dwCalls;			// Number of primitive-render calls
	DWORD		dwShader_Changes;	// Number of times the shader changes
	DWORD		dwTexture_Changes;
	DWORD		dwSND_Played,dwSND_Allocated;	// Play/Alloc

	CStatTimer	EngineTOTAL;		// 
	CStatTimer	UpdateSheduled;		// 
	CStatTimer	UpdateClient;		// 
	CStatTimer	Physics;			// movement+collision
	CStatTimer	AI_Range;			// query: range
	CStatTimer	AI_Path;			// query: path
	CStatTimer	AI_Node;			// query: node
	CStatTimer	AI_Vis;				// visibility detection - total
	CStatTimer	AI_Vis_Query;		// visibility detection - portal traversal and frustum culling
	CStatTimer	AI_Vis_RayTests;	// visibility detection - ray casting

	CStatTimer	RenderTOTAL;		// 
	CStatTimer	RenderTOTAL_Real;	
	CStatTimer	RenderCALC;			// portal traversal, frustum culling, entities "OnVisible"
	CStatTimer	Animation;			// skeleton calculation
	CStatTimer	RenderDUMP;			// actual primitive rendering
	CStatTimer	RenderDUMP_SKIN;	// ...skinning
	CStatTimer	RenderDUMP_Cached;	// ...cached rendering
	CStatTimer	RenderDUMP_HUD;		// ...hud rendering
	CStatTimer	RenderDUMP_Glows;	// ...glows vis-testing,sorting,render
	CStatTimer	RenderDUMP_Lights;	// ...d-lights building/rendering
	CStatTimer	RenderDUMP_WM;		// ...walmark sorting, rendering
	CStatTimer	RenderDUMP_DT_VIS;	// ...details visibility detection
	CStatTimer	RenderDUMP_DT_Render;// ...details rendering
	CStatTimer	RenderDUMP_DT_Cache;// ...details slot cache access
	CStatTimer	RenderDUMP_Shads;	// ...shadows building, rendering

	CStatTimer	Sound;				// total time taken by sound subsystem (accurate only in single-threaded mode)
	CStatTimer	Input;				// total time taken by input subsystem (accurate only in single-threaded mode)
	CStatTimer	clRAY;				// total: ray-testing
	CStatTimer	clBOX;				// total: box query
	
	CStatTimer	netClient;
	CStatTimer	netServer;
	
	CStatTimer	TEST;				// debug counter

	void	Show		(void);

	CStats	();
	~CStats	();
};

#define UPDATEC(vert,poly,pass)		{ Device.Statistic.dwVert+=(vert)*(pass); Device.Statistic.dwPoly+=(poly)*pass; Device.Statistic.dwCalls+=pass; }

#endif // !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
