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
IDirect3DStateBlock9*	CShaderManager::_CreateState		(SimulatorStates& state_code)
{
	// Search equal code
	for (u32 it=0; it<v_states.size(); it++)
	{
		SState&				C		= *v_states[it];;
		SimulatorStates&	base	= C.state_code;
		if (base.equal(state_code))	{
			C.dwReference	++;
			return C.state;
		}
	}

	// Create New
	v_states.push_back				(xr_new<SState>());
	v_states.back()->state			= state_code.record();
	v_states.back()->dwReference	= 1;
	v_states.back()->state_code		= state_code;
	return v_states.back()->state;
}

void		CShaderManager::_DeleteState	(IDirect3DStateBlock9*& state)
{
	R_ASSERT	(state);

	// Dummy search
	for (u32 it=0; it<v_states.size(); it++)
	{
		SState&			C		= *(v_states[it]);
		if (C.state == state)	{
			C.dwReference	--;
			return;
		}
	}

	// Fail
	Debug.fatal("Failed to find compiled shader or stateblock");
}

SPass*							CShaderManager::_CreatePass			(SPass& P)
{
	for (u32 it=0; it<v_passes.size(); it++)
	{
		if (v_passes[it]->equal(P))	return v_passes[it];
	}

	v_passes.push_back			(xr_new<SPass>(P));
	return v_passes.back();
}

void							CShaderManager::_DeletePass			(SPass* &P)
{
	P	=	NULL;
}


//--------------------------------------------------------------------------------------------------------------
static BOOL	dcl_equal			(D3DVERTEXELEMENT9* a, D3DVERTEXELEMENT9* b)
{
	// check sizes
	u32 a_size	= D3DXGetDeclLength(a);
	u32 b_size	= D3DXGetDeclLength(b);
	if (a_size!=b_size)	return FALSE;
	return 0==memcmp	(a,b,a_size*sizeof(D3DVERTEXELEMENT9));
}

IDirect3DVertexDeclaration9*	CShaderManager::_CreateDecl	(D3DVERTEXELEMENT9* dcl)
{
	// Search equal code
	for (u32 it=0; it<v_declarations.size(); it++)
	{
		SDeclaration&		D		= *v_declarations[it];;
		if (dcl_equal(dcl,D.dcl_code.begin()))
		{
			D.dwReference	++;
			return D.dcl;
		}
	}

	// Create _new
	SDeclaration* D			= xr_new<SDeclaration>();
	D->dwReference			= 1;
	u32 dcl_size			= D3DXGetDeclLength(dcl)+1;
	CHK_DX					(HW.pDevice->CreateVertexDeclaration(dcl,&D->dcl));
	D->dcl_code.assign		(dcl,dcl+dcl_size);
	v_declarations.push_back(D);
	return D->dcl;
}

void		CShaderManager::_DeleteDecl		(IDirect3DVertexDeclaration9*& dcl)
{
	R_ASSERT(dcl);

	// Dummy search
	for (u32 it=0; it<v_declarations.size(); it++)
	{
		SDeclaration&		D	= *(v_declarations[it]);
		if (D.dcl == dcl)	{
			D.dwReference	--;
			return;
		}
	}

	// Fail
	Debug.fatal("Failed to find compiled declaration");
}

