#pragma once
#include "customtarget.h"
#include "..\feel_touch.h"

class CTargetCSBase :
	public CCustomTarget,
	public Feel::Touch
{
	float				radius;
	u8					team;
public:
	CTargetCSBase		(void);
	~CTargetCSBase		(void);
	BOOL net_Spawn		(LPVOID DC);
	virtual void		OnDeviceCreate		();

	virtual void						feel_touch_new			(CObject* O);
	virtual void						feel_touch_delete		(CObject* O);
};
