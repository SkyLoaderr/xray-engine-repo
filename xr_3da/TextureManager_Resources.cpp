#include "stdafx.h"

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "TextureManager.h"
#include "tss.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"

//--------------------------------------------------------------------------------------------------------------
SState*		CShaderManager::_CreateState		(SimulatorStates& state_code)
{
	// Search equal state-code 
	for (u32 it=0; it<v_states.size(); it++)
	{
		SState*				C		= v_states[it];;
		SimulatorStates&	base	= C->state_code;
		if (base.equal(state_code))	return C;
	}

	// Create New
	v_states.push_back				(xr_new<SState>());
	v_states.back()->state			= state_code.record();
	v_states.back()->state_code		= state_code;
	return v_states.back();
}

void		CShaderManager::_DeleteState		(SState* state)
{
	// Dummy search
	for (u32 it=0; it<v_states.size(); it++)
		if (v_states[it] == state)	
		{
			xr_delete		(v_states[it]);
			v_states.erase	(v_states.begin()+it);
		}

	// Fail
	Msg	("! ERROR: Failed to find compiled shader or stateblock");
}

//--------------------------------------------------------------------------------------------------------------
SPass*		CShaderManager::_CreatePass			(SPass& P)
{
	for (u32 it=0; it<v_passes.size(); it++)
		if (v_passes[it]->equal(P))	return v_passes[it];

	v_passes.push_back			(xr_new<SPass>(P));
	return v_passes.back();
}

void		CShaderManager::_DeletePass			(SPass* &P)
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
		if (dcl_equal(dcl,&*D.dcl_code.begin()))	return D.dcl;
	}

	// Create _new
	SDeclaration* D			= xr_new<SDeclaration>();
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
		if (D.dcl == dcl)	return;
	}

	// Fail
	Debug.fatal("Failed to find compiled declaration");
}

