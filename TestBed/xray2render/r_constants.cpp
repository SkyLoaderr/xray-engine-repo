#include "stdafx.h"
#include "xrPool.h"
#include "r_constants.h"

static	poolSS<R_constant,64>	g_constant_allocator;

void	R_constant_table::fatal		(LPCSTR S)
{
}

R_constant* R_constant_table::get	(LPCSTR S)
{
	// assumption - sorted by name

}

BOOL	R_constant_table::parse	(D3DXSHADER_CONSTANTTABLE* desc, u16 destination)
{
	D3DXSHADER_CONSTANTINFO* it		= (D3DXSHADER_CONSTANTINFO*) (LPBYTE(desc)+desc->ConstantInfo);
	LPBYTE					 ptr	= LPBYTE(desc);
	for (u32 dwCount = desc->Constants; dwCount; dwCount--)
	{
		// Name
		LPCSTR	name		=	LPCSTR(ptr+it->Name);

		// Type
		u16		type		=	RC_float;
		if	(D3DXRS_BOOL == it->RegisterSet)	type	= RC_bool;
		if	(D3DXRS_INT4 == it->RegisterSet)	type	= RC_int;

		// Rindex,Rcount
		u16		r_index		=	it->RegisterIndex;
		u16		r_count		=	it->RegisterCount;
		u16		r_type		=	u16(-1);

		// TypeInfo + class
		D3DXSHADER_TYPEINFO*	T	= (D3DXSHADER_TYPEINFO*)(ptr+it->TypeInfo);
		BOOL bSkip					= FALSE;
		switch (T->Class)
		{
		case D3DXPC_SCALAR:			r_type		=	RC_1x1;		break;
		case D3DXPC_VECTOR:			r_type		=	RC_1x4;		break;
		case D3DXPC_MATRIX_ROWS:
			{
				switch (T->Columns)
				{
				case 4:
					switch (T->Rows)
					{
					case 3:			r_type		=	RC_3x4;		break;
					case 4:			r_type		=	RC_4x4;		break;
					default:
						fatal		("MATRIX_ROWS: unsupported number of Rows");
						break;
					}
					break;
				default:
					fatal		("MATRIX_ROWS: unsupported number of Columns");
					break;
				}
			}
			break;
		case D3DXPC_MATRIX_COLUMNS:
			fatal		("Pclass MATRIX_COLUMNS unsupported");
			break;
		case D3DXPC_STRUCT:
			fatal		("Pclass D3DXPC_STRUCT unsupported");
			break;
		default:
			bSkip		= TRUE;
			break;
		}

		// We have determined all valuable info, search if constant already created
		R_constant*	C		=	get(name);
		if (0==C)	C		=	g_constant_allocator.create();

	}
	return TRUE;
}
