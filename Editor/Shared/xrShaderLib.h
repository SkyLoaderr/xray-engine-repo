#ifndef SH_LIB
#define SH_LIB

#pragma once

#include "xrShaderDef.h"

void	shLibrary_Sort		(std::vector<SH_ShaderDef> &LIB);
DWORD	shLibrary_GetCount	(const char *Name);
void	shLibrary_Load		(const char *Name, std::vector<SH_ShaderDef> &LIB);
void	shLibrary_Save		(const char *Name, std::vector<SH_ShaderDef> &LIB);
int		shLibrary_Find		(const char* Name, std::vector<SH_ShaderDef> &LIB);
#endif
