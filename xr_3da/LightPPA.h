// LightPPA.h: interface for the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
#define AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_
#pragma once

#include "xr_list.h"

struct ENGINE_API	CLightPPA_Vertex
{
	Fvector			P;
	Fvector			N;
	float			u0,v0;
	float			u1,v1;
};

class ENGINE_API	CLightPPA  
{
public:
	Fsphere			sphere;
	Fcolor			color;
public:
	CLightPPA		();
	~CLightPPA		();

	IC void			SetPosition		(const Fvector& P)				{ sphere.P.set(P);	}
	IC void			SetRange		(float R)						{ sphere.R = R;		}
	IC void			SetColor		(const Fcolor& C)				{ color.set(C);		}
	IC void			SetColor		(float r, float g, float b)		{ color.set(r,g,b,1); }

	void			Render			(CVertexStream* VS);
};

class ENGINE_API	CLightPPA_Manager
{
	svector<CLightPPA*,128>			container;
	
	Shader*							SH;
	CVertexStream*					VS;
public:
	void			Initialize		();
	void			Destroy			();

	IC void			Add				(CLightPPA* L)	{ container.push_back(L);	}
	void			Render			();
};

#endif // !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
