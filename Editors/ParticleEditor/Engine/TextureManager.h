// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "shader.h"
#include "tss_def.h"

class ENGINE_API CShaderManager
{
private:
	struct str_pred : public binary_function<char*, char*, bool>
	{
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};
	struct texture_detail
	{
		LPCSTR T;
		LPCSTR M;
	};
public:
	DEFINE_MAP_PRED(LPSTR,CBlender*,	map_Blender,	map_BlenderIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CTexture*,	map_Texture,	map_TextureIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CMatrix*,		map_Matrix,		map_MatrixIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CConstant*,	map_Constant,	map_ConstantIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CRT*,			map_RT,			map_RTIt,			str_pred);
	DEFINE_MAP_PRED(LPSTR,SVS*,			map_VS,			map_VSIt,			str_pred);
	DEFINE_MAP_PRED(LPSTR,SPS*,			map_PS,			map_PSIt,			str_pred);
	DEFINE_MAP_PRED(LPSTR,texture_detail,map_TD,		map_TDIt,			str_pred);
private:
	// data
	map_Blender						m_blenders;
	map_Texture						m_textures;
	map_Matrix						m_matrices;
	map_Constant					m_constants;
	map_RT							m_rtargets;
	map_VS							m_vs;
	map_PS							m_ps;
	map_TD							m_td;
	
	vector<SState*>					v_states;
	vector<SDeclaration*>			v_declarations;
	vector<SGeometry*>				v_geoms;
	vector<R_constant_table*>		v_constant_tables;

	// lists
	vector<STextureList*>			lst_textures;
	vector<SMatrixList*>			lst_matrices;
	vector<SConstantList*>			lst_constants;
	
	// main shader-array
	vector<SPass*>					v_passes;
	vector<ShaderElement*>			v_elements;
	vector<Shader*>					v_shaders;

	// misc
	BOOL							bDeferredLoad;
public:
	// Miscelaneous
	void							_ParseList			(sh_list& dest, LPCSTR names);
	CBlender*						_GetBlender			(LPCSTR Name);
	CBlender* 						_FindBlender		(LPCSTR Name);
	u32								_GetMemoryUsage		();
	BOOL							_GetDetailTexture	(LPCSTR Name, LPCSTR& T, LPCSTR& M);

    map_Blender&					_GetBlenders		()		{	return m_blenders;	}
	
	// Debug
	LPCSTR							DBG_GetTextureName	(CTexture*);
	LPCSTR							DBG_GetMatrixName	(CMatrix*);
	LPCSTR							DBG_GetConstantName	(CConstant*);
	LPCSTR							DBG_GetRTName		(CRT*);
	LPCSTR							DBG_GetVSName		(SGeometry*);
	void							DBG_VerifyGeoms		();
	
	// Editor cooperation
	void							ED_UpdateBlender	(LPCSTR Name, CBlender*		data);
	void							ED_UpdateMatrix		(LPCSTR Name, CMatrix*		data);
	void							ED_UpdateConstant	(LPCSTR Name, CConstant*	data);
	void							ED_UpdateTextures	(vector<LPSTR>* names);
	
	// Low level resource creation
	CTexture*						_CreateTexture		(LPCSTR Name);
	void							_DeleteTexture		(CTexture* &T);

	CMatrix*						_CreateMatrix		(LPCSTR Name);
	void							_DeleteMatrix		(CMatrix*  &M);

	CConstant*						_CreateConstant		(LPCSTR Name);
	void							_DeleteConstant		(CConstant* &C);

	R_constant_table*				_CreateConstantTable(R_constant_table* ps, R_constant_table* vs);

	CRT*							_CreateRT			(LPCSTR Name, u32 w, u32 h);
	void							_DeleteRT			(CRT* &RT	);

	SPS*							_CreatePS			(LPCSTR Name);
	void							_DeletePS			(IDirect3DPixelShader9* &PS);

	SVS*							_CreateVS			(LPCSTR Name);
	void							_DeleteVS			(IDirect3DVertexShader9* &PS);

	SPass*							_CreatePass			(SPass& P);
	void							_DeletePass			(SPass* &P);

	// Shader compiling / optimizing
	IDirect3DStateBlock9*			_CreateState		(SimulatorStates& Code);
	void							_DeleteState		(IDirect3DStateBlock9*& SB);

	IDirect3DVertexDeclaration9*	_CreateDecl			(D3DVERTEXELEMENT9* dcl);
	void							_DeleteDecl			(IDirect3DVertexDeclaration9*& dcl);
	
	STextureList*					_CreateTextureList	(STextureList& L);
	void							_DeleteTextureList	(STextureList* &L);
	
	SMatrixList*					_CreateMatrixList	(SMatrixList& L);
	void							_DeleteMatrixList	(SMatrixList* &L);
	
	SConstantList*					_CreateConstantList	(SConstantList& L);
	void							_DeleteConstantList	(SConstantList* &L);
	
	ShaderElement*					_CreateElement		(CBlender_Compile& C);
	void							_DeleteElement		(ShaderElement* &L);
	
	CShaderManager			()
	{
		bDeferredLoad		= FALSE;
	}
	
	void			xrStartUp			();
	void			xrShutDown			();
	
	void			OnDeviceCreate		(CStream* F);
	void			OnDeviceCreate		(LPCSTR name);
	void			OnDeviceDestroy		(BOOL   bKeepTextures);
	
	// Creation/Destroying
	Shader*			Create				(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void			Delete				(Shader*	&S);

	SGeometry*		CreateGeom			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	SGeometry*		CreateGeom			(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void			DeleteGeom			(SGeometry* &VS				);
	void			DeferredLoad		(BOOL E)					{ bDeferredLoad=E;	}
	void			DeferredUpload		();
	void			DeferredUnload		();
	void			Evict				();
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
