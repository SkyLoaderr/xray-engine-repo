// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "shader.h"
#include "tss_def.h"
#include "vs_constants.h"

class ENGINE_API CShaderManager
{
private:
	struct str_pred : public binary_function<char*, char*, bool>
	{
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};
public:
	DEFINE_MAP_PRED(LPSTR,CConstant*,ConstantMap,ConstantPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CMatrix*,MatrixMap,MatrixPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CBlender*,BlenderMap,BlenderPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CTexture*,TextureMap,TexturePairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CRT*,RTMap,RTPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CVS*,VSMap,VSPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CPS*,PSMap,PSPairIt,str_pred);
private:
	// data
	BlenderMap						blenders;
	TextureMap						textures;
	MatrixMap						matrices;
	ConstantMap						constants;
	RTMap							rtargets;
	VSMap							vs;
	PSMap							ps;
	
	// shader code array
	struct sh_Code {
		DWORD				SB;
		DWORD				Reference;
		SimulatorStates		Code;
	};
	vector<sh_Code>					codes;

	// lists
	vector<STextureList*>			lst_textures;
	vector<SMatrixList*>			lst_matrices;
	vector<SConstantList*>			lst_constants;
	
	// main shader-array
	vector<ShaderElement*>			elements;
	vector<Shader*>					shaders;

	// cache
	struct XYU
	{
		CPass						pass;
		CTexture*					surfaces	[8];
		CMatrix*					matrices	[8];
		IDirect3DSurface8*			pRT;
		IDirect3DSurface8*			pZB;
		
		void						Invalidate	()
		{	
			ZeroMemory	(&pass,		sizeof(pass)	);
			ZeroMemory	(surfaces,	sizeof(surfaces));
			ZeroMemory	(matrices,	sizeof(matrices));
		}
	}								cache;

	// misc
	BOOL							bDeferredLoad;
public:
	CVS_Constants					VSC;
public:
	// Miscelaneous
	void							_ParseList			(sh_list& dest, LPCSTR names);
	CBlender*						_GetBlender			(LPCSTR Name);
	CBlender* 						_FindBlender		(LPCSTR Name);
	DWORD							_GetMemoryUsage		();

    BlenderMap&						_GetBlenders		(){return blenders;}
	
	// Debug
	LPCSTR							DBG_GetTextureName	(CTexture*);
	LPCSTR							DBG_GetMatrixName	(CMatrix*);
	LPCSTR							DBG_GetConstantName	(CConstant*);
	LPCSTR							DBG_GetRTName		(CRT*);
	LPCSTR							DBG_GetVSName		(CVS*);
	
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
	CRT*							_CreateRT			(LPCSTR Name, DWORD w, DWORD h);
	void							_DeleteRT			(CRT* &RT	);
	CVS*							_CreateVS			(LPCSTR Name, LPDWORD decl, DWORD stride);
	CVS*							_CreateVS			(DWORD FVF	);
	void							_DeleteVS			(CVS* &VS	);
	CPS*							_CreatePS			(LPCSTR Name);
	void							_DeletePS			(CPS* &PS);
	
	// Shader compiling / optimizing
	DWORD							_CreateCode			(SimulatorStates& Code);
	void							_DeleteCode			(DWORD& SB);
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
		cache.Invalidate	();
	}
	
	void	xrStartUp		();
	void	xrShutDown		();
	
	void	OnDeviceCreate	(CStream* F);
	void	OnDeviceCreate	(LPCSTR name);
	void	OnDeviceDestroy	(BOOL   bKeepTextures);
	void	OnFrameEnd		();
	
	// Creation/Destroying
	Shader*	Create			(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void	Delete			(Shader*	&S);
	void	DeferredLoad	(BOOL E)	{ bDeferredLoad=E;	}
	void	DeferredUpload	();
	void	DeferredUnload	();
	void	Evict			();
	
	// API
	IC void set_RT			(IDirect3DSurface8* RT, IDirect3DSurface8* ZB);
	IC void	set_Code		(DWORD dwCode);
	IC void set_Textures	(STextureList* T);
	IC void set_Matrices	(SMatrixList* M);
	IC void set_Constants	(SConstantList* C, BOOL		bPS);
	IC void set_Element		(ShaderElement* S, DWORD	pass=0)
	{
		CPass&	P		= S->Passes[pass];
		set_Code		(P.dwStateBlock);
		set_Textures	(P.T);
		set_Matrices	(P.M);
		set_Constants	(P.C,S->Flags.bPixelShader);
	}
	IC void set_Shader		(Shader* S, DWORD pass=0)
	{
		set_Element			(S->lod0,pass);
	}
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
