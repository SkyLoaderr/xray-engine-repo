// Blender_Vertex.h: interface for the CBlender_Vertex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_VERTEX_H__D3B42F77_7018_4672_B6A5_6EE6BD947662__INCLUDED_)
#define AFX_BLENDER_VERTEX_H__D3B42F77_7018_4672_B6A5_6EE6BD947662__INCLUDED_
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_Vertex : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "base vertex blender";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);

	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices);

	CBlender_Vertex();
	virtual ~CBlender_Vertex();
};

#endif // !defined(AFX_BLENDER_VERTEX_H__D3B42F77_7018_4672_B6A5_6EE6BD947662__INCLUDED_)
