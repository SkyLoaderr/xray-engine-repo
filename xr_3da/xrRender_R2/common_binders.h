#pragma once

// binders
struct cl_vecXYZ0	: public R_constant_setup {
	virtual		void setup (R_constant* C)	{	RCache.set_c	(C,P.x,P.y,P.z,0);	}

	Fvector4	P;
};

struct cl_binders
{
	cl_vecXYZ0		l_position;
};
