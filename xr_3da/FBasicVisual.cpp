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
	hVS					= 0;

	bv_Radius			= 0.f;
	bv_Position.set		(0,0,0);
	bv_BBox.invalidate	();

	dwFrame				= 0xfffffffe;
}

CVisual::~CVisual		()
{
}

void CVisual::Release	()
{
	Device.Shader.Delete	(hShader);
	Device.Shader._DeleteVS	(hVS);
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
		bv_BBox.set		(bbox.min,bbox.max);
		bv_Position.sub	(bbox.max,bbox.min);
		bv_Position.div	(2);
		bv_Radius		= bv_Position.magnitude();
		bv_Position.add	(bbox.min);
		bv_BBox.grow	(EPS_S);
	} else {
		THROW;
	}
	Log	("---");
	Log	("pos",bv_Position);
	Log	("r  ",bv_Radius);

	// Sphere (if exists)
	if (data->FindChunk(OGF_BSPHERE))
	{
		data->Read(&bv_Position,3*sizeof(float));
		data->Read(&bv_Radius,sizeof(float));
	}

	if ((*LPDWORD(&bv_Radius) == 0xffc00000)||fis_gremlin(bv_Radius)||fis_denormal(bv_Radius))
	{
		bv_Position.sub	(bbox.max,bbox.min);
		bv_Position.div	(2);
		bv_Radius		= bv_Position.magnitude();
		bv_Position.add	(bbox.min);
	}

	Log	("---2n");
	Log	("pos",bv_Position);
	Log	("r  ",bv_Radius);

	// textures
	if (data->FindChunk(OGF_TEXTURE_L)) {
#ifndef _EDITOR
		u32 T = data->Rdword();
		u32 S = data->Rdword();
		hShader = pCreator->LL_CreateShader(S,T,-1,-1);
#endif
	} else {
		if (data->FindChunk(OGF_TEXTURE)) {
			FILE_NAME fnT,fnS;
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
	PCOPY(hVS);
	PCOPY(bv_Position);
	PCOPY(bv_Radius);
	PCOPY(bv_BBox);
}

