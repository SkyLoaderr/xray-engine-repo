//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "ParticleEffect.h"

#define _game_data_			"$game_data$"

bool ps_sort_pred	(const PS::SDef& a, 	const PS::SDef& b)		{	return xr_strcmp(a.m_Name,b.m_Name)<0;	}
bool ped_sort_pred	(const PS::CPEDef* a, 	const PS::CPEDef* b)	{	return xr_strcmp(a->Name(),b->Name())<0;}
bool pgd_sort_pred	(const PS::CPGDef* a, 	const PS::CPGDef* b)	{	return xr_strcmp(a->m_Name,b->m_Name)<0;}

bool ps_find_pred	(const PS::SDef& a, 	LPCSTR b)				{	return xr_strcmp(a.m_Name,b)<0;	}
bool ped_find_pred	(const PS::CPEDef* a, 	LPCSTR b)				{	return xr_strcmp(a->Name(),b)<0;}
bool pgd_find_pred	(const PS::CPGDef* a, 	LPCSTR b)				{	return xr_strcmp(a->m_Name,b)<0;}
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
	for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
		if (s_it->m_ShaderName[0]&&s_it->m_TextureName[0])	
			s_it->m_CachedShader.create(s_it->m_ShaderName,s_it->m_TextureName);
	for (PS::PEDIt e_it = m_PEDs.begin(); e_it!=m_PEDs.end(); e_it++)
    	(*e_it)->CreateShader();
}
 
void CPSLibrary::OnDestroy()
{
	for (PS::PSIt s_it = m_PSs.begin(); s_it!=m_PSs.end(); s_it++)
		s_it->m_CachedShader.destroy	();
	for (PS::PEDIt e_it = m_PEDs.begin(); e_it!=m_PEDs.end(); e_it++)
    	(*e_it)->DestroyShader();

    m_PSs.clear		();

	for (e_it = m_PEDs.begin(); e_it!=m_PEDs.end(); e_it++)
		xr_delete	(*e_it);
	m_PEDs.clear	();

	for (PS::PGDIt g_it = m_PGDs.begin(); g_it!=m_PGDs.end(); g_it++)
		xr_delete	(*g_it);
	m_PGDs.clear	();
}
//----------------------------------------------------

PS::SDef* CPSLibrary::FindPS			(LPCSTR Name)
{
#ifdef _EDITOR
	for (PS::PSIt it=m_PSs.begin(); it!=m_PSs.end(); it++)
    	if (0==xr_strcmp(it->m_Name,Name)) return &*it;
	return NULL;
#else                                       
	PS::PSIt I = std::lower_bound(m_PSs.begin(),m_PSs.end(),Name,ps_find_pred);
	if (I==m_PSs.end() || (0!=xr_strcmp(I->m_Name,Name)))	return 0;
	else													return &*I;
#endif
}

PS::PEDIt CPSLibrary::FindPEDIt(LPCSTR Name)
{
	if (!Name) return m_PEDs.end();
#ifdef _EDITOR
	for (PS::PEDIt it=m_PEDs.begin(); it!=m_PEDs.end(); it++)
    	if (0==xr_strcmp((*it)->Name(),Name)) return it;
	return m_PEDs.end();
#else
	PS::PEDIt I = std::lower_bound(m_PEDs.begin(),m_PEDs.end(),Name,ped_find_pred);
	if (I==m_PEDs.end() || (0!=xr_strcmp((*I)->m_Name,Name)))	return m_PEDs.end();
	else														return I;
#endif
}

PS::CPEDef* CPSLibrary::FindPED(LPCSTR Name)
{
	PS::PEDIt it = FindPEDIt(Name);
    return (it==m_PEDs.end())?0:*it;
}

PS::PGDIt CPSLibrary::FindPGDIt(LPCSTR Name)
{
	if (!Name) return m_PGDs.end();
#ifdef _EDITOR
	for (PS::PGDIt it=m_PGDs.begin(); it!=m_PGDs.end(); it++)
    	if (0==xr_strcmp((*it)->m_Name,Name)) return it;
	return m_PGDs.end();
#else
	PS::PGDIt I = std::lower_bound(m_PGDs.begin(),m_PGDs.end(),Name,pgd_find_pred);
	if (I==m_PGDs.end() || (0!=xr_strcmp((*I)->m_Name,Name)))	return m_PGDs.end();
	else														return I;
#endif
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
	    	(*it)->DestroyShader();
        	xr_delete		(*it);
	       	m_PEDs.erase	(it);
        }else{
            PS::PGDIt it = FindPGDIt(nm);
            if (it!=m_PGDs.end()){
    	    	xr_delete	(*it);
		       	m_PGDs.erase(it);
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

	std::sort			(m_PSs.begin(),m_PSs.end(),ps_sort_pred);
	std::sort			(m_PEDs.begin(),m_PEDs.end(),ped_sort_pred);
	std::sort			(m_PGDs.begin(),m_PGDs.end(),pgd_sort_pred);
   
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

