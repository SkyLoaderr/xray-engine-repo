#pragma once

#include "r_constants.h"

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

class	R_constant_array
{
public:
	typedef		R_constant_cache<Fvector4,256>	t_f;
	typedef		R_constant_cache<Ivector4,16>	t_i;
	typedef		R_constant_cache<BOOL,16>		t_b;
public:
	BOOL							b_dirty;
	t_f								c_f;
	t_i								c_i;
	t_b								c_b;
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
	R_constant_array		a_pixel;
	R_constant_array		a_vertex;
	
	void					flush_cache	();
public:
	// fp, non-array versions
	void					set		(R_constant* C, Fmatrix& A)		{
		if (C->destination&1)		{ a_pixel.set	(C,C->ps,A); a_pixel.b_dirty=TRUE;		}
		if (C->destination&2)		{ a_vertex.set	(C,C->vs,A); a_vertex.b_dirty=TRUE;		}
	}
	void					set		(R_constant* C, Fvector4& A)	{
		if (C->destination&1)		{ a_pixel.set	(C,C->ps,A); a_pixel.b_dirty=TRUE;		}
		if (C->destination&2)		{ a_vertex.set	(C,C->vs,A); a_vertex.b_dirty=TRUE;		}
	}
	void					set		(R_constant* C, float x, float y, float z, float w)	{
		Fvector4 data;		data.set(x,y,z,w);
		set					(C,data);
	}

	// fp, array versions
	void					seta	(R_constant* C, u32 e, Fmatrix& A)		{
		if (C->destination&1)		{ a_pixel.seta	(C,C->ps,e,A); a_pixel.b_dirty=TRUE;	}
		if (C->destination&2)		{ a_vertex.seta	(C,C->vs,e,A); a_vertex.b_dirty=TRUE;	}
	}
	void					seta	(R_constant* C, u32 e, Fvector4& A)	{
		if (C->destination&1)		{ a_pixel.seta	(C,C->ps,e,A); a_pixel.b_dirty=TRUE;	}
		if (C->destination&2)		{ a_vertex.seta	(C,C->vs,e,A); a_vertex.b_dirty=TRUE;	}
	}
	void					seta	(R_constant* C, u32 e, float x, float y, float z, float w)	{
		Fvector4 data;		data.set(x,y,z,w);
		seta				(C,e,data);
	}

	// 
	void					flush	(IDirect3DDevice9* D)
	{
		if (a_pixel.b_dirty || a_vertex.b_dirty)
		{
			if (a_pixel.b_dirty)
			{
				// fp
				R_constant_array::t_f&	F	= a_pixel.c_f;
				{
					u32		count		= F.r_hi()-F.r_lo();
					if (count)			{
						D->SetPixelShaderConstantF	(F.r_lo(), F.access(F.r_lo()),count);
						F.flush			();
					}
				}
			}
			if (a_vertex.b_dirty)
			{
				// fp
				R_constant_array::t_f&	F	= a_vertex.c_f;
				{
					u32		count		= F.r_hi()-F.r_lo();
					if (count)			{
						D->SetVertexShaderConstantF	(F.r_lo(), F.access(F.r_lo()),count);
						F.flush			();
					}
				}
			}
		}
	}
};
