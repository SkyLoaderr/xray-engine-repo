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
	CPrimitive				P;
	DWORD					dwVertCount;
	DWORD					dwPrimsCount;
public:
	virtual FBasicVisual*	CreateInstance(void);

	virtual void Render		(float LOD);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load		(const char* N, CStream *data,DWORD dwFlags);
	virtual void Copy		(FBasicVisual *pFrom);
	virtual void Release	();

	Fvisual();
	virtual ~Fvisual();
};

#endif // !defined(AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_)
