// IRender_Visual.cpp: implementation of the IRender_Visual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "render.h"
#include "fbasicvisual.h"
#include "fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IRender_Visual::IRender_Visual		()
{
	Type				= 0;
	hShader				= 0;
	hGeom				= 0;
 
	vis.clear			();
}

IRender_Visual::~IRender_Visual		()
{
}

void IRender_Visual::Release		()
{
}

CStatTimer						tscreate;

void IRender_Visual::Load		(const char* N, IReader *data, u32 dwFlags)
{
	// header
	VERIFY		(data);
	ogf_header	hdr;
	if (data->r_chunk_safe(OGF_HEADER,&hdr,sizeof(hdr)))
	{
		R_ASSERT			(hdr.format_version==xrOGF_FormatVersion);
		Type				= hdr.type;
		if (hdr.shader_id)	hShader	= ::Render->getShader(hdr.shader_id);
	} else {
		Debug.fatal			("Invalid visual");
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
		Debug.fatal		("Invalid visual");
	}

	// Sphere (if exists)
	if (data->find_chunk(OGF_BSPHERE))
	{
		data->r(&vis.sphere.P,3*sizeof(float));
		data->r(&vis.sphere.R,sizeof(float));
	}

	// Shader
	if (data->find_chunk(OGF_TEXTURE)) {
		string256		fnT,fnS;
		data->r_stringZ	(fnT);
		data->r_stringZ	(fnS);
		hShader.create	(fnS,fnT);
	}

    // desc
#ifdef _EDITOR
    if (data->find_chunk(OGF_DESC)) 
	    desc.Load		(*data);
#endif
}

#define PCOPY(a)	a = pFrom->a
void	IRender_Visual::Copy(IRender_Visual *pFrom)
{
	PCOPY(Type);
	PCOPY(hShader);
	PCOPY(hGeom);
	PCOPY(vis);
#ifdef _EDITOR
	PCOPY(desc);
#endif
}
