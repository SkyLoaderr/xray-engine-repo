#ifndef _XR_LIGHT_
#define _XR_LIGHT_
#pragma once

#pragma pack(push,4)
struct xrLIGHT : public Flight
{
	DWORD		dwFrame;

	struct {
		DWORD			bAffectStatic	: 1;
		DWORD			bAffectDynamic	: 1;
		DWORD			bProcedural		: 1;
	}					flags;
	string64			name;
};
#pragma pack(pop)

#endif