#include "stdafx.h"
#pragma hdrstop

#include "EThumbnail.h"
#include "ImageManager.h"

//------------------------------------------------------------------------------
#define THM_SOUND_VERSION				0x0012
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ESoundThumbnail::ESoundThumbnail(LPCSTR src_name, bool bLoad):ECustomThumbnail(src_name,ETSound)
{
    if (bLoad) 	Load();
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
        ELog.Msg	( mtError, "Thumbnail: Unsupported version.");
        return 		false;
    }

    R_ASSERT		(F->find_chunk(THM_CHUNK_TYPE));
    m_Type			= THMType(F->r_u32());
    R_ASSERT		(m_Type==ETSound);

    R_ASSERT		(F->find_chunk(THM_CHUNK_SOUNDPARAM));
    m_fQuality 		= F->r_float();
    m_fMinDist	 	= F->r_float();
	
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
    F.close_chunk	();

	AnsiString fn 	= m_Name;
    if (path) 		FS.update_path(path,fn);
    else			FS.update_path(_sounds_,fn);
    F.save_to		(fn.c_str());

    FS.set_file_age	(fn.c_str(),age?age:m_Age);
}
//------------------------------------------------------------------------------

void ESoundThumbnail::FillProp(PropItemVec& items)
{
    PHelper.CreateFloat	(items, "Quality", 	&m_fQuality);
    PHelper.CreateFloat	(items, "Min Dist",	&m_fMinDist, 0.f,10000.f);
}
//------------------------------------------------------------------------------

