// Blender_Vertex_aref.h: interface for the CBlender_Vertex_aref class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_)
#define AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_Vertex_aref : public CBlender  
{
public:
	xrP_Integer	oAREF;
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: diffuse*base.aref";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Vertex_aref();
	virtual ~CBlender_Vertex_aref();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_)
