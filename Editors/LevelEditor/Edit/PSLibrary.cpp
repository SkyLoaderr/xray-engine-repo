//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"

CPSLibrary PSLib;

//----------------------------------------------------
void CPSLibrary::OnCreate()
{
	AnsiString fn;
    fn = "particles.xr";//PSLIB_FILENAME;
    Engine.FS.m_GameRoot.Update(fn);
	if (Engine.FS.Exist(fn.c_str(),true)){
    	if (!Load(fn.c_str())) Msg("PS Library: Unsupported version.");
    }
}

void CPSLibrary::OnDestroy()
{
    m_PSs.clear();
}

IC bool sort_pred(const PS::SDef& A, const PS::SDef& B)
{	return strcmp(A.m_Name,B.m_Name)<0; }

void CPSLibrary::Sort()
{
	std::sort(m_PSs.begin(),m_PSs.end(),sort_pred);
}

PS::SDef* CPSLibrary::FindSorted(const char* Name)
{
	PS::SDef	D; 
	strcpy		(D.m_Name,Name);
	vector<PS::SDef>::iterator	P = std::lower_bound	(m_PSs.begin(),m_PSs.end(),D,sort_pred);
	if ((P!=m_PSs.end()) && (0==strcmp(P->m_Name,Name)) ) return &*P;
	return NULL;
}

PS::SDef* CPSLibrary::FindUnsorted(const char* Name)
{
	for (PS::PSIt it=m_PSs.begin(); it!=m_PSs.end(); it++)
    	if (0==strcmp(it->m_Name,Name)) return &*it;
	return NULL;
}

void CPSLibrary::RenamePS(PS::SDef* src, LPCSTR new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

PS::SDef* CPSLibrary::AppendPS(PS::SDef* src)
{
	if (src)
    	m_PSs.push_back(*src);
	else{
    	m_PSs.push_back(PS::SDef());
	    m_PSs.back().InitDefault();
    }
    return &m_PSs.back();
}

void CPSLibrary::RemovePS(const char* nm)
{
    PS::SDef* sh = FindUnsorted(nm);
    if (sh) m_PSs.erase(sh);
}

//----------------------------------------------------
#define PS_LIB_SIGN "PS_LIB"
bool CPSLibrary::Load(const char* nm)
{
	// Check if file is compressed already
	string32	ID			= PS_LIB_SIGN;
	string32	id;
	CStream*	F			= Engine.FS.Open(nm);
	F->Read		(&id,8);
	if (0==strncmp(id,ID,8))	
	{
		Engine.FS.Close		(F);
		F					= xr_new<CCompressedStream> (nm,ID);
	}else{
    	F->Seek	(0);
    }
	CStream&				FS	= *F;
    
	m_PSs.clear				();
    if (0){
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
        CStream* O 			= FS.OpenChunk(PS_CHUNK_SECONDGEN); R_ASSERT(O);
        
    }else{
    	// single version
        WORD version		= FS.Rword();
        if (version!=PARTICLESYSTEM_VERSION) return false;
        u32 count 			= FS.Rdword();
        if (count){
            m_PSs.resize	(count);
            FS.Read			(m_PSs.begin(), count*sizeof(PS::SDef));
        }
    }
    
    Sort				();

	Engine.FS.Close		(F);
    return true;
}
//----------------------------------------------------
void CPSLibrary::Reload()
{
	OnDestroy();
    OnCreate();
	Msg( "PS Library was succesfully reloaded." );
}
//----------------------------------------------------

