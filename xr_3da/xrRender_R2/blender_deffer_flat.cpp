#include "stdafx.h"
#pragma hdrstop

#include "Blender_deffer_flat.h"

CBlender_deffer_flat::CBlender_deffer_flat	()	{	description.CLS		= B_DEFAULT;	}
CBlender_deffer_flat::~CBlender_deffer_flat	()	{	}

void	CBlender_deffer_flat::Save	(	IWriter& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_deffer_flat::Load	(	IReader& FS, WORD version )
{
	CBlender::Load	(FS,version);
}
void	CBlender_deffer_flat::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	C.r2_Pass				("deffer_flat_base","deffer_flat_base");
	C.r2_Sampler			("s_base",C.L_textures[0]);
	C.r2_End				();
}
