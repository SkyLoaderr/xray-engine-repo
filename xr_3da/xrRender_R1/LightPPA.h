// LightPPA.h: interface for the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
#define AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_
#pragma once

struct	CLightPPA_Vertex
{
	Fvector			P;
	Fvector			N;
	float			u0,v0;
	float			u1,v1;
};

class	CLightPPA	:	public IRender_Light
{
public:
	bool				bActive;
	Fsphere				sphere;
	Fcolor				color;
public:
	CLightPPA			();
	virtual ~CLightPPA	();

	virtual void	set_type		(LT type)						{ };
	virtual void	set_active		(bool b);
	virtual bool	get_active		()								{ return bActive;	}
	virtual void	set_shadow		(bool)							{ };
	virtual void	set_position	(const Fvector& P)				{ sphere.P.set(P);	}
	virtual void	set_direction	(const Fvector& P)				{};
	virtual void	set_cone		(float angle)					{};
	virtual void	set_range		(float R)						{ sphere.R = R;		}
	virtual void	set_color		(const Fcolor& C)				{ color.set(C);		}
	virtual void	set_color		(float r, float g, float b)		{ color.set(r,g,b,1); }
	virtual void	set_texture		(LPCSTR name)					{};

	void			Render			(ref_geom& hGeom);
};

class CLightPPA_Manager
{
	xr_set<CLightPPA*>				active;
	xr_set<CLightPPA*>				inactive;
	
	ref_shader						hShader;
	ref_geom						hGeom;
public:
	CLightPPA_Manager	();
	~CLightPPA_Manager	();

	CLightPPA*		Create			();
	void			Destroy			(CLightPPA*);

	void			Activate		(CLightPPA* L);
	void			Deactivate		(CLightPPA* L);

	void			Render			();
};

#endif // !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
