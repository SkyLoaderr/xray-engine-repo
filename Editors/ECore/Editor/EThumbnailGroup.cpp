#include "stdafx.h"
#pragma hdrstop

#include "EThumbnail.h"
//#include "ImageManager.h"
#pragma package(smart_init)

//------------------------------------------------------------------------------
#define THM_GROUP_VERSION				0x0001
//------------------------------------------------------------------------------
#define THM_CHUNK_GROUPPARAM			0x0001
//------------------------------------------------------------------------------
EGroupThumbnail::EGroupThumbnail(LPCSTR src_name, bool bLoad):EImageThumbnail(src_name,ETObject)
{
    if (bLoad) 	Load();
}
//------------------------------------------------------------------------------

EGroupThumbnail::~EGroupThumbnail()
{
	m_Pixels.clear();
}
//------------------------------------------------------------------------------

void EGroupThumbnail::CreateFromData(u32* p, u32 w, u32 h, const SStringVec& lst)
{
	EImageThumbnail::CreatePixels(p, w, h);
    objects			= lst;
}
//------------------------------------------------------------------------------

bool EGroupThumbnail::Load(LPCSTR src_name, LPCSTR path)
{
	xr_string fn 	= EFS.ChangeFileExt(src_name?src_name:m_Name.c_str(),".thm");
    if (path) 		FS.update_path(fn,path,fn.c_str());
    else			FS.update_path(fn,_objects_,fn.c_str());
    if (!FS.exist(fn.c_str())) return false;
    
    IReader* F 		= FS.r_open(fn.c_str());
    u16 version 	= 0;

    R_ASSERT(F->r_chunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_GROUP_VERSION ){
		Msg			("!Thumbnail: Unsupported version.");
        return 		false;
    }

    IReader* D 		= F->open_chunk(THM_CHUNK_DATA); R_ASSERT(D);
    m_Pixels.resize	(THUMB_SIZE);
    D->r			(m_Pixels.begin(),THUMB_SIZE*sizeof(u32));
    D->close		();

    R_ASSERT		(F->find_chunk(THM_CHUNK_TYPE));
    m_Type			= THMType(F->r_u32());
    R_ASSERT		(m_Type==ETObject);

    R_ASSERT		(F->find_chunk(THM_CHUNK_GROUPPARAM));
    objects.resize	(F->r_u32());
    for (SStringVecIt it=objects.begin(); it!=objects.end(); it++)
    	F->r_stringZ(*it);
	
    m_Age 			= FS.get_file_age(fn.c_str());

    FS.r_close		(F);

    return true;
}
//------------------------------------------------------------------------------

void EGroupThumbnail::Save(int age, LPCSTR path)
{
	if (!Valid()) 	return;

    CMemoryWriter F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.w_u16			(THM_GROUP_VERSION);
	F.close_chunk	();

	F.w_chunk		(THM_CHUNK_DATA | CFS_CompressMark,m_Pixels.begin(),m_Pixels.size()*sizeof(u32));

    F.open_chunk	(THM_CHUNK_TYPE);
    F.w_u32			(m_Type);
	F.close_chunk	();

    F.open_chunk	(THM_CHUNK_GROUPPARAM);
    F.w_u32			(objects.size());
    for (SStringVecIt it=objects.begin(); it!=objects.end(); it++)
    	F.w_stringZ	(*it);
    F.close_chunk	();

	xr_string fn;
    if (path) 		FS.update_path(fn,path,m_Name.c_str());
    else			FS.update_path(fn,_objects_,m_Name.c_str());
    if (F.save_to(fn.c_str())){
	    FS.set_file_age	(fn.c_str(),age?age:m_Age);
    }else{
        Log			("!Can't save thumbnail:",fn.c_str());
    }
}
//------------------------------------------------------------------------------

void EGroupThumbnail::FillProp(PropItemVec& items)
{
    PHelper().CreateCaption	(items, "Objects\\Count",						AnsiString(objects.size()).c_str());
    for (SStringVecIt it=objects.begin(); it!=objects.end(); it++)
	    PHelper().CreateCaption	(items, AnsiString().sprintf("Objects\\#%d",it-objects.begin()).c_str(),it->c_str());
}
//------------------------------------------------------------------------------

void EGroupThumbnail::FillInfo(PropItemVec& items)
{
	FillProp		(items);
}
//------------------------------------------------------------------------------

