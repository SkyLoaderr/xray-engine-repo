// TextureManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "TextureManager.h"
#include "tss.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"

//--------------------------------------------------------------------------------------------------------------
IBlender* CShaderManager::_GetBlender		(LPCSTR Name)
{
	R_ASSERT(Name && Name[0]);

	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,IBlender*,str_pred>::iterator I = m_blenders.find	(N);
#ifdef _EDITOR
	if (I==m_blenders.end())	return 0;
#else
	if (I==m_blenders.end())	{ Debug.fatal("Shader '%s' not found in library.",Name); return 0; }
#endif
	else					return I->second;
}

IBlender* CShaderManager::_FindBlender		(LPCSTR Name)
{
	if (!(Name && Name[0])) return 0;

	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,IBlender*,str_pred>::iterator I = m_blenders.find	(N);
	if (I==m_blenders.end())	return 0;
	else						return I->second;
}

void	CShaderManager::ED_UpdateBlender	(LPCSTR Name, IBlender* data)
{
	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,IBlender*,str_pred>::iterator I = m_blenders.find	(N);
	if (I!=m_blenders.end())	{
		R_ASSERT	(data->getDescription().CLS == I->second->getDescription().CLS);
		xr_delete	(I->second);
		I->second	= data;
	} else {
		m_blenders.insert	(mk_pair(xr_strdup(Name),data));
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void	CShaderManager::_ParseList(sh_list& dest, LPCSTR names)
{
    if (0==names) 		names 	= "$null";

	ZeroMemory(&dest, sizeof(dest));
	char*	P = (char*) names;
	svector<char,64>	N;

	while (*P)
	{
		if (*P == ',') {
			// flush
			N.push_back(0);
			strcpy(dest.last(),N.begin());
			strlwr(dest.last());
            if (strext(dest.last())) *strext(dest.last())=0;
			dest.inc();
			N.clear();
		} else {
			N.push_back(*P);
		}
		P++;
	}
	if (N.size())
	{
		// flush
		N.push_back(0);
		strcpy(dest.last(),N.begin());
		strlwr(dest.last());
		if (strext(dest.last())) *strext(dest.last())=0;
		dest.inc();
	}
}

ShaderElement* CShaderManager::_CreateElement(	CBlender_Compile& C)
{
	// Options + Shader def
	ShaderElement		S;
	C.Compile			(&S);

	// Search equal in shaders array
	for (u32 it=0; it<v_elements.size(); it++)
		if (S.equal(*(v_elements[it])))	return v_elements[it];

	// Create _new_ entry
	ShaderElement*	N		= xr_new<ShaderElement>(S);
	v_elements.push_back	(N);
	return N;
}

Shader*	CShaderManager::Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	CBlender_Compile	C;
	Shader				S;

	// Access to template
	C.BT				= _GetBlender	(s_shader?s_shader:"null");
	C.bEditor			= FALSE;
	C.bDetail			= FALSE;
#ifdef _EDITOR
    if (!C.BT)			{ ELog.Msg(mtError,"Can't find shader '%s'",s_shader); return 0; }
	C.bEditor			= TRUE;
#endif

	// Parse names
	_ParseList			(C.L_textures,	s_textures	);
	_ParseList			(C.L_constants,	s_constants	);
	_ParseList			(C.L_matrices,	s_matrices	);

	// Compile element
	C.iElement			= 0;
	C.bDetail			= TRUE;
	S.E[0]				= _CreateElement	(C);

	// Compile element
	C.iElement			= 1;
	C.bDetail			= FALSE;
	S.E[1]				= _CreateElement	(C);

	// Compile element
	C.iElement			= 2;
	C.bDetail			= FALSE;
	S.E[2]				= _CreateElement	(C);

	// Compile element
	C.iElement			= 3;
	C.bDetail			= FALSE;
	S.E[3]				= _CreateElement	(C);

	// Search equal in shaders array
	for (u32 it=0; it<v_shaders.size(); it++)
		if (S.equal(v_shaders[it]))	return v_shaders[it];

	// Create _new_ entry
	Shader*		N		= xr_new<Shader>(S);
	v_shaders.push_back	(N);
	return N;
}

Shader*	CShaderManager::Create_B	(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	CBlender_Compile	C;
	Shader				S;

	// Access to template
	C.BT				= B;
	C.bEditor			= FALSE;
	C.bDetail			= FALSE;
#ifdef _EDITOR
	if (!C.BT)			{ ELog.Msg(mtError,"Can't find shader '%s'",s_shader); return 0; }
	C.bEditor			= TRUE;
#endif

	// Parse names
	_ParseList			(C.L_textures,	s_textures	);
	_ParseList			(C.L_constants,	s_constants	);
	_ParseList			(C.L_matrices,	s_matrices	);

	// Compile element
	C.iElement			= 0;
	C.bDetail			= TRUE;
	S.E[0]				= _CreateElement	(C);

	// Compile element
	C.iElement			= 1;
	C.bDetail			= FALSE;
	S.E[1]				= _CreateElement	(C);

	// Compile element
	C.iElement			= 2;
	C.bDetail			= FALSE;
	S.E[2]				= _CreateElement	(C);

	// Compile element
	C.iElement			= 3;
	C.bDetail			= FALSE;
	S.E[3]				= _CreateElement	(C);

	// Search equal in shaders array
	for (u32 it=0; it<v_shaders.size(); it++)
		if (S.equal(v_shaders[it]))	return v_shaders[it];

	// Create _new_ entry
	Shader*		N		= xr_new<Shader>	(S);
	v_shaders.push_back	(N);
	return N;
}

void CShaderManager::_DeleteElement(ShaderElement* &S)
{
	if (0==S)				return;

	for (u32 p=0; p<S->Passes.size(); p++)
	{
		SPass& P			= *S->Passes[p];
		_DeleteState		(P.state);
		_DeletePS			(P.ps);
		_DeleteVS			(P.vs);
		_DeleteTextureList	(P.T);
		_DeleteMatrixList	(P.M);
		_DeleteConstantList	(P.C);
	}
}

void CShaderManager::Delete(Shader* &S)
{
	if (0==S)		return;

	_DeleteElement	(S->E[2]);
	_DeleteElement	(S->E[1]);
	_DeleteElement	(S->E[0]);
	S				= 0;
}

void	CShaderManager::DeferredUpload	()
{
	if (!Device.bReady)				return;
	for (map_TextureIt t=m_textures.begin(); t!=m_textures.end(); t++)
		t->second->Load(t->first);
}

void	CShaderManager::DeferredUnload	()
{
	if (!Device.bReady)				return;
	for (map_TextureIt t=m_textures.begin(); t!=m_textures.end(); t++)
		t->second->Unload();
}
#ifdef _EDITOR
void	CShaderManager::ED_UpdateTextures(AStringVec* names)
{
	// 1. Unload
    if (names){
        for (u32 nid=0; nid<names->size(); nid++)
        {
            xr_map<LPSTR,CTexture*,str_pred>::iterator I = m_textures.find	((*names)[nid].c_str());
            if (I!=m_textures.end())	I->second->Unload();
        }
    }else{
		for (xr_map<LPSTR,CTexture*,str_pred>::iterator t=m_textures.begin(); t!=m_textures.end(); t++)
			t->second->Unload();
    }

	// 2. Load
	DeferredUpload	();
}
#endif
void	CShaderManager::_GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
	m_base=c_base=m_lmaps=c_lmaps=0;

	xr_map<LPSTR,CTexture*,str_pred>::iterator I = m_textures.begin	();
	xr_map<LPSTR,CTexture*,str_pred>::iterator E = m_textures.end		();
	for (; I!=E; I++)
	{
		u32 m = I->second->dwMemoryUsage;
		if (strstr(I->first,"lmap"))
		{
			c_lmaps	++;
			m_lmaps	+= m;
		} else {
			c_base	++;
			m_base	+= m;
		}
	}
}

void	CShaderManager::Evict()
{
	CHK_DX	(HW.pDevice->EvictManagedResources());
}

BOOL	CShaderManager::_GetDetailTexture(LPCSTR Name,LPCSTR& T, LPCSTR& M)
{
	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,texture_detail,str_pred>::iterator I = m_td.find	(N);
	if (I!=m_td.end())
	{
		T = I->second.T;
		M = I->second.M;
		return TRUE;
	} else {
		return FALSE;
	}
}
