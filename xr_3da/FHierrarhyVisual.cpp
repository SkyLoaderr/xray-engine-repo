// FHierrarhyVisual.cpp: implementation of the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "FHierrarhyVisual.h"
#include "fmesh.h"
#ifndef _EDITOR
 #include "render.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FHierrarhyVisual::FHierrarhyVisual()  : CVisual()
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
	CVisual::Load(N,data,dwFlags);
	if (data->FindChunk(OGF_CHIELDS_L)) {
		DWORD cnt = data->Rdword();
		chields.resize(cnt);
		for (DWORD i=0; i<cnt; i++) {
			DWORD ID	= data->Rdword();
#ifdef _EDITOR
			THROW;
#else
			chields[i]	= ::Render->getVisual(ID);
#endif
		}
		bDontDelete = TRUE;
	}else{
    	if (data->FindChunk(OGF_CHILDREN)){
            CStream* OBJ = data->OpenChunk(OGF_CHILDREN);
            if (OBJ){
                CStream* O = OBJ->OpenChunk(0);
                for (int count=1; O; count++) {
#ifdef _EDITOR
                    chields.push_back	(::Device.Models.Create(O));
#else
					chields.push_back	(::Render->model_Create(O));
#endif
                    O->Close();
                    O = OBJ->OpenChunk(count);
                }
                OBJ->Close();
            }
			bDontDelete = FALSE;
        }else{
			if (data->FindChunk(OGF_CHIELDS)) {
                string32	c_drv;
                string256	c_dir;
                string256	fn,fn_full;
                _splitpath	(N,c_drv,c_dir,0,0);
                int			cnt = data->Rdword();
                chields.reserve(cnt);
                for (int i=0; i<cnt; i++) {
                    data->RstringZ		(fn);
                    strconcat			(fn_full,c_drv,c_dir,fn);
#ifdef _EDITOR
                    chields.push_back	(::Device.Models.Create(fn_full));
#else
                    chields.push_back	(::Render->model_Create(fn_full));
#endif
                }
                bDontDelete = FALSE;
            } else {
                THROW;
            }
    	}
	}
}

void	FHierrarhyVisual::Copy(CVisual *pSrc)
{
	CVisual::Copy	(pSrc);

	FHierrarhyVisual	*pFrom = (FHierrarhyVisual *)pSrc;

	chields.clear	();
	chields.reserve	(pFrom->chields.size());
	for (DWORD i=0; i<pFrom->chields.size(); i++) {
#ifdef _EDITOR
		CVisual *p = ::Device.Models.Instance_Duplicate(pFrom->chields[i]);
#else
		CVisual *p = ::Render->model_Duplicate	(pFrom->chields[i]);
#endif
		chields.push_back(p);
	}
	bDontDelete = FALSE;
}
