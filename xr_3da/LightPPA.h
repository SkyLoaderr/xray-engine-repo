// LightPPA.h: interface for the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
#define AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_
#pragma once

#include "xr_list.h"

struct ENGINE_API	PPA_Vertex
{
	Fvector			P;
	Fvector			N;
	float			u0,v0;
	float			u1,v1;
};
typedef CList<PPA_Vertex>	PPA_VB;


class ENGINE_API	CLightPPA  
{
public:
	Fsphere			sphere;
	Fcolor			color;
public:
	CLightPPA		();
	~CLightPPA		();

	IC void			SetPosition		(Fvector& P)	{ sphere.P.set(P);	}
	IC void			SetRange		(float R)		{ sphere.R = R;		}
	IC void			SetColor		(Fcolor& C)		{ color.set(C);		}

	void			Render			(PPA_VB& vlist);
};

class ENGINE_API	CLightPPA_Manager
{
	svector<CLightPPA*,128>			container;
	PPA_VB							storage;
	CVertexStream*					VS;
public:
	void			Initialize		();

	IC void			Add				(CLightPPA* L)	{ container.push_back(L);	}
	void			Render			();
};

#endif // !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
