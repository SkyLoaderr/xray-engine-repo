// TextureManager.cpp: implementation of the CResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "ResourceManager.h"
#include "tss.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"

//--------------------------------------------------------------------------------------------------------------
template <class T>
BOOL	reclaim		(xr_vector<T*>& vec, const T* ptr)
{
	xr_vector<T*>::iterator it	= vec.begin	();
	xr_vector<T*>::iterator end	= vec.end	();
	for (; it!=end; it++)
		if (*it == ptr)	{ vec.erase	(it); return TRUE; }
		return FALSE;
}

//--------------------------------------------------------------------------------------------------------------
IBlender* CResourceManager::_GetBlender		(LPCSTR Name)
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

IBlender* CResourceManager::_FindBlender		(LPCSTR Name)
{
	if (!(Name && Name[0])) return 0;

	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,IBlender*,str_pred>::iterator I = m_blenders.find	(N);
	if (I==m_blenders.end())	return 0;
	else						return I->second;
}

void	CResourceManager::ED_UpdateBlender	(LPCSTR Name, IBlender* data)
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
void	CResourceManager::_ParseList(sh_list& dest, LPCSTR names)
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

ShaderElement* CResourceManager::_CreateElement(	CBlender_Compile& C)
{
	// Options + Shader def
	ShaderElement		S;
	C.Compile			(&S);

	// Search equal in shaders array
	for (u32 it=0; it<v_elements.size(); it++)
		if (S.equal(*(v_elements[it])))	return v_elements[it];

	// Create _new_ entry
	ShaderElement*	N		=	xr_new<ShaderElement>(S);
	N->dwFlags				|=	xr_resource::RF_REGISTERED;
	v_elements.push_back	(N);
	return N;
}

void CResourceManager::_DeleteElement(const ShaderElement* S)
{
	if (0==(S->dwFlags&xr_resource::RF_REGISTERED))	return;
	if (reclaim(v_elements,S))						return;
	Msg	("! ERROR: Failed to find compiled 'shader-element'");
}

Shader*	CResourceManager::Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
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
	Shader*		N			=	xr_new<Shader>(S);
	N->dwFlags				|=	xr_resource::RF_REGISTERED;
	v_shaders.push_back		(N);
	return N;
}

Shader*	CResourceManager::Create_B	(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
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
	Shader*		N		=	xr_new<Shader>	(S);
	N->dwFlags			|=	xr_resource::RF_REGISTERED;
	v_shaders.push_back	(N);
	return N;
}

void CResourceManager::Delete(const Shader* S)
{
	if (0==(S->dwFlags&xr_resource::RF_REGISTERED))	return;
	if (reclaim(v_shaders,S))						return;
	Msg	("! ERROR: Failed to find complete shader");
}

void	CResourceManager::DeferredUpload	()
{
	if (!Device.bReady)				return;
	for (map_TextureIt t=m_textures.begin(); t!=m_textures.end(); t++)
		t->second->Load();
}

void	CResourceManager::DeferredUnload	()
{
	if (!Device.bReady)				return;
	for (map_TextureIt t=m_textures.begin(); t!=m_textures.end(); t++)
		t->second->Unload();
}
#ifdef _EDITOR
void	CResourceManager::ED_UpdateTextures(AStringVec* names)
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
void	CResourceManager::_GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
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

void	CResourceManager::Evict()
{
	CHK_DX	(HW.pDevice->EvictManagedResources());
}

BOOL	CResourceManager::_GetDetailTexture(LPCSTR Name,LPCSTR& T, R_constant_setup* &CS)
{
	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,texture_detail,str_pred>::iterator I = m_td.find	(N);
	if (I!=m_td.end())
	{
		T	= I->second.T;
		CS	= I->second.cs;
		return TRUE;
	} else {
		return FALSE;
	}
}

void	CResourceManager::SetHLSL_path(LPCSTR path)
{
	strcpy	(HLSL_Path,path);
}
