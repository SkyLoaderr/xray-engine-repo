// Blender_Vertex.h: interface for the CBlender_LIGHT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_VERTEX_H__D3B42F77_7018_4672_B6A5_6EE6BD947662__INCLUDED_1)
#define AFX_BLENDER_VERTEX_H__D3B42F77_7018_4672_B6A5_6EE6BD947662__INCLUDED_1
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_LIGHT : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: lighting effect";	}

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_LIGHT();
	virtual ~CBlender_LIGHT();
};

#endif // !defined(AFX_BLENDER_VERTEX_H__D3B42F77_7018_4672_B6A5_6EE6BD947662__INCLUDED_1)