//--------------------------------------------------------------------------------------------------------------
SVS*	CShaderManager::_CreateVS		(LPCSTR name)
{
	LPSTR N				= LPSTR(name);
	map_VS::iterator I	= m_vs.find	(N);
	if (I!=m_vs.end())
	{
		SVS *vs				=	I->second;
		vs->dwReference		+=	1;
		return		vs;
	}
	else
	{
		SVS*	_vs					= xr_new<SVS>	();
		m_vs.insert					(make_pair(xr_strdup(name),_vs));
		if (0==stricmp(name,"null"))	{
			_vs->dwReference	= 1;
			_vs->vs				= NULL;
			return _vs;
		}

		LPD3DXBUFFER				pShaderBuf	= NULL;
		LPD3DXBUFFER				pErrorBuf	= NULL;
		LPD3DXSHADER_CONSTANTTABLE	pConstants	= NULL;
		HRESULT						_hr			= S_OK;
		string64					cname;
		FS.update_path				(cname,	"$game_shaders$", strconcat(cname,name,".vs"));
		LPCSTR						target		= NULL;

		/*if (HW.Caps.vertex.dwVersion>=CAP_VERSION(3,0))			target="vs_3_0";
		else*/ if (HW.Caps.vertex.dwVersion>=CAP_VERSION(2,0))	target="vs_2_0";
		else 													target="vs_1_1";


		// vertex
		IReader*					fs			= FS.r_open(cname);
		_hr = D3DXCompileShader		(LPCSTR(fs->pointer()),fs->length(), NULL, NULL, "main", target, D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR, &pShaderBuf, &pErrorBuf, NULL);
		FS.r_close					(fs);

		if (SUCCEEDED(_hr))
		{
			if (pShaderBuf)
			{
				_hr = HW.pDevice->CreateVertexShader	((DWORD*)pShaderBuf->GetBufferPointer(), &_vs->vs);
				if (SUCCEEDED(_hr))	{
					LPCVOID			data		= NULL;
					_hr	= D3DXFindShaderComment	((DWORD*)pShaderBuf->GetBufferPointer(),MAKEFOURCC('C','T','A','B'),&data,NULL);
					if (SUCCEEDED(_hr) && data)
					{
						pConstants				= LPD3DXSHADER_CONSTANTTABLE(data);
						_vs->constants.parse	(pConstants,0x2);
					} else	_hr = E_FAIL;
				}
			}
			else	_hr = E_FAIL;
		}
		_RELEASE	(pShaderBuf);
		_RELEASE	(pErrorBuf);
		pConstants	= NULL;
		R_CHK		(_hr);
		return		_vs;
	}
}
void	CShaderManager::_DeleteVS			(IDirect3DVertexShader9* &vs)
{
	vs = NULL;
}

//--------------------------------------------------------------------------------------------------------------
SPS*	CShaderManager::_CreatePS			(LPCSTR name)
{
	LPSTR N				= LPSTR(name);
	map_PS::iterator I	= m_ps.find	(N);
	if (I!=m_ps.end())
	{
		SPS *ps				=	I->second;
		ps->dwReference		+=	1;
		return		ps;
	}
	else
	{
		SPS*	_ps					= xr_new<SPS>	();
		m_ps.insert					(make_pair(xr_strdup(name),_ps));
		if (0==stricmp(name,"null"))	{
			_ps->dwReference	= 1;
			_ps->ps				= NULL;
			return _ps;
		}

		// Open file
		string64					cname;
		FS.update_path				(cname,	"$game_shaders$", strconcat(cname,name,".ps"));
		IReader*					fs			= FS.r_open(cname);

		// Select target
		LPCSTR						c_target	= "ps_2_0";
		LPCSTR						c_entry		= "main";
		LPSTR						text		= LPSTR(fs->pointer());
		u32							text_size	= fs->length();
		text[text_size-1]						= 0;
		if (strstr(text,"main_ps_1_1"))			{ c_target = "ps_1_1"; c_entry = "main_ps_1_1";	}
		if (strstr(text,"main_ps_1_2"))			{ c_target = "ps_1_2"; c_entry = "main_ps_1_2";	}
		if (strstr(text,"main_ps_1_3"))			{ c_target = "ps_1_3"; c_entry = "main_ps_1_3";	}
		if (strstr(text,"main_ps_1_4"))			{ c_target = "ps_1_4"; c_entry = "main_ps_1_4";	}

		// Compile
		LPD3DXBUFFER				pShaderBuf	= NULL;
		LPD3DXBUFFER				pErrorBuf	= NULL;
		LPD3DXSHADER_CONSTANTTABLE	pConstants	= NULL;
		HRESULT						_hr			= S_OK;
		_hr = D3DXCompileShader		(text,text_size, NULL, NULL, c_entry, c_target, D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR, &pShaderBuf, &pErrorBuf, NULL);
		FS.r_close					(fs);
		if (SUCCEEDED(_hr))
		{
			if (pShaderBuf)
			{
				_hr = HW.pDevice->CreatePixelShader	((DWORD*)pShaderBuf->GetBufferPointer(), &_ps->ps);
				if (SUCCEEDED(_hr))	{
					LPCVOID			data		= NULL;
					_hr	= D3DXFindShaderComment	((DWORD*)pShaderBuf->GetBufferPointer(),MAKEFOURCC('C','T','A','B'),&data,NULL);
					if (SUCCEEDED(_hr) && data)
					{
						pConstants				= LPD3DXSHADER_CONSTANTTABLE(data);
						_ps->constants.parse	(pConstants,0x1);
					} else	_hr = E_FAIL;
				}
			}
			else	_hr = E_FAIL;
		}
		_RELEASE		(pShaderBuf);
		_RELEASE		(pErrorBuf);
		pConstants		= NULL;
		R_CHK			(_hr);
		return			_ps;
	}
}
void	CShaderManager::_DeletePS			(IDirect3DPixelShader9* &ps)
{
	ps = NULL;
}

