#ifndef PS_LIB
#define PS_LIB
#pragma once

#include "ParticleSystem.h"

void				psLibrary_Sort			(PS::PSVec &LIB);
u32				psLibrary_GetCount		(const char* Name);
BOOL				psLibrary_Load			(const char* Name, PS::PSVec &LIB);
void				psLibrary_Save			(const char* Name, PS::PSVec &LIB);
PS::SDef* 			psLibrary_FindUnsorted	(const char* Name, PS::PSVec &LIB);
PS::SDef* 			psLibrary_FindSorted	(const char* Name, PS::PSVec &LIB);

#endif // PS_LIB
