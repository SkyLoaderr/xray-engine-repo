// Stats.h: interface for the CStats class.
//
//////////////////////////////////////////////////////////////////////

#ifndef EStatsH
#define EStatsH
#pragma once

// refs
class CGameFont;

class ENGINE_API CStatTimer
{
	friend class CStats;
	friend class CRenderDevice;
private:
	CTimer		T;
	__int64		accum;
	float		result;
	u32		count;
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
	{	accum += T.GetElapsed_clk(); }
    IC void 	Dump()
    { 	Log("Stat timer: ",result); }
};

class ENGINE_API CStats
{
public:
	float		fFPS,fRFPS,fTPS;	// FPS, RenderFPS, TPS
	u32		dwShader_Codes;		// Number of times the shader-code changes
	u32		dwShader_Textures;	// Number of times the shader-tex changes
	u32		dwShader_Matrices;	// Number of times the shader-xform changes
	u32		dwShader_Constants;	// Number of times the shader-consts changes
	u32		dwSND_Played,dwSND_Allocated;	// Play/Alloc
    u32		dwTotalLight,dwLightInScene;
	u32		dwLevelSelFaceCount;	// Number of faces in scene
	u32		dwLevelSelVertexCount;	// Number of vertices in scene

	CStatTimer	RenderDUMP_RT;
	CStatTimer	RenderTOTAL;		//
	CStatTimer	RenderTOTAL_Real;
	CStatTimer	RenderCALC;			// portal traversal, frustum culling, entities "OnVisible"
	CStatTimer	RenderDUMP_SKIN;
	CStatTimer	Animation;			// skeleton calculation
	CStatTimer	RenderDUMP_DT_VIS;	// ...details visibility detection
	CStatTimer	RenderDUMP_DT_Render;// ...details rendering
	CStatTimer	RenderDUMP_DT_Cache;// ...details slot cache access

	CStatTimer	Input;				// total time taken by input subsystem (accurate only in single-threaded mode)
	CStatTimer	clRAY;				// total: ray-testing
	CStatTimer	clBOX;				// total: box query
    CStatTimer	clFRUSTUM;			// total: frustum query

	CStatTimer	TEST0;				// debug counter
	CStatTimer	TEST1;				// debug counter
	CStatTimer	TEST2;				// debug counter
	CStatTimer	TEST3;				// debug counter

	void	Show		(CGameFont* font);

	CStats	();
	~CStats	();
};

#define UPDATEC(vert,poly,pass)		{ Device.Statistic.dwVert+=(vert)*(pass); Device.Statistic.dwPoly+=(poly)*pass; Device.Statistic.dwCalls+=pass; }

#endif // !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
