#include "stdafx.h"
#pragma hdrstop

#include "EThumbnail.h"
#ifndef XR_EPROPS_EXPORTS
	#include "ImageManager.h"
#endif
#pragma package(smart_init)

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
    std::string fn;
    FS.update_path	(fn,_game_textures_,EFS.ChangeFileExt(m_Name.c_str(),".seq").c_str());
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
	std::string fn 	= EFS.ChangeFileExt(src_name?src_name:m_Name.c_str(),".thm");
    if (path) 		FS.update_path(fn,path,fn.c_str());
    else			FS.update_path(fn,_textures_,fn.c_str());
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

	std::string fn;
    if (path) 		FS.update_path(fn,path,			m_Name.c_str());
    else			FS.update_path(fn,_textures_,	m_Name.c_str());
    F.save_to		(fn.c_str());

    FS.set_file_age	(fn.c_str(),age?age:m_Age);
}
//------------------------------------------------------------------------------

void ETextureThumbnail::FillProp(PropItemVec& items, PropValue::TOnChange on_type_change)
{
	m_TexParams.FillProp		(m_SrcName.c_str(),items,on_type_change);
}
//------------------------------------------------------------------------------

void ETextureThumbnail::FillInfo(PropItemVec& items)
{                                                                         
	STextureParams& F			= m_TexParams;
    PHelper().CreateCaption		(items, "Format",					get_token_name(tfmt_token,F.fmt));
    PHelper().CreateCaption		(items, "Type",						get_token_name(ttype_token,F.type));
    PHelper().CreateCaption		(items, "Width",					shared_str().sprintf("%d",_Width()));
    PHelper().CreateCaption		(items, "Height",					shared_str().sprintf("%d",_Height()));
    PHelper().CreateCaption		(items, "Alpha",					_Alpha()?"on":"off");
}
//------------------------------------------------------------------------------

LPCSTR ETextureThumbnail::FormatString()
{
    return get_token_name(tfmt_token,m_TexParams.fmt);
}
//------------------------------------------------------------------------------

void ETextureThumbnail::Draw(HDC hdc, const Irect& R)
{
	if (Valid()){
        Irect r;
        r.x1 = R.x1+1; 
        r.y1 = R.y1+1;
        r.x2 = R.x2-1; 
        r.y2 = R.y2-1;
        if (_Width()!=_Height())	FHelper.FillRect(hdc,r,0x00000000);
        if (_Width()>_Height())	{   r.y2 -= r.height()-iFloor(r.height()*float(_Height())/float(_Width()));}
        else					{	r.x2 -= r.width()-iFloor(r.width()*float(_Width())/float(_Height()));}
        inherited::Draw(hdc,r);
    }
}

