#include "stdafx.h"
#pragma hdrstop

#include "..\igame_level.h"
#include "..\environment.h"
#include "..\fmesh.h"
#include "..\xr_effsun.h"

#include "ftreevisual.h"

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
		Msg				("scale: %f,%f, bias: %f,%f",c_scale.hemi,c_scale.sun,c_bias.hemi,c_bias.sun);
	}

	// Geom
	hGeom.create		(vFormat,pVertices,pIndices);

	// Get constants
	R_constant_table& T	= *(hShader->E[0]->Passes[0]->constants);
	c_consts			= T.get	("consts");
	c_wave				= T.get	("wave");
	c_wind				= T.get	("wind");
	c_c_bias			= T.get	("c_bias");
	c_c_scale			= T.get	("c_scale");
	c_c_sun				= T.get	("c_sun");
}

struct	FTreeVisual_setup
{
	u32			dwFrame;
	float		scale;
	Fvector4	wave;
	Fvector4	wind;
	Fvector4	l_dir;
	Fvector4	l_color;

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

		// D-Light
		CSun&	sun				= *(g_pGamePersistant->Environment.Suns.front());

		// setup constants
		wave.set				(ps_r__Tree_Wave.x,	ps_r__Tree_Wave.y,	ps_r__Tree_Wave.z,	Device.fTimeGlobal*ps_r__Tree_w_speed);			// wave
		l_dir.set				(-sun.Direction().x,	-sun.Direction().y,	-sun.Direction().z,	0);							// L-dir
		l_color.set				(sun.Color().r,		sun.Color().g,		sun.Color().b,			0);							// L-color
	}
};

void FTreeVisual::Render	(float LOD)
{
	static FTreeVisual_setup	tvs;
	if (tvs.dwFrame!=Device.dwFrame)	tvs.calculate();


#pragma todo("5color scale/bias")
	// setup constants
	RCache.set_c			(c_consts,	tvs.scale,tvs.scale,0,0);									// consts/scale
	RCache.set_c			(c_wave,	tvs.wave);													// wave
	RCache.set_c			(c_wind,	tvs.wind);													// wind
	RCache.set_c			(c_c_scale,	c_scale.rgb.x,c_scale.rgb.y,c_scale.rgb.z,c_scale.hemi);	// scale
	RCache.set_c			(c_c_bias,	c_bias.rgb.x,c_bias.rgb.y,c_bias.rgb.z,c_bias.hemi);		// bias
	RCache.set_c			(c_c_sun,	c_scale.sun,c_bias.sun,0,0);								// sun

	// render
#if RENDER==R_R1
	RCache.set_xform_world	(xform);
#endif
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

	PCOPY(c_consts);
	PCOPY(c_wave);
	PCOPY(c_wind);
	PCOPY(c_c_bias);
	PCOPY(c_c_scale);
	PCOPY(c_c_sun);

	PCOPY(xform);
	PCOPY(c_scale);
	PCOPY(c_bias);
}
