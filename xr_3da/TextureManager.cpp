// TextureManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texture.h"
#include "xrShader.h"
#include "xrShaderLib.h"
#include "TextureManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXRShader* CShaderManager::_CreateShader(const char *Name) 
{
	VERIFY(Name);
	VERIFY(Name[0]);

	for (DWORD i=0; i<shaders.size(); i++) {
		// shader present - check name matching
		if (strcmp(shaders[i]->cName,Name)==0) 
			return shaders[i];
	}
	Device.Fatal("! Shader \"%s\" not found",Name);
	return NULL;
}

CTexture* CShaderManager::_CreateTexture(const char* Name, BOOL bMipmaps) 
{
	VERIFY(Name);
	VERIFY(Name[0]);

	// ***** first pass - search already loaded texture
	for (DWORD i=0; i<textures.size(); i++) 
	{
		CTexture* T = textures[i];
		if (stricmp(T->cName,Name)==0) 
		{
			T->dwRefCount++;
			return T;
		}
	}

	// ***** real creation
	CTexture *p		= new CTexture(Name,!!bMipmaps);
	p->dwRefCount	= 1;
	if (Device.bReady && !bDeferredLoad) p->Load();

	// ***** empty slot not found - expand array
	textures.push_back(p);
	return p;
}

DWORD CShaderManager::_GetMemoryUsage	()
{
	DWORD Result = 0;
	for (DWORD i=0; i<textures.size(); i++) 
	{
		Result += textures[i]->dwMemoryUsage;
	}
	return Result;
}

Shader*	CShaderManager::Create(const char *sName, const char *tName, BOOL bNeedMipmaps) 
{
	// Parse names
	tex_names	Names;
	{
		char*	P = (char*) tName;
		svector<char,64>	N;

		while (*P)
		{
			if (*P == ',') {
				// flush
				N.push_back(0);
				strcpy(Names.last(),N.begin());
				strlwr(Names.last());
				Names.inc();
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
			strcpy(Names.last(),N.begin());
			strlwr(Names.last());
			Names.inc();
		}
	}

	// Create shader
	Shader* S	= new Shader;
	S->shader	= _CreateShader	(sName);
	S->shader->CompileTextures	(Names,S->H);

	// Search if this combination exists
	for (DWORD i=0; i<sh_list.size(); i++) {
		if (sh_list[i]->isEqual(S)) {
			delete S;
			sh_list[i]->dwRefCount++;
			return sh_list[i];
		}
	}

	// Combination not found - add it
	S->dwRefCount++;
	sh_list.push_back(S);
	return S;
}

void CShaderManager::Delete(Shader* &S) 
{
	R_ASSERT(S);

	// Decrease texture references
	tex_handles& H = S->H;
	for (DWORD pass=0; pass<H.size(); pass++)
		for (DWORD stage=0; stage<H[pass].size(); stage++)
		{
			CTexture* T = H[pass][stage];
			T->dwRefCount--;
		}

	// Real destroy
	S->dwRefCount--;
	if (S->dwRefCount==0) {
		Shader** P = find(sh_list.begin(),sh_list.end(),S);
		R_ASSERT(P!=sh_list.end());
		sh_list.erase(P);
		_DELETE(S);
	}
	S=NULL;
}

void	CShaderManager::xrStartUp	()
{
	// load library
	vector<SH_ShaderDef>	LIB;
	shLibrary_Load			("shaders.xr",LIB);
	shaders.resize			(LIB.size());
	for (DWORD i=0; i<LIB.size(); i++)
	{
		shaders[i] = new CXRShader;
		shaders[i]->CompileInit(LIB[i]);
	}

	// load "NULL" shader
	Create					();
}

void	CShaderManager::xrShutDown	()
{
	// destroy "NULL" shader
	Shader*	hNull	= Create();
	Shader*	hCopy	= hNull;
	Delete			(hNull);
	Delete			(hCopy);

	// destroy library
	for (DWORD i=0; i<shaders.size(); i++)
		_DELETE(shaders[i]);
	shaders.clear();

	// Unload "cached" textures
	TexturesUnload	();
}

void	CShaderManager::OnDeviceDestroy(void) 
{
	if (Device.bReady) return;

	// unload textures
	TexturesUnload();

	// unload shaders
	for (DWORD i=0; i<shaders.size(); i++)
		shaders[i]->Release();
}

void	CShaderManager::OnDeviceCreate(void) 
{
	if (!Device.bReady) return;

	// load shaders
	vector<SH_ShaderDef>	LIB;
	shLibrary_Load			("shaders.xr",LIB);
	R_ASSERT				(LIB.size()==shaders.size());
	for (DWORD i=0; i<shaders.size(); i++)
		shaders[i]->Compile(LIB[i]);

	// load textures
	TexturesLoad			();

	// invalidate caches
	current_shader			= NULL;
	current_SBH				= 0xffffffff;
	ZeroMemory				(current_surf,sizeof(current_surf));
}

void CShaderManager::TexturesLoad	()
{
	// load textures
	for (DWORD i=0; i<textures.size(); i++) 
		textures[i]->Load();
}
void CShaderManager::TexturesUnload	()
{
	// unload textures
	for (DWORD i=0; i<textures.size(); i++) 
	{
		textures[i]->Unload();
		if (textures[i]->dwRefCount == 0) {
			_DELETE(textures[i]);
			textures.erase(textures.begin()+i);
			i--;
		}
	}
}

void CShaderManager::Set(Shader* S) 
{
	VERIFY(S);
	if (current_shader==S) return;

	Device.Statistic.dwShader_Changes++;

	current_shader		= S;
	S->shader->Activate	();
	current_SBH			= 0xffffffff;

	dwPassesRequired = S->shader->Passes_Count;
}

void CShaderManager::SetupPass(DWORD pass) 
{
	VERIFY(current_shader);
	CXRShader* S = current_shader->shader;
	if (S->Passes[pass].SB != current_SBH)  
	{
		current_SBH = S->Passes[pass].SB;
		S->Passes[pass].Apply();
		tex_vector &V = current_shader->H[pass];
		for (DWORD i=0; i<V.size(); i++) 
		{
			if (current_surf[i]!=V[i]) 
			{
				current_surf[i]=V[i];
				V[i]->Apply(i);
			}
		}
	}
}
