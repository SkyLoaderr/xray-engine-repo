#include "stdafx.h"
#pragma hdrstop

#include "..\igame_level.h"
#include "..\environment.h"
#include "..\fmesh.h"

#include "ftreevisual.h"

shared_str						c_consts;
shared_str						c_wave;
shared_str						c_wind;
shared_str						c_c_bias;
shared_str						c_c_scale;
shared_str						c_c_sun;

FTreeVisual::FTreeVisual	(void)
{
	pVertices			= 0;
	pIndices			= 0;
}

FTreeVisual::~FTreeVisual	(void)
{
}

void FTreeVisual::Release	()
{
	IRender_Visual::Release	();
	_RELEASE			(pVertices);
	_RELEASE			(pIndices);
}

void FTreeVisual::Load		(const char* N, IReader *data, u32 dwFlags)
{
	IRender_Visual::Load		(N,data,dwFlags);

	D3DVERTEXELEMENT9*	vFormat	= NULL;

	// read vertices
	R_ASSERT			(data->find_chunk(OGF_VCONTAINER));
	{
		u32 ID				= data->r_u32				();
		vBase				= data->r_u32				();
		vCount				= data->r_u32				();
		vFormat				= ::Render->getVB_Format	(ID);
		pVertices			= ::Render->getVB			(ID);
		pVertices->AddRef	();
	}

	// indices
	R_ASSERT			(data->find_chunk(OGF_ICONTAINER));
	{
		dwPrimitives		= 0;
		u32 ID				= data->r_u32			();
		iBase				= data->r_u32			();
		iCount				= data->r_u32			();
		dwPrimitives		= iCount/3;
		pIndices			= ::Render->getIB		(ID);
		pIndices->AddRef	();
	}

	// load tree-def
	R_ASSERT			(data->find_chunk(OGF_TREEDEF2));
	{
		data->r			(&xform,	sizeof(xform));
		data->r			(&c_scale,	sizeof(c_scale));
		data->r			(&c_bias,	sizeof(c_bias));
	}

	// Geom
	hGeom.create		(vFormat,pVertices,pIndices);

	// Get constants
	c_consts			= "consts";
	c_wave				= "wave";
	c_wind				= "wind";
	c_c_bias			= "c_bias";
	c_c_scale			= "c_scale";
	c_c_sun				= "c_sun";
}

struct	FTreeVisual_setup
{
	u32			dwFrame;
	float		scale;
	Fvector4	wave;
	Fvector4	wind;

	FTreeVisual_setup() 
	{
		dwFrame	= 0;
	}

	void		calculate	()
	{
		dwFrame					= Device.dwFrame;

		// Calc wind-vector3, scale
		float	tm_rot			= PI_MUL_2*Device.fTimeGlobal/ps_r__Tree_w_rot;
		wind.set				(_sin(tm_rot),0,_cos(tm_rot),0);	wind.normalize	();	wind.mul(ps_r__Tree_w_amp);	// dir1*amplitude
		scale					= 1.f/float(FTreeVisual_quant);

		// setup constants
		wave.set				(ps_r__Tree_Wave.x,	ps_r__Tree_Wave.y,	ps_r__Tree_Wave.z,	Device.fTimeGlobal*ps_r__Tree_w_speed);			// wave
		wave.div				(PI_MUL_2);
	}
};

void FTreeVisual::Render	(float LOD)
{
	static FTreeVisual_setup	tvs;
	if (tvs.dwFrame!=Device.dwFrame)	tvs.calculate();

	// setup constants
	RCache.set_c			(c_consts,	tvs.scale,tvs.scale,0,0);									// consts/scale
	RCache.set_c			(c_wave,	tvs.wave);													// wave
	RCache.set_c			(c_wind,	tvs.wind);													// wind
	RCache.set_c			(c_c_scale,	c_scale.rgb.x,c_scale.rgb.y,c_scale.rgb.z,c_scale.hemi);	// scale
	RCache.set_c			(c_c_bias,	c_bias.rgb.x,c_bias.rgb.y,c_bias.rgb.z,c_bias.hemi);		// bias
	RCache.set_c			(c_c_sun,	c_scale.sun,c_bias.sun,0,0);								// sun

	// render
	// RCache.set_xform_world (xform);
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
}

#define PCOPY(a)	a = pFrom->a
void	FTreeVisual::Copy			(IRender_Visual *pSrc)
{
	IRender_Visual::Copy				(pSrc);

	FTreeVisual	*pFrom				= dynamic_cast<FTreeVisual*> (pSrc);

	PCOPY(pVertices);
	PCOPY(vBase);
	PCOPY(vCount);

	PCOPY(pIndices);
	PCOPY(iBase);
	PCOPY(iCount);
	PCOPY(dwPrimitives);

	PCOPY(xform);
	PCOPY(c_scale);
	PCOPY(c_bias);
}
