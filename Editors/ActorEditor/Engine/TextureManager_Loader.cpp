#include "stdafx.h"
#pragma hdrstop

#include "fs.h"
#include "blenders\blender.h"


void	CShaderManager::OnDeviceDestroy(BOOL bKeepTextures) 
{
	DWORD it;

	if (Device.bReady) return;

	cache.pRT		= 0;
	cache.pZB		= 0;
	
	//************************************************************************************
	// RTargets
	for (map<LPSTR,CRT*,str_pred>::iterator r=rtargets.begin(); r!=rtargets.end(); r++)
	{
		R_ASSERT	(0==r->second->dwReference);
		xr_free		(r->first);
		r->second->Destroy	();
		delete		r->second;
	}
	rtargets.clear	();
	
	//************************************************************************************
	// Shaders
	for (it=0; it!=shaders.size(); it++)	
	{
		Shader& S = *(shaders[it]);
		if (0!=S.dwReference)	{
			STextureList*		T	= S.lod0->Passes.front().T;
			if (T)	Device.Fatal	("Shader still referenced (%d). Texture: %s",S.dwReference,DBG_GetTextureName(T->front()));
			else	Device.Fatal	("Shader still referenced (%d).",S.dwReference);
		}
		_DELETE(shaders[it]);
	}
	shaders.clear();

	// Elements
	for (it=0; it!=elements.size(); it++)
	{
		ShaderElement& S = *(elements[it]);
		if (0!=S.dwReference)		Device.Fatal("Element still referenced.");
		_DELETE(elements[it]);
	}
	elements.clear();
	
	//************************************************************************************
	// Texture List
	for (it=0; it<lst_textures.size(); it++)	{
		if (0!=lst_textures[it]->dwReference)
			Device.Fatal("Texture list still referenced: %s",DBG_GetTextureName(lst_textures[it]->front()));
		_DELETE (lst_textures[it]);
	}
	lst_textures.clear	();
	
	// Matrix List
	for (it=0; it<lst_matrices.size(); it++)	{
		if (0!=lst_matrices[it]->dwReference)
			Device.Fatal("Matrix list still referenced: %s",DBG_GetMatrixName(lst_matrices[it]->front()));
		_DELETE (lst_matrices[it]);
	}
	lst_matrices.clear	();
	
	// Constant List
	for (it=0; it<lst_constants.size(); it++)	{
		if (0!=lst_constants[it]->dwReference)
			Device.Fatal("Constant list still referenced: %s",DBG_GetConstantName(lst_constants[it]->front()));
		_DELETE (lst_constants[it]);
	}
	lst_constants.clear	();
	
	// Codes
	for (it=0; it<codes.size(); it++)			{
		R_ASSERT(0==codes[it].Reference);
		CHK_DX	(HW.pDevice->DeleteStateBlock(codes[it].SB));
	}
	codes.clear	();
	
	//************************************************************************************
	// Textures
	if (!bKeepTextures)	{
		for (map<LPSTR,CTexture*,str_pred>::iterator t=textures.begin(); t!=textures.end(); t++)
		{
			R_ASSERT	(0==t->second->dwReference);
			xr_free		(t->first);
			delete		t->second;
		}
		textures.clear	();
	}
	
	// Matrices
	for (map<LPSTR,CMatrix*,str_pred>::iterator m=matrices.begin(); m!=matrices.end(); m++)
	{
		R_ASSERT	(0==m->second->dwReference);
		xr_free		(m->first);
		delete		m->second;
	}
	matrices.clear	();
	
	// Constants
	for (map<LPSTR,CConstant*,str_pred>::iterator c=constants.begin(); c!=constants.end(); c++)
	{
		R_ASSERT	(0==c->second->dwReference);
		xr_free		(c->first);
		delete		c->second;
	}
	constants.clear	();

	// VS
	for (map<LPSTR,CVS*,str_pred>::iterator v=vs.begin(); v!=vs.end(); v++)
	{
		R_ASSERT	(0==v->second->dwReference);
		xr_free		(v->first);
		R_CHK		(HW.pDevice->DeleteVertexShader(v->second->dwHandle));
		delete		v->second;
	}
	matrices.clear	();
	
	// Release blenders
	for (map<LPSTR,CBlender*,str_pred>::iterator b=blenders.begin(); b!=blenders.end(); b++)
	{
		xr_free	(b->first);
		delete	b->second;
	}
	blenders.clear	();
}

void	CShaderManager::OnDeviceCreate	(CStream* FS){
	if (!Device.bReady) return;
	cache.Invalidate	();
	cache.pRT			= HW.pBaseRT;
	cache.pZB			= HW.pBaseZB;

	string256	name;

	// Load constants
	{
		CStream*	fs		= FS->OpenChunk(0);
		while (fs&&!fs->Eof())	{
			fs->RstringZ	(name);
			CConstant*		C = new CConstant;
			C->Load			(fs);
			constants.insert(make_pair(xr_strdup(name),C));
		}
		fs->Close();
	}

	// Load matrices
	{
		CStream*	fs		= FS->OpenChunk(1);
		while (fs&&!fs->Eof())	{
			fs->RstringZ	(name);
			CMatrix*		M = new CMatrix;
			M->Load			(fs);
			matrices.insert	(make_pair(xr_strdup(name),M));
		}
		fs->Close();
	}

	// Load blenders
	{
		CStream*	fs		= FS->OpenChunk	(2);
		CStream*	chunk	= NULL;
		int			chunk_id= 0;

		while ((chunk=fs->OpenChunk(chunk_id))!=NULL)
		{
			CBlender_DESC	desc;
			chunk->Read		(&desc,sizeof(desc));
			CBlender*		B = CBlender::Create(desc.CLS);
			if	(B->getDescription().version != desc.version)
			{
				Msg			("! Version conflict in shader '%s'",desc.cName);
			}

            chunk->Seek		(0);
            B->Load			(*chunk,desc.version);

			pair<BlenderPairIt, bool> I =  blenders.insert	(make_pair(xr_strdup(desc.cName),B));
            R_ASSERT2		(I.second,"shader.xr - found duplicate name!!!");

			chunk->Close	();
			chunk_id		+= 1;
		}
		fs->Close();
	}
}

void	CShaderManager::OnDeviceCreate	(LPCSTR shName)
{
#ifdef _EDITOR
	if (!Engine.FS.Exist(shName)) return;
#endif
	CCompressedStream		FS(shName,		"shENGINE");
    OnDeviceCreate			(&FS);
}

void CShaderManager::xrStartUp()
{
}

void CShaderManager::xrShutDown()
{
}
