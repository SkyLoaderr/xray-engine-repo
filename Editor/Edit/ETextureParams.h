//----------------------------------------------------
// file: TextureParam.h
//----------------------------------------------------
#ifndef _INCDEF_TextureParam_H_
#define _INCDEF_TextureParam_H_


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
		ForceDWORD	= DWORD(-1)
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
		flNormalMap			= (1<<2),
		flDuDvMap			= (1<<3),
		flAlphaBorder		= (1<<4),
		flColorBorder		= (1<<5),
		flFadeToColor		= (1<<6),
		flFadeToAlpha		= (1<<7),
		flDitherColor		= (1<<8)
	};
	struct Flags{
		DWORD bGenerateMipMaps	: 1;
		DWORD bBinaryAlpha		: 1;
		DWORD bNormalMap		: 1;
		DWORD bDuDvMap			: 1;
		DWORD bAlphaBorder		: 1;
		DWORD bColorBorder		: 1;
		DWORD bFadeToColor		: 1;
		DWORD bFadeToAlpha		: 1;
		DWORD bDitherColor		: 1;
		DWORD reserved0			: 15;

		DWORD bImplicitLighted	: 1;
	};

	ETFormat		fmt;
	Flags			flag;
	DWORD			border_color;
    DWORD			fade_color;
    WORD			fade_amount;
	WORD			mip_filter;		
    DWORD			reserved[4];
    STextureParams	()
	{
		ZeroMemory(this,sizeof(STextureParams)); 
		flag.bGenerateMipMaps	= TRUE;
		flag.bDitherColor		= TRUE;
		flag.eMIPFilter			= dMIPFilterBox;
	}
	IC BOOL HasAlphaChannel()
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

xr_token							tparam_token					[ ]={
	{ "Box",				STextureParams::dMIPFilterBox				},
	{ "Cubic",				STextureParams::dMIPFilterCubic				},
	{ "Full DFT",			STextureParams::dMIPFilterFullDFT			},
	{ "Kaiser",				STextureParams::dMIPFilterKaiser			},
	{ "Linear light Kaiser",STextureParams::dMIPFilterLinearLightKaiser	},
	{ "Advanced",			STextureParams::dMIPFilterAdvanced			},
	{ 0,					0											}
};

//----------------------------------------------------
#define THM_CURRENT_VERSION				0x0011
//----------------------------------------------------
#define THM_CHUNK_VERSION				0x0810
#define THM_CHUNK_DATA					0x0811
#define THM_CHUNK_TEXTUREPARAM			0x0812
#define THM_CHUNK_SIZE					0x0813
//----------------------------------------------------
#define THM_SIGN 		"THM_FILE"
#define THUMB_WIDTH 	128
#define THUMB_HEIGHT 	128
#define THUMB_SIZE 		THUMB_HEIGHT*THUMB_WIDTH
//----------------------------------------------------
#endif /*_INCDEF_TextureParam_H_*/

