// FVisual.h: interface for the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_)
#define AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_
#pragma once

#include "fbasicvisual.h"

class ENGINE_API Fvisual : public FBasicVisual
{
protected:
	LPDIRECT3DVERTEXBUFFER7	pVertexBuffer;
	vector<WORD>			Indices;
	DWORD					dwVertCount;
	Fmaterial				mmx;
public:
	virtual FBasicVisual*	CreateInstance(void);

	virtual void Render(float LOD);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load(CStream *data);
	virtual void PerformLighting(void);
	virtual void PerformOptimize(void);
	virtual void Dump(void);
	virtual void Copy(FBasicVisual *pFrom);

	Fvisual();
	virtual ~Fvisual();
};

#endif // !defined(AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_)
