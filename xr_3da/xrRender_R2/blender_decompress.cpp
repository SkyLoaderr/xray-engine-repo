#include "stdafx.h"
#pragma hdrstop

#include "Blender_decompress.h"

CBlender_decompress::CBlender_decompress	()	{	description.CLS		= 0;	}
CBlender_decompress::~CBlender_decompress	()	{	}
 
void	CBlender_decompress::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	if (RImplementation.b_nv3x)
	{
		switch (C.iElement)
		{
		case 0:		// Position		(should speedup everything)
			C.r_Pass			("null",			"decompress_pos",	FALSE,	FALSE);
			C.r_Sampler_rtf		("s_position0",		r2_RT_DEFFER,	false,		r2_RT_DEFFER_P0		);
			C.r_Sampler_rtf		("s_position1",		r2_RT_DEFFER,	false,		r2_RT_DEFFER_P1		);
			C.r_End				();
			break;
		case 1:		// Normal		(should speedup lighting)
			C.r_Pass			("null",			"decompress_norm",	FALSE,	FALSE);
			C.r_Sampler_rtf		("s_normal",		r2_RT_DEFFER,	false,		r2_RT_DEFFER_N_H	);
			C.r_End				();
			break;
		case 2:		// Color		(should speedup bloom / combine)
			C.r_Pass			("null",			"decompress_color",	FALSE,	FALSE);
			C.r_Sampler_rtf		("s_color",			r2_RT_DEFFER,	false,		r2_RT_DEFFER_D_G	);
			C.r_End				();
			break;
		}
	}
	else
	{
	}
}
