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

FHierrarhyVisual::FHierrarhyVisual()  : IRender_Visual()
{
	bDontDelete	= FALSE;
}

FHierrarhyVisual::~FHierrarhyVisual()
{
	if (!bDontDelete) {
		for (u32 i=0; i<children.size(); i++)	
			::Render->model_Delete(children[i]);
	}
	children.clear();
}

void FHierrarhyVisual::Load(const char* N, IReader *data, u32 dwFlags)
{
	IRender_Visual::Load(N,data,dwFlags);
	if (data->find_chunk(OGF_CHIELDS_L)) 
	{
		// From Link
		u32 cnt = data->r_u32		();
		children.resize				(cnt);
		for (u32 i=0; i<cnt; i++)	{
#ifdef _EDITOR
			THROW;
#else
			u32 ID	= data->r_u32();
			children[i]	= ::Render->getVisual(ID);
#endif
		}
		bDontDelete = TRUE;
	}
	else
	{	
    	if (data->find_chunk(OGF_CHILDREN))
		{
			// From stream
            IReader* OBJ = data->open_chunk(OGF_CHILDREN);
            if (OBJ){
                IReader* O = OBJ->open_chunk(0);
                for (int count=1; O; count++) {
					string256			name_load;
					sprintf				(name_load,"%s_%d",N,count);
					children.push_back	(::Render->model_Create(name_load,O));
                    O->close			();
                    O = OBJ->open_chunk	(count);
                }
                OBJ->close();
            }
			bDontDelete = FALSE;
        }
		else
		{
			// From FILE
			if (data->find_chunk(OGF_CHIELDS)) {
                string32		c_drv;
                string256		c_dir;
                string256		fn,fn_full;
                _splitpath		(N,c_drv,c_dir,0,0);
                int				cnt = data->r_u32();
                children.reserve(cnt);
                for (int i=0; i<cnt; i++) 
				{
                    data->r_stringZ		(fn);
                    strconcat			(fn_full,c_drv,c_dir,fn);
                    children.push_back	(::Render->model_Create(fn_full));
                }
                bDontDelete = FALSE;
            } else {
				Debug.fatal	("Invalid visual");
            }
    	}
	}
}

void	FHierrarhyVisual::Copy(IRender_Visual *pSrc)
{
	IRender_Visual::Copy	(pSrc);

	FHierrarhyVisual	*pFrom = (FHierrarhyVisual *)pSrc;

	children.clear	();
	children.reserve(pFrom->children.size());
	for (u32 i=0; i<pFrom->children.size(); i++) {
		IRender_Visual *p = ::Render->model_Duplicate	(pFrom->children[i]);
		children.push_back(p);
	}
	bDontDelete = FALSE;
}
