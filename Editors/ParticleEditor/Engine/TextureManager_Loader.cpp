#include "stdafx.h"
#pragma hdrstop

#include "blenders\blender.h"


void	CShaderManager::OnDeviceDestroy(BOOL bKeepTextures)
{
	u32 it;

	if (Device.bReady) return;

	//************************************************************************************
	// RTargets
	for (xr_map<LPSTR,CRT*,str_pred>::iterator r=m_rtargets.begin(); r!=m_rtargets.end(); r++)
	{
		R_ASSERT	(0==r->second->dwReference);
		xr_free		((char*)r->first);
		r->second->Destroy	();
		xr_delete   (r->second);
	}
	m_rtargets.clear	();

	//************************************************************************************
	// Shaders
	for (it=0; it!=v_shaders.size(); it++)
	{
		Shader& S = *(v_shaders[it]);
		if (0!=S.dwReference)	{
			/*
			STextureList*		T	= S.lod0->Passes.front()->T;
			if (T)	Debug.fatal	("Shader still referenced (%d). Texture: %s",S.dwReference,DBG_GetTextureName(T->front()));
			else	Debug.fatal	("Shader still referenced (%d).",S.dwReference);
			*/
		}
		xr_delete(v_shaders[it]);
	}
	v_shaders.clear();

	// Elements
	for (it=0; it!=v_elements.size(); it++)
	{
		// ShaderElement& S = *(v_elements[it]);
		// if (0!=S.dwReference)	Debug.fatal("Element still referenced.");
		xr_delete(v_elements[it]);
	}
	v_elements.clear();

	//************************************************************************************
	// Texture List
	for (it=0; it<lst_textures.size(); it++)	{
//		if (0!=lst_textures[it]->dwReference)
//			Debug.fatal("Texture list still referenced: %s",DBG_GetTextureName(lst_textures[it]->front()));
		xr_delete (lst_textures[it]);
	}
	lst_textures.clear	();

	// Matrix List
	for (it=0; it<lst_matrices.size(); it++)	{
//		if (0!=lst_matrices[it]->dwReference)
//			Debug.fatal("Matrix list still referenced: %s",DBG_GetMatrixName(lst_matrices[it]->front()));
		xr_delete (lst_matrices[it]);
	}
	lst_matrices.clear	();

	// Constant List
	for (it=0; it<lst_constants.size(); it++)	{
//		if (0!=lst_constants[it]->dwReference)
//			Debug.fatal("Constant list still referenced: %s",DBG_GetConstantName(lst_constants[it]->front()));
		xr_delete (lst_constants[it]);
	}
	lst_constants.clear	();

	// Codes
	for (it=0; it<v_states.size(); it++)			{
//		R_ASSERT	(0==v_states[it]->dwReference);
		_RELEASE	(v_states[it]->state);
		xr_delete	(v_states[it]);
	}
	v_states.clear	();

	// Decls
	for (it=0; it<v_declarations.size(); it++)			{
		// R_ASSERT	(0==v_declarations[it]->dwReference);
		_RELEASE	(v_declarations[it]->dcl);
		xr_delete	(v_declarations[it]);
	}
	v_declarations.clear	();

	//************************************************************************************
	// Textures
	if (!bKeepTextures)	{
		for (xr_map<LPSTR,CTexture*,str_pred>::iterator t=m_textures.begin(); t!=m_textures.end(); t++)
		{
//			R_ASSERT	(0==t->second->dwReference);
			xr_free		((char*)t->first);
			xr_delete	(t->second);
		}
		m_textures.clear	();
	}

	// Matrices
	for (xr_map<LPSTR,CMatrix*,str_pred>::iterator m=m_matrices.begin(); m!=m_matrices.end(); m++)
	{
//		if (m->second->dwMode!=CMatrix::modeDetail)	R_ASSERT(0==m->second->dwReference);
		xr_free		((char*)m->first);
		xr_delete	(m->second);
	}
	m_matrices.clear	();

	// Constants
	for (xr_map<LPSTR,CConstant*,str_pred>::iterator c=m_constants.begin(); c!=m_constants.end(); c++)
	{
//		R_ASSERT	(0==c->second->dwReference);
		xr_free		((char*)c->first);
		xr_delete	(c->second);
	}
	m_constants.clear	();

	// VS
    {
        for (map_VSIt v=m_vs.begin(); v!=m_vs.end(); v++)
        {
    //		if (0!=v->second->dwReference)
    //			Msg("! WARNING: Vertex shader still referenced [%d]: '%s'",v->second->dwReference,v->first);
            xr_free		((char*)v->first);
            xr_delete	(v->second);
        }
        m_vs.clear	();
    }

	// PS
    {
        for (map_PSIt v=m_ps.begin(); v!=m_ps.end(); v++)
        {
    //		if (0!=v->second->dwReference)
    //			Msg("! WARNING: Pixel shader still referenced [%d]: '%s'",v->second->dwReference,v->first);
            xr_free		((char*)v->first);
            xr_delete	(v->second);
        }
        m_ps.clear	();
    }

	// Release blenders
	for (map_BlenderIt b=m_blenders.begin(); b!=m_blenders.end(); b++)
	{
		xr_free				((char*)b->first);
		CBlender::Destroy	(b->second);
	}
	m_blenders.clear	();

	// destroy TD
	for (map_TDIt _t=m_td.begin(); _t!=m_td.end(); _t++)
	{
		xr_free((char*)_t->first);
		xr_free((char*&)_t->second.T);
		xr_free((char*&)_t->second.M);
	}
	m_td.clear			();

    // clear Geoms
	for (u32 g_it=0; g_it<v_geoms.size(); g_it++)
	{
		xr_delete(v_geoms[g_it]);
    }
    v_geoms.clear();
}

