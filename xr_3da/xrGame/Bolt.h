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
	
	virtual bool Attach(PIItem pIItem);
	virtual bool Detach(PIItem pIItem);
	
	virtual void Throw();
	virtual bool Action(s32 cmd, u32 flags);
	virtual bool Useful();
	virtual void Destroy();

	virtual void			net_Export			(NET_Packet& P) {};				// export to server
	virtual void			net_Import			(NET_Packet& P) {};				// import from server


protected:
	//присоединенные объекты
	TIItemList m_subs;
};
