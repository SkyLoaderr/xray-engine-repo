//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "xrParticlesLib.h"

//----------------------------------------------------
void CPSLibrary::xrStartUp(){
	if (!psLibrary_Load(PSLIB_FILENAME,m_PSs))
		Msg("* PS Library: Unsuported version.");
	else
		Msg("* FS: Download %d particle system(s).",m_PSs.size());
}
//----------------------------------------------------
void CPSLibrary::xrShutDown(){
    m_PSs.clear();
}
//----------------------------------------------------
PS::SDef* CPSLibrary::FindPS(const char* name){
	return psLibrary_Find(name,m_PSs);
}
//----------------------------------------------------
