#ifndef r_constantsH
#define r_constantsH
#pragma once

enum
{
	RC_float	= 0,
	RC_int		= 1,
	RC_bool		= 2,
	RC_sampler	= 99
};
enum
{
	RC_1x1		= 0,		// vector1, or scalar
	RC_1x4,					// vector4
	RC_3x4,					// 4x3 matrix, transpose
	RC_4x4,					// 4x4 matrix, transpose
	RC_1x4a,				// array: vector4
	RC_3x4a,				// array: 4x3 matrix, transpose
	RC_4x4a					// array: 4x4 matrix, transpose
};
enum
{
	RC_dest_pixel	= (1<<0),
	RC_dest_vertex	= (1<<1),
	RC_dest_sampler	= (1<<2)
};

struct	R_constant_load
{
	u16						index;		// linear index (pixel)
	u16						cls;		// element class

	R_constant_load() : index(u16(-1)), cls(u16(-1)) {};

	IC BOOL					equal		(R_constant_load& C)
	{
		return (index==C.index) && (cls == C.cls);
	}
};

struct	R_constant
{
	string64				name;		// HLSL-name
	u16						type;		// float=0/integer=1/boolean=2
	u16						destination;// pixel/vertex/(or both)/sampler

	R_constant_load			ps;
	R_constant_load			vs;
	R_constant_load			samp;

	R_constant() : type(u16(-1)), destination(0) { name[0]=0; };

	IC BOOL					equal		(R_constant& C)
	{
		return (0==strcmp(name,C.name)) && (type==C.type) && (destination==C.destination) && ps.equal(C.ps) && vs.equal(C.vs) && samp.equal(C.samp);
	}
	IC BOOL					equal		(R_constant* C)
	{
		return equal(*C);
	}
};

class	ENGINE_API R_constant_table
{
private:
	typedef svector<R_constant*,32>		c_table;

	c_table					table;
	void					fatal		(LPCSTR s);
public:
	BOOL					parse		(D3DXSHADER_CONSTANTTABLE* desc, u16 destination);
	void					merge		(R_constant_table* C);
	R_constant*				get			(LPCSTR name);

	IC BOOL					empty		()
	{
		return 0==table.size();
	}

	IC BOOL					equal		(R_constant_table& C)
	{
		if (table.size() != C.table.size())	return FALSE;
		u32 size			= table.size();
		for (u32 it=0; it<size; it++)
		{
			if (!table[it]->equal(C.table[it]))	return FALSE;
		}
		return TRUE;
	}
	IC BOOL					equal		(R_constant_table* C)
	{
		return equal(*C);
	}
};
#endif
