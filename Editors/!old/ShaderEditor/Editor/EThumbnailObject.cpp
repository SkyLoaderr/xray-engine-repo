#include "stdafx.h"
#pragma hdrstop

#include "EThumbnail.h"
//#include "ImageManager.h"
#pragma package(smart_init)

//------------------------------------------------------------------------------
#define THM_OBJECT_VERSION				0x0012
//------------------------------------------------------------------------------
#define THM_CHUNK_OBJECTPARAM			0x0816
//------------------------------------------------------------------------------
EObjectThumbnail::EObjectThumbnail(LPCSTR src_name, bool bLoad):EImageThumbnail(src_name,ETObject)
{
    if (bLoad) 	Load();
}
//------------------------------------------------------------------------------

EObjectThumbnail::~EObjectThumbnail()
{
	m_Pixels.clear();
}
//------------------------------------------------------------------------------

void EObjectThumbnail::CreateFromData(u32* p, u32 w, u32 h, int fc, int vc)
{
	EImageThumbnail::CreatePixels(p, w, h);
    face_count	 	= fc;
    vertex_count  	= vc;
}
//------------------------------------------------------------------------------

bool EObjectThumbnail::Load(LPCSTR src_name, LPCSTR path)
{
	xr_string fn 	= EFS.ChangeFileExt(src_name?src_name:m_Name.c_str(),".thm");
    if (path) 		FS.update_path(fn,path,fn.c_str());
    else			FS.update_path(fn,_objects_,fn.c_str());
    if (!FS.exist(fn.c_str())) return false;
    
    IReader* F 		= FS.r_open(fn.c_str());
    u16 version 	= 0;

    R_ASSERT(F->r_chunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_OBJECT_VERSION ){
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

    R_ASSERT		(F->find_chunk(THM_CHUNK_OBJECTPARAM));
    face_count 		= F->r_u32();
    vertex_count 	= F->r_u32();
	
    m_Age 			= FS.get_file_age(fn.c_str());

    FS.r_close		(F);

    return true;
}
//------------------------------------------------------------------------------

void EObjectThumbnail::Save(int age, LPCSTR path)
{
	if (!Valid()) 	return;

    CMemoryWriter F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.w_u16			(THM_OBJECT_VERSION);
	F.close_chunk	();

	F.w_chunk		(THM_CHUNK_DATA | CFS_CompressMark,m_Pixels.begin(),m_Pixels.size()*sizeof(u32));

    F.open_chunk	(THM_CHUNK_TYPE);
    F.w_u32			(m_Type);
	F.close_chunk	();

    F.open_chunk	(THM_CHUNK_OBJECTPARAM);
    F.w_u32			(face_count);
    F.w_u32			(vertex_count);
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

void EObjectThumbnail::FillProp(PropItemVec& items)
{
    PHelper().CreateCaption	(items, "Face Count",				AnsiString(face_count).c_str());
    PHelper().CreateCaption	(items, "Vertex Count",				AnsiString(vertex_count).c_str());
}
//------------------------------------------------------------------------------

void EObjectThumbnail::FillInfo(PropItemVec& items)
{
    PHelper().CreateCaption	(items, "Face Count",				AnsiString(face_count).c_str());
    PHelper().CreateCaption	(items, "Vertex Count",				AnsiString(vertex_count).c_str());
}
//------------------------------------------------------------------------------

