// FBasicVisual.h: interface for the FBasicVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#define VLOAD_NOVERTICES		(1<<0)
#define VLOAD_NOINDICES			(1<<1)
#define VLOAD_FORCESOFTWARE		(1<<2)

// The class itself
class ENGINE_API			CVisual
{
public:
	u32						Type;					// visual's type
	u32						dwFrame;				// mark

	// Common data for rendering
	Shader*					hShader;				// pipe state, shared
	SGeometry*				hGeom;					// vertex shader, shared
	Fvector					bv_Position;
	float					bv_Radius;
	Fbox					bv_BBox;

	virtual void			Render					(float LOD)		{};		// LOD - Level Of Detail  [0..1], Ignored
	virtual void			Load					(const char* N, CStream *data, u32 dwFlags);
	virtual void			Release					();						// Shared memory release
	virtual void			Copy					(CVisual* from);

	CVisual			();
	virtual ~CVisual();
};

#endif // !defined(AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_)
