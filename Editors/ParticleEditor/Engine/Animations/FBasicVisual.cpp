// CVisual.cpp: implementation of the CVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "fbasicvisual.h"
#include "fmesh.h"
#ifndef _EDITOR
	#include "xr_creator.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVisual::CVisual		()
{
	Type				= 0;
	hShader				= 0;
	hGeom				= 0;
 
	vis.clear			();
}

CVisual::~CVisual		()
{
}

void CVisual::Release	()
{
	Device.Shader.Delete		(hShader);
	Device.Shader.DeleteGeom	(hGeom	);
}

void CVisual::Load		(const char* N, CStream *data, u32 dwFlags)
{
	// header
	VERIFY(data);
	ogf_header hdr;
	if (data->ReadChunkSafe(OGF_HEADER,&hdr,sizeof(hdr)))
	{
		VERIFY	(hdr.format_version==xrOGF_FormatVersion);
		Type	= hdr.type;
	} else {
		THROW;
	}

	// BBox
	ogf_bbox bbox;
	if (data->ReadChunkSafe(OGF_BBOX,&bbox,sizeof(bbox)))
	{
		vis.box.set		(bbox.min,bbox.max);
		vis.sphere.P.sub(bbox.max,bbox.min);
		vis.sphere.P.div(2);
		vis.sphere.R	= vis.sphere.P.magnitude();
		vis.sphere.P.add(bbox.min);
		vis.box.grow	(EPS_S);
	} else {
		THROW;
	}

	// Sphere (if exists)
	if (data->FindChunk(OGF_BSPHERE))
	{
		data->Read(&vis.sphere.P,3*sizeof(float));
		data->Read(&vis.sphere.R,sizeof(float));
	}

	// textures
	if (data->FindChunk(OGF_TEXTURE_L)) {
#ifndef _EDITOR
		u32 T = data->Rdword();
		u32 S = data->Rdword();
		hShader = pCreator->LL_CreateShader(S,T,-1,-1);
#endif
	} else {
		if (data->FindChunk(OGF_TEXTURE)) {
			string256 fnT,fnS;
			data->RstringZ(fnT);
			data->RstringZ(fnS);
			hShader = Device.Shader.Create(fnS,fnT);
		} else {
			hShader = 0; // Device.Shader.Create("null","$null");
		}
	}
}

#define PCOPY(a)	a = pFrom->a
void	CVisual::Copy(CVisual *pFrom)
{
	PCOPY(Type);
	PCOPY(hShader);
	PCOPY(hGeom);
	PCOPY(vis);
}

