#ifndef PS_LIB
#define PS_LIB
#pragma once

#include "ParticleSystem.h"

void				psLibrary_Sort		(vector<PS::SDef> &LIB);
DWORD				psLibrary_GetCount	(const char* Name);
BOOL				psLibrary_Load		(const char* Name, vector<PS::SDef> &LIB);
void				psLibrary_Save		(const char* Name, vector<PS::SDef> &LIB);
PS::SDef* 			psLibrary_Find		(const char* Name, vector<PS::SDef> &LIB);

#endif // PS_LIB