#include "stdafx.h"
#include "DisablingParams.h"



SAllDDWParams	worldDisablingParams	=	
	{
	//object
		{
			{0.01f	,	0.1f	}		,	//translational		vel	,	accel
			{0.01f	,	0.6f	}		,	//rotational		vel	,	accel
			10								//level2 frames
		}								,								
		2.f									//reanable factor
	};



void SOneDDOParams::Mul(float v)
{
	velocity			*=	v			;
	acceleration		*=	v			;
}

void SAllDDOParams::Reset()
{
	*this=worldDisablingParams.objects_params;
}

void SAllDDOParams::Load(CInifile* ini)
{
	if(!ini)							return;
	if(!ini->section_exist("disable"))  return;
	Reset();
	if(ini->line_exist("disable","linear_factor"))	translational	.Mul(ini->r_float("disable","linear_factor"))		;
	if(ini->line_exist("disable","angular_factor"))	rotational		.Mul(ini->r_float("disable","angular_factor"))		;
}