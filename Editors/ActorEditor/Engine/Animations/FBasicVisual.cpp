// IVisual.cpp: implementation of the IVisual class.
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

IVisual::IVisual		()
{
	Type				= 0;
	hShader				= 0;
	hGeom				= 0;
 
	vis.clear			();
}

IVisual::~IVisual		()
{
}

void IVisual::Release	()
{
	Device.Shader.Delete		(hShader);
	Device.Shader.DeleteGeom	(hGeom	);
}

void IVisual::Load		(const char* N, IReader *data, u32 dwFlags)
{
	// header
	VERIFY(data);
	ogf_header hdr;
	if (data->r_chunk_safe(OGF_HEADER,&hdr,sizeof(hdr)))
	{
		VERIFY	(hdr.format_version==xrOGF_FormatVersion);
		Type	= hdr.type;
	} else {
		Debug.fatal	("Invalid visual");
	}

	// BBox
	ogf_bbox bbox;
	if (data->r_chunk_safe(OGF_BBOX,&bbox,sizeof(bbox)))
	{
		vis.box.set		(bbox.min,bbox.max);
		vis.sphere.P.sub(bbox.max,bbox.min);
		vis.sphere.P.div(2);
		vis.sphere.R	= vis.sphere.P.magnitude();
		vis.sphere.P.add(bbox.min);
		vis.box.grow	(EPS_S);
	} else {
		Debug.fatal	("Invalid visual");
	}

	// Sphere (if exists)
	if (data->find_chunk(OGF_BSPHERE))
	{
		data->r(&vis.sphere.P,3*sizeof(float));
		data->r(&vis.sphere.R,sizeof(float));
	}

	// textures
	if (data->find_chunk(OGF_TEXTURE_L)) {
#ifndef _EDITOR
		u32 T = data->r_u32();
		u32 S = data->r_u32();
		hShader = pCreator->LL_CreateShader(S,T,-1,-1);
#endif
	} else {
		if (data->find_chunk(OGF_TEXTURE)) {
			string256 fnT,fnS;
			data->r_stringZ(fnT);
			data->r_stringZ(fnS);
			hShader = Device.Shader.Create(fnS,fnT);
		} else {
			hShader = 0; // Device.Shader.Create("null","$null");
		}
	}
}

#define PCOPY(a)	a = pFrom->a
void	IVisual::Copy(IVisual *pFrom)
{
	PCOPY(Type);
	PCOPY(hShader);
	PCOPY(hGeom);
	PCOPY(vis);
}

