// FHierrarhyVisual.h: interface for the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FHIERRARHYVISUAL_H__D93A4EA1_12D8_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FHIERRARHYVISUAL_H__D93A4EA1_12D8_11D4_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "fbasicvisual.h"

class ENGINE_API FHierrarhyVisual : public IRender_Visual
{
public:
	xr_vector<IRender_Visual*>		children;
	BOOL					bDontDelete;

	virtual void	Load	(const char* N, IReader *data, u32 dwFlags);
	virtual void	Copy	(IRender_Visual *pFrom);

	FHierrarhyVisual();
	virtual ~FHierrarhyVisual();
};

#endif // !defined(AFX_FHIERRARHYVISUAL_H__D93A4EA1_12D8_11D4_B4E3_4854E82A090D__INCLUDED_)
