// LightPPA.h: interface for the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
#define AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_
#pragma once

#include "light.h"

struct	CLightPPA_Vertex
{
	Fvector			P;
	Fvector			N;
	float			u0,v0;
	float			u1,v1;
};

class	CLightPPA_Manager
{
	ref_shader						hShader;
	ref_geom						hGeom;
public:
	CLightPPA_Manager				();
	virtual ~CLightPPA_Manager		();

	CLightPPA*		Create			();
	void			Destroy			(CLightPPA*);

	void			Activate		(CLightPPA* L);
	void			Deactivate		(CLightPPA* L);

	void			Render			();
};

#endif // !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
