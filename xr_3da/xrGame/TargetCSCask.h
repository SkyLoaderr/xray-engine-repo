#pragma once
#include "customtarget.h"

class CTargetCSCask :
	public CCustomTarget
{
	typedef				CCustomTarget inherited;
public:
	CTargetCSCask(void);
	~CTargetCSCask(void);
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void OnDeviceCreate();
};
