#ifndef NVDXT_OPTIONS_H
#define NVDXT_OPTIONS_H
enum
{
    dSaveButton = 1,

	dDXT1 = 10,
	dTextureFormatFirst = dDXT1,

	dDXT1a = 11,  // DXT1 with one bit alpha
	dDXT3 = 12,   // explicit alpha
	dDXT5 = 13,   // interpolated alpha

	d4444 = 14,   // a4 r4 g4 b4
	d1555 = 15,   // a1 r5 g5 b5
	d565 = 16,    // a0 r5 g6 b5
	d8888 = 17,   // a8 r8 g8 b8
	d888 = 18,    // a0 r8 g8 b8
	d555 = 19,    // a0 r5 g5 b5
    dNVHS = 20,   // signed HILO
    dNVHU = 21,   // unsighed HILO


	dTextureFormatLast = dNVHU,


    // 3d viewing options
    d3DPreview = 300, 

    dViewDXT1 = 200,
    dViewDXT2 = 201,
    dViewDXT3 = 202,
    dViewDXT5 = 203,
    dViewA4R4G4B4 = 204,
    dViewA1R5G5B5 = 205,
    dViewR5G6B5 = 206,
    dViewA8R8G8B8 = 207,


    dGenerateMipMaps = 30,
    dMIPMapSourceFirst = dGenerateMipMaps,
	dUseExistingMipMaps = 31,
	dNoMipMaps = 32,
    dMIPMapSourceLast = dNoMipMaps,

    // MIP filters
    dMIPFilterBox = 33,
    dMIPFilterFirst = dMIPFilterBox,
    dMIPFilterCubic = 34,
    dMIPFilterFullDFT = 35,
    dMIPFilterKaiser = 36,
    dMIPFilterLinearLightKaiser = 37,

    dMIPFilterLast = dMIPFilterLinearLightKaiser,


    dShowDifferences = 40,
    dShowFiltering = 41,
    dShowMipMapping = 42,
    dShowAnisotropic = 43,

    dChangeClearColor = 50,
    dViewXBOX1c = 51,
    dViewXBOX1a = 52,
    dDitherColor = 53,

    dLoadBackgroundImage = 54,
    dUseBackgroundImage = 55,

    dBinaryAlpha = 56,
    dAlphaBlending = 57,
    dFade = 58,
    dFadeAlpha = 59,

    dFadeToColor = 60,
    dAlphaBorder = 61,
    dBorder = 62,
    dBorderColor = 63,
	dNormalMap = 64,
	dDuDvMap = 65,
    dDitherEachMIPLevel = 66,
    dGreyScale = 67,

    dZoom = 70,

	dTextureType2D = 80,
	dTextureTypeFirst = dTextureType2D,

	dTextureTypeCube = 81,
	dTextureTypeImage = 82,
	//dTextureTypeVolume = 83,  to be added
	dTextureTypeLast = dTextureTypeImage,

    dFadeAmount = 90,


};



#ifndef TRGBA
#define TRGBA
typedef	struct	
{
	BYTE	rgba[4];
} rgba_t;
#endif

#ifndef TPIXEL
#define TPIXEL
union tPixel
{
  unsigned long u;
  rgba_t c;
};
#endif


// Windows handle for our plug-in (seen as a dynamically linked library):
extern HANDLE hDllInstance;
class CMyD3DApplication;

typedef struct CompressionOptions
{
    bool        bMipMapsInImage;  // mip have been loaded in during read
    short       MipMapType;      // dNoMipMaps, dUseExistingMipMaps, dGenerateMipMaps


    short       MIPFilterType;    // for MIP maps
    /* 
        dMIPFilterBox 
        dMIPFilterCubic 
        dMIPFilterFullDFT 
        dMIPFilterKaiser 
        dMIPFilterLinearLightKaiser 
        */


    bool        bBinaryAlpha;   // zero or one 

    bool        bNormalMap;     // Is a normal Map
    bool        bDuDvMap;     // Is a DuDv map

    bool        bAlphaBorder;   // make an alpha border
    bool        bBorder;        // make a color border
    tPixel      BorderColor;   // color of border


    bool        bFade;          // fade to color over MIP maps
    bool        bFadeAlpha;          // fade to color over MIP maps
    tPixel      FadeToColor;   // color to fade to
    int         FadeAmount;    // percentage of color to fade in

    bool        bDitherColor;        // enable dithering during 16 bit conversion
    bool        bDitherEachMIPLevel;        // enable dithering during 16 bit conversion for each MIP level (after filtering)
    bool        bGreyScale;        // trat image as a grey scale


	short 		TextureType;    // regular decal, cube or volume  
	/*
        dTextureType2D 
    	dTextureTypeCube 
    	dTextureTypeImage 
     */

	short 		TextureFormat;
	//  dDXT1, dDXT1a, dDXT3, dDXT5, d4444, 
	//  d1555, 	d565,	d8888, 	d888, 	d555, dNVHS, dNVHU

    bool        bSwapRGB;



} CompressionOptions;


#endif