//--------------------------------------------------------------------------------------------------------------
SVS*	CShaderManager::_CreateVS		(LPCSTR name)
{
	LPSTR N				= LPSTR(name);
	map_VS::iterator I	= m_vs.find	(N);
	if (I!=m_vs.end())	return I->second;
	else
	{
		SVS*	_vs					= xr_new<SVS>	();
		m_vs.insert					(mk_pair(xr_strdup(name),_vs));
		if (0==stricmp(name,"null"))	{
			_vs->vs				= NULL;
			return _vs;
		}

		LPD3DXBUFFER				pShaderBuf	= NULL;
		LPD3DXBUFFER				pErrorBuf	= NULL;
		LPD3DXSHADER_CONSTANTTABLE	pConstants	= NULL;
		HRESULT						_hr			= S_OK;
		string256					cname;
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
	if (I!=m_ps.end())	return		I->second;
	else
	{
		SPS*	_ps					= xr_new<SPS>	();
		m_ps.insert					(mk_pair(xr_strdup(name),_ps));
		if (0==stricmp(name,"null"))	{
			_ps->ps				= NULL;
			return _ps;
		}

		// Open file
		string256					cname;
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
		return		RT;
	}
	else
	{
		CRT *RT				=	xr_new<CRT>();
		m_rtargets.insert		(mk_pair(xr_strdup(Name),RT));
		if (Device.bReady)	RT->Create	(Name,w,h,f);
		return				RT;
	}
}
void	CShaderManager::_DeleteRT		(CRT* &RT)
{
	if	(0==RT)		return;
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
CRTC*	CShaderManager::_CreateRTC		(LPCSTR Name, u32 size,	D3DFORMAT f)
{
	R_ASSERT(Name && Name[0] && size);

	// ***** first pass - search already created RTC
	LPSTR N = LPSTR(Name);
	map_RTC::iterator I = m_rtargets_c.find	(N);
	if (I!=m_rtargets_c.end())
	{
		CRTC *RT			=	I->second;
		return		RT;
	}
	else
	{
		CRTC *RT			=	xr_new<CRTC>();
		m_rtargets_c.insert	(mk_pair(xr_strdup(Name),RT));
		if (Device.bReady)	RT->Create	(Name,size,f);
		return				RT;
	}
}
void	CShaderManager::_DeleteRTC		(CRTC* &RT)
{
	if	(0==RT)		return;
	RT				= 0;
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
	R_ASSERT							(decl && vb);

	IDirect3DVertexDeclaration9* dcl 	= _CreateDecl			(decl);
	u32 vb_stride						= D3DXGetDeclVertexSize	(decl,0);

	// ***** first pass - search already loaded shader
	for (u32 it=0; it<v_geoms.size(); it++)
	{
		SGeometry& G	= *(v_geoms[it]);
		if ((G.dcl==dcl) && (G.vb==vb) && (G.ib==ib) && (G.vb_stride==vb_stride))	return v_geoms[it];
	}

	SGeometry *Geom		=	xr_new<SGeometry>();
	Geom->dcl			=	dcl;
	Geom->vb			=	vb;
	Geom->vb_stride		=	vb_stride;
	Geom->ib			=	ib;
	v_geoms.push_back	(Geom);
	return	Geom;
}
SGeometry*	CShaderManager::CreateGeom	(u32 FVF, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	D3DVERTEXELEMENT9	dcl	[MAX_FVF_DECL_SIZE];
	CHK_DX				(D3DXDeclaratorFromFVF(FVF,dcl));
	SGeometry* g		=  CreateGeom	(dcl,vb,ib);
	return	g;
}

void	CShaderManager::DeleteGeom		(SGeometry* &Geom)
{
	if (Geom)
	{
		Geom = 0;
	}
}

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
		return		T;
	}
	else
	{
		CTexture *T		= xr_new<CTexture>();
		m_textures.insert	(mk_pair(xr_strdup(Name),T));
		if (Device.bReady && !bDeferredLoad) T->Load(Name);
		return		T;
	}
}
void	CShaderManager::_DeleteTexture		(CTexture* &T)
{
	if (0==T)		return;
	T=0;
}
LPCSTR	CShaderManager::DBG_GetTextureName	(CTexture* T)
{
	R_ASSERT(T);
	for (xr_map<LPSTR,CTexture*,str_pred>::iterator I=m_textures.begin(); I!=m_textures.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
//--------------------------------------------------------------------------------------------------------------
CMatrix*	CShaderManager::_CreateMatrix	(LPCSTR Name)
{
	R_ASSERT(Name && Name[0]);
	if (0==stricmp(Name,"$null"))	return NULL;

	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,CMatrix*,str_pred>::iterator I = m_matrices.find	(N);
	if (I!=m_matrices.end())
	{
		CMatrix* M		=	I->second;
		return	M;
	}
	else
	{
		CMatrix* M		=	xr_new<CMatrix>();
		m_matrices.insert	(mk_pair(xr_strdup(Name),M));
		return	M;
	}
}
void	CShaderManager::_DeleteMatrix		(CMatrix* &M)
{
	if (0==M)	return;
	M=0;
}
LPCSTR	CShaderManager::DBG_GetMatrixName	(CMatrix* T)
{
	R_ASSERT(T);
	for (xr_map<LPSTR,CMatrix*,str_pred>::iterator I=m_matrices.begin(); I!=m_matrices.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
void	CShaderManager::ED_UpdateMatrix		(LPCSTR Name, CMatrix* data)
{
	CMatrix*	M	= _CreateMatrix	(Name);
	*M				= *data;
}
//--------------------------------------------------------------------------------------------------------------
CConstant*	CShaderManager::_CreateConstant	(LPCSTR Name)
{
	R_ASSERT(Name && Name[0]);
	if (0==stricmp(Name,"$null"))	return NULL;

	LPSTR N = LPSTR(Name);
	xr_map<LPSTR,CConstant*,str_pred>::iterator I = m_constants.find	(N);
	if (I!=m_constants.end())
	{
		CConstant* C	=	I->second;
		return	C;
	}
	else
	{
		CConstant* C	=	xr_new<CConstant>();
		m_constants.insert	(mk_pair(xr_strdup(Name),C));
		return	C;
	}
}
void	CShaderManager::_DeleteConstant		(CConstant* &C)
{
	if (0==C)	return;
	C=0;
}
LPCSTR	CShaderManager::DBG_GetConstantName	(CConstant* T)
{
	R_ASSERT(T);
	for (xr_map<LPSTR,CConstant*,str_pred>::iterator I=m_constants.begin(); I!=m_constants.end(); I++)
		if (I->second == T)	return I->first;
	return 0;
}
void	CShaderManager::ED_UpdateConstant	(LPCSTR Name, CConstant* data)
{
	CConstant*	C	= _CreateConstant	(Name);
	*C				= *data;
}

//--------------------------------------------------------------------------------------------------------------
STextureList*	CShaderManager::_CreateTextureList(STextureList& L)
{
	for (u32 it=0; it<lst_textures.size(); it++)
	{
		STextureList*	base		= lst_textures[it];
		if (L.equal(*base))			return base;
	}
	STextureList*	lst		= xr_new<STextureList>(L);
	lst_textures.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteTextureList(STextureList* &L)
{
	for (u32 it=0; it<L->size(); it++)	{ CTexture* T = (*L)[it]; _DeleteTexture(T); };
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
		if (L.equal(*base))			return base;
	}
	SMatrixList*	lst		= xr_new<SMatrixList>(L);
	lst_matrices.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteMatrixList (	SMatrixList* &L )
{
	if (0==L)	return;
	for (u32 it=0; it<L->size(); it++)	{ CMatrix* M = (*L)[it]; _DeleteMatrix (M); };
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
		if (L.equal(*base))			return base;
	}
	SConstantList*	lst		= xr_new<SConstantList>(L);
	lst_constants.push_back	(lst);
	return lst;
}
void			CShaderManager::_DeleteConstantList(SConstantList* &L )
{
	if (0==L)	return;
	for (u32 it=0; it<L->size(); it++)	{ CConstant* C = (*L)[it]; _DeleteConstant (C); };
}