R_constant_table*	CShaderManager::_CreateConstantTable(R_constant_table& C)
{
	if (C.empty())		return NULL;
	for (u32 it=0; it<v_constant_tables.size(); it++)
		if (v_constant_tables[it]->equal(C))	return v_constant_tables[it];
	v_constant_tables.push_back(xr_new<R_constant_table>(C));
	return v_constant_tables.back();
}

//--------------------------------------------------------------------------------------------------------------
CRT*	CShaderManager::_CreateRT		(LPCSTR Name, u32 w, u32 h,	D3DFORMAT f)
{
	R_ASSERT(Name && Name[0] && w && h);

	// ***** first pass - search already created RT
	LPSTR N = LPSTR(Name);
	map_RT::iterator I = m_rtargets.find	(N);
	if (I!=m_rtargets.end())
	{
		CRT *RT				=	I->second;
		RT->dwReference		+=	1;
		return		RT;
	}
	else
	{
		CRT *RT				=	xr_new<CRT>();
		RT->dwReference		=	1;
		m_rtargets.insert		(make_pair(xr_strdup(Name),RT));
		if (Device.bReady)	RT->Create	(Name,w,h,f);
		return				RT;
	}
}
void	CShaderManager::_DeleteRT		(CRT* &RT)
{
	if	(0==RT)		return;
	RT->dwReference	--;
	RT				= 0;
}
LPCSTR	CShaderManager::DBG_GetRTName	(CRT* T)
{
	R_ASSERT(T);
	for (map_RT::iterator I=m_rtargets.begin(); I!=m_rtargets.end(); I++)
		if (I->second == T)	return I->first;
		return 0;
}
//--------------------------------------------------------------------------------------------------------------
void	CShaderManager::DBG_VerifyGeoms	()
{
	for (u32 it=0; it<v_geoms.size(); it++)
	{
		SGeometry* G					= v_geoms[it];

		D3DVERTEXELEMENT9		test	[MAX_FVF_DECL_SIZE];
		u32						size	= 0;
		G->dcl->GetDeclaration			(test,(unsigned int*)&size);
		u32 vb_stride					= D3DXGetDeclVertexSize	(test,0);
		u32 vb_stride_cached			= G->vb_stride;
		R_ASSERT						(vb_stride == vb_stride_cached);
	}
}

SGeometry*	CShaderManager::CreateGeom	(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	DBG_VerifyGeoms						();
	R_ASSERT							(decl && vb);

	IDirect3DVertexDeclaration9* dcl 	= _CreateDecl			(decl);
	u32 vb_stride						= D3DXGetDeclVertexSize	(decl,0);

	// ***** first pass - search already loaded shader
	for (u32 it=0; it<v_geoms.size(); it++)
	{
		SGeometry& G	= *(v_geoms[it]);
		if ((G.dcl==dcl) && (G.vb==vb) && (G.ib==ib) && (G.vb_stride==vb_stride))
		{
			// match found
			G.dwReference	++;
			return v_geoms[it];
		}
	}

	SGeometry *Geom		=	xr_new<SGeometry>();
	Geom->dwReference	=	1;
	Geom->dcl			=	dcl;
	Geom->vb			=	vb;
	Geom->vb_stride		=	vb_stride;
	Geom->ib			=	ib;
	v_geoms.push_back	(Geom);
	return	Geom;
}
SGeometry*	CShaderManager::CreateGeom	(u32 FVF, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	DBG_VerifyGeoms						();
	D3DVERTEXELEMENT9	dcl	[MAX_FVF_DECL_SIZE];
	CHK_DX				(D3DXDeclaratorFromFVF(FVF,dcl));
	SGeometry* g		=  CreateGeom	(dcl,vb,ib);
	DBG_VerifyGeoms						();
	return	g;
}

