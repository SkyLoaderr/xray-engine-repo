#pragma once
#include "fhierrarhyvisual.h"

class ENGINE_API FLOD	:	public FHierrarhyVisual
{
	typedef FHierrarhyVisual inherited;
public:
	struct _vertex
	{
		Fvector		v;
		Fvector2	t;
		DWORD		c;
	};
	struct _face
	{
		_vertex		v[4];
		Fvector		N;
	};

	_face			facets		[8];
public:
	virtual void Render			(float LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load			(LPCSTR N, CStream *data, DWORD dwFlags);
	virtual void Copy			(CVisual *pFrom	);
};
