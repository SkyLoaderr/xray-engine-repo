// TextureManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "TextureManager.h"
#include "tss.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"

DWORD		CShaderManager::_CreateCode		(SimulatorStates& code)
{
	// Search equal code
	for (DWORD it=0; it<codes.size(); it++)
	{
		sh_Code&			C		= codes[it];;
		SimulatorStates&	base	= C.Code;
		if (base.equal(code))	{
			C.Reference	+= 1;
			return C.SB;
		}
	}

	// Create New
	codes.push_back			(sh_Code());
	codes.back().SB			= code.record();
	codes.back().Reference	= 1;
	codes.back().Code		= code;
	return codes.back().SB;
}

void		CShaderManager::_DeleteCode		(DWORD& SB)
{
	R_ASSERT(SB);
	// Dummy search
	for (DWORD it=0; it<codes.size(); it++)
	{
		sh_Code&			C		= codes[it];;
		if (C.SB == SB)	{
			C.Reference	--;
			return;
		}
	}

	// Fail
	Device.Fatal("Failed to find compiled shader or stateblock");
}

//--------------------------------------------------------------------------------------------------------------
CTexture* CShaderManager::_CreateTexture	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	// ***** first pass - search already loaded texture
	LPSTR N = LPSTR(Name);
	map<LPSTR,CTexture*,str_pred>::iterator I = textures.find	(N);
	if (I!=textures.end())	
	{
		CTexture *T		=	I->second;
		T->dwReference	+=	1;
		return		T;
	}
	else 
	{
		CTexture *T		= new CTexture;
		T->dwReference	= 1;
		textures.insert	(make_pair(strdup(Name),T));
		if (Device.bReady && !bDeferredLoad) T->Load(Name);
		return		T;
	}
}
void	CShaderManager::_DeleteTexture		(CTexture* &T)
{
	R_ASSERT(T);
	T->dwReference	--;
	T=0;
}
LPCSTR	CShaderManager::DBG_GetTextureName	(CTexture* T)
{
	R_ASSERT(T);
	for (map<LPSTR,CTexture*,str_pred>::iterator I=textures.begin(); I!=textures.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
//--------------------------------------------------------------------------------------------------------------
CMatrix*	CShaderManager::_CreateMatrix	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);
	if (0==stricmp(Name,"$null"))	return NULL;

	LPSTR N = LPSTR(Name);
	map<LPSTR,CMatrix*,str_pred>::iterator I = matrices.find	(N);
	if (I!=matrices.end())
	{
		CMatrix* M		=	I->second;
		M->dwReference	+=	1;
		return	M;
	}
	else
	{
		CMatrix* M		=	new CMatrix;
		M->dwReference	=	1;
		matrices.insert	(make_pair(strdup(Name),M));
		return	M;
	}
}
void	CShaderManager::_DeleteMatrix		(CMatrix* &M)
{
	if (0==M)	return;
	M->dwReference	--;
	M=0;
}
LPCSTR	CShaderManager::DBG_GetMatrixName	(CMatrix* T)
{
	R_ASSERT(T);
	for (map<LPSTR,CMatrix*,str_pred>::iterator I=matrices.begin(); I!=matrices.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
void	CShaderManager::ED_UpdateMatrix		(LPCSTR Name, CMatrix* data)
{
	CMatrix*	M	= _CreateMatrix	(Name);
	DWORD		ref = M->dwReference;
	*M				= *data;
	M->dwReference	= ref-1;
}
//--------------------------------------------------------------------------------------------------------------
CConstant*	CShaderManager::_CreateConstant	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);
	if (0==stricmp(Name,"$null"))	return NULL;
	
	LPSTR N = LPSTR(Name);
	map<LPSTR,CConstant*,str_pred>::iterator I = constants.find	(N);
	if (I!=constants.end())	
	{
		CConstant* C	=	I->second;
		C->dwReference	+=	1;
		return	C;
	}
	else
	{
		CConstant* C	=	new CConstant;
		C->dwReference	=	1;
		constants.insert	(make_pair(strdup(Name),C));
		return	C;
	}
}
void	CShaderManager::_DeleteConstant		(CConstant* &C)
{
	if (0==C)	return;
	C->dwReference	--;
	C=0;
}
LPCSTR	CShaderManager::DBG_GetConstantName	(CConstant* T)
{
	R_ASSERT(T);
	for (map<LPSTR,CConstant*,str_pred>::iterator I=constants.begin(); I!=constants.end(); I++)
		if (I->second == T)	return I->first;
		return 0;
}
void	CShaderManager::ED_UpdateConstant	(LPCSTR Name, CConstant* data)
{
	CConstant*	C	= _CreateConstant	(Name);
	DWORD		ref = C->dwReference;
	*C				= *data;
	C->dwReference	= ref-1;
}
//--------------------------------------------------------------------------------------------------------------
CBlender* CShaderManager::_GetBlender		(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);
	
	LPSTR N = LPSTR(Name);
	map<LPSTR,CBlender*,str_pred>::iterator I = blenders.find	(N);