void	CShaderManager::OnDeviceCreate	(IReader* F)
{
	if (!Device.bReady) return;

	string256	name;

	// Load constants
	{
		IReader*	fs		= F->open_chunk	(0);
		while (fs && !fs->eof())	{
			fs->r_stringZ	(name);
			CConstant*		C = xr_new<CConstant>();
			C->Load			(fs);
			m_constants.insert(mk_pair(xr_strdup(name),C));
		}
		fs->close();
	}

	// Load matrices
	{
		IReader*	fs		= F->open_chunk(1);
		while (fs&&!fs->eof())	{
			fs->r_stringZ	(name);
			CMatrix*		M	= xr_new<CMatrix>();
			M->Load				(fs);
			m_matrices.insert	(mk_pair(xr_strdup(name),M));
		}
		fs->close();
	}

	// Load blenders
	{
		IReader*	fs		= F->open_chunk	(2);
		IReader*	chunk	= NULL;
		int			chunk_id= 0;

		while ((chunk=fs->open_chunk(chunk_id))!=NULL)
		{
			CBlender_DESC	desc;
			chunk->r		(&desc,sizeof(desc));
			CBlender*		B = CBlender::Create(desc.CLS);
			if	(0==B)
			{
				Msg				("! Renderer doesn't support blender '%s'",desc.cName);
			}
			else
			{
				if	(B->getDescription().version != desc.version)
				{
					Msg			("! Version conflict in shader '%s'",desc.cName);
				}

				chunk->seek		(0);
				B->Load			(*chunk,desc.version);

				std::pair<map_BlenderIt, bool> I =  m_blenders.insert	(mk_pair(xr_strdup(desc.cName),B));
				R_ASSERT2		(I.second,"shader.xr - found duplicate name!!!");
			}
			chunk->close	();
			chunk_id		+= 1;
		}
		fs->close();
	}

	// Load detail textures association
	string256 fname;		
	FS.update_path	(fname,"$game_textures$","textures.ltx");
	LPCSTR	Iname	= fname;
	if (FS.exist(Iname))
	{
		CInifile	ini		(Iname);
        if (ini.section_exist("association")){
            CInifile::Sect& 	data = ini.r_section("association");
            for (CInifile::SectIt I=data.begin(); I!=data.end(); I++)
            {
                texture_detail	D;
                string256		T,M;
                float			s;

                CInifile::Item& item		= *I;
                sscanf			(item.second,"%[^,],%f",T,&s);

                // Search or create matrix
                M[0]			= 0;
                for (map_MatrixIt m=m_matrices.begin(); m!=m_matrices.end(); m++)
                {
                    if (CMatrix::modeDetail == m->second->dwMode)
                    {
                        if (fsimilar(m->second->xform._11,s)) {
                            // ok
                            strcpy(M,m->first);
                            break;
                        }
                    }
                }
                if (0==M[0])	{
                    strconcat		(M,"$user$td$",T);
                    CMatrix* _M		= _CreateMatrix	(M);
                    _M->dwMode		= CMatrix::modeDetail;
                    _M->xform.scale	(s,s,s);
                }

                //
                D.T				= xr_strdup	(T);
                D.M				= xr_strdup	(M);
                LPSTR N			= xr_strdup	(item.first);
                m_td.insert		(mk_pair(N,D));
            }
        }
	}
}

void	CShaderManager::OnDeviceCreate	(LPCSTR shName)
{
#ifdef _EDITOR
	if (!FS.exist(shName)) return;
#endif

	// Check if file is compressed already
	string32	ID			= "shENGINE";
	string32	id;
	IReader*	F			= FS.r_open(shName);
	F->r		(&id,8);
	if (0==strncmp(id,ID,8))
	{
		Debug.fatal			("Unsupported blender library. Compressed?");
	}
	OnDeviceCreate			(F);
    FS.r_close	(F);
}
