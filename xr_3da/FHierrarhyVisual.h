// FHierrarhyVisual.h: interface for the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FHIERRARHYVISUAL_H__D93A4EA1_12D8_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FHIERRARHYVISUAL_H__D93A4EA1_12D8_11D4_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "FBasicVisual.h"

class ENGINE_API FHierrarhyVisual : public FBasicVisual
{
public:
	vector<FBasicVisual*>	chields;
	BOOL					bDontDelete;

	virtual void	Load	(const char* N, CStream *data, DWORD dwFlags);
	virtual void	Copy	(FBasicVisual *pFrom);

	FHierrarhyVisual();
	virtual ~FHierrarhyVisual();
};

#endif // !defined(AFX_FHIERRARHYVISUAL_H__D93A4EA1_12D8_11D4_B4E3_4854E82A090D__INCLUDED_)
