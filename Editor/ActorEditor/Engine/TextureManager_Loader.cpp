#include "stdafx.h"
#pragma hdrstop

#include "fs.h"
#include "blenders\blender.h"


void	CShaderManager::OnDeviceDestroy(BOOL bKeepTextures) 
{
	DWORD it;

	if (Device.bReady) return;

	//************************************************************************************
	// Shaders
	for (it=0; it!=shaders.size(); it++)	
	{
		Shader& S = *(shaders[it]);
		if (0!=S.dwReference)	{
			STextureList*		T = S.Passes.front().T;
			if (T)	Device.Fatal	("Shader still referenced. Texture: %s",DBG_GetTextureName(T->front()));
			else	Device.Fatal	("Shader still referenced.");
		}
		_DELETE(shaders[it]);
	}
	shaders.clear();
	
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
			free		(t->first);
			delete		t->second;
		}
		textures.clear	();
	}
	
	// Matrices
	for (map<LPSTR,CMatrix*,str_pred>::iterator m=matrices.begin(); m!=matrices.end(); m++)
	{
		R_ASSERT	(0==m->second->dwReference);
		free		(m->first);
		delete		m->second;
	}
	matrices.clear	();
	
	// Constants
	for (map<LPSTR,CConstant*,str_pred>::iterator c=constants.begin(); c!=constants.end(); c++)
	{
		R_ASSERT	(0==c->second->dwReference);
		free		(c->first);
		delete		c->second;
	}
	constants.clear	();
	
	// Release blenders
	for (map<LPSTR,CBlender*,str_pred>::iterator b=blenders.begin(); b!=blenders.end(); b++)
	{
		free	(b->first);
		delete	b->second;
	}
	blenders.clear	();
}

void	CShaderManager::OnDeviceCreate	(LPCSTR shName)
{
	if (!Device.bReady) return;
	cache.Invalidate	();

#ifdef _EDITOR
	AnsiString sh			= shName;
    FS.m_GameRoot.Update    (sh);
	if (!FS.Exist(sh.c_str())) return;
	CCompressedStream		FS(sh.c_str(),	"shENGINE");
#else
	CCompressedStream		FS(shName,		"shENGINE");
#endif
	char					name	[256];
	
	// Load constants
	{
		CStream*	fs		= FS.OpenChunk(0);
		while (fs&&!fs->Eof())	{
			fs->RstringZ	(name);
			CConstant*		C = new CConstant;
			C->Load			(fs);
			constants.insert(make_pair(strdup(name),C));
		}
		fs->Close();
	}
	
	// Load matrices
	{
		CStream*	fs		= FS.OpenChunk(1);
		while (fs&&!fs->Eof())	{
			fs->RstringZ	(name);
			CMatrix*		M = new CMatrix;
			M->Load			(fs);
			matrices.insert	(make_pair(strdup(name),M));
		}
		fs->Close();
	}

	// Load blenders
	{
		CStream*	fs		= FS.OpenChunk	(2);
		CStream*	chunk	= NULL;
		int			chunk_id= 0;
		
		while ((chunk=fs->OpenChunk(chunk_id))!=NULL)
		{
			CBlender_DESC	desc;
			chunk->Read		(&desc,sizeof(desc));
			CBlender*		B = CBlender::Create(desc.CLS);
#ifdef _EDITOR
			if (B->getDescription().version!=desc.version){
                _DELETE		(B);
                ELog.DlgMsg	(mtError,"Can't load blender '%s'. Unsupported version.",desc.cName);
                chunk->Close	();
                chunk_id++;
                continue;
            }
#else
			R_ASSERT		(B->getDescription().version == desc.version);
#endif
            chunk->Seek		(0);
            B->Load			(*chunk);

			pair<BlenderPairIt, bool> I =  blenders.insert	(make_pair(strdup(desc.cName),B));
            R_ASSERT2		(I.second,"shader.xr - found duplicate name!!!");

			chunk->Close	();
			chunk_id		+= 1;
		}
		fs->Close();
	}
}

void CShaderManager::xrStartUp()
{
}

void CShaderManager::xrShutDown()
{
}
