//----------------------------------------------------
// file: TextureParam.h
//----------------------------------------------------
#ifndef _INCDEF_TextureParams_H_
#define _INCDEF_TextureParams_H_

#pragma pack(push,1)
struct STextureParams{
	enum ETFormat{
    	tfDXT1 = 0,
        tfADXT1,
        tfDXT3,
        tfDXT5,
        tf4444,
        tf1555,
        tf565,
        tfRGB,
        tfRGBA,
		tfNVHS,
		tfNVHU,
		tfForceU32	= u32(-1)
	};
	enum ETType{
    	ttImage	= 0,
        ttCubeMap,
        ttNormalMap,
        ttDuDvMap,
		ttForceU32	= u32(-1)
	};
	enum{
        dMIPFilterBox				= 0,
        dMIPFilterCubic				= 1,
        dMIPFilterFullDFT			= 2,
        dMIPFilterKaiser			= 3,
        dMIPFilterLinearLightKaiser	= 4,
		dMIPFilterAdvanced			= 5
	};
	enum{
		flGenerateMipMaps	= (1<<0),
		flBinaryAlpha		= (1<<1),
		flAlphaBorder		= (1<<4),
		flColorBorder		= (1<<5),
		flFadeToColor		= (1<<6),
		flFadeToAlpha		= (1<<7),
		flDitherColor		= (1<<8),
		flDitherEachMIPLevel= (1<<9),
		flGreyScale			= (1<<10),

		flHasDetailTexture	= (1<<23),
		flImplicitLighted	= (1<<24),
		flHasAlpha			= (1<<25),
		flForceU32			= u32(-1)
	};

    // texture part
    struct{
        ETFormat	fmt;
        Flags32		flags;
        u32			border_color;
        u32			fade_color;
        u32			fade_amount;
        u32			mip_filter;
        int			width;
        int			height;
        // detail ext
        string128	detail_name;
        float		detail_scale;
        ETType		type;
    };
    // object part
    struct{
        int			face_count;
        int			vertex_count;
    };

    STextureParams		()
	{
		ZeroMemory		(this,sizeof(STextureParams));
		flags.set		(flGenerateMipMaps|flDitherColor,TRUE);
		mip_filter		= dMIPFilterBox;
        width			= 0;
        height			= 0;
        detail_scale	= 1;             
        face_count		= 0;
        vertex_count	= 0;                       
	}
    IC BOOL HasAlpha(){ // исходная текстура содержит альфа канал
    	return flags.is(flHasAlpha);
    }
	IC BOOL HasAlphaChannel() // игровая текстура содержит альфа канал
	{
		switch (fmt)
		{
		case tfADXT1:
		case tfDXT3:
		case tfDXT5:
		case tf4444:
		case tf1555:
		case tfRGBA:
			return TRUE;
		default:
			return FALSE;
		}
	}
};
#pragma pack( pop )

struct xr_token;
extern xr_token	tparam_token[];
extern xr_token	tfmt_token[];
extern xr_token	ttype_token[];

//----------------------------------------------------
#define THM_CURRENT_VERSION				0x0012
//----------------------------------------------------
#define THM_CHUNK_VERSION				0x0810
#define THM_CHUNK_DATA					0x0811
#define THM_CHUNK_TEXTUREPARAM			0x0812
#define THM_CHUNK_TYPE					0x0813
#define THM_CHUNK_TEXTURE_TYPE			0x0814
#define THM_CHUNK_DETAIL_EXT			0x0815
#define THM_CHUNK_OBJECTPARAM			0x0816
//----------------------------------------------------
#define THM_SIGN 		"THM_FILE"
#define THUMB_WIDTH 	128
#define THUMB_HEIGHT 	128
#define THUMB_SIZE 		THUMB_HEIGHT*THUMB_WIDTH
//----------------------------------------------------
#endif /*_INCDEF_TextureParams_H_*/

