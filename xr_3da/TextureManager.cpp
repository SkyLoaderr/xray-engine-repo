// TextureManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextureManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBlender* CShaderManager::_CreateBlender	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	map<LPSTR,CBlender*,str_pred>::iterator I = blenders.find	(Name);
	if (I==blenders.end())	Device.Fatal("Blender '%s' not found in library.",Name);
	else					return I->second;
}
CShader* CShaderManager::_CreateShader		(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	map<LPSTR,CShader*,str_pred>::iterator I = shaders.find	(Name);
	if (I==shaders.end())	Device.Fatal("Shader '%s' not found in library.",Name);
	else					return I->second;
}
CTexture* CShaderManager::_CreateTexture	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	// ***** first pass - search already loaded texture
	map<LPSTR,CTexture*,str_pred>::iterator I = textures.find	(Name);
	if (I!=textures.end())	{
		I->second->dwRefCount++;
		return		I->second;
	}

	// ***** real creation
	CTexture *p		= new CTexture	(Name);
	p->dwRefCount	= 1;
	if (Device.bReady && !bDeferredLoad) p->Load();
	textures.insert(make_pair(strdup(Name),p));
	return p;
}
CConstant*	CShaderManager::_CreateConstant	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	map<LPSTR,CConstant*,str_pred>::iterator I = constants.find	(Name);
	if (I==constants.end())	Device.Fatal("Constant '%s' not found in library.",Name);
	else					return I->second;
}
CMatrix*	CShaderManager::_CreateMatrix	(LPCSTR Name) 
{
	R_ASSERT(Name && Name[0]);

	map<LPSTR,CMatrix*,str_pred>::iterator I = matrices.find	(Name);
	if (I==matrices.end())	Device.Fatal("Matrix '%s' not found in library.",Name);
	else					return I->second;
}

DWORD CShaderManager::_GetMemoryUsage	()
{
	DWORD Result	= 0;
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
