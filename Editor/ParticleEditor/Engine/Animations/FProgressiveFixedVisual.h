// FProgressiveFixedVisual.h: interface for the FProgressiveFixedVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FPROGRESSIVEFIXEDVISUAL_H__778ADCC0_9140_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FPROGRESSIVEFIXEDVISUAL_H__778ADCC0_9140_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "FVisual.h"

#pragma pack(push,1)
struct ENGINE_API Vsplit
{
	WORD	vsplitVert;		// the vert which I'm splitting off of;
							// (the vert I create is just the current end of the VB)
	BYTE	numNewTriangles;
	BYTE	numFixFaces;
							// the index here is an offset in the FixFaces list
							// which contains offsets in the triangle list
};
#pragma pack(pop)

#define PM_REFRESH	6

class ENGINE_API FProgressiveFixedVisual : public Fvisual
{
protected:
	DWORD			V_Minimal;
	DWORD			V_Current;
	DWORD			I_Current;
	DWORD			FIX_Current;
	DWORD			iRefresh;		// 0..PM_REFRESH
	WORD*			faces_affected;	// shared
	Vsplit*			vsplit;			// shared

	void			pm_copy			(FBasicVisual *pFrom);
public:
	virtual void Render	(float LOD);	// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Used
	virtual void Load	(const char* N, CStream *data,DWORD dwFlags);
	virtual void Copy	(FBasicVisual *pFrom);
	virtual void Release();

	// functionality
	void	SetLOD		(float LOD);

	FProgressiveFixedVisual();
	virtual ~FProgressiveFixedVisual();
};

#endif // !defined(AFX_FPROGRESSIVEFIXEDVISUAL_H__778ADCC0_9140_11D3_B4E3_4854E82A090D__INCLUDED_)
