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
	struct texture_detail
	{
		LPCSTR T;
		LPCSTR M;
	};
public:
	DEFINE_MAP_PRED(LPSTR,CConstant*,ConstantMap,ConstantPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CMatrix*,MatrixMap,MatrixPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CBlender*,BlenderMap,BlenderPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CTexture*,TextureMap,TexturePairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CRT*,RTMap,RTPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CVS*,VSMap,VSPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,CPS*,PSMap,PSPairIt,str_pred);
	DEFINE_MAP_PRED(LPSTR,texture_detail,TDMap,TDPairIt,str_pred);
private:
	// data
	BlenderMap						blenders;
	TextureMap						textures;
	MatrixMap						matrices;
	ConstantMap						constants;
	RTMap							rtargets;
	VSMap							vs;
	PSMap							ps;
	TDMap							td;
	
	// shader code array
	struct sh_Code {
		u32				SB;
		u32				Reference;
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
			Memory.mem_fill			(&pass,0,sizeof(pass));
			Memory.mem_fill			(surfaces,0,sizeof(surfaces));
			Memory.mem_fill			(matrices,0,sizeof(matrices));
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
	u32							_GetMemoryUsage		();
	BOOL							_GetDetailTexture	(LPCSTR Name, LPCSTR& T, LPCSTR& M);

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
	CRT*							_CreateRT			(LPCSTR Name, u32 w, u32 h);
	void							_DeleteRT			(CRT* &RT	);
	CVS*							_CreateVS			(LPCSTR Name, LPDWORD decl, u32 stride);
	CVS*							_CreateVS			(u32 FVF	);
	void							_DeleteVS			(CVS* &VS	);
	CPS*							_CreatePS			(LPCSTR Name);
	void							_DeletePS			(CPS* &PS);
	
	// Shader compiling / optimizing
	u32							_CreateCode			(SimulatorStates& Code);
	void							_DeleteCode			(u32& SB);
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
	IC void	set_Code		(u32 dwCode);
	IC void set_Textures	(STextureList* T);
	IC void set_Matrices	(SMatrixList* M);
	IC void set_Constants	(SConstantList* C, BOOL		bPS);
	IC void set_Element		(ShaderElement* S, u32	pass=0)
	{
		CPass&	P		= S->Passes[pass];
		set_Code		(P.dwStateBlock);
		set_Textures	(P.T);
		set_Matrices	(P.M);
		set_Constants	(P.C,S->Flags.bPixelShader);
	}
	IC void set_Shader		(Shader* S, u32 pass=0)
	{
		set_Element			(S->lod0,pass);
	}
	IC	CTexture*	get_ActiveTexture	(u32 stage)	
	{
		return cache.surfaces[stage];
	}
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
