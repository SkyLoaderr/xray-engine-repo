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
	m_PEDs.clear();
	m_PGDs.clear();
}

//----------------------------------------------------
void CPSLibrary::OnDeviceCreate			()
{
	for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
		if (s_it->m_ShaderName[0]&&s_it->m_TextureName[0])	
			s_it->m_CachedShader.create(s_it->m_ShaderName,s_it->m_TextureName);
	for (PS::PEDIt g_it = m_PEDs.begin(); g_it!=m_PEDs.end(); g_it++)
		if (((*g_it)->m_ShaderName&&(*g_it)->m_ShaderName[0]&&(*g_it)->m_TextureName&&(*g_it)->m_TextureName[0]))	
			(*g_it)->m_CachedShader.create((*g_it)->m_ShaderName,(*g_it)->m_TextureName);
}

void CPSLibrary::OnDeviceDestroy		()
{
	for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
		s_it->m_CachedShader.destroy	();
	for (PS::PEDIt e_it = m_PEDs.begin(); e_it!=m_PEDs.end(); e_it++)
		(*e_it)->m_CachedShader.destroy	();
}
PS::SDef* CPSLibrary::FindPS			(LPCSTR Name)
{
	for (PS::PSIt it=m_PSs.begin(); it!=m_PSs.end(); it++)
    	if (0==strcmp(it->m_Name,Name)) return &*it;
	return NULL;
}

PS::PEDIt CPSLibrary::FindPEDIt(LPCSTR Name)
{
	for (PS::PEDIt it=m_PEDs.begin(); it!=m_PEDs.end(); it++)
    	if (0==strcmp((*it)->m_Name,Name)) return it;
	return m_PEDs.end();
}

PS::CPEDef* CPSLibrary::FindPED(LPCSTR Name)
{
	PS::PEDIt it = FindPEDIt(Name);
    return (it==m_PEDs.end())?0:*it;
}

PS::PGDIt CPSLibrary::FindPGDIt(LPCSTR Name)
{
	for (PS::PGDIt it=m_PGDs.begin(); it!=m_PGDs.end(); it++)
    	if (0==strcmp((*it)->m_Name,Name)) return it;
	return m_PGDs.end();
}

PS::CPGDef* CPSLibrary::FindPGD(LPCSTR Name)
{
	PS::PGDIt it = FindPGDIt(Name);
    return (it==m_PGDs.end())?0:*it;
}

void CPSLibrary::RenamePS(PS::SDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::RenamePED(PS::CPEDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::RenamePGD(PS::CPGDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::Remove(const char* nm)
{
    PS::SDef* sh = FindPS(nm);
    if (sh){ 
    	sh->m_CachedShader.destroy();
    	m_PSs.erase(sh);
    }else{
    	PS::PEDIt it = FindPEDIt(nm);
        if (it!=m_PEDs.end()){
	    	(*it)->m_CachedShader.destroy();
	       	m_PEDs.erase	(it);
        	xr_delete		(*it);
        }else{
            PS::PGDIt it = FindPGDIt(nm);
            if (it!=m_PGDs.end()){
		       	m_PGDs.erase(it);
    	    	xr_delete	(*it);
            }
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
    if (count)
	{
        m_PSs.resize		(count);
        F->r				(&*m_PSs.begin(), count*sizeof(PS::SDef));
        for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
            s_it->m_CachedShader._clear();
    }
    // second generation
    IReader* OBJ;
    OBJ			 			= F->open_chunk(PS_CHUNK_SECONDGEN);
    if (OBJ){
        IReader* O   		= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            PS::CPEDef*	def	= xr_new<PS::CPEDef>();
            if (def->Load(*O)) m_PEDs.push_back(def);
            else{ bRes = false; xr_delete(def); }
            O->close();
            if (!bRes)	break;
            O 			= OBJ->open_chunk(count);
        }
        OBJ->close();
    }
    // second generation
    OBJ 					= F->open_chunk(PS_CHUNK_THIRDGEN);
    if (OBJ){
        IReader* O   		= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            PS::CPGDef*	def	= xr_new<PS::CPGDef>();
            if (def->Load(*O)) m_PGDs.push_back(def);
            else{ bRes = false; xr_delete(def); }
            O->close();
            if (!bRes) break;
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
	OnDeviceDestroy();
	OnDestroy();
    OnCreate();
	OnDeviceCreate();  
	Msg( "PS Library was succesfully reloaded." );
}
//----------------------------------------------------

