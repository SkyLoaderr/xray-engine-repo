#pragma once

#include "..\ispatial.h"

class light		:	public IRender_Light, public ISpatial
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
	float			range;
	float			cone;
	Fcolor			color;

	ref_shader		s_spot;
	ref_shader		s_point;

	u32				dwFrame;
public:
	virtual void	set_type		(LT type)						{ flags.type = type;		}
	virtual void	set_active		(bool b);
	virtual bool	get_active		()								{ return flags.bActive;		}
	virtual void	set_shadow		(bool b)						{ flags.bShadow=b;			}
	virtual void	set_position	(const Fvector& P);
	virtual void	set_direction	(const Fvector& D);
	virtual void	set_cone		(float angle);
	virtual void	set_range		(float R);
	virtual void	set_color		(const Fcolor& C)				{ color.set(C);				}
	virtual void	set_color		(float r, float g, float b)		{ color.set(r,g,b,1);		}
	virtual void	set_texture		(LPCSTR name);

	virtual	void	spatial_move		();
	virtual	Fvector	spatial_sector_point();

	light();
	virtual ~light();
};
