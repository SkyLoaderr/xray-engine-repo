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
	shader				= 0;
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
#ifdef DEBUG
	dbg_name	= N;
#endif

	// header
	VERIFY		(data);
	ogf_header	hdr;
	if (data->r_chunk_safe(OGF_HEADER,&hdr,sizeof(hdr)))
	{
		R_ASSERT2			(hdr.format_version==xrOGF_FormatVersion, "Invalid visual version");
		Type				= hdr.type;
		if (hdr.shader_id)	hShader	= ::Render->getShader(hdr.shader_id);
		vis.box.set			(hdr.bb.min,hdr.bb.max	);	
		vis.sphere.set		(hdr.bs.c,	hdr.bs.r	);
	} else {
		Debug.fatal			("Invalid visual");
	}

	// Shader
	if (data->find_chunk(OGF_TEXTURE)) {
		string256		fnT,fnS;
		data->r_stringZ	(fnT,sizeof(fnT));
		data->r_stringZ	(fnS,sizeof(fnS));
		hShader.create	(fnS,fnT);
	}

    // desc
#ifdef _EDITOR
    if (data->find_chunk(OGF_S_DESC)) 
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
