#include "stdafx.h"
#pragma hdrstop

#include "r_constants_cache.h"

void R_constants::flush_cache()
{
	if (a_pixel.b_dirty)
	{
		// fp
		R_constant_array::t_f&	F	= a_pixel.c_f;
		{
			u32		count		= F.r_hi()-F.r_lo();
			if (count)			{
				PGO		(Msg("PGO:P_CONST:%d",count));
				CHK_DX	(HW.pDevice->SetPixelShaderConstantF	(F.r_lo(), (float*)F.access(F.r_lo()),count));
				F.flush	();
			}
		}
		a_pixel.b_dirty		= false;
	}
	if (a_vertex.b_dirty)
	{
		// fp
		R_constant_array::t_f&	F	= a_vertex.c_f;
		{
			u32		count		= F.r_hi()-F.r_lo();
			if (count)			{
				PGO		(Msg("PGO:V_CONST:%d",count));
				CHK_DX	(HW.pDevice->SetVertexShaderConstantF	(F.r_lo(), (float*)F.access(F.r_lo()),count));
				F.flush	();
			}
		}
		a_vertex.b_dirty	= false;
	}
}
