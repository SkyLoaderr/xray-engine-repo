// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "shader.h"

class ENGINE_API CShaderManager
{
private:
	struct str_pred : public binary_function<char*, char*, bool> 
	{	
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};
	
	// data
	map<LPSTR,CShader*,str_pred>	shaders;
	map<LPSTR,CTexture*,str_pred>	textures;

	map<LPSTR,CConstant*,str_pred>	constants;
	map<LPSTR,CMatrix*,str_pred>	matrices;
	map<LPSTR,CBlender*,str_pred>	blenders;

	vector<CTextureArray*>			comb_textures;
	vector<CConstantArray*>			comb_constants;

	BOOL							bDeferredLoad;

	// cache
	enum 
	{
		c_shader	= (1<<0),
		c_textures	= (1<<1),
		c_constants	= (1<<2),
		c_matrices	= (1<<3),
		c_all		= c_shader|c_textures|c_constants|c_matrices
	};
	struct
	{
		Shader						S;
		DWORD						pass;
		CTexture*					surfaces	[8];
		CMatrix*					matrices	[8];
		DWORD						changes;
	} cache;
public:
	CBlender*						_CreateBlender	(LPCSTR Name);
	CShader*						_CreateShader	(LPCSTR Name);
	CTexture*						_CreateTexture	(LPCSTR Name);
	CConstant*						_CreateConstant (LPCSTR Name);
	CMatrix*						_CreateMatrix	(LPCSTR Name);
	DWORD							_GetMemoryUsage	();
	void							_CompileShaders ();

	DWORD							dwPassesRequired;

	CShaderManager			()
	{
		dwPassesRequired	= 0;
		bDeferredLoad		= FALSE;
		ZeroMemory			(&cache,sizeof(cache));
	}

	void	xrStartUp		();
	void	xrShutDown		();

	void	OnDeviceDestroy	(void);
	void	OnDeviceCreate	(void);

	// Creation/Destroying
	Shader	Create			(LPCSTR s_shader="null", LPCSTR s_textures = "$null", LPCSTR s_constants = "", LPCSTR s_matrices = "");
	void	Delete			(Shader	&S);
	void	DeferredLoad	(BOOL E)	{ bDeferredLoad=E;	}

	void	TexturesLoad	();
	void	TexturesUnload	();

	// API
	void	__fastcall	Set			(Shader& S);
	void	__fastcall	SetupPass	(DWORD	pass);
	void				SetNULL		() { Set(sh_list[0]); }
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
