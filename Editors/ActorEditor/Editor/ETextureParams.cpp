#include "stdafx.h"
#pragma hdrstop

#include "ETextureParams.h"
#include "xr_tokens.h"

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
	{ "Image",				STextureParams::ttImage						},
	{ "Cube",				STextureParams::ttCube						},
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
	{ "24 bit (8:8:8)",		STextureParams::tfRGB				},
	{ "32 bit (8:8:8:8)",	STextureParams::tfRGBA				},
	{ "NVHS (signed HILO)",	STextureParams::tfNVHS				},
	{ "NVHU (unsigned HILO)",STextureParams::tfNVHU				},
	{ 0,					0									}
};



