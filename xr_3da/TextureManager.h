// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "xrShaderTypes.h"

class ENGINE_API CShaderManager
{
private:
	map<LPSTR,CShader*>		shaders;	// data
	vector<CTexture *>		textures;	// data

	vector<Shader *>		sh_list;
	Shader*					current_shader;
	DWORD					current_SBH;
	CTexture*				current_surf	[sh_STAGE_MAX];

	BOOL					bDeferredLoad;
public:
	CTexture*				_CreateTexture	(const char* Name, BOOL bMipmaps=TRUE);
	CShader*				_CreateShader	(const char* Name);
	DWORD					_GetMemoryUsage	();

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
	Shader	Create			(const char *sh_name="null", const char *t_name = "$null", BOOL bNeedMipmaps=true);
	void	Delete			(Shader &S);
	void	DeferredLoad	(BOOL E)	{ bDeferredLoad=E;	}

	void	TexturesLoad	();
	void	TexturesUnload	();

	// Info
	int		GetShaderCount	(void)	{ return sh_list.size(); }

	// API
	void	__fastcall	Set			(Shader* S);
	void	__fastcall	SetupPass	(DWORD	pass);
	void				SetNULL		() { Set(sh_list[0]); }
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
