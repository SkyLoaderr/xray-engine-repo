#pragma once

#include "..\ispatial.h"
#include "light_smapvis.h"
#include "light_GI.h"

class	light		:	public IRender_Light, public ISpatial
{
public:
	struct {
		u32			type	:	4;
		u32			bStatic	:	1;
		u32			bActive	:	1;
		u32			bShadow	:	1;
	}				flags;
	Fvector			position;
	Fvector			direction;
	Fvector			right;
	float			range;
	float			cone;
	Fcolor			color;

	u32				frame_render;

#if RENDER==R_R2
	xr_vector<light_indirect>	indirect;
	u32							indirect_photons;

	smapvis			svis;

	ref_shader		s_spot;
	ref_shader		s_point;

	u32				m_xform_frame;
	Fmatrix			m_xform;

	struct _vis		{
		u32			frame2test;		// frame the test is sheduled to
		u32			query_id;		// ID of occlusion query
		u32			query_order;	// order of occlusion query
		bool		visible;		// visible/invisible
		bool		pending;		// test is still pending
	}				vis;

	union			_xform	{
		struct		_D		{
			Fmatrix						view;
			Fmatrix						project;
			Fmatrix						combine;
		}	D;
		struct		_P		{
			Fmatrix						world;
			Fmatrix						view;
			Fmatrix						project;
			Fmatrix						combine;
		}	P;
		struct		_S		{
			Fmatrix						view;
			Fmatrix						project;
			Fmatrix						combine;
			u32							size;
			u32							posX;
			u32							posY;
		}	S;
	}	X;
#endif

public:
	virtual void	set_type				(LT type)						{ flags.type = type;		}
	virtual void	set_active				(bool b);
	virtual bool	get_active				()								{ return flags.bActive;		}
	virtual void	set_shadow				(bool b)						{ flags.bShadow=b;			}
	virtual void	set_position			(const Fvector& P);
	virtual void	set_rotation			(const Fvector& D, const Fvector& R);
	virtual void	set_cone				(float angle);
	virtual void	set_range				(float R);
	virtual void	set_virtual_size		(float R)						{};
	virtual void	set_color				(const Fcolor& C)				{ color.set(C);				}
	virtual void	set_color				(float r, float g, float b)		{ color.set(r,g,b,1);		}
	virtual void	set_texture				(LPCSTR name);

	virtual	void	spatial_move			();
	virtual	Fvector	spatial_sector_point	();

#if RENDER==R_R2
	void			gi_generate				();
	void			xform_calc				();
	void			vis_prepare				();
	void			vis_update				();
#endif

	light();
	virtual ~light();
};
