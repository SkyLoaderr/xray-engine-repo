//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "xrParticlesLib.h"

//----------------------------------------------------
void CPSLibrary::xrStartUp()
{
	if (!psLibrary_Load(PSLIB_FILENAME,m_PSs))
		Msg("* PS Library: Unsuported version.");
	else
		Msg("* FS: Download %d particle system(s).",m_PSs.size());
}
//----------------------------------------------------
void CPSLibrary::xrShutDown()
{
    m_PSs.clear();
}
//----------------------------------------------------
PS::SDef* CPSLibrary::FindPS(const char* name)
{
	return psLibrary_FindSorted(name,m_PSs);
}
//----------------------------------------------------
void CPSLibrary::OnDeviceCreate	()
{
	for (PSIt it = m_PSs.begin(); it!=m_PSs.end(); it++)
		it->m_CachedShader	= Device.Shader.Create(it->m_ShaderName,it->m_TextureName);
}
void CPSLibrary::OnDeviceDestroy()
{
	for (PSIt it = m_PSs.begin(); it!=m_PSs.end(); it++)
		Device.Shader.Delete(it->m_CachedShader);
}
