#ifndef xrPROPERTIES_H
#define xrPROPERTIES_H

#pragma once
#pragma pack(push,4)

// Parameter/Property specifications

// *** FORMAT ***
// DWORD	type
// stringZ	name
// []		data

enum	xrProperties
{
	xrP_MARKER	= 0,
	xrP_MATRIX,		// really only name(stringZ) is written into stream
	xrP_CONSTANT,	// really only name(stringZ) is written into stream
	xrP_TEXTURE,	// really only name(stringZ) is written into stream
	xrP_OBJECT,	// really only name(stringZ) is written into stream
	xrP_INTEGER,
	xrP_FLOAT,
	xrP_BOOL,
	xrP_TOKEN,
	xrP_FORCEDWORD=DWORD(-1)
};

struct	xrPDEF_Integer
{
	int					value;
	int					min;
	int					max;

	xrPDEF_Integer() : value(0), min(0), max(255)	{}
};

struct	xrPDEF_Float
{
	float				value;
	float				min;
	float				max;

	xrPDEF_Float()	: value(0), min(0), max(1)		{}
};

struct	xrPDEF_BOOL
{
	BOOL				value;
	xrPDEF_BOOL() : value(FALSE)					{}
};

struct	xrPDEF_TOKEN
{
	struct Item {
		DWORD		ID;
		string64	str;
	};

	DWORD				IDselected;
	DWORD				Count;
	
	//--- elements:		(ID,string64)

	xrPDEF_TOKEN()	: IDselected(0), Count(0)		{}
};

#pragma pack(pop)
#endif // xrPROPERTIES_H