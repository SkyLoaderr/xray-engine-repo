// LightPPA.h: interface for the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
#define AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_
#pragma once

struct ENGINE_API	PPA_Vertex
{
	Fvector			P;
	Fvector			N;
	float			u0,v0;
	float			u1,v1;
};

class ENGINE_API	CLightPPA  
{
	Fsphere			sphere;
	Fcolor			color;

	Flight			D3D_light;
public:
	CLightPPA		();
	~CLightPPA		();

	IC void			SetPosition		(Fvector& P)	{ sphere.P.set(P);	}
	IC void			SetRange		(float R)		{ sphere.R = R;		}
	IC void			SetColor		(Fcolor& C)		{ color.set(C);		}

	void			Render			(CList<PPA_Vertex>&	vlist);
};

#endif // !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
