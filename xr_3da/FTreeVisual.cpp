#include "stdafx.h"
#pragma hdrstop

#include "xr_creator.h"
#include "ftreevisual.h"
#include "fmesh.h"
#include "render.h"
#include "xr_effsun.h"

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
	IVisual::Release	();
	_RELEASE			(pVertices);
	_RELEASE			(pIndices);
}

void FTreeVisual::Load		(const char* N, IReader *data, u32 dwFlags)
{
	IVisual::Load		(N,data,dwFlags);

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
	R_ASSERT			(data->find_chunk(OGF_TREEDEF));
	{
		data->r			(&xform,	sizeof(xform));
		data->r			(&c_scale,	sizeof(c_scale));
		data->r			(&c_bias,	sizeof(c_bias));
	}

	// Geom
	hGeom				= Device.Shader.CreateGeom	(vFormat,pVertices,pIndices);

	// Get constants
	R_constant_table& T	= *(hShader->E[0]->Passes[0]->constants);
	c_consts			= T.get	("consts");
	c_wave				= T.get	("wave");
	c_wind				= T.get	("wind");
	c_c_bias			= T.get	("c_bias");
	c_c_scale			= T.get	("c_scale");
	c_eye				= T.get	("v_eye");
	c_fog				= T.get	("fog");
	c_l_dir				= T.get	("l_dir");
	c_l_color			= T.get	("l_color");
}

float	psTree_w_rot		= 10.0f;
float	psTree_w_speed		= 1.00f;
float	psTree_w_amp		= 0.01f;
Fvector psTree_Wave			= {.1f, .01f, .11f};

struct	FTreeVisual_setup
{
	u32			dwFrame;
	float		scale;
	Fvector4	wave;
	Fvector4	wind;
	Fvector4	fog_plane;
	Fvector4	fog_params;
	Fvector4	l_dir;
	Fvector4	l_color;

	FTreeVisual_setup() 
	{
		dwFrame	= 0;
	}

	void		calculate	()
	{
		dwFrame					= Device.dwFrame;

		// Calc wind-vector, scale
		float	tm_rot			= PI_MUL_2*Device.fTimeGlobal/psTree_w_rot;
		wind.set				(sinf(tm_rot),0,cosf(tm_rot),0);	wind.normalize	();	wind.mul(psTree_w_amp);	// dir1*amplitude
		scale					= 1.f/float(FTreeVisual_quant);

		// Fog
		float	f_near			= pCreator->Environment.c_FogNear;
		float	f_far			= 1/(pCreator->Environment.c_FogFar - f_near);
		Fvector4 plane;			

		// Near plane for fog
		Fmatrix& M				= Device.mFullTransform;
		plane.x					= -(M._14 + M._13);
		plane.y					= -(M._24 + M._23);
		plane.z					= -(M._34 + M._33);
		plane.w					= -(M._44 + M._43);
		float denom				= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));

		// D-Light
		CSun&	sun				= *(pCreator->Environment.Suns.front());

		// setup constants
		wave.set				(psTree_Wave.x,	psTree_Wave.y,	psTree_Wave.z,	Device.fTimeGlobal*psTree_w_speed);			// wave
		fog_plane.set			(plane.x*denom,	plane.y*denom,	plane.z*denom,	plane.w*denom);								// view-plane
		fog_params.set			(f_near,	f_far,	0,		0);																// fog-params
		l_dir.set				(-sun.Direction().x,	-sun.Direction().y,	-sun.Direction().z,	0);							// L-dir
		l_color.set				(sun.Color().r,		sun.Color().g,		sun.Color().b,			0);							// L-color
	}
};

void FTreeVisual::Render	(float LOD)
{
	static FTreeVisual_setup	tvs;
	if (tvs.dwFrame!=Device.dwFrame)	tvs.calculate();

	// setup constants
	RCache.set_c			(c_consts,	tvs.scale,tvs.scale,0,0);	// consts/scale
	RCache.set_c			(c_wave,	tvs.wave);					// wave
	RCache.set_c			(c_wind,	tvs.wind);					// wind
	RCache.set_c			(c_c_scale,	c_scale);					// scale
	RCache.set_c			(c_c_bias,	c_bias);					// bias
	RCache.set_c			(c_eye,		tvs.fog_plane);				// view-pos
	RCache.set_c			(c_fog,		tvs.fog_params);			// fog-params
	RCache.set_c			(c_l_dir,	tvs.l_dir);					// L-dir
	RCache.set_c			(c_l_color,	tvs.l_color);				// L-color

	// render
#if RENDER==R_R1
	RCache.set_xform_world	(xform);
#endif
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
}

#define PCOPY(a)	a = pFrom->a
void	FTreeVisual::Copy			(IVisual *pSrc)
{
	IVisual::Copy				(pSrc);

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

	PCOPY(xform);
	PCOPY(c_scale);
	PCOPY(c_bias);

	PCOPY(c_eye);
	PCOPY(c_fog);

	PCOPY(c_l_dir);
	PCOPY(c_l_color);
}
