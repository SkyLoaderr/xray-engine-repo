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
	bDontDelete	= FALSE;
}

FHierrarhyVisual::~FHierrarhyVisual()
{
	if (!bDontDelete) {
		for (u32 i=0; i<children.size(); i++)	
#ifdef _EDITOR
			::Device.Models.Delete(children[i]);
#else
			::Render->model_Delete(children[i]);
#endif
	}
	children.clear();
}

void FHierrarhyVisual::Load(const char* N, CStream *data, u32 dwFlags)
{
	CVisual::Load(N,data,dwFlags);
	if (data->FindChunk(OGF_CHIELDS_L)) 
	{
		// From Link
		u32 cnt = data->Rdword();
		children.resize(cnt);
		for (u32 i=0; i<cnt; i++) {
#ifdef _EDITOR
			THROW;
#else
			u32 ID	= data->Rdword();
			children[i]	= ::Render->getVisual(ID);
#endif
		}
		bDontDelete = TRUE;

		// Correct BBs
		{
			bv_BBox.invalidate		();
			for (u32 i=0; i<cnt; i++) 
			{
				bv_BBox.merge	(children[i]->bv_BBox);
			}
			bv_Position.sub	(bv_BBox.max,bv_BBox.min);
			bv_Position.div	(2);
			bv_Radius		= bv_Position.magnitude();
			bv_Position.add	(bv_BBox.min);
			bv_BBox.grow	(EPS_S);
		}
	}
	else
	{	
    	if (data->FindChunk(OGF_CHILDREN))
		{
			// From stream
            CStream* OBJ = data->OpenChunk(OGF_CHILDREN);
            if (OBJ){
                CStream* O = OBJ->OpenChunk(0);
                for (int count=1; O; count++) {
#ifdef _EDITOR
                    children.push_back	(::Device.Models.Create(O));
#else
					string256 name_load;
					sprintf				(name_load,"%s_%d",N,count);
					children.push_back	(::Render->model_Create(name_load,O));
#endif
                    O->Close();
                    O = OBJ->OpenChunk(count);
                }
                OBJ->Close();
            }
			bDontDelete = FALSE;
        }
		else
		{
			// From FILE
			if (data->FindChunk(OGF_CHIELDS)) {
                string32	c_drv;
                string256	c_dir;
                string256	fn,fn_full;
                _splitpath	(N,c_drv,c_dir,0,0);
                int			cnt = data->Rdword();
                children.reserve(cnt);
                for (int i=0; i<cnt; i++) 
				{
                    data->RstringZ		(fn);
                    strconcat			(fn_full,c_drv,c_dir,fn);
#ifdef _EDITOR
                    children.push_back	(::Device.Models.Create(fn_full));
#else
                    children.push_back	(::Render->model_Create(fn_full));
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

	children.clear	();
	children.reserve(pFrom->children.size());
	for (u32 i=0; i<pFrom->children.size(); i++) {
#ifdef _EDITOR
		CVisual *p = ::Device.Models.Instance_Duplicate(pFrom->children[i]);
#else
		CVisual *p = ::Render->model_Duplicate	(pFrom->children[i]);
#endif
		children.push_back(p);
	}
	bDontDelete = FALSE;
}
