#pragma once

enum
{
	RC_float	= 0,
	RC_int		= 1,
	RC_bool		= 2
};

struct	R_constant_load
{
	u16						index;		// linear index (pixel)
	u16						width;
	u16						count;
};

class	R_constant 
{
	string64				name;		// HLSL-name
	u16						type;		// float=0/integer=1/boolean=2
	u16						destination;// pixel/vertex/both

	R_constant_load			ps;
	R_constant_load			vs;
};
/
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
	IC u32					r_lo	()						{ return lo;								}
	IC u32					r_hi	()						{ return hi;								}
};

class	R_constants_array
{
private:
	BOOL							b_dirty;
	R_constant_cache<float,256*4>	c_f;
	R_constant_cache<int,8*4>		c_i;
	R_constant_cache<BOOL,16>		c_b;
public:
	void					set		(R_constant* C, R_constant_load& L, Fmatrix& M)
	{
		VERIFY	(RC_float == C->type);
		switch	(L->count)
		{
		case 1:	// single register
			switch (L->width)
			{
			case 1:	// float1
				break;
			case 2:	// float2
				break;
			case 3:	// float3
				break;
			case 4:	// float4
				break;
			}
			break;
		case 2:	// 2 registers
			break;
		case 3:	// 3 registers
			break;
		case 4:	// 4 registers
			break;
		}
	}
};

class	R_constants 
{
private:
	R_constants_array		a_pixel;
	R_constants_array		a_vertex;
public:
	// fp
	void					set		(R_constant* C, Fmatrix& M)
	{
		if (C->destination&1)		a_pixel.set		(C,C->ps,M);
		if (C->destination&2)		a_vertex.set	(C,C->vs,M);
	}
};
