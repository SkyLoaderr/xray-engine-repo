// FBasicVisual.h: interface for the FBasicVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "fmesh.h"

// refs
class ENGINE_API CStream;

// The class itself
class ENGINE_API FBasicVisual
{
	// Helper functions
protected:

	// Sort faces data to archive maximal performance for rendering
	// Especially for T&L devices
	void			SortFaces(WORD *pIndices, int idxCnt);

public:
	DWORD			Type;
	char			fName	[_MAX_PATH];

	// Common data for rendering
	DWORD			dwRMode;
	Fvector			vPosition;
	float			fRadius;

	// Other common variables
	Fvector			min;
	Fvector			max;

	virtual FBasicVisual*	CreateInstance(void);

	virtual void	Dump();
	virtual void	ApplyMaterials(void)		{};
	virtual void	Render(float LOD)			{};// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void	Load(CStream *data);
	virtual void	PerformOptimize(void)		{};
	virtual void	Copy(FBasicVisual *pFrom);
	virtual void	EnableMode	(DWORD mask)	{ dwRMode|=mask;	}
	virtual void	DisableMode	(DWORD mask)	{ dwRMode&=~mask;	}

	FBasicVisual();
	virtual ~FBasicVisual();
};

extern	FBasicVisual*	LoadVisual(char *name);

#endif // !defined(AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_)