#ifdef _EDITOR
	if (I==blenders.end())	return 0;
#else
	if (I==blenders.end())	Device.Fatal("Shader '%s' not found in library.",Name);
#endif
	else					return I->second;
}

CBlender* CShaderManager::_FindBlender		(LPCSTR Name)
{
	if (!(Name && Name[0])) return 0;

	LPSTR N = LPSTR(Name);
	map<LPSTR,CBlender*,str_pred>::iterator I = blenders.find	(N);
	if (I==blenders.end())	return 0;
	else					return I->second;
}

void	CShaderManager::ED_UpdateBlender	(LPCSTR Name, CBlender* data)
{
	LPSTR N = LPSTR(Name);
	map<LPSTR,CBlender*,str_pred>::iterator I = blenders.find	(N);
	if (I!=blenders.end())	{
		R_ASSERT	(data->getDescription().CLS == I->second->getDescription().CLS);
		_DELETE		(I->second);
		I->second	= data;
	} else {
		blenders.insert	(make_pair(strdup(Name),data));
	}
}

//--------------------------------------------------------------------------------------------------------------
STextureList*	CShaderManager::_CreateTextureList(STextureList& L)
{
	for (DWORD it=0; it<lst_textures.size(); it++)
	{
		STextureList*	base		= lst_textures[it];
		if (L.equal(*base))			{
			base->dwReference	++;
			return base;
		}
	}
	STextureList*	lst		= new STextureList(L);
	lst->dwReference		= 1;
	lst_textures.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteTextureList(STextureList* &L)
{
	for (DWORD it=0; it<L->size(); it++)	{ CTexture* T = (*L)[it]; _DeleteTexture(T); };
	L->dwReference	--;
}
//--------------------------------------------------------------------------------------------------------------
SMatrixList*	CShaderManager::_CreateMatrixList(SMatrixList& L)
{
	BOOL bEmpty = TRUE;
	for (DWORD i=0; i<L.size(); i++)	if (L[i]) { bEmpty=FALSE; break; }
	if (bEmpty)	return NULL;
	
	for (DWORD it=0; it<lst_matrices.size(); it++)
	{
		SMatrixList*	base		= lst_matrices[it];
		if (L.equal(*base))			{
			base->dwReference	++;
			return base;
		}
	}
	SMatrixList*	lst		= new SMatrixList(L);
	lst->dwReference		= 1;
	lst_matrices.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteMatrixList (	SMatrixList* &L )
{
	if (0==L)	return;
	for (DWORD it=0; it<L->size(); it++)	{ CMatrix* M = (*L)[it]; _DeleteMatrix (M); };
	L->dwReference	--;
}
//--------------------------------------------------------------------------------------------------------------
SConstantList*	CShaderManager::_CreateConstantList(SConstantList& L)
{
	BOOL bEmpty = TRUE;
	for (DWORD i=0; i<L.size(); i++)	if (L[i]) { bEmpty=FALSE; break; }
	if (bEmpty)	return NULL;

	for (DWORD it=0; it<lst_constants.size(); it++)
	{
		SConstantList*	base		= lst_constants[it];
		if (L.equal(*base))			{
			base->dwReference	++;
			return base;
		}
	}
	SConstantList*	lst		= new SConstantList(L);
	lst->dwReference		= 1;
	lst_constants.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteConstantList(SConstantList* &L )
{
	if (0==L)	return;
	for (DWORD it=0; it<L->size(); it++)	{ CConstant* C = (*L)[it]; _DeleteConstant (C); };
	L->dwReference	--;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void	CShaderManager::_ParseList(sh_list& dest, LPCSTR names)
{
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

Shader*	CShaderManager::Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	if (0==s_shader) 	s_shader	= "null";
    if (0==s_textures) 	s_textures 	= "$null";
    if (0==s_constants) s_constants = "$null";
    if (0==s_matrices) 	s_matrices 	= "$null";
	
//	Msg("--- %s --- %s",s_shader,s_textures);

	// Parse names
	sh_list				L_textures,L_constants,L_matrices;
	_ParseList			(L_textures,s_textures	);
	_ParseList			(L_constants,s_constants);
	_ParseList			(L_matrices,s_matrices	);

	// Compile shader
	Shader		S;
	CBlender*	B		= _GetBlender	(s_shader);
	CBlender_Recorder	Recorder		(&S);
#ifdef M_BORLAND
    if (!B){
    	ELog.Msg(mtError,"Can't find shader '%s'",s_shader);
    	return 0;
    }
	B->Compile			(Recorder, L_textures, L_constants, L_matrices,0,TRUE);
#else
	B->Compile			(Recorder, L_textures, L_constants, L_matrices,0,FALSE);
#endif
	
	// Search equal in shaders array
	for (DWORD it=0; it<shaders.size(); it++) 
	{
		if (S.equal(*(shaders[it])))	{
			shaders[it]->dwReference	++;
			return shaders[it];
		}
	}
	
	// Create new entry
	Shader*		N		= new Shader(S);
	N->dwReference		= 1;
	shaders.push_back	(N);
	return N;
}

void CShaderManager::Delete(Shader* &S) 
{
	if (0==S)	return;

	for (DWORD p=0; p<S->Passes.size(); p++)
	{
		CPass& P = S->Passes[p];
		_DeleteCode			(P.dwStateBlock);
		_DeleteTextureList	(P.T);
		_DeleteMatrixList	(P.M);
		_DeleteConstantList	(P.C);
	}
	S->dwReference	--;
	S				= 0;
}

void	CShaderManager::OnFrameEnd	()
{
	for (DWORD stage=0; stage<HW.Caps.pixel.dwStages; stage++)
		CHK_DX(HW.pDevice->SetTexture(0,0));
	cache.Invalidate	();
}

void CShaderManager::DeferredUpload	()
{
	if (!Device.bReady)				return;
	for (map<LPSTR,CTexture*,str_pred>::iterator t=textures.begin(); t!=textures.end(); t++)
		t->second->Load(t->first);
}

void CShaderManager::DeferredUnload	()
{
	if (!Device.bReady)				return;
	for (map<LPSTR,CTexture*,str_pred>::iterator t=textures.begin(); t!=textures.end(); t++)
		t->second->Unload();
}

void CShaderManager::ED_UpdateTextures(vector<LPSTR>& names)
{
	// 1. Unload
	for (DWORD nid=0; nid<names.size(); nid++)
	{
		map<LPSTR,CTexture*,str_pred>::iterator I = textures.find	(names[nid]);
		if (I!=textures.end())	I->second->Unload();
	}

	// 2. Load
	DeferredUpload	();
}

DWORD CShaderManager::_GetMemoryUsage()
{
	DWORD mem = 0;
	map<LPSTR,CTexture*,str_pred>::iterator I = textures.begin	();
	map<LPSTR,CTexture*,str_pred>::iterator E = textures.end	();
	for (; I!=E; I++)
	{
		mem += I->second->dwMemoryUsage;
	}
	return mem;
}
