#ifndef BLENDER_PARAMS_H
#define BLENDER_PARAMS_H

#pragma once
#pragma pack(push,4)

// Parameter/Property specifications

// *** FORMAT ***
// DWORD	type
// stringZ	name
// []		data

enum	BPID
{
	BPID_MARKER	= 0,
	BPID_MATRIX,	// really only name(stringZ) is written into stream
	BPID_CONSTANT,	// really only name(stringZ) is written into stream
	BPID_TEXTURE,	// really only name(stringZ) is written into stream
	BPID_INTEGER,
	BPID_FLOAT,
	BPID_BOOL,
	BPID_TOKEN,
	BPID_FORCEDWORD=DWORD(-1)
};

struct	BP_Integer
{
	int					value;
	int					min;
	int					max;

	BP_Integer() : value(0), min(0), max(255)	{}
};

struct	BP_Float
{
	float				value;
	float				min;
	float				max;

	BP_Float() : value(0), min(0), max(1)	{}
};

struct	BP_BOOL
{
	BOOL				value;
	BP_BOOL() : value(0)	{}
};

//---- token
struct	BP_TOKEN
{
	struct Item {
		DWORD		ID;
		string64	str;
	};

	DWORD				IDselected;
	DWORD				Count;
	
	//--- elements:		(ID,string64)
};
#pragma pack(pop)
#endif