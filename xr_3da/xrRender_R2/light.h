#pragma once

class light		:	public IRender_Light
{
public:
	struct {
		u32			bStatic	:	1;
		u32			bActive	:	1;
		u32			bShadow	:	1;
	}				flags;
	Fsphere			sphere;
	Fcolor			color;
	u32				controller;

	u32				dwFrame;
public:
	virtual void	set_active		(bool b);
	virtual void	set_shadow		(bool b)						{ flags.bShadow=b;		}
	virtual void	set_position	(const Fvector& P)				{ sphere.P.set(P);		}
	virtual void	set_range		(float R)						{ sphere.R = R;			}
	virtual void	set_color		(const Fcolor& C)				{ color.set(C);			}
	virtual void	set_color		(float r, float g, float b)		{ color.set(r,g,b,1);	}

	light();
	virtual ~light();
};
