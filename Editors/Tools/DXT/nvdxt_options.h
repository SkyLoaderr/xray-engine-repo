#ifndef NVDXT_OPTIONS_H
#define NVDXT_OPTIONS_H
enum
{
    dSaveButton = 1,
    dCancelButton = 2,

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
    d8   = 20,   // paletted

    dNVHS = 21,   // signed HILO TODO
    dNVHU = 22,   // unsighed HILO TODO


	dTextureFormatLast = dNVHU,


    // 3d viewing options
    d3DPreviewButton = 300, 

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
	dSpecifyMipMaps = 31,
	dUseExistingMipMaps = 32,
	dNoMipMaps = 33,
    dMIPMapSourceLast = dNoMipMaps,

    dSpecifiedMipMaps = 39,

    // MIP filters
    dMIPFilterBox = 133,
    dMIPFilterFirst = dMIPFilterBox,
    dMIPFilterCubic = 134,
    dMIPFilterFullDFT = 135,
    dMIPFilterKaiser = 136,
    dMIPFilterLinearLightKaiser = 137,

    dMIPFilterLast = dMIPFilterLinearLightKaiser,


    dShowDifferences = 40,
    dShowFiltering = 41,
    dShowMipMapping = 42,
    dShowAnisotropic = 43,

    dChangeClearColorButton = 50,
    dViewXBOX1c = 51,
    dViewXBOX1a = 52,
    dDitherColor = 53,

    dLoadBackgroundImageButton = 54,
    dUseBackgroundImage = 55,

    dBinaryAlpha = 56,
    dAlphaBlending = 57,
    dFadeColor = 58,
    dFadeAlpha = 59,

    dFadeToColorButton = 60,
    dAlphaBorder = 61,
    dBorder = 62,
    dBorderColorButton = 63,
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
    dFadeToAlpha = 91,
    dFadeToDelay = 92,

    dAskToLoadMIPMaps = 400,
    dShowAlphaWarning = 401,
    dShowPower2Warning = 402,

    dAdvancedBlendingButton = 500,
    dUserSpecifiedFadingAmounts = 501



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

typedef enum RescaleOption
{
    RESCALE_NONE,               // no rescale
    RESCALE_NEAREST_POWER2,     // rescale to nearest power of two
    RESCALE_BIGGEST_POWER2,   // rescale to next bigger power of 2
    RESCALE_SMALLEST_POWER2,  // rescale to next smaller power of 2
} RescaleOption;


typedef struct CompressionOptions
{
    RescaleOption   bRescaleImageToPower2; 
    bool            bMipMapsInImage;    // mip have been loaded in during read
    short           MipMapType;         // dNoMipMaps, dSpecifyMipMaps, dUseExistingMipMaps, dGenerateMipMaps
    short           SpecifiedMipMaps;   // if dSpecifyMipMaps is set (number of mipmaps to generate)

    short           MIPFilterType;      // for MIP maps
    /* 
        for MIPFilterType, specify one of:
            dMIPFilterBox 
            dMIPFilterCubic 
            dMIPFilterFullDFT 
            dMIPFilterKaiser 
            dMIPFilterLinearLightKaiser 
    */


    bool        bBinaryAlpha;       // zero or one alpha channel

    bool        bNormalMap;         // Is a normal Map
    bool        bDuDvMap;           // Is a DuDv (EMBM) map

    bool        bAlphaBorder;       // make an alpha border
    bool        bBorder;            // make a color border
    tPixel      BorderColor;        // color of border


    bool        bFadeColor;         // fade color over MIP maps
    bool        bFadeAlpha;         // fade alpha over MIP maps

    tPixel      FadeToColor;        // color to fade to
    int         FadeToAlpha;        // alpha value to fade to (0-255)

    int         FadeToDelay;        // start fading after 'n' MIP maps
    int         FadeAmount;         // percentage of color to fade in

    bool        bDitherColor;       // enable dithering during 16 bit conversion
    bool        bDitherEachMIPLevel;// enable dithering during 16 bit conversion for each MIP level (after filtering)
    bool        bGreyScale;         // treat image as a grey scale
    bool        bForceDXT1FourColors;  // do not let DXT1 use 3 color representation


	short 		TextureType;        // regular decal, cube or volume  
	/*
        for TextureType, specify one of:
            dTextureType2D 
    	    dTextureTypeCube 
    	    dTextureTypeImage 
     */

	short 		TextureFormat;	    
    /* 
        for TextureFormat, specify one of:
            dDXT1, 
            dDXT1a, 
            dDXT3, 
            dDXT5, 
            d4444, 
	        d1555, 	
            d565,	
            d8888, 	
            d888, 
            d555, 
            not supported yet dNVHS, dNVHU
        */

    bool        bSwapRGB;           // swap color positions R and G



} CompressionOptions;


#endif
