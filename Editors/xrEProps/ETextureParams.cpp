#include "stdafx.h"
#pragma hdrstop

#include "ETextureParams.h"
#pragma package(smart_init)

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
	{ "Bump Map",			STextureParams::ttBumpMap					},
	{ 0,					0											}
};

xr_token					tfmt_token								[ ]={
	{ "DXT1",				STextureParams::tfDXT1						},
	{ "DXT1 Alpha",			STextureParams::tfADXT1						},
	{ "DXT3",				STextureParams::tfDXT3						},
	{ "DXT5",				STextureParams::tfDXT5						},
	{ "16 bit (4:4:4:4)",	STextureParams::tf4444						},
	{ "16 bit (1:5:5:5)",	STextureParams::tf1555						},
	{ "16 bit (5:6:5)",		STextureParams::tf565						},
	{ "32 bit (8:8:8:8)",	STextureParams::tfRGBA						},
	{ 0,					0											}
};

xr_token					tmtl_token								[ ]={
	{ "OrenNayar <-> Blin",	STextureParams::tmOrenNayar_Blin			},
	{ "Blin <-> Phong",		STextureParams::tmBlin_Phong				},
	{ "Phong <-> Metal",	STextureParams::tmPhong_Metal				},
	{ "Metal <-> OrenNayar",STextureParams::tmMetal_OrenNayar			},
	{ 0,					0											}
};

xr_token					tbmode_token							[ ]={
	{ "Autogen",			STextureParams::tbmAutogen					},
	{ "None",				STextureParams::tbmNone						},
	{ "Use",				STextureParams::tbmUse						},
	{ 0,					0											}
};

void STextureParams::Load(IReader& F)
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

    if (F.find_chunk(THM_CHUNK_MATERIAL)){
    	material		= (ETMaterial)F.r_u32		();
	    material_weight = F.r_float	();
    }

    if (F.find_chunk(THM_CHUNK_BUMP)){
	    bump_virtual_height	= F.r_float				();
	    bump_mode			= (ETBumpMode)F.r_u32	();
    	F.r_stringZ			(bump_name);
    }
}

void STextureParams::Save(IWriter& F)
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

    F.open_chunk	(THM_CHUNK_MATERIAL);
    F.w_u32			(material);
	F.w_float		(material_weight);
    F.close_chunk	();

    F.open_chunk	(THM_CHUNK_BUMP);
	F.w_float		(bump_virtual_height);
    F.w_u32			(bump_mode);
    F.w_stringZ		(bump_name);
    F.close_chunk	();
}


#ifdef _EDITOR
#include "PropertiesListHelper.h"

#ifndef XR_EPROPS_EXPORTS    
	#include "ImageEditor.h"
#endif
void STextureParams::OnTypeChange(PropValue* prop)
{
	switch (type){
    case ttImage:	
    case ttCubeMap:	
    break;
    case ttBumpMap:	
	    flags.set	(flGenerateMipMaps,FALSE);
    break;
    }
#ifndef XR_EPROPS_EXPORTS    
    TfrmImageLib::UpdateProperties();
#endif
}

void STextureParams::FillProp(PropItemVec& items)
{
    PropValue* P	= PHelper.CreateToken<u32>	(items, "Type",			(u32*)&type,		ttype_token);
    P->OnChangeEvent= OnTypeChange;
    PHelper.CreateCaption			(items, "Source\\Width",			width);
    PHelper.CreateCaption			(items, "Source\\Height",			height);
    PHelper.CreateCaption			(items, "Source\\Alpha",			HasAlphaChannel()?"on":"off"); 
	switch (type){
    case ttImage:	
    case ttCubeMap:	
	    PHelper.CreateToken<u32>	(items, "Format",	   				(u32*)&fmt, 		tfmt_token);

	    PHelper.CreateFlag<Flags32>	(items, "MipMaps\\Enabled",			&flags,				flGenerateMipMaps);
    	PHelper.CreateToken<u32>	(items, "MipMaps\\Filter",			(u32*)&mip_filter,	tparam_token);

    	P = PHelper.CreateToken<u32>(items, "Bump\\Mode",				(u32*)&bump_mode,	tbmode_token);
        P->OnChangeEvent			= OnTypeChange;
        if (tbmUse==bump_mode)
        	PHelper.CreateChoose	(items, "Bump\\Texture",			&bump_name,			smTexture);
        if (tbmAutogen==bump_mode)
            PHelper.CreateFloat		(items, "Bump\\Virtual Height (m)",	&bump_virtual_height, 0.f, 0.1f, 0.001f, 3);
        
        PHelper.CreateFlag<Flags32>	(items, "Details\\Use As Diffuse",	&flags,				flDiffuseDetail);
        PHelper.CreateFlag<Flags32>	(items, "Details\\Use As Bump (R2)",&flags,				flBumpDetail);
        PHelper.CreateChoose		(items, "Details\\Texture",			&detail_name,		smTexture);
        PHelper.CreateFloat			(items, "Details\\Scale",			&detail_scale,		0.1f,10000.f,0.1f,2);

        PHelper.CreateToken<u32>	(items, "Material\\Base",			(u32*)&material,	tmtl_token);
        PHelper.CreateFloat			(items, "Material\\Weight",			&material_weight	);
        
//		PHelper.CreateFlag<Flags32>	(items, "Flags\\Grayscale",			&flags,				flGreyScale);
//		PHelper.CreateFlag<Flags32>	(items, "Flags\\Binary Alpha",		&flags,				flBinaryAlpha);
        PHelper.CreateFlag<Flags32>	(items, "Flags\\Dither",			&flags,				flDitherColor);
        PHelper.CreateFlag<Flags32>	(items, "Flags\\Dither Each MIP",	&flags,				flDitherEachMIPLevel);
        PHelper.CreateFlag<Flags32>	(items, "Flags\\Implicit Lighted",	&flags,				flImplicitLighted);

        PHelper.CreateFlag<Flags32>	(items, "Fade\\Enabled Color",		&flags,				flFadeToColor);
        PHelper.CreateFlag<Flags32>	(items, "Fade\\Enabled Alpha",		&flags,				flFadeToAlpha);
        PHelper.CreateU32			(items, "Fade\\Amount",				&fade_amount,		0,1000,0);
        PHelper.CreateColor			(items, "Fade\\Color",				&fade_color			);

        PHelper.CreateFlag<Flags32>	(items, "Border\\Enabled Color",	&flags,				flColorBorder);
        PHelper.CreateFlag<Flags32>	(items, "Border\\Enabled Alpha",	&flags,				flAlphaBorder);
        PHelper.CreateColor			(items, "Border\\Color",			&border_color		);
    break;
    case ttBumpMap:	
        PHelper.CreateFloat			(items, "Bump\\Virtual Height (m)",	&bump_virtual_height, 0.f, 0.1f, 0.001f, 3);
    break;
    }

}
#endif
