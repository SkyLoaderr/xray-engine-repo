#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "ChoseForm.h"
#include "ParticleGroup.h"
//------------------------------------------------------------------------------

LPCSTR CPSLibrary::ChoosePG()
{
	LPCSTR T=0;
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smPG,T,1,m_CurrentPG)){
    	strcpy(m_CurrentPG,T);
        return m_CurrentPG;
    }else return 0;
}
//------------------------------------------------------------------------------

char* CPSLibrary::GenerateName(LPSTR buffer, LPCSTR folder, LPCSTR pref )
{
    int cnt = 0;
    string128 prefix; prefix[0]=0;
    if (pref&&pref[0]){
    	strcpy( prefix, pref );
        for (int i=strlen(prefix)-1; i>=0; i--) if (isdigit(prefix[i])) prefix[i]=0; else break;
		sprintf( buffer, "%s%s%04d", folder?folder:"", prefix, cnt++);
    }else        sprintf( buffer, "%sps_%04d", folder?folder:"", cnt++ );
    while (FindPS(buffer)||FindPG(buffer)){
        if (pref&&pref[0])	sprintf( buffer, "%s%s%04d", folder?folder:"", prefix, cnt++ );
        else   	  			sprintf( buffer, "%sps_%04d", folder?folder:"", cnt++ );
	}
    return buffer;
}
//------------------------------------------------------------------------------

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

PS::CPGDef* CPSLibrary::AppendPG(PS::CPGDef* src)
{
	m_PGs.push_back(src?xr_new<PS::CPGDef>(*src):xr_new<PS::CPGDef>());
    return m_PGs.back();
}
//------------------------------------------------------------------------------

void CPSLibrary::Save()
{
	AnsiString fn;
    FS.update_path(fn,_game_data_,PSLIB_FILENAME);
    Save(fn.c_str());
}
//------------------------------------------------------------------------------

void CPSLibrary::Save(const char* nm)
{
	CMemoryWriter F;

	F.open_chunk	(PS_CHUNK_VERSION);
    F.w_u16			(PS_VERSION);
	F.close_chunk	();

	F.open_chunk	(PS_CHUNK_FIRSTGEN);
    F.w_u32			(m_PSs.size());
	F.w				(m_PSs.begin(), m_PSs.size()*sizeof(PS::SDef));
	F.close_chunk	();

	F.open_chunk	(PS_CHUNK_SECONDGEN);
    for (PS::PGIt it=m_PGs.begin(); it!=m_PGs.end(); it++){
		F.open_chunk(it-m_PGs.begin());
        (*it)->Save	(F);
		F.close_chunk();
    }
	F.close_chunk	();

    F.save_to		(nm);
}
//------------------------------------------------------------------------------

