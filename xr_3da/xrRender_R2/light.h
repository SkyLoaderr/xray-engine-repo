#pragma once

class light		:	public IRender_Light
{
public:
	struct {
		u32			bActive	:	1;
		u32			bShadow	:	1;
	}				flags;
	Fsphere			sphere;
	Fcolor			color;
	u32				controller;

	u32				dwFrame;
public:
	virtual void	set_mode		(u32 M);
	virtual void	set_position	(const Fvector& P)				{ sphere.P.set(P);		}
	virtual void	set_range		(float R)						{ sphere.R = R;			}
	virtual void	set_color		(const Fcolor& C)				{ color.set(C);			}
	virtual void	set_color		(float r, float g, float b)		{ color.set(r,g,b,1);	}

	light();
	virtual ~light();
};
