#include "stdafx.h"
#pragma hdrstop

#include "Blender_fat_flat.h"

CBlender_fat_flat::CBlender_fat_flat	()	{	description.CLS		= B_DEFAULT;	}
CBlender_fat_flat::~CBlender_fat_flat	()	{	}

void	CBlender_fat_flat::Save	(	IWriter& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_fat_flat::Load	(	IReader& FS, WORD version )
{
	CBlender::Load	(FS,version);
}
void	CBlender_fat_flat::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	C.PassBegin				();
	C.PassSET_ZB			(TRUE,TRUE);
	C.PassSET_Blend_SET		();
	C.PassSET_LightFog		(FALSE,FALSE);
	C.StageBegin			();
	C.StageEnd				();
	C.PassEnd				();
}
