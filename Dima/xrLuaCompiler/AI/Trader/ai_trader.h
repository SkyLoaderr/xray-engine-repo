////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_trader.h
//	Created 	: 16.04.2003
//  Modified 	: 16.04.2003
//	Author		: Jim
//	Description : Trader class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\..\\inventory.h"
#include "..\\..\\ai_space.h"

class CAI_Trader : public CEntityAlive, public CInventoryOwner 
{
public:
	typedef CEntityAlive inherited;
	CMotionDef*			m_tAnimation;
	bool				m_bPlaying;
	ARTEFACT_TRADER_ORDER_MAP	m_tpOrderedArtefacts;


						CAI_Trader		();
	virtual				~CAI_Trader		();
	virtual void		Load			( LPCSTR section );
	virtual BOOL		net_Spawn		( LPVOID DC );
	virtual void		net_Export		(NET_Packet& P);
	virtual void		net_Import		(NET_Packet& P);
	virtual void		Die				();
	virtual void		Think			();
	virtual void		HitSignal		(float P, Fvector &local_dir,	CObject* who, s16 element){};
	virtual void		HitImpulse		(float P, Fvector &vWorldDir, 	Fvector& vLocalDir){};
	virtual void		SelectAnimation	(const Fvector& _view, const Fvector& _move, float speed);
	
	static void			AnimCallback	(CBlend* B)
	{
		CAI_Trader		*tpTrader = (CAI_Trader*)B->CallbackParam;
		tpTrader->m_bPlaying = false;
	}

	virtual void		g_fireParams			(Fvector& P, Fvector& D);
	virtual void		g_WeaponBones			(int &L, int &R1, int &R2);
	virtual	float		ffGetFov				()	{return 150.f;}
	virtual	float		ffGetRange				()	{return 30.f;}
	virtual	void		OnEvent					(NET_Packet& P, u16 type);
	virtual	void		feel_touch_new			(CObject* O);
	virtual	void		DropItemSendMessage		(CObject *O);
	virtual void		shedule_Update			(u32 dt);
	virtual void		renderable_Render		();

};