#include "stdafx.h"
#pragma hdrstop

#include "EThumbnail.h"
#include "ImageManager.h"

//------------------------------------------------------------------------------
#define THM_TEXTURE_VERSION				0x0012
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ETextureThumbnail::ETextureThumbnail(LPCSTR src_name, bool bLoad):EImageThumbnail(src_name,ETTexture)
{
    if (bLoad) 	if (!Load()) ImageLib.CreateTextureThumbnail(this,src_name);
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
        ELog.Msg	( mtError, "Thumbnail: Unsupported version.");
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
	STextureParams& F			= m_TexParams;
    PHelper.CreateToken<u32>	(items, "Format",					(u32*)&F.fmt, 			tfmt_token);
    PHelper.CreateToken<u32>	(items, "Type",						(u32*)&F.type,			ttype_token);

    PHelper.CreateFlag<Flags32>	(items, "MipMaps\\Enabled",			&F.flags,				STextureParams::flGenerateMipMaps);
    PHelper.CreateToken<u32>	(items, "MipMaps\\Filter",			(u32*)&F.mip_filter,	tparam_token);

    PHelper.CreateFlag<Flags32>	(items, "Details\\Enabled",			&F.flags,				STextureParams::flHasDetailTexture);
    PHelper.CreateTexture		(items, "Details\\Texture",			F.detail_name,			sizeof(F.detail_name));
    PHelper.CreateFloat			(items, "Details\\Scale",			&F.detail_scale,		0.1f,10000.f,0.1f,2);

    PHelper.CreateFlag<Flags32>	(items, "Flags\\Grayscale",			&F.flags,				STextureParams::flGreyScale);
    PHelper.CreateFlag<Flags32>	(items, "Flags\\Binary Alpha",		&F.flags,				STextureParams::flBinaryAlpha);
    PHelper.CreateFlag<Flags32>	(items, "Flags\\Dither",			&F.flags,				STextureParams::flDitherColor);
    PHelper.CreateFlag<Flags32>	(items, "Flags\\Dither Each MIP",	&F.flags,				STextureParams::flDitherEachMIPLevel);
    PHelper.CreateFlag<Flags32>	(items, "Flags\\Implicit Lighted",	&F.flags,				STextureParams::flImplicitLighted);

    PHelper.CreateFlag<Flags32>	(items, "Fade\\Enabled Color",		&F.flags,				STextureParams::flFadeToColor);
    PHelper.CreateFlag<Flags32>	(items, "Fade\\Enabled Alpha",		&F.flags,				STextureParams::flFadeToAlpha);
    PHelper.CreateU32			(items, "Fade\\Amount",				&F.fade_amount,			0,1000,0);
    PHelper.CreateColor			(items, "Fade\\Color",				&F.fade_color			);

    PHelper.CreateFlag<Flags32>	(items, "Border\\Enabled Color",	&F.flags,				STextureParams::flColorBorder);
    PHelper.CreateFlag<Flags32>	(items, "Border\\Enabled Alpha",	&F.flags,				STextureParams::flAlphaBorder);
    PHelper.CreateColor			(items, "Border\\Color",			&F.border_color			);
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

