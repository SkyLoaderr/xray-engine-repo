// Stats.h: interface for the CStats class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_
#pragma once

// refs
class CFontBase;

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
    IC void 	Dump()
    { 	Log("Stat timer: ",result); }
};

class ENGINE_API CStats
{
public:
	float		fFPS,fRFPS,fTPS;	// FPS, RenderFPS, TPS
	DWORD		dwShader_Codes;		// Number of times the shader-code changes
	DWORD		dwShader_Textures;	// Number of times the shader-tex changes
	DWORD		dwShader_Matrices;	// Number of times the shader-xform changes
	DWORD		dwShader_Constants;	// Number of times the shader-consts changes
	DWORD		dwSND_Played,dwSND_Allocated;	// Play/Alloc
    DWORD		dwTotalLight,dwLightInScene;

	CStatTimer	RenderDUMP_RT;
	CStatTimer	RenderTOTAL;		//
	CStatTimer	RenderTOTAL_Real;
	CStatTimer	RenderCALC;			// portal traversal, frustum culling, entities "OnVisible"
	CStatTimer	RenderDUMP_SKIN;
	CStatTimer	Animation;			// skeleton calculation

	CStatTimer	Input;				// total time taken by input subsystem (accurate only in single-threaded mode)
	CStatTimer	clRAY;				// total: ray-testing
	CStatTimer	clBOX;				// total: box query
    CStatTimer	clFRUSTUM;			// total: frustum query

	void	Show		(CFontBase* font);

	CStats	();
	~CStats	();
};

#define UPDATEC(vert,poly,pass)		{ Device.Statistic.dwVert+=(vert)*(pass); Device.Statistic.dwPoly+=(poly)*pass; Device.Statistic.dwCalls+=pass; }

#endif // !defined(AFX_STATS_H__4C8D1860_0EE2_11D4_B4E3_4854E82A090D__INCLUDED_)
