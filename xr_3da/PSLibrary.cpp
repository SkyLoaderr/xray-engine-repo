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
	vector<PS::SDef>	library;
	if (!psLibrary_Load(PSLIB_FILENAME,library))	Msg("* PS Library: Unsuported version.");
	else											
	{
		m_PSs.resize	(library.size());
		for (u32 it=0; it<library.size(); it++)
		{
			void*	Pdst				= &m_PSs	[it];
			void*	Psrc				= &library	[it];
			PSGP.memCopy				(Pdst,Psrc,sizeof(PS::SDef));
			m_PSs[it].m_CachedShader	= NULL;
		}
		Msg("* FS: Download %d particle system(s).",m_PSs.size());
	}
}
//----------------------------------------------------
void CPSLibrary::xrShutDown()
{
    m_PSs.clear				();
}
//----------------------------------------------------
IC bool sort_pred(const PS::SDef_RT& A, const PS::SDef_RT& B)
{	return strcmp(A.m_Name,B.m_Name)<0; }

PS::SDef_RT* CPSLibrary::FindPS	(const char* Name)
{
	PS::SDef_RT	D;	strcpy(D.m_Name,Name);
	PS::SDef_RT*	P = &*std::lower_bound(m_PSs.begin(),m_PSs.end(),D,sort_pred);
	if ((P!=m_PSs.end()) && (0==strcmp(P->m_Name,Name)) ) return P;
	return NULL;
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
