#include "stdafx.h"
#include "r_cosnstants_cache.h"

void R_constants::flush_cache(IDirect3DDevice9* D)
{
	if (a_pixel.b_dirty)
	{
		// fp
		R_constant_array::t_f&	F	= a_pixel.c_f;
		{
			u32		count		= F.r_hi()-F.r_lo();
			if (count)			{
				D->SetPixelShaderConstantF	(F.r_lo(), F.access(F.r_lo()),count);
				F.flush			();
			}
		}
	}
	if (a_vertex.b_dirty)
	{
		// fp
		R_constant_array::t_f&	F	= a_vertex.c_f;
		{
			u32		count		= F.r_hi()-F.r_lo();
			if (count)			{
				D->SetVertexShaderConstantF	(F.r_lo(), F.access(F.r_lo()),count);
				F.flush			();
			}
		}
	}
}
