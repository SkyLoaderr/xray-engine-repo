// FHierrarhyVisual.cpp: implementation of the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FHierrarhyVisual.h"
#include "fstaticrender.h"
#include "fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FHierrarhyVisual::FHierrarhyVisual()  : FBasicVisual()
{
}

FHierrarhyVisual::~FHierrarhyVisual()
{
	if (!bDontDelete) {
		for (DWORD i=0; i<chields.size(); i++)	_DELETE(chields[i]);
	}
	chields.clear();
}

void FHierrarhyVisual::Load(const char* N, CStream *data, DWORD dwFlags)
{
	FBasicVisual::Load(N,data,dwFlags);
	if (data->FindChunk(OGF_CHIELDS_L)) {
		DWORD cnt = data->Rdword();
		chields.resize(cnt);
		for (DWORD i=0; i<cnt; i++) {
			DWORD ID	= data->Rdword();
			chields[i]	= ::Render.Visuals[ID];
		}
		bDontDelete = TRUE;
	} else {
		if (data->FindChunk(OGF_CHIELDS)) {
			FILE_NAME	fn;
			int			cnt = data->Rdword();
			chields.reserve(cnt);
			for (int i=0; i<cnt; i++) {
				data->RstringZ(fn);
				chields.push_back(::Render.Models.Create(fn));
			}
			bDontDelete = FALSE;
		} else {
			THROW;
		}
	}
}

void	FHierrarhyVisual::Copy(FBasicVisual *pSrc)
{
	FBasicVisual::Copy	(pSrc);

	FHierrarhyVisual	*pFrom = (FHierrarhyVisual *)pSrc;

	chields.clear	();
	chields.reserve	(pFrom->chields.size());
	for (DWORD i=0; i<pFrom->chields.size(); i++) {
		FBasicVisual *p = ::Render.Models.Instance_Duplicate(pFrom->chields[i]);
		chields.push_back(p);
	}
	bDontDelete = FALSE;
}
