#include "stdafx.h"
#pragma hdrstop

#include "ETextureParams.h"

xr_token					tparam_token							[ ]={
	{ "Box",				STextureParams::dMIPFilterBox				},
	{ "Cubic",				STextureParams::dMIPFilterCubic				},
	{ "Full DFT",			STextureParams::dMIPFilterFullDFT			},
	{ "Kaiser",				STextureParams::dMIPFilterKaiser			},
	{ "Linear light Kaiser",STextureParams::dMIPFilterLinearLightKaiser	},
	{ "Advanced",			STextureParams::dMIPFilterAdvanced			},
	{ 0,					0											}
};

xr_token					ttype_token								[ ]={
	{ "2D Texture",			STextureParams::ttImage						},
	{ "Cube Map",  			STextureParams::ttCubeMap					},
	{ "Normal Map",			STextureParams::ttNormalMap					},
	{ "Du Dv Map",			STextureParams::ttDuDvMap					},
	{ 0,					0											}
};

xr_token					tfmt_token						[ ]={
	{ "DXT1",				STextureParams::tfDXT1				},
	{ "DXT1 Alpha",			STextureParams::tfADXT1				},
	{ "DXT3",				STextureParams::tfDXT3				},
	{ "DXT5",				STextureParams::tfDXT5				},
	{ "16 bit (4:4:4:4)",	STextureParams::tf4444				},
	{ "16 bit (1:5:5:5)",	STextureParams::tf1555				},
	{ "16 bit (5:6:5)",		STextureParams::tf565				},
	{ "32 bit (8:8:8:8)",	STextureParams::tfRGBA				},
	{ 0,					0									}
};

void STextureParams::LoadTex(IReader& F)
{
    R_ASSERT(F.find_chunk(THM_CHUNK_TEXTUREPARAM));
    F.r					(&fmt,sizeof(ETFormat));
    flags.set	(F.r_u32());
    border_color= F.r_u32();
    fade_color	= F.r_u32();
    fade_amount	= F.r_u32();
    mip_filter	= F.r_u32();
    width		= F.r_u32();
    height		= F.r_u32();

    if (F.find_chunk(THM_CHUNK_TEXTURE_TYPE)){
        type	= (ETType)F.r_u32();
    }

    if (F.find_chunk(THM_CHUNK_DETAIL_EXT)){
        F.r_stringZ(detail_name);
        detail_scale = F.r_float();
    }
}
void STextureParams::LoadObj(IReader& F)
{
    if (F.find_chunk(THM_CHUNK_OBJECTPARAM)){
        face_count 		= F.r_u32();
        vertex_count 	= F.r_u32();
    }
}

void STextureParams::SaveTex(IWriter& F)
{
    F.open_chunk	(THM_CHUNK_TEXTUREPARAM);
    F.w				(&fmt,sizeof(ETFormat));
    F.w_u32			(flags.get());
    F.w_u32			(border_color);
    F.w_u32			(fade_color);
    F.w_u32			(fade_amount);
    F.w_u32			(mip_filter);
    F.w_u32			(width);
    F.w_u32			(height);
    F.close_chunk	();

    F.open_chunk	(THM_CHUNK_TEXTURE_TYPE);
    F.w_u32			(type);
    F.close_chunk	();


    F.open_chunk	(THM_CHUNK_DETAIL_EXT);
    F.w_stringZ		(detail_name);
    F.w_float		(detail_scale);
    F.close_chunk	();
}

void STextureParams::SaveObj(IWriter& F)
{
    F.open_chunk	(THM_CHUNK_OBJECTPARAM);
    F.w_u32			(face_count);
    F.w_u32			(vertex_count);
    F.close_chunk	();
}


