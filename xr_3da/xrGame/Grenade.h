#pragma once
#include "missile.h"
#include "explosive.h"
#include "../feel_touch.h"

#define SND_RIC_COUNT 5

class CGrenade :
	public CMissile,
	public CExplosive
{
	typedef CMissile inherited;
public:
	CGrenade(void);
	virtual ~CGrenade(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	
	virtual void OnEvent(NET_Packet& P, u16 type);
	
	virtual void OnAnimationEnd();
	virtual void UpdateCL();

	virtual bool Activate();
	virtual void Deactivate();
	virtual void Throw();
	virtual void Destroy();
	
	virtual bool Action(s32 cmd, u32 flags);
	virtual bool Useful();
	virtual u32 State(u32 state);

	virtual void						net_Import			(NET_Packet& P);					// import from server
	virtual void						net_Export			(NET_Packet& P);					// export to server

	virtual void renderable_Render() {inherited::renderable_Render();}
	virtual void OnH_B_Chield() {inherited::OnH_B_Chield();}
	virtual bool IsHidden() {return inherited::IsHidden();}
	virtual bool IsPending() {return inherited::IsPending();}
protected:
	//объект факовой гранаты
	CGrenade *m_pFake;

	struct	net_update 		
	{
		u32					dwTimeStamp;
		Fvector				pos,angles;
	};

	xr_deque<net_update>	NET;
	net_update				NET_Last;
};
