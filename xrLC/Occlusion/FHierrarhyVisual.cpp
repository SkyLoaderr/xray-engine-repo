// FHierrarhyVisual.cpp: implementation of the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FHierrarhyVisual.h"
#include "ffileops.h"

FBasicVisual*	FHierrarhyVisual::CreateInstance(void)
{	return new FHierrarhyVisual;	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_E(FHierrarhyVisual::FHierrarhyVisual()  : FBasicVisual())
_end;

_E(FHierrarhyVisual::~FHierrarhyVisual())
	for (int i=0; i<chields.size(); i++)
		_DELETE(chields[i]);
	chields.clear();
_end;

_E(void FHierrarhyVisual::Load(CStream *data))
	FBasicVisual::Load(data);
	if (data->FindChunk(OGF_CHIELDS)) {
		FILE_NAME	fn;
		int			cnt = data->ReadDWORD();
		for (int i=0; i<cnt; i++) {
			data->ReadStringZ(fn);
			chields.push_back(LoadVisual(fn));
		}
	} else {
		THROW;
	}
_end;

void	FHierrarhyVisual::Copy(FBasicVisual *pSrc)
{
	FBasicVisual::Copy(pSrc);

	FHierrarhyVisual	*pFrom = (FHierrarhyVisual *)pSrc;

	for (DWORD i=0; i<pFrom->chields.size(); i++) {
		FBasicVisual *p = pFrom->chields[i]->CreateInstance();
		p->Copy(pFrom->chields[i]);
		chields.push_back(p);
	}
}

void FHierrarhyVisual::PerformOptimize(void)
{
	for (int i=0; i<chields.size(); i++)
		chields[i]->PerformOptimize();
}

void FHierrarhyVisual::EnableMode(DWORD mask)
{
	FBasicVisual::EnableMode(mask);
	for (int i=0; i<chields.size(); i++) 
		chields[i]->EnableMode(mask);
}

void FHierrarhyVisual::DisableMode(DWORD mask)
{
	FBasicVisual::DisableMode(mask);
	for (int i=0; i<chields.size(); i++) 
		chields[i]->DisableMode(mask);
}
