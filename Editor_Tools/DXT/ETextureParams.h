//----------------------------------------------------
// file: TextureParam.h
//----------------------------------------------------
#ifndef _INCDEF_TextureParam_H_
#define _INCDEF_TextureParam_H_

//----------------------------------------------------
#define EF_GENMIPMAP		0x00000001
#define EF_DITHER			0x00000002
#define EF_BINARYALPHA		0x00000004
#define EF_ALPHAZEROBORDER	0x00000008
#define EF_NORMALMAP		0x00000010
#define EF_FADE				0x00000020
#define EF_USE_EXIST_MIPMAP	0x00000040
#define EF_NO_MIPMAP		0x00000080
#define EF_IMPLICIT_LIGHTED	0x00000100

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
	ETFormat		fmt;
    DWORD			flag;
    DWORD			fade_color;
    WORD			fade_mips;
    WORD			reserved;
    STextureParams(){ZeroMemory(this,sizeof(STextureParams)); flag=EF_GENMIPMAP|EF_DITHER;}
};
#pragma pack( pop )

//----------------------------------------------------
#define THM_CURRENT_VERSION				0x0010
//----------------------------------------------------
#define THM_CHUNK_VERSION				0x0810
#define THM_CHUNK_DATA					0x0811
#define THM_CHUNK_TEXTUREPARAM			0x0812
//----------------------------------------------------
#endif /*_INCDEF_TextureParam_H_*/

