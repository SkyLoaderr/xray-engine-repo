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

struct	R_constant 
{
	string64				name;		// HLSL-name
	u16						type;		// float=0/integer=1/boolean=2
	u16						destination;// pixel/vertex/both

	R_constant_load			ps;
	R_constant_load			vs;
};

class	R_constant_table
{
private:

};
