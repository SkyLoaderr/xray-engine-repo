#pragma once
#include "missile.h"

class CBolt :
	public CMissile
{
	typedef CMissile inherited;
public:
	CBolt(void);
	virtual ~CBolt(void);

	virtual void OnH_A_Chield();
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void OnH_B_Independent();
	
	virtual bool Activate();
	virtual void Deactivate();
	
	virtual void Throw();
	virtual bool Action(s32 cmd, u32 flags);
	virtual bool Useful() const;
	virtual void Destroy();
};
