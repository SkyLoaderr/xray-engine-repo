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
	xrPID_MARKER	= 0,
	xrPID_MATRIX,		// really only name(stringZ) is written into stream
	xrPID_CONSTANT,		// really only name(stringZ) is written into stream
	xrPID_TEXTURE,		// really only name(stringZ) is written into stream
	xrPID_INTEGER,
	xrPID_FLOAT,
	xrPID_BOOL,
	xrPID_TOKEN,
	xrPID_CLSID,
	xrPID_OBJECT,		// really only name(stringZ) is written into stream
	xrPID_FORCEDWORD=DWORD(-1)
};

struct	xrP_Integer
{
	int					value;
	int					min;
	int					max;

	xrP_Integer() : value(0), min(0), max(255)	{}
};

struct	xrP_Float
{
	float				value;
	float				min;
	float				max;

	xrP_Float()	: value(0), min(0), max(1)		{}
};

struct	xrP_BOOL
{
	BOOL				value;
	xrP_BOOL() : value(FALSE)					{}
};

struct	xrP_TOKEN
{
	struct Item {
		DWORD		ID;
		string64	str;
	};

	DWORD				IDselected;
	DWORD				Count;
	
	//--- elements:		(ID,string64)

	xrP_TOKEN()	: IDselected(0), Count(0)		{}
};
struct	xrP_CLSID
{
	CLASS_ID			Selected;
	DWORD				Count;
	//--- elements:		(...)
};

#pragma pack(pop)
#endif // xrPROPERTIES_H