#ifndef _XR_LIGHT_
#define _XR_LIGHT_
#pragma once

#pragma pack(push,4)
class xrLIGHT 
{
public:
	DWORD			dwFrame;
	DWORD			dwController;
	Flight			data;
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