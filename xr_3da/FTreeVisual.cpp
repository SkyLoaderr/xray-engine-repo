#include "stdafx.h"
#pragma hdrstop

#include "ftreevisual.h"
#include "fmesh.h"
#include "render.h"

FTreeVisual::FTreeVisual(void)
{
	pVertices			= 0;
	pIndices			= 0;
}

FTreeVisual::~FTreeVisual(void)
{
}

void FTreeVisual::Release	()
{
	CVisual::Release	();
	_RELEASE			(pVertices);
	_RELEASE			(pIndices);
}

void FTreeVisual::Load		(const char* N, CStream *data, u32 dwFlags)
{
	CVisual::Load		(N,data,dwFlags);

	D3DVERTEXELEMENT9*	vFormat	= NULL;

	// read vertices
	R_ASSERT			(data->FindChunk(OGF_VCONTAINER));
	{
		u32 ID				= data->Rdword				();
		vBase				= data->Rdword				();
		vCount				= data->Rdword				();
		vFormat				= ::Render->getVB_Format	(ID);
		pVertices			= ::Render->getVB			(ID);
		pVertices->AddRef	();
	}

	// indices
	R_ASSERT			(data->FindChunk(OGF_ICONTAINER));
	{
		dwPrimitives		= 0;
		u32 ID				= data->Rdword			();
		iBase				= data->Rdword			();
		iCount				= data->Rdword			();
		dwPrimitives		= iCount/3;
		pIndices			= ::Render->getIB		(ID);
		pIndices->AddRef	();
	}

	// load tree-def
	R_ASSERT			(data->FindChunk(OGF_TREEDEF));
	{
		data->Read			(&xform,	sizeof(xform));
		data->Read			(&c_scale,	sizeof(c_scale));
		data->Read			(&c_bias,	sizeof(c_bias));
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
	c_m_m2w				= T.get	("m_m2w");
	c_m_w2v2p			= T.get	("m_w2v2p");
}

float	psTree_w_rot1		= 30.f;
float	psTree_w_speed		= 2.f;
float	psTree_w_amp		= .1f;

void FTreeVisual::Render	(float LOD)
{
	// Calc wind-vector, scale
	Fvector4 wind;
	float	tm_rot1			= PI_MUL_2*Device.fTimeGlobal/psTree_w_rot1;
	wind.set				(sinf(tm_rot1),0,cosf(tm_rot1),0);	wind.normalize	();	wind.mul(psTree_w_amp);	// dir1*amplitude
	float	scale			= 1.f/float(FTreeVisual_quant);

	// setup constants
	RCache.set_c			(c_consts,	scale,		scale,		0,					0);
	RCache.set_c			(c_wave,	1.f/5.f,	1.f/7.f,	1.f/3.f,			Device.fTimeGlobal*psTree_w_speed);	// wave
	RCache.set_c			(c_wind,	wind);																			// wind
	RCache.set_c			(c_c_scale,	c_scale);																		// scale
	RCache.set_c			(c_c_bias,	c_bias);																		// bias
	RCache.set_c			(c_m_m2w,	xform);																			// xform
	RCache.set_c			(c_m_w2v2p,	Device.mFullTransform);															// view-projection

	// render
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
}

#define PCOPY(a)	a = pFrom->a
void	FTreeVisual::Copy			(CVisual *pSrc)
{
	CVisual::Copy				(pSrc);

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
	PCOPY(c_m_m2w);
	PCOPY(c_m_w2v2p);

	PCOPY(xform);
	PCOPY(c_scale);
	PCOPY(c_bias);
}
