//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "ParticleGroup.h"

CPSLibrary PSLib;

#define _game_data_			"$game_data$"

//----------------------------------------------------
void CPSLibrary::OnCreate()
{
	m_CurrentPS[0]	= 0;
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
}

//----------------------------------------------------
void CPSLibrary::OnDeviceCreate	()
{
	for (PS::PSIt it = m_PSs.begin(); it!=m_PSs.end(); it++)
		it->m_CachedShader	= Device.Shader.Create(it->m_ShaderName,it->m_TextureName);
}
void CPSLibrary::OnDeviceDestroy()
{
	for (PS::PSIt it = m_PSs.begin(); it!=m_PSs.end(); it++)
		Device.Shader.Delete(it->m_CachedShader);
}

PS::SDef* CPSLibrary::FindPS(LPCSTR Name)
{
	for (PS::PSIt it=m_PSs.begin(); it!=m_PSs.end(); it++)
    	if (0==strcmp(it->m_Name,Name)) return &*it;
	return NULL;
}

PS::PGIt CPSLibrary::FindPGIt(LPCSTR Name)
{
	for (PS::PGIt it=m_PGs.begin(); it!=m_PGs.end(); it++)
    	if (0==strcmp((*it)->m_Name,Name)) return it;
	return m_PGs.end();
}

PS::CPGDef* CPSLibrary::FindPG(LPCSTR Name)
{
	PS::PGIt it = FindPGIt(Name);
    return (it==m_PGs.end())?0:*it;
}

void CPSLibrary::RenamePS(PS::SDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::RenamePG(PS::CPGDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::Remove(const char* nm)
{
    PS::SDef* sh = FindPS(nm);
    if (sh) m_PSs.erase(sh);
    else{
    	PS::PGIt it = FindPGIt(nm);
        if (it!=m_PGs.end()){
	       	m_PGs.erase	(it);
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
        F->r				(m_PSs.begin(), count*sizeof(PS::SDef));
    }
    // second generation
    IReader* OBJ 			= F->open_chunk(PS_CHUNK_SECONDGEN);
    if (OBJ){
        IReader* O   		= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            PS::CPGDef*	def	= xr_new<PS::CPGDef>();
            if (def->Load(*O)) m_PGs.push_back(def);
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

