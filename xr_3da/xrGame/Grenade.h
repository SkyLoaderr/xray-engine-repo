#pragma once
#include "missile.h"

class CGrenade :
	public CMissile
{
	typedef CMissile inherited;
public:
	CGrenade(void);
	virtual ~CGrenade(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void OnAnimationEnd();

	virtual bool Activate();
	virtual void Deactivate();
	virtual void Throw();

	virtual bool Useful();

	CGrenade *m_pFake;
};
