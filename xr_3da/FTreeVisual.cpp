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

float	psTree_w_rot		= 10.0f;
float	psTree_w_speed		= 1.00f;
float	psTree_w_amp		= 0.01f;
Fvector psTree_Wave			= {.1, .01, .11};

void FTreeVisual::Render	(float LOD)
{
	// Calc wind-vector, scale
	Fvector4 wind;
	float	tm_rot			= PI_MUL_2*Device.fTimeGlobal/psTree_w_rot;
	wind.set				(sinf(tm_rot),0,cosf(tm_rot),0);	wind.normalize	();	wind.mul(psTree_w_amp);	// dir1*amplitude
	float	scale			= 1.f/float(FTreeVisual_quant);

	// setup constants
	RCache.set_c			(c_consts,	scale,		scale,		0,					0);
	RCache.set_c			(c_wave,	psTree_Wave.x,	psTree_Wave.y,	psTree_Wave.z,	Device.fTimeGlobal*psTree_w_speed);	// wave
	RCache.set_c			(c_wind,	wind);																			// wind
	RCache.set_c			(c_c_scale,	c_scale);																		// scale
	RCache.set_c			(c_c_bias,	c_bias);																		// bias
	RCache.set_c			(c_m_m2w,	xform);																			// xform
	RCache.set_c			(c_m_w2v2p,	Device.mFullTransform);															// view-projection

	// render
	RCache.set_Shader		(hShader);
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
	RCache.set_VS			(0);
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
