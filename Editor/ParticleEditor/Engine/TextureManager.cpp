// TextureManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "TextureManager.h"
#include "tss.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"

//--------------------------------------------------------------------------------------------------------------
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
CRT* CShaderManager::_CreateRT		(LPCSTR Name, DWORD w, DWORD h) 
{
	R_ASSERT(Name && Name[0] && w && h);
	
	// ***** first pass - search already created RT
	LPSTR N = LPSTR(Name);
	RTMap::iterator I = rtargets.find	(N);
	if (I!=rtargets.end())	
	{
		CRT *RT			=	I->second;
		RT->dwReference	+=	1;
		return		RT;
	}
	else 
	{
		CRT *RT			=	new CRT;
		RT->dwReference	=	1;
		rtargets.insert	(make_pair(strdup(Name),RT));
		if (Device.bReady)	RT->Create	(Name,w,h);
		return		RT;
	}
}
void	CShaderManager::_DeleteRT		(CRT* &RT)
{
	R_ASSERT		(RT);
	RT->dwReference	--;
	RT				= 0;
}
LPCSTR	CShaderManager::DBG_GetRTName	(CRT* T)
{
	R_ASSERT(T);
	for (RTMap::iterator I=rtargets.begin(); I!=rtargets.end(); I++)
		if (I->second == T)	return I->first;
		return 0;
}
//--------------------------------------------------------------------------------------------------------------
CVS*	CShaderManager::_CreateVS		(LPCSTR cName, LPDWORD decl) 
{
	R_ASSERT			(cName && cName[0] && decl);
	string256			Name;
	strlwr				(strcpy(Name,cName));
	if (strext(Name))	*strext(Name)=0;
	
	// ***** first pass - search already loaded texture
	LPSTR N = LPSTR(Name);
	VSMap::iterator I = vs.find	(N);
	if (I!=vs.end())	
	{
		CVS *VS			=	I->second;
		VS->dwReference	+=	1;
		return		VS;
	}
	else 
	{
		CVS *VS			=	new CVS;
		VS->dwReference	=	1;
		vs.insert		(make_pair(strdup(Name),VS));
		
		// Load vertex shader
		string256		fname;
		strconcat		(fname,"data\\shaders\\",Name,".vs");
		LPD3DXBUFFER	code	= 0;
		LPD3DXBUFFER	errors	= 0;
		CStream*		fs		= Engine.FS.Open(fname);
		R_CHK			(D3DXAssembleShader(LPCSTR(fs->Pointer()),fs->Length(),0,NULL,&code,&errors));
		Engine.FS.Close	(fs);
		R_CHK			(HW.pDevice->CreateVertexShader(decl,LPDWORD(code->GetBufferPointer()),&VS->dwHandle,0));
		_RELEASE		(code);
		_RELEASE		(errors);
		
		// Return
		return			VS;
	}
}
void	CShaderManager::_DeleteVS	(CVS* &VS)
{
	R_ASSERT		(VS);
	VS->dwReference	--;
	VS = 0;
}
LPCSTR	CShaderManager::DBG_GetVSName(CVS* T)
{
	R_ASSERT(T);
	for (VSMap::iterator I=vs.begin(); I!=vs.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
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

	// Options + Shader def
	Shader				S;
	CBlender_Compile	C;
	C.Initialize		(&S);
	C.bEditor			= FALSE;
	C.bLighting			= TRUE;
	C.iLayers			= 1;
	
	// Parse names
	_ParseList			(C.L_textures,	s_textures	);
	_ParseList			(C.L_constants,	s_constants	);
	_ParseList			(C.L_matrices,	s_matrices	);

	// Compile shader
	CBlender*	B		= _GetBlender	(s_shader);
#ifdef M_BORLAND
    if (!B)				{ ELog.Msg(mtError,"Can't find shader '%s'",s_shader); return 0; }
	C.bEditor			= TRUE;
#endif
	B->Compile			(C);
	
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

void	CShaderManager::DeferredUpload	()
{
	if (!Device.bReady)				return;
	for (map<LPSTR,CTexture*,str_pred>::iterator t=textures.begin(); t!=textures.end(); t++)
		t->second->Load(t->first);
}

void	CShaderManager::DeferredUnload	()
{
	if (!Device.bReady)				return;
	for (map<LPSTR,CTexture*,str_pred>::iterator t=textures.begin(); t!=textures.end(); t++)
		t->second->Unload();
}

void	CShaderManager::ED_UpdateTextures(vector<LPSTR>& names)
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

DWORD	CShaderManager::_GetMemoryUsage()
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

void	CShaderManager::Evict()
{
	CHK_DX(HW.pDevice->ResourceManagerDiscardBytes(0));
}
