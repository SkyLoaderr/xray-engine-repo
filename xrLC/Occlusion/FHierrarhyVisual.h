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
	vector<FBasicVisual*> chields;

	virtual FBasicVisual*	CreateInstance(void);

	virtual void	Load(CStream *data);
	virtual void	Copy(FBasicVisual *pFrom);
	virtual void	PerformOptimize(void);
	virtual void	EnableMode(DWORD mask);
	virtual void	DisableMode(DWORD mask);

	FHierrarhyVisual();
	virtual ~FHierrarhyVisual();
};

#endif // !defined(AFX_FHIERRARHYVISUAL_H__D93A4EA1_12D8_11D4_B4E3_4854E82A090D__INCLUDED_)
