#pragma once
#include "customtarget.h"
#include "..\feel_touch.h"

class CTargetCSBase :
	public CCustomTarget,
	public Feel::Touch
{
	typedef				CCustomTarget inherited;

	float				radius;
	u8					team;
public:
	CTargetCSBase		(void);
	~CTargetCSBase		(void);

	virtual BOOL		net_Spawn			(LPVOID DC);
	virtual void		net_Destroy			();

	virtual void		OnDeviceCreate		();

	virtual void		Update				(u32 dt);
	virtual void		feel_touch_new		(CObject* O);
	virtual void		feel_touch_delete	(CObject* O);

	IC u8				g_Team				(){return team;};
};
