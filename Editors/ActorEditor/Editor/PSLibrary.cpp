//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "ParticleGroup.h"

CPSLibrary PSLib;

//----------------------------------------------------
void CPSLibrary::OnCreate()
{
	AnsiString fn;
//    fn = "particles.xr";
    fn = PSLIB_FILENAME;
    Engine.FS.m_GameRoot.Update(fn);
	if (Engine.FS.Exist(fn.c_str(),true)){
    	if (!Load(fn.c_str())) Msg("PS Library: Unsupported version.");
    }
}

void CPSLibrary::OnDestroy()
{
    m_PSs.clear();
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
	// Check if file is compressed already
	string32	ID			= PS_LIB_SIGN;
	string32	id;
	CStream*	F			= Engine.FS.Open(nm);
	F->Read		(&id,8);
	if (0==strncmp(id,ID,8)){
		Engine.FS.Close		(F);
		F					= xr_new<CCompressedStream> (nm,ID);
	}else{
    	F->Seek	(0);
    }
	CStream&				FS	= *F;
    
	m_PSs.clear				();

	bool bRes 				= true;
    R_ASSERT(FS.FindChunk(PS_CHUNK_VERSION));
    u16 ver				= FS.Rword();
    if (ver!=PS_VERSION) return false;
    // two version
    // first generation
    R_ASSERT(FS.FindChunk(PS_CHUNK_FIRSTGEN));
    u32 count 			= FS.Rdword();
    if (count){
        m_PSs.resize	(count);
        FS.Read			(m_PSs.begin(), count*sizeof(PS::SDef));
    }
    // second generation
    CStream* OBJ 		= FS.OpenChunk(PS_CHUNK_SECONDGEN);
    if (OBJ){
        CStream* O   	= OBJ->OpenChunk(0);
        for (int count=1; O; count++) {
            PS::CPGDef*	def	= xr_new<PS::CPGDef>();
            if (def->Load(*O)) m_PGs.push_back(def);
            else{ bRes = false; xr_delete(def); }
            O->Close();
            if (!bRes)	break;
            O 			= OBJ->OpenChunk(count);
        }
        OBJ->Close();
    }

    // final
	Engine.FS.Close		(F);
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

