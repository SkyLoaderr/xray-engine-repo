// TextureManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureManager.h"
#include "tss.h"

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
			SB = 0;
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
	map<LPSTR,CTexture*,str_pred>::iterator I = textures.find	(Name);
	if (I!=textures.end())	
	{
		CTexture *T		=	I->second;
		T->dwRefCount	+=	1;
		return		T;
	}
	else 
	{
		CTexture *T		= new CTexture;
		T->dwRefCount	= 1;
		textures.insert	(make_pair(strdup(Name),T));
		if (Device.bReady && !bDeferredLoad) T->Load(Name);
		return		T;
	}
}
void	CShaderManager::_DeleteTexture		(CTexture* &T)
{
	R_ASSERT(T);
	T->dwRefCount	--;
	T=0;
}
//--------------------------------------------------------------------------------------------------------------
CMatrix*	CShaderManager::_CreateMatrix	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	map<LPSTR,CMatrix*,str_pred>::iterator I = matrices.find	(Name);
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
	R_ASSERT(M);
	M->dwReference	--;
	M=0;
}
//--------------------------------------------------------------------------------------------------------------
CConstant*	CShaderManager::_CreateConstant	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	map<LPSTR,CConstant*,str_pred>::iterator I = constants.find	(Name);
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
	R_ASSERT(C);
	C->dwReference	--;
	C=0;
}

//--------------------------------------------------------------------------------------------------------------
CBlender* CShaderManager::_GetBlender		(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	map<LPSTR,CBlender*,str_pred>::iterator I = blenders.find	(Name);
	if (I==blenders.end())	Device.Fatal("Shader '%s' not found in library.",Name);
	else					return I->second;
}
//--------------------------------------------------------------------------------------------------------------
STextureList*	CShaderManager::_CreateTextureList(STextureList& L)
{
	for (DWORD it=0; it<lst_textures.size(); it++)
	{
		STextureList*	base		= lst_textures[it];
		if (L.equal(*base))			return base;
	}

	lst_textures.push_back	(new STextureList(L.begin(),L.size()));
	return lst_textures.back();
}
void			CShaderManager::_DeleteTextureList(STextureList* &L)
{
	for (DWORD it=0; it<L->size(); it++)
		_DeleteTexture	((*L)[it]);
	L->clear ();
	L = 0;
}
//--------------------------------------------------------------------------------------------------------------
SMatrixList*	CShaderManager::_CreateMatrixList(SMatrixList& L)
{
	for (DWORD it=0; it<lst_matrices.size(); it++)
	{
		SMatrixList*	base		= lst_matrices[it];
		if (L.equal(*base))			return base;
	}
	lst_matrices.push_back	(new SMatrixList(L.begin(),L.size()));
	return lst_matrices.back();
}
void			CShaderManager::_DeleteMatrixList (	SMatrixList* &L )
{
	for (DWORD it=0; it<L->size(); it++)
		_DeleteMatrix ((*L)[it]);
	L->clear();
	L = 0;
}
//--------------------------------------------------------------------------------------------------------------
SConstantList*	CShaderManager::_CreateConstantList(SConstantList& L)
{
	for (DWORD it=0; it<lst_constants.size(); it++)
	{
		SConstantList*	base		= lst_constants[it];
		if (L.equal(*base))			return base;
	}
	lst_constants.push_back	(new SConstantList(L.begin(),L.size()));
	return lst_constants.back();
}
void			CShaderManager::_DeleteConstantList(SConstantList* &L )
{
	for (DWORD it=0; it<L->size(); it++)
		_DeleteConstant ((*L)[it]);
	L->clear();
	L = 0;
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
		dest.inc();
	}
}

Shader	CShaderManager::Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
	// Parse names
	sh_list				L_textures,L_constants,L_matrices;
	_ParseList			(L_textures,s_textures	);
	_ParseList			(L_constants,s_constants);
	_ParseList			(L_matrices,s_matrices	);

	// Compile shader
	Shader		S;
	CBlender*	B		= _GetBlender	(s_shader);
	CBlender_Recorder	Recorder		(&S);
	B->Compile			(Recorder);

	// ok
	return S;
}

void CShaderManager::Delete(Shader &S) 
{
	R_ASSERT(S);
	for (DWORD p=0; p<S.Passes.size(); p++)
	{
		CPass& P = S.Passes[p];
		_DeleteCode			(P.dwStateBlock);
		_DeleteTextureList	(P.T);
		_DeleteMatrixList	(P.M);
		_DeleteConstantList	(P.C);
	}
	ZeroMemory(&S,sizeof(S));
}

void	CShaderManager::OnDeviceDestroy(void) 
{
	DWORD it;

	if (Device.bReady) return;

	// Delete Texture List
	for (it=0; it<lst_textures.size(); it++)	

	lst_textures.clear	();
	lst_matrices.clear	();
	lst_constants.clear	();

	// Dele
}

void	CShaderManager::OnDeviceCreate(void) 
{
	if (!Device.bReady) return;

	cache.Invalidate();

}

void CShaderManager::TexturesLoad	()
{
	// load textures
	for (DWORD i=0; i<textures.size(); i++) 
		textures[i]->Load();
}

void CShaderManager::TexturesUnload	()
{
	// unload textures
	for (DWORD i=0; i<textures.size(); i++) 
	{
		textures[i]->Unload();
		if (textures[i]->dwRefCount == 0) {
			_DELETE(textures[i]);
			textures.erase(textures.begin()+i);
			i--;
		}
	}
}
