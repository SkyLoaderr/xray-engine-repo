#pragma once
#include "customtarget.h"

class CTargetCSBase :
	public CCustomTarget
{
	float				radius;
	u8					team;
public:
	CTargetCSBase		(void);
	~CTargetCSBase		(void);
	BOOL net_Spawn		(LPVOID DC);
	virtual void		OnDeviceCreate		();
};
