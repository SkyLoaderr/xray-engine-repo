#include "stdafx.h"
#include "LW_Shader.h"
#include "XRShaderDef.h"
#include "XRShaderLib.h"

vector<SH_ShaderDef> LIB;
/*
void	shLibrary_Sort		(vector<SH_ShaderDef> &LIB);
DWORD	shLibrary_GetCount	(const char *Name);
void	shLibrary_Load		(const char *Name, vector<SH_ShaderDef> &LIB);
void	shLibrary_Save		(const char *Name, vector<SH_ShaderDef> &LIB);
int		shLibrary_Find		(const char* Name, vector<SH_ShaderDef> &LIB);
*/
extern "C" { EShaderList EShaders; }

extern "C" {
	void __cdecl LoadShaders()
	{
		shLibrary_Load("x:\\game\\shaders.xr", LIB);
		EShaders.count=0;
		for (int k=0; k<LIB.size(); k++){
			strcpy(EShaders.Names[EShaders.count],LIB[k].cName);
			EShaders.count++;
		}
		LIB.clear();
	}
};