#include "stdafx.h"
#pragma hdrstop

#include "EThumbnail.h"
//#include "ImageManager.h"
#pragma package(smart_init)

//------------------------------------------------------------------------------
#define THM_SOUND_VERSION				0x0014
//------------------------------------------------------------------------------
#define THM_CHUNK_SOUNDPARAM			0x1000
#define THM_CHUNK_SOUNDPARAM2			0x1001
//------------------------------------------------------------------------------
ESoundThumbnail::ESoundThumbnail(LPCSTR src_name, bool bLoad):ECustomThumbnail(src_name,ETSound)
{
    m_fQuality 		= 0.f;
    m_fMinDist 		= 1.f;
    m_fMaxDist		= 300.f;
    m_fVolume  		= 1.f;
    m_uGameType		= 0;
    if (bLoad) 		Load();
}
//------------------------------------------------------------------------------

ESoundThumbnail::~ESoundThumbnail()
{
}
//------------------------------------------------------------------------------

bool ESoundThumbnail::Load(LPCSTR src_name, LPCSTR path)
{
	AnsiString fn = ChangeFileExt(src_name?AnsiString(src_name):m_Name,".thm");
    if (path) 		FS.update_path(path,fn);
    else			FS.update_path(_sounds_,fn);
    if (!FS.exist(fn.c_str())) return false;
    
    IReader* F 		= FS.r_open(fn.c_str());
    u16 version 	= 0;

    R_ASSERT(F->r_chunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_SOUND_VERSION ){
		Msg			("!Thumbnail: Unsupported version.");
        return 		false;
    }

    R_ASSERT		(F->find_chunk(THM_CHUNK_TYPE));
    m_Type			= THMType(F->r_u32());
    R_ASSERT		(m_Type==ETSound);

    R_ASSERT		(F->find_chunk(THM_CHUNK_SOUNDPARAM));
    m_fQuality 		= F->r_float();
    m_fMinDist		= F->r_float();
    m_fMaxDist		= F->r_float();
    m_uGameType		= F->r_u32();

    if (F->find_chunk(THM_CHUNK_SOUNDPARAM2))
		m_fVolume	= F->r_float();
	
    m_Age 			= FS.get_file_age(fn.c_str());

    FS.r_close		(F);

    return true;
}
//------------------------------------------------------------------------------

void ESoundThumbnail::Save(int age, LPCSTR path)
{
	if (!Valid()) 	return;

    CMemoryWriter F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.w_u16			(THM_SOUND_VERSION);
	F.close_chunk	();

    F.open_chunk	(THM_CHUNK_TYPE);
    F.w_u32			(m_Type);
	F.close_chunk	();

    F.open_chunk	(THM_CHUNK_SOUNDPARAM);
    F.w_float		(m_fQuality);
    F.w_float		(m_fMinDist);
    F.w_float		(m_fMaxDist);
    F.w_u32			(m_uGameType);
    F.close_chunk	();

    F.open_chunk	(THM_CHUNK_SOUNDPARAM2);
    F.w_float		(m_fVolume);
    F.close_chunk	();
    
	AnsiString fn 	= m_Name;
    if (path) 		FS.update_path(path,fn);
    else			FS.update_path(_sounds_,fn);
    F.save_to		(fn.c_str());

//	int gf 			= FS.get_file_age(fn.c_str());

    FS.set_file_age	(fn.c_str(),age?age:m_Age);
}
//------------------------------------------------------------------------------

#include "ai_sounds.h"

void ESoundThumbnail::FillProp(PropItemVec& items)
{
    PHelper.CreateFloat			(items, "Quality", 	&m_fQuality);
    PHelper.CreateFloat			(items, "Min Dist",	&m_fMinDist, 0.f,10000.f);
    PHelper.CreateFloat			(items, "Max Dist",	&m_fMaxDist, 0.f,10000.f);
    PHelper.CreateFloat			(items, "Volume",	&m_fVolume, 0.f,2.f);
    PHelper.CreateToken<u32>	(items, "Game Type",&m_uGameType, anomaly_type_token);
}
//------------------------------------------------------------------------------

void ESoundThumbnail::FillInfo(PropItemVec& items)
{
    PHelper.CreateCaption		(items, "Quality", 	AnsiString().sprintf("%3.2f",m_fQuality));
    PHelper.CreateCaption		(items, "Min Dist", AnsiString().sprintf("%3.2f",m_fMinDist));
    PHelper.CreateCaption		(items, "Max Dist",	AnsiString().sprintf("%3.2f",m_fMaxDist));
    PHelper.CreateCaption		(items, "Volume",	AnsiString().sprintf("%3.2f",m_fVolume));
    LPCSTR gt_name=0;
    for (int k=0; anomaly_type_token[k].name; k++)
    	if (m_uGameType==anomaly_type_token[k].id){ gt_name=anomaly_type_token[k].name; break;}
    PHelper.CreateCaption		(items, "Game Type",gt_name);
}
//------------------------------------------------------------------------------

