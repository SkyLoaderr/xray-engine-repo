#pragma once

enum
{
	RC_float	= 0,
	RC_int		= 1,
	RC_bool		= 2
};
enum
{
	RC_1x1		= 0,		// vector1, or scalar
	RC_1x4,					// vector4
	RC_3x4,					// 4x3 matrix, transpose
	RC_4x4,					// 4x4 matrix, transpose
	RC_1x4a,				// array: vector4
	RC_3x4a,				// array: 4x3 matrix, transpose
	RC_4x4a,				// array: 4x4 matrix, transpose
};

struct	R_constant_load
{
	u16						index;		// linear index (pixel)
	u16						cls;		// element class
};

class	R_constant 
{
	string64				name;		// HLSL-name
	u16						type;		// float=0/integer=1/boolean=2
	u16						destination;// pixel/vertex/both

	R_constant_load			ps;
	R_constant_load			vs;
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
	IC u32					r_lo	()						{ return lo;								}
	IC u32					r_hi	()						{ return hi;								}
};

class	R_constants_array
{
private:
	BOOL							b_dirty;
	R_constant_cache<Fvector4,256>	c_f;
	R_constant_cache<Ivector4,16>	c_i;
	R_constant_cache<BOOL,16>		c_b;
public:
	void					fatal	();

	void					set		(R_constant* C, R_constant_load& L, Fmatrix& A)
	{
		VERIFY		(RC_float == C->type);
		Fvector4*	it	= c_f.access	(L->index);
		switch		(L->cls)
		{
		case RC_3x4:
			c_f.dirty			(L->index,L->index+3);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			break;
		case RC_4x4:
			c_f.dirty			(L->index,L->index+4);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			it[3].set			(A._14, A._24, A._34, A._44);
			break;
		default:
			fatal();
			break;
		}
	}

	void					set		(R_constant* C, R_constant_load& L, Fvector4& A)
	{
		VERIFY		(RC_float	== C->type);
		VERIFY		(RC_1x4		== L->cls);
		c_f.access	(L->index)->set	(A);
		c_f.dirty	(L->index,L->index+1);
	}

	void					seta	(R_constant* C, R_constant_load& L, u32 e, Fmatrix& A)
	{
		VERIFY		(RC_float == C->type);
		u32			base;
		Fvector4*	it;
		switch		(L->cls)
		{
		case RC_3x4:
			base				= L->index + 3*e;
			it					= c_f.access	(base);
			c_f.dirty			(base,base+3);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			break;
		case RC_4x4:
			base				= L->index + 4*e;
			it					= c_f.access	(base);
			c_f.dirty			(base,base+4);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			it[3].set			(A._14, A._24, A._34, A._44);
			break;
		default:
			fatal();
			break;
		}
	}

	void					seta	(R_constant* C, R_constant_load& L, u32 e, Fvector4& A)
	{
		VERIFY		(RC_float	== C->type);
		VERIFY		(RC_1x4		== L->cls);
		u32			base	= L->index + e;
		c_f.access	(base)->set	(A);
		c_f.dirty	(base,base+1);
	}
};

class	R_constants 
{
private:
	R_constants_array		a_pixel;
	R_constants_array		a_vertex;
public:
	// fp, non-array versions
	void					set		(R_constant* C, Fmatrix& A)		{
		if (C->destination&1)		a_pixel.set		(C,C->ps,A);
		if (C->destination&2)		a_vertex.set	(C,C->vs,A);
	}
	void					set		(R_constant* C, Fvector4& A)	{
		if (C->destination&1)		a_pixel.set		(C,C->ps,A);
		if (C->destination&2)		a_vertex.set	(C,C->vs,A);
	}
	void					set		(R_constant* C, float x, float y, float z, float w)	{
		Fvector4 data;		data.set(x,y,z,w);
		set					(C,data);
	}

	// fp, array versions
	void					seta	(R_constant* C, u32 e, Fmatrix& A)		{
		if (C->destination&1)		a_pixel.seta	(C,C->ps,e,A);
		if (C->destination&2)		a_vertex.seta	(C,C->vs,e,A);
	}
	void					seta	(R_constant* C, u32 e, Fvector4& A)	{
		if (C->destination&1)		a_pixel.seta	(C,C->ps,e,A);
		if (C->destination&2)		a_vertex.seta	(C,C->vs,e,A);
	}
	void					seta	(R_constant* C, u32 e, float x, float y, float z, float w)	{
		Fvector4 data;		data.set(x,y,z,w);
		seta				(C,e,data);
	}
};
