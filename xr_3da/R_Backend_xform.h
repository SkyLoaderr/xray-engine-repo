#pragma once

class ENGINE_API	R_xforms
{
public:
	enum	
	{
		dirty_W		= (1<<0),	// world
		dirty_V		= (1<<1),	// view
		dirty_P		= (1<<2),	// projection
		dirty_M		= (1<<3)	// mapping
	};

	u32				flags;

	Fmatrix			m_w;		// Basic	- world
	Fmatrix			m_v;		// Basic	- view
	Fmatrix			m_p;		// Basic	- projection
	Fmatrix			m_wv;		// Derived	- world2view
	Fmatrix			m_vp;		// Derived	- view2projection
	Fmatrix			m_wvp;		// Derived	- world2view2projection

	R_mapping*		mapping;
private:
	void			flush_cache	();
public:
	R_xforms()
	{
		set_W		(Fidentity);
		set_V		(Fidentity);
		set_P		(Fidentity);
		set_mapping	(NULL);
	}
	IC void			set_W		(Fmatrix& m)		{	flags |= dirty_W; m_w.set(m);	}
	IC void			set_V		(Fmatrix& m)		{	flags |= dirty_V; m_v.set(m);	}
	IC void			set_P		(Fmatrix& m)		{	flags |= dirty_P; m_p.set(m);	}
	IC void			set_mapping	(R_mapping* C)
	{
		if (C && !C->empty)		
		{
			mapping	=	C;
			flags	|=	dirty_M;
		} 
		else 
		{
			mapping	=	NULL;
			flags	&=	~dirty_M;
		}
	}
	IC void			flush		()					{	if (flags && mapping)	flush_cache();	};
};
