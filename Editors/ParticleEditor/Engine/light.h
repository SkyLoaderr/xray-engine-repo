#ifndef _XR_LIGHT_
#define _XR_LIGHT_
#pragma once

#pragma pack(push,4)
class xrLIGHT       : public Flight
{
public:
	DWORD			dwFrame;
	DWORD			dwController;
};

class xrLIGHT_control
{
public:
	DWORD			dwFrame;
	CConstant*		dest;
	string64		name;
	vector<DWORD>	data;
};
#pragma pack(pop)

#endif