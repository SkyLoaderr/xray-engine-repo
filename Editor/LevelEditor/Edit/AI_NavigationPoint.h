#pragma once

enum EAI_Navigation
{
	NP_Standart=0,
	NP_Item,

	NP_Last,
	NP_ForceDword = DWORD(-1)
};

#pragma pack(push,4)
struct AI_NaviPoint {
	DWORD	type;
	Fvector	position;
};
#pragma pack(pop)
