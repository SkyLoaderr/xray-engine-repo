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
	
	// data
	map<LPSTR,CBlender*,str_pred>	blenders;
	map<LPSTR,CTexture*,str_pred>	textures;
	map<LPSTR,CMatrix*,str_pred>	matrices;
	map<LPSTR,CConstant*,str_pred>	constants;

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
	vector<Shader*>					shaders;

	// cache
	struct XXX
	{
		CPass						pass;
		CTexture*					surfaces	[8];
		CMatrix*					matrices	[8];

		void						Invalidate	()
		{	ZeroMemory(this,sizeof(*this));		}
	}								cache;

	// misc
	BOOL							bDeferredLoad;
public:
	// Miscelaneous
	void							_ParseList			(sh_list& dest, LPCSTR names);
	CBlender*						_GetBlender			(LPCSTR Name);
	DWORD							_GetMemoryUsage		();

	// Low level resource creation
	CTexture*						_CreateTexture		(LPCSTR Name);
	void							_DeleteTexture		(CTexture* &T);
	CMatrix*						_CreateMatrix		(LPCSTR Name);
	void							_DeleteMatrix		(CMatrix*  &M);
	CConstant*						_CreateConstant		(LPCSTR Name);
	void							_DeleteConstant		(CConstant* &C);

	// Shader compiling / optimizing
	DWORD							_CreateCode			(SimulatorStates& Code);
	void							_DeleteCode			(DWORD& SB);
	STextureList*					_CreateTextureList	(STextureList& L);
	void							_DeleteTextureList	(STextureList* &L);
	SMatrixList*					_CreateMatrixList	(SMatrixList& L);
	void							_DeleteMatrixList	(SMatrixList* &L);
	SConstantList*					_CreateConstantList	(SConstantList& L);
	void							_DeleteConstantList	(SConstantList* &L);

	CShaderManager			()
	{
		bDeferredLoad		= FALSE;
		cache.Invalidate	();
	}

	void	xrStartUp		();
	void	xrShutDown		();

	void	OnDeviceCreate	(void);
	void	OnDeviceDestroy	(void);
	void	OnFrameEnd		();

	// Creation/Destroying
	Shader*	Create			(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void	Delete			(Shader*	&S);
	void	DeferredLoad	(BOOL E)	{ bDeferredLoad=E;	}
	void	DeferredUpload	();

	// API
	IC void	set_Code		(DWORD dwCode);
	IC void set_Textures	(STextureList* T);
	IC void set_Matrices	(SMatrixList* M);
	IC void set_Constants	(SConstantList* C, BOOL bPS);
	IC void set_Shader		(Shader* S, DWORD pass=0)
	{
		CPass&	P		= S->Passes[pass];
		set_Code		(P.dwStateBlock);
		set_Textures	(P.T);
		set_Matrices	(P.M);
		set_Constants	(P.C,S->Flags.bPixelShader);
	}
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
