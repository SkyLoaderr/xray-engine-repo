#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "blenders\blender.h"

// eye-params
float					r__dtex_range	= 50;
class cl_dt_scaler		: public R_constant_setup {
public:
	float				scale;

	cl_dt_scaler		(float s) : scale(s)	{};
	virtual void setup	(R_constant* C)
	{
		RCache.set_c	(C,scale,scale,scale,1/r__dtex_range);
	}
};

void	CResourceManager::OnDeviceDestroy(BOOL bKeepTextures)
{
	if (Device.bReady)	return;
	xr_delete			(m_description);

	// Matrices
	for (map_Matrix::iterator m=m_matrices.begin(); m!=m_matrices.end(); m++)	{
		R_ASSERT		(1==m->second->dwReference);
		xr_delete		(m->second);
	}
	m_matrices.clear	();
    
	// Constants
	for (map_Constant::iterator c=m_constants.begin(); c!=m_constants.end(); c++)
	{
		R_ASSERT		(1==c->second->dwReference);
		xr_delete		(c->second);
	}
	m_constants.clear	();

   	// Release blenders
	for (map_BlenderIt b=m_blenders.begin(); b!=m_blenders.end(); b++)
	{
		xr_free				((char*&)b->first);
		IBlender::Destroy	(b->second);
	}
	m_blenders.clear	();

	// destroy TD
	for (map_TDIt _t=m_td.begin(); _t!=m_td.end(); _t++)
	{
		xr_free		((char*&)_t->first);
		xr_free		((char*&)_t->second.T);
		xr_delete	(_t->second.cs);
	}
	m_td.clear		();

	// scripting
#ifndef _EDITOR
	LS_Unload				();
#endif
}

void	CResourceManager::OnDeviceCreate	(IReader* F)
{
	if (!Device.bReady) return;

	string256	name;

#ifndef _EDITOR
	// scripting
	LS_Load					();
#endif
	// Load constants
	{
		IReader*	fs		= F->open_chunk	(0);
		while (fs && !fs->eof())	{
			fs->r_stringZ	(name,sizeof(name));
			CConstant*	C	= _CreateConstant	(name);
			C->Load			(fs);
		}
		fs->close();
	}

	// Load matrices
	{
		IReader*	fs		= F->open_chunk(1);
		while (fs&&!fs->eof())	{
			fs->r_stringZ	(name,sizeof(name));
			CMatrix*	M	= _CreateMatrix	(name);
			M->Load			(fs);
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
			IBlender*		B = IBlender::Create(desc.CLS);
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
	string256		fname;		
	FS.update_path	(fname,"$game_textures$","textures.ltx");
	LPCSTR	Iname	= fname;
	if (FS.exist(Iname))
	{
		xr_delete		(m_description);
		m_description	= xr_new<CInifile>	(Iname);
		CInifile&	ini	= *m_description;
		if (ini.section_exist("association")){
			CInifile::Sect& 	data = ini.r_section("association");
			for (CInifile::SectIt I=data.begin(); I!=data.end(); I++)	{
				texture_detail			D;
				string256				T;
				float					s;

				CInifile::Item& item	= *I;
				sscanf					(*item.second,"%[^,],%f",T,&s);

				//
				D.T				= xr_strdup				(T);
				D.cs			= xr_new<cl_dt_scaler>	(s);
				LPSTR N			= xr_strdup				(*item.first);
				m_td.insert		(mk_pair(N,D));
			}
		}
	}
}

void	CResourceManager::OnDeviceCreate	(LPCSTR shName)
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
	FS.r_close				(F);
}
