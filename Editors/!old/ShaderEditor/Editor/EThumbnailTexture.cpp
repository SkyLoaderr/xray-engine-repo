#include "stdafx.h"
#pragma hdrstop

#include "EThumbnail.h"
#ifndef XR_EPROPS_EXPORTS
	#include "ImageManager.h"
#endif

//------------------------------------------------------------------------------
#define THM_TEXTURE_VERSION				0x0012
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ETextureThumbnail::ETextureThumbnail(LPCSTR src_name, bool bLoad):EImageThumbnail(src_name,ETTexture)
{
    if (bLoad) 	
#ifdef XR_EPROPS_EXPORTS
    	Load(); 
#else
		if (!Load()) ImageLib.CreateTextureThumbnail(this,src_name);
#endif
}
//------------------------------------------------------------------------------

ETextureThumbnail::~ETextureThumbnail()
{
	m_Pixels.clear();
}
//------------------------------------------------------------------------------

int ETextureThumbnail::MemoryUsage()
{
	int mem_usage = _Width()*_Height()*4;
    switch (m_TexParams.fmt){
    case STextureParams::tfDXT1:
    case STextureParams::tfADXT1: 	mem_usage/=6; break;
    case STextureParams::tfDXT3:
    case STextureParams::tfDXT5: 	mem_usage/=4; break;
    case STextureParams::tf4444:
    case STextureParams::tf1555:
    case STextureParams::tf565: 	mem_usage/=2; break;
    case STextureParams::tfRGBA:	break;
    }
    AnsiString fn 	= ChangeFileExt(m_Name,".seq");
    FS.update_path(_game_textures_,fn);
    if (FS.exist(fn.c_str())){
        string128		buffer;
        IReader* F		= FS.r_open(0,fn.c_str());
        F->r_string		(buffer);
        int cnt = 0;
        while (!F->eof()){
            F->r_string(buffer);
            cnt++;
        }
        FS.r_close		(F);
        mem_usage *= cnt?cnt:1;
    }
    return mem_usage;
}
//------------------------------------------------------------------------------

void ETextureThumbnail::CreateFromData(u32* p, u32 w, u32 h)
{
	EImageThumbnail::CreatePixels(p, w, h);
    m_TexParams.width = w;
    m_TexParams.height= h;
    m_TexParams.flags.set(STextureParams::flHasAlpha,FALSE);
}
//------------------------------------------------------------------------------

bool ETextureThumbnail::Load(LPCSTR src_name, LPCSTR path)
{
	AnsiString fn = ChangeFileExt(src_name?AnsiString(src_name):m_Name,".thm");
    if (path) 		FS.update_path(path,fn);
    else			FS.update_path(_textures_,fn);
    if (!FS.exist(fn.c_str())) return false;
    
    IReader* F 		= FS.r_open(fn.c_str());
    u16 version 	= 0;

    R_ASSERT(F->r_chunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_TEXTURE_VERSION ){
		Msg			("!Thumbnail: Unsupported version.");
        return 		false;
    }

    IReader* D 		= F->open_chunk(THM_CHUNK_DATA); R_ASSERT(D);
    m_Pixels.resize	(THUMB_SIZE);
    D->r			(m_Pixels.begin(),THUMB_SIZE*sizeof(u32));
    D->close		();

    R_ASSERT		(F->find_chunk(THM_CHUNK_TYPE));
    m_Type			= THMType(F->r_u32());
    R_ASSERT		(m_Type==ETTexture);

    m_TexParams.Load(*F);
    m_Age 			= FS.get_file_age(fn.c_str());

    FS.r_close		(F);

    return true;
}
//------------------------------------------------------------------------------

void ETextureThumbnail::Save(int age, LPCSTR path)
{
	if (!Valid()) 	return;

    CMemoryWriter F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.w_u16			(THM_TEXTURE_VERSION);
	F.close_chunk	();

	F.w_chunk		(THM_CHUNK_DATA | CFS_CompressMark,m_Pixels.begin(),m_Pixels.size()*sizeof(u32));

    F.open_chunk	(THM_CHUNK_TYPE);
    F.w_u32			(m_Type);
	F.close_chunk	();

	m_TexParams.Save(F);

	AnsiString fn 	= m_Name;
    if (path) 		FS.update_path(path,fn);
    else			FS.update_path(_textures_,fn);
    F.save_to		(fn.c_str());

    FS.set_file_age	(fn.c_str(),age?age:m_Age);
}
//------------------------------------------------------------------------------

void ETextureThumbnail::FillProp(PropItemVec& items)
{
	m_TexParams.FillProp		(items);
}
//------------------------------------------------------------------------------

void ETextureThumbnail::FillInfo(PropItemVec& items)
{
	STextureParams& F			= m_TexParams;
    PHelper.CreateCaption		(items, "Format",					tfmt_token[F.fmt].name	);
    PHelper.CreateCaption		(items, "Type",						ttype_token[F.type].name);
    PHelper.CreateCaption		(items, "Width",					_Width());
    PHelper.CreateCaption		(items, "Height",					_Height());
    PHelper.CreateCaption		(items, "Alpha",					_Alpha()?"on":"off");
}
//------------------------------------------------------------------------------

LPCSTR ETextureThumbnail::FormatString()
{
	LPCSTR c_fmt = 0;
    for(int i=0; tfmt_token[i].name; i++)
        if (tfmt_token[i].id==m_TexParams.fmt){
            c_fmt=tfmt_token[i].name;
            break;
        }
    return c_fmt;
}
//------------------------------------------------------------------------------

