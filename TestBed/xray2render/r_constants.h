#pragma once

class	R_constant 
{
	string64				name;		// HLSL-name
	u32						r_dest;		// pixel/vertex/both
	u32						r_index;	// linear index
	u32						r_count;	// linear count
	u32						p_rows;
	u32						p_columns;
	u32						p_class;	
	u32						p_type;		// float/integer/boolean		
	u32						bytes;		// total bytes occupied
};

template <class T, u32 limit>
class	R_constant_cache
{
private:
	svector<T,limit>		array;
	u32						lo,hi;
public:
	IC void					init	(u32 size)				{ array.resize(size); flush();				}
	IC T*					access	(u32 id)				{ return &array[id];						}
	IC void					flush	()						{ lo=hi=0;									}
	IC void					dirty	(u32 _lo, u32 _hi)		{ if (_lo<lo) lo=_lo; if (_hi>hi) hi=_hi;	}
	IC u32					r_lo	()						{ return lo;	}
	IC u32					r_hi	()						{ return hi;	}
};

class	R_constants_array
{
private:
	BOOL							b_dirty;
	R_constant_cache<float,256*4>	c_f;
	R_constant_cache<BOOL,16>		c_b;
	R_constant_cache<int,16>		c_i;
public:
};

class	R_constants 
{
private:
	R_constants_array		a_pixel;
	R_constants_array		a_vertex;
public:
	IC void					set		()
};