void	CShaderManager::DeleteGeom		(SGeometry* &Geom)
{
	if (Geom)
	{
		Geom->dwReference	--;
		Geom = 0;
	}
}

/*
LPCSTR	CShaderManager::DBG_GetVSName(SGeometry* T)
{
	R_ASSERT(T);
	for (map_VSIt I=m_vs.begin(); I!=vs.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}

//--------------------------------------------------------------------------------------------------------------
CPS*	CShaderManager::_CreatePS		(LPCSTR cName)
{
	R_ASSERT			(cName && cName[0]);
	string256			Name;
	strlwr				(strcpy(Name,cName));
	if (strext(Name))	*strext(Name)=0;

	// ***** first pass - search already loaded shader
	LPSTR N = LPSTR(Name);
	PSMap::iterator I = ps.find	(N);
	if (I!=ps.end())
	{
		CPS *PS			=	I->second;
		PS->dwReference	+=	1;
		return		PS;
	}
	else
	{
		CPS *PS			=	xr_new<CPS>();
		PS->dwReference	=	1;
		ps.insert		(make_pair(xr_strdup(Name),PS));

		// Load vertex shader
		string256		fname;
		strconcat		(fname,Path.GameData,"shaders\\",Name,".ps");
		LPD3DXBUFFER	code	= 0;
		LPD3DXBUFFER	errors	= 0;
		IReader*		fs		= FS.r_open(fname);
		R_CHK			(D3DXAssembleShader(LPCSTR(fs->Pointer()),fs->Length(),0,NULL,&code,&errors));
		FS.r_close		(fs);
		R_CHK			(HW.pDevice->CreatePixelShader(LPDWORD(code->GetBufferPointer()),&PS->dwHandle));
		_RELEASE		(code);
		_RELEASE		(errors);

		// Return
		return			PS;
	}
}

void	CShaderManager::_DeletePS	(CPS* &PS)
{
	R_ASSERT		(PS);
	PS->dwReference	--;
	PS = 0;
}
*/

