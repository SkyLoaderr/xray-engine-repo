#pragma once

#include "r_constants.h"

class	R_shader
{
	IDirect3DPixelShader9*		ps;
	IDirect3DVertexShader9*		vs;
	R_constant_table			constants;
public:
	BOOL						compile		(LPCSTR name);
};
