#pragma once

class ENGINE_API	R_xforms
{
public:
	Fmatrix			m_w;		// Basic	- world
	Fmatrix			m_v;		// Basic	- view
	Fmatrix			m_p;		// Basic	- projection
	Fmatrix			m_wv;		// Derived	- world2view
	Fmatrix			m_vp;		// Derived	- view2projection
	Fmatrix			m_wvp;		// Derived	- world2view2projection

	R_constant*		c_w;
	R_constant*		c_v;
	R_constant*		c_p;
	R_constant*		c_wv;
	R_constant*		c_vp;
	R_constant*		c_wvp;
public:
	R_xforms		();
	void			unmap		();
	void			set_W		(const Fmatrix& m);
	void			set_V		(const Fmatrix& m);
	void			set_P		(const Fmatrix& m);
	IC void			set_c_w		(R_constant* C)		{	c_w		= C;	RCache.set_c(C,m_w);	};
	IC void			set_c_v		(R_constant* C)		{	c_v		= C;	RCache.set_c(C,m_v);	};
	IC void			set_c_p		(R_constant* C)		{	c_p		= C;	RCache.set_c(C,m_p);	};
	IC void			set_c_wv	(R_constant* C)		{	c_wv	= C;	RCache.set_c(C,m_wv);	};
	IC void			set_c_vp	(R_constant* C)		{	c_vp	= C;	RCache.set_c(C,m_vp);	};
	IC void			set_c_wvp	(R_constant* C)		{	c_wvp	= C;	RCache.set_c(C,m_wvp);	};
};
