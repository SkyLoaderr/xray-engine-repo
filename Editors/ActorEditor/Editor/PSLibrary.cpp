//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "ParticleEffect.h"

#define _game_data_			"$game_data$"

//----------------------------------------------------
void CPSLibrary::OnCreate()
{
	m_CurrentPE[0]	= 0;
	string256 fn;
    FS.update_path(fn,_game_data_,PSLIB_FILENAME);
	if (FS.exist(fn)){
    	if (!Load(fn)) Msg("PS Library: Unsupported version.");
    }else{
    	Msg("Can't find file: '%s'",fn);
    }
}

void CPSLibrary::OnDestroy()
{
    m_PSs.clear();
	m_PEs.clear();
}

//----------------------------------------------------
void CPSLibrary::OnDeviceCreate	()
{
	for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
		s_it->m_CachedShader	= (s_it->m_ShaderName[0]&&s_it->m_TextureName[0])?Device.Shader.Create(s_it->m_ShaderName,s_it->m_TextureName):0;
	for (PS::PEIt g_it = m_PEs.begin(); g_it!=m_PEs.end(); g_it++)
		(*g_it)->m_CachedShader	= ((*g_it)->m_ShaderName&&(*g_it)->m_ShaderName[0]&&(*g_it)->m_TextureName&&(*g_it)->m_TextureName[0])?Device.Shader.Create((*g_it)->m_ShaderName,(*g_it)->m_TextureName):0;
}
void CPSLibrary::OnDeviceDestroy()
{
	for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
		Device.Shader.Delete(s_it->m_CachedShader);
	for (PS::PEIt g_it = m_PEs.begin(); g_it!=m_PEs.end(); g_it++)
		Device.Shader.Delete((*g_it)->m_CachedShader);
}

PS::SDef* CPSLibrary::FindPS(LPCSTR Name)
{
	for (PS::PSIt it=m_PSs.begin(); it!=m_PSs.end(); it++)
    	if (0==strcmp(it->m_Name,Name)) return &*it;
	return NULL;
}

PS::PEIt CPSLibrary::FindPEIt(LPCSTR Name)
{
	for (PS::PEIt it=m_PEs.begin(); it!=m_PEs.end(); it++)
    	if (0==strcmp((*it)->m_Name,Name)) return it;
	return m_PEs.end();
}

PS::CPEDef* CPSLibrary::FindPE(LPCSTR Name)
{
	PS::PEIt it = FindPEIt(Name);
    return (it==m_PEs.end())?0:*it;
}

void CPSLibrary::RenamePS(PS::SDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::RenamePE(PS::CPEDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::Remove(const char* nm)
{
    PS::SDef* sh = FindPS(nm);
    if (sh){ 
    	Device.Shader.Delete(sh->m_CachedShader);
    	m_PSs.erase(sh);
    }else{
    	PS::PEIt it = FindPEIt(nm);
        if (it!=m_PEs.end()){
	    	Device.Shader.Delete((*it)->m_CachedShader);
	       	m_PEs.erase	(it);
        	xr_delete	(*it);
        }
    }
}

//----------------------------------------------------
bool CPSLibrary::Load(const char* nm)
{
	IReader*	F			= FS.r_open(nm);
	m_PSs.clear				();

	bool bRes 				= true;
    R_ASSERT(F->find_chunk(PS_CHUNK_VERSION));
    u16 ver					= F->r_u16();
    if (ver!=PS_VERSION) return false;
    // two version
    // first generation
    R_ASSERT(F->find_chunk(PS_CHUNK_FIRSTGEN));
    u32 count 				= F->r_u32();
    if (count){
        m_PSs.resize		(count);
        F->r				(&*m_PSs.begin(), count*sizeof(PS::SDef));
        for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
            s_it->m_CachedShader = 0;
    }
    // second generation
    IReader* OBJ 			= F->open_chunk(PS_CHUNK_SECONDGEN);
    if (OBJ){
        IReader* O   		= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            PS::CPEDef*	def	= xr_new<PS::CPEDef>();
            if (def->Load(*O)) m_PEs.push_back(def);
            else{ bRes = false; xr_delete(def); }
            O->close();
            if (!bRes)	break;
            O 			= OBJ->open_chunk(count);
        }
        OBJ->close();
    }

    // final
	FS.r_close			(F);
    return bRes;
}
//----------------------------------------------------
void CPSLibrary::Reload()
{
	OnDestroy();
    OnCreate();
	Msg( "PS Library was succesfully reloaded." );
}
//----------------------------------------------------

