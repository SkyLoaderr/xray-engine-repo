// FVisual.h: interface for the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_)
#define AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_
#pragma once

#include "../fbasicvisual.h"

class	Fvisual		: public IRender_Visual
{
public:
	struct _mesh	
	{
		// format
		ref_geom					geom;

		// verts
		IDirect3DVertexBuffer9*		pVertices;
		u32							vBase;
		u32							vCount;
		
		// indices
		IDirect3DIndexBuffer9*		pIndices;
		u32							iBase;
		u32							iCount;
		u32							dwPrimitives;

		_mesh()						{ pVertices=0; pIndices=0;					}
		~_mesh()					{ _RELEASE(pVertices); _RELEASE(pIndices);	}
	};
	_mesh						m_base	;
	_mesh						m_fast	;	
public:
	virtual void				Render			(float LOD		);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
	virtual void				Render_Fast		(float LOD		);		// LOD - Level Of Detail  [0..1], Ignored
	virtual void				Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void				Copy			(IRender_Visual *pFrom	);
	virtual void				Release			();

	Fvisual();
	virtual ~Fvisual();
};

#endif // !defined(AFX_FVISUAL_H__9ACFCFC1_8091_11D3_83D8_00C02610C34E__INCLUDED_)
