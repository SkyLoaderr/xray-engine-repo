#include "stdafx.h"
#pragma hdrstop

#include "xr_tokens.h"

//---------------------------------------------------------------------------------------
//for CInifile defines
//---------------------------------------------------------------------------------------
xr_token							light_model_token						[ ]={
	{ "Lightmap",					0			  								},
	{ "Vertex",						1			 								},
	{ 0,							0											}
};
xr_token							tex_format_token						[ ]={
	{ "16 bit",						2											},
	{ "32 bit",						3											},
	{ "DXTC",						4											},
	{ "FXTC",						6											},
	{ 0,							0											}
};
xr_token							game_speed_token						[ ]={
	{ "Slowest",					1											},
	{ "Slow",						2											},
	{ "Normal",						3											},
	{ "Fast",						4											},
	{ "Fastest",					5											},
	{ 0,							0											}
};
xr_token							snd_freq_token							[ ]={
	{ "11khz",						0											},
	{ "22khz",						1											},
	{ "44khz",						2											},
	{ 0,							0											}
};
xr_token							snd_model_token							[ ]={
	{ "Default",					0											},
	{ "Normal",						1											},
	{ "Light",						2											},
	{ "High",						3											},
	{ 0,							0											}
};
xr_token							vid_mode_token							[ ]={
#ifdef DEBUG
	{ "160x120",					160											},
	{ "320x240",					320											},
#endif
	{ "512x384",					512											},
	{ "640x480",					640											},
	{ "800x600",					800											},
	{ "1024x768",					1024										},
	{ "1280x1024",					1280										},
	{ "1600x1200",					1600										},
	{ 0,							0											}
};
xr_token							vid_bpp_token							[ ]={
	{ "16",							16											},
	{ "24",							24											},
	{ "32",							32											},
	{ 0,							0											}
};
xr_token							BOOL_token								[ ]={
	{ "off",						FALSE										},
	{ "on",							TRUE										},
	{ "false",						FALSE										},
	{ "true",						TRUE										},
	{ "no",							FALSE										},
	{ "yes",						TRUE										},
	{ "0",							FALSE										},
	{ "1",							TRUE										},
	{ 0,							FALSE										}
};
xr_token							axis_token								[ ]={
	{ "x",							0											},
	{ "y",							1											},
	{ "z",							2											},
	{ "X",							0											},
	{ "Y",							1											},
	{ "Z",							2											},
	{ 0,							0											}
};

