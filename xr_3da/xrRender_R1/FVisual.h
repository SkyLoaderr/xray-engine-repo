// FVisual.h: interface for the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_)
#define AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_
#pragma once

#include "../fbasicvisual.h"

class	Fvisual					: public		IRender_Visual, public IRender_Mesh
{
public:
	IRender_Mesh*				m_fast			;	
public:
	virtual void				Render			(float LOD		);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
	virtual void				Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void				Copy			(IRender_Visual *pFrom	);
	virtual void				Release			();

	Fvisual();
	virtual ~Fvisual();
};

#endif // !defined(AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_)
