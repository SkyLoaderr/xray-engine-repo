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
	// data
	vector<CTexture *>				textures;
	vector<CShader*>				shaders;
	vector<CConstant*>				constants;

	vector<CTextureArray*>			comb_textures;
	vector<CConstantArray*>			comb_constants;

	BOOL							bDeferredLoad;

	// cache
	CShader*						cache_shader;
	CTexture*						cache_surfaces	[8];
	CTextureArray*					cache_textures;
	CConstantArray*					cache_constants;
	DWORD							cache_pass;
	
public:
	CTexture*						_CreateTexture	(LPCSTR Name);
	CShader*						_CreateShader	(LPCSTR Name);
	CConstant*						_CreateConstant (LPCSTR Name);
	DWORD							_GetMemoryUsage	();

	DWORD					dwPassesRequired;

	CShaderManager			()
	{
		dwPassesRequired	= 0;
		bDeferredLoad		= FALSE;
		current_shader		= 0;
		current_SBH			= 0;
		ZeroMemory			(current_surf,sizeof(current_surf));
	}

	void	xrStartUp		();
	void	xrShutDown		();

	void	OnDeviceDestroy	(void);
	void	OnDeviceCreate	(void);

	// Creation/Destroying
	Shader	Create			(LPCSTR sh_name="null", DWORD lights, LPCSTR t_name = "$null");
	void	Delete			(Shader &S);
	void	DeferredLoad	(BOOL E)	{ bDeferredLoad=E;	}

	void	TexturesLoad	();
	void	TexturesUnload	();

	// Info
	int		GetShaderCount	(void)	{ return sh_list.size(); }

	// API
	void	__fastcall	Set			(Shader& S);
	void	__fastcall	SetupPass	(DWORD	pass);
	void				SetNULL		() { Set(sh_list[0]); }
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
