// FBasicVisual.cpp: implementation of the FBasicVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FBasicVisual.h"
#include "fmesh.h"
#include "xr_creator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FBasicVisual::FBasicVisual()
{
	Type				= 0;
	hShader				= 0;
	bv_Radius			= 0.f;
	bv_Position.set		(0,0,0);
	bv_BBox.invalidate	();

	dwFrame				= 0xfffffffe;
}

FBasicVisual::~FBasicVisual()
{
}

void FBasicVisual::Release()
{
	Device.Shader.Delete(hShader);
}

void FBasicVisual::Load(const char* N, CStream *data, DWORD dwFlags)
{
	// header
	VERIFY(data);
	ogf_header hdr;
	if (data->ReadChunk(OGF_HEADER,&hdr))
	{
		VERIFY(hdr.format_version==xrOGF_FormatVersion);
		Type			= hdr.type;
	} else {
		THROW;
	}

	// BBox
	ogf_bbox bbox;
	if (data->ReadChunk(OGF_BBOX,&bbox))
	{
		bv_BBox.set		(bbox.min,bbox.max);
		bv_Position.sub	(bbox.max,bbox.min);
		bv_Position.div	(2);
		bv_Radius		= bv_Position.magnitude();
		bv_Position.add	(bbox.min);
	} else {
		THROW;
	}

	// Sphere (if exists)
	if (data->FindChunk(OGF_BSPHERE))
	{
		data->Read(&bv_Position,3*sizeof(float));
		data->Read(&bv_Radius,sizeof(float));
	}

	if ((*((DWORD*)&bv_Radius)==0xffc00000)||(pCreator->Environment.pm_bug))
	{
		bv_Position.sub	(bbox.max,bbox.min);
		bv_Position.div	(2);
		bv_Radius		= bv_Position.magnitude();
		bv_Position.add	(bbox.min);
	}

	// textures
	if (data->FindChunk(OGF_TEXTURE_L)) {
		DWORD T = data->Rdword();
		DWORD S = data->Rdword();
		hShader = pCreator->LL_CreateShader(S,T);
	} else {
		if (data->FindChunk(OGF_TEXTURE)) {
			FILE_NAME fnT,fnS;
			data->RstringZ(fnT);
			data->RstringZ(fnS);
			hShader = Device.Shader.Create(fnS,fnT);
		} else {
			hShader = Device.Shader.Create("null","$null");
		}
	}
}

#define PCOPY(a)	a = pFrom->a
void	FBasicVisual::Copy(FBasicVisual *pFrom)
{
	PCOPY(Type);
	PCOPY(hShader);
	PCOPY(bv_Position);
	PCOPY(bv_Radius);
	PCOPY(bv_BBox);
}