//--------------------------------------------------------------------------------------------------------------
CTexture* CShaderManager::_CreateTexture	(LPCSTR Name)
{
	R_ASSERT(Name && Name[0]);

	// ***** first pass - search already loaded texture
	LPSTR N			= LPSTR(Name);
	map_TextureIt I = m_textures.find	(N);
	if (I!=m_textures.end())
	{
		CTexture *T		=	I->second;
		T->dwReference	+=	1;
		return		T;
	}
	else
	{
		CTexture *T		= xr_new<CTexture>();
		T->dwReference	= 1;
		m_textures.insert	(make_pair(xr_strdup(Name),T));
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
	for (map<LPSTR,CTexture*,str_pred>::iterator I=m_textures.begin(); I!=m_textures.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
//--------------------------------------------------------------------------------------------------------------
CMatrix*	CShaderManager::_CreateMatrix	(LPCSTR Name)
{
	R_ASSERT(Name && Name[0]);
	if (0==stricmp(Name,"$null"))	return NULL;

	LPSTR N = LPSTR(Name);
	map<LPSTR,CMatrix*,str_pred>::iterator I = m_matrices.find	(N);
	if (I!=m_matrices.end())
	{
		CMatrix* M		=	I->second;
		M->dwReference	+=	1;
		return	M;
	}
	else
	{
		CMatrix* M		=	xr_new<CMatrix>();
		M->dwReference	=	1;
		m_matrices.insert	(make_pair(xr_strdup(Name),M));
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
	for (map<LPSTR,CMatrix*,str_pred>::iterator I=m_matrices.begin(); I!=m_matrices.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
void	CShaderManager::ED_UpdateMatrix		(LPCSTR Name, CMatrix* data)
{
	CMatrix*	M	= _CreateMatrix	(Name);
	u32		ref = M->dwReference;
	*M				= *data;
	M->dwReference	= ref-1;
}
//--------------------------------------------------------------------------------------------------------------
CConstant*	CShaderManager::_CreateConstant	(LPCSTR Name)
{
	R_ASSERT(Name && Name[0]);
	if (0==stricmp(Name,"$null"))	return NULL;

	LPSTR N = LPSTR(Name);
	map<LPSTR,CConstant*,str_pred>::iterator I = m_constants.find	(N);
	if (I!=m_constants.end())
	{
		CConstant* C	=	I->second;
		C->dwReference	+=	1;
		return	C;
	}
	else
	{
		CConstant* C	=	xr_new<CConstant>();
		C->dwReference	=	1;
		m_constants.insert	(make_pair(xr_strdup(Name),C));
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
	for (map<LPSTR,CConstant*,str_pred>::iterator I=m_constants.begin(); I!=m_constants.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
void	CShaderManager::ED_UpdateConstant	(LPCSTR Name, CConstant* data)
{
	CConstant*	C	= _CreateConstant	(Name);
	u32		ref = C->dwReference;
	*C				= *data;
	C->dwReference	= ref-1;
}
//--------------------------------------------------------------------------------------------------------------
CBlender* CShaderManager::_GetBlender		(LPCSTR Name)
{
	R_ASSERT(Name && Name[0]);

	LPSTR N = LPSTR(Name);
	map<LPSTR,CBlender*,str_pred>::iterator I = m_blenders.find	(N);
#ifdef _EDITOR
	if (I==m_blenders.end())	return 0;
#else
	if (I==m_blenders.end())	{ Debug.fatal("Shader '%s' not found in library.",Name); return 0; }
#endif
	else					return I->second;
}

CBlender* CShaderManager::_FindBlender		(LPCSTR Name)
{
	if (!(Name && Name[0])) return 0;

	LPSTR N = LPSTR(Name);
	map<LPSTR,CBlender*,str_pred>::iterator I = m_blenders.find	(N);
	if (I==m_blenders.end())	return 0;
	else						return I->second;
}

void	CShaderManager::ED_UpdateBlender	(LPCSTR Name, CBlender* data)
{
	LPSTR N = LPSTR(Name);
	map<LPSTR,CBlender*,str_pred>::iterator I = m_blenders.find	(N);
	if (I!=m_blenders.end())	{
		R_ASSERT	(data->getDescription().CLS == I->second->getDescription().CLS);
		xr_delete	(I->second);
		I->second	= data;
	} else {
		m_blenders.insert	(make_pair(xr_strdup(Name),data));
	}
}

//--------------------------------------------------------------------------------------------------------------
STextureList*	CShaderManager::_CreateTextureList(STextureList& L)
{
	for (u32 it=0; it<lst_textures.size(); it++)
	{
		STextureList*	base		= lst_textures[it];
		if (L.equal(*base))			{
			base->dwReference	++;
			return base;
		}
	}
	STextureList*	lst		= xr_new<STextureList>(L);
	lst->dwReference		= 1;
	lst_textures.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteTextureList(STextureList* &L)
{
	for (u32 it=0; it<L->size(); it++)	{ CTexture* T = (*L)[it]; _DeleteTexture(T); };
	L->dwReference	--;
}
//--------------------------------------------------------------------------------------------------------------
SMatrixList*	CShaderManager::_CreateMatrixList(SMatrixList& L)
{
	BOOL bEmpty = TRUE;
	for (u32 i=0; i<L.size(); i++)	if (L[i]) { bEmpty=FALSE; break; }
	if (bEmpty)	return NULL;

	for (u32 it=0; it<lst_matrices.size(); it++)
	{
		SMatrixList*	base		= lst_matrices[it];
		if (L.equal(*base))			{
			base->dwReference	++;
			return base;
		}
	}
	SMatrixList*	lst		= xr_new<SMatrixList>(L);
	lst->dwReference		= 1;
	lst_matrices.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteMatrixList (	SMatrixList* &L )
{
	if (0==L)	return;
	for (u32 it=0; it<L->size(); it++)	{ CMatrix* M = (*L)[it]; _DeleteMatrix (M); };
	L->dwReference	--;
}
//--------------------------------------------------------------------------------------------------------------
SConstantList*	CShaderManager::_CreateConstantList(SConstantList& L)
{
	BOOL bEmpty = TRUE;
	for (u32 i=0; i<L.size(); i++)	if (L[i]) { bEmpty=FALSE; break; }
	if (bEmpty)	return NULL;

	for (u32 it=0; it<lst_constants.size(); it++)
	{
		SConstantList*	base		= lst_constants[it];
		if (L.equal(*base))			{
			base->dwReference	++;
			return base;
		}
	}
	SConstantList*	lst		= xr_new<SConstantList>(L);
	lst->dwReference		= 1;
	lst_constants.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteConstantList(SConstantList* &L )
{
	if (0==L)	return;
	for (u32 it=0; it<L->size(); it++)	{ CConstant* C = (*L)[it]; _DeleteConstant (C); };
	L->dwReference	--;
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
	{
		if (S.equal(*(v_elements[it])))	{
			v_elements[it]->dwReference	++;
			return v_elements[it];
		}
	}

	// Create _new_ entry
	ShaderElement*	N	= xr_new<ShaderElement>(S);
	N->dwReference		= 1;
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

	// Search equal in shaders array
	for (u32 it=0; it<v_shaders.size(); it++)
	{
		if (S.equal(v_shaders[it]))	{
			v_shaders[it]->dwReference	++;
			return v_shaders[it];
		}
	}

	// Create _new_ entry
	Shader*		N		= xr_new<Shader>(S);
	N->dwReference		= 1;
	v_shaders.push_back	(N);
	return N;
}

Shader*	CShaderManager::Create_B	(CBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
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

	// Search equal in shaders array
	for (u32 it=0; it<v_shaders.size(); it++)
	{
		if (S.equal(v_shaders[it]))	{
			v_shaders[it]->dwReference	++;
			return v_shaders[it];
		}
	}

	// Create _new_ entry
	Shader*		N		= xr_new<Shader>(S);
	N->dwReference		= 1;
	v_shaders.push_back	(N);
	return N;
}

void CShaderManager::_DeleteElement(ShaderElement* &S)
{
	if (0==S)	return;

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
	S->dwReference	--;
}

void CShaderManager::Delete(Shader* &S)
{
	if (0==S)	return;

	_DeleteElement	(S->E[2]);
	_DeleteElement	(S->E[1]);
	_DeleteElement	(S->E[0]);
	S->dwReference	--;
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
            map<LPSTR,CTexture*,str_pred>::iterator I = m_textures.find	((*names)[nid].c_str());
            if (I!=m_textures.end())	I->second->Unload();
        }
    }else{
		for (map<LPSTR,CTexture*,str_pred>::iterator t=m_textures.begin(); t!=m_textures.end(); t++)
			t->second->Unload();
    }

	// 2. Load
	DeferredUpload	();
}
#endif
void	CShaderManager::_GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
	m_base=c_base=m_lmaps=c_lmaps=0;

	map<LPSTR,CTexture*,str_pred>::iterator I = m_textures.begin	();
	map<LPSTR,CTexture*,str_pred>::iterator E = m_textures.end		();
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
	CHK_DX(HW.pDevice->EvictManagedResources());
}

BOOL	CShaderManager::_GetDetailTexture(LPCSTR Name,LPCSTR& T, LPCSTR& M)
{
	LPSTR N = LPSTR(Name);
	map<LPSTR,texture_detail,str_pred>::iterator I = m_td.find	(N);
	if (I!=m_td.end())
	{
		T = I->second.T;
		M = I->second.M;
		return TRUE;
	} else {
		return FALSE;
	}
}
