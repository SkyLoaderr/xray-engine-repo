#pragma once
#include "gameobject.h"

class CCustomTarget : public CGameObject
{
	typedef CGameObject inherited;
public:
	// Generic
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server
	virtual void			Update				(DWORD dt);
	virtual void			UpdateCL			();
//	virtual void			OnEvent				(NET_Packet& P, u16 type);
	virtual void			OnH_A_Chield		();
	virtual void			OnH_B_Independent	();

	CCustomTarget();
	virtual ~CCustomTarget();
};
