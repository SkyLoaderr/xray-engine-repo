#include "stdafx.h"
#include "xrPool.h"
#include "r_constants.h"

// pool
static	poolSS<R_constant,64>		g_constant_allocator;

void	R_constant_table::fatal		(LPCSTR S)
{
}

// predicates
bool	p_search	(R_constant* C, LPCSTR S)
{
	return strcmp(C->name,S)<0;
}
bool	p_sort		(R_constant* C1, R_constant* C2)
{
	return strcmp(C1->name,C2->name)<0;
}

R_constant* R_constant_table::get	(LPCSTR S)
{
	// assumption - sorted by name
	c_table::iterator I	= std::lower_bound(table.begin(),table.end(),S,p_search);
	if (I==table.end() || (0!=strcmp((*I)->name,S)))	return 0;
	else												return *I;
}

BOOL	R_constant_table::parse	(D3DXSHADER_CONSTANTTABLE* desc, u16 destination)
{
	D3DXSHADER_CONSTANTINFO* it		= (D3DXSHADER_CONSTANTINFO*) (LPBYTE(desc)+desc->ConstantInfo);
	LPBYTE					 ptr	= LPBYTE(desc);
	for (u32 dwCount = desc->Constants; dwCount; dwCount--,it++)
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
		if (bSkip)			continue;

		// We have determined all valuable info, search if constant already created
		R_constant*	C		=	get	(name);
		if (0==C)	{
			C					=	g_constant_allocator.create();
			strcpy				(C->name,name);
			C->destination		=	destination;
			C->type				=	type;
			R_constant_load& L	=	(destination&1)?C->ps:C->vs;
			L.index				=	r_index;
			L.cls				=	r_type;
			table.push_back		(C);
		} else {
			C->destination		|=	destination;
			VERIFY	(C->type	==	type);
			R_constant_load& L	=	(destination&1)?C->ps:C->vs;
			L.index				=	r_index;
			L.cls				=	r_type;
		}

	}
	std::sort	(table.begin(),table.end(),p_sort);
	return		TRUE;
}

void R_constant_table::merge(R_constant_table* T)
{
	if (0==T)	return;
	for (u32 it=0; it<T->table.size(); it++)
	{
		R_constant*	src			=	T->table[it];
		R_constant*	C			=	get	(src->name);
		if (0==C)	{
			C					=	g_constant_allocator.create();
			strcpy				(C->name,src->name);
			C->destination		=	src->destination;
			C->type				=	src->type;
			C->ps				=	src->ps;
			C->vs				=	src->vs;
			table.push_back		(C);
		} else {
			C->destination		|=	src->destination;
			VERIFY	(C->type	==	src->type);
			R_constant_load& sL	=	(src->destination&1)?src->ps:src->vs;
			R_constant_load& dL	=	(src->destination&1)?C->ps:C->vs;
			dL.index			=	sL.index;
			dL.cls				=	sL.cls;
		}
	}
	std::sort	(table.begin(),table.end(),p_sort);
}
