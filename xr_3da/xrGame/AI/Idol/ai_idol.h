////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_idol.h
//	Created 	: 16.04.2003
//  Modified 	: 16.04.2003
//	Author		: Dmitriy Iassenev
//	Description : No AI, just an idol
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"

class CAI_Idol : public CCustomMonster, public CInventoryOwner 
{
public:
	typedef CCustomMonster inherited;
	vector<CMotionDef*>	m_tpaAnims;
	CBlend				*m_tpCurrentBlend;
	bool				m_bPlaying;
	u32					m_dwAnyPlayType;
	u32					m_dwCurrentAnimationIndex;
	
						CAI_Idol		();
	virtual				~CAI_Idol		();
	virtual void		Load			( LPCSTR section );
	virtual BOOL		net_Spawn		( LPVOID DC );
	virtual void		net_Export		(NET_Packet& P){};
	virtual void		net_Import		(NET_Packet& P){};
	virtual void		Die				(){};
	virtual void		Think			(){};
	virtual void		HitSignal		(float P, Fvector &local_dir,	CObject* who, s16 element){};
	virtual void		HitImpulse		(float P, Fvector &vWorldDir, 	Fvector& vLocalDir){};
	virtual void		SelectAnimation	(const Fvector& _view, const Fvector& _move, float speed);
	
	static void			AnimCallback	(CBlend* B)
	{
		CAI_Idol		*tpIdol = (CAI_Idol*)B->CallbackParam;
		tpIdol->m_bPlaying = false;
	}

	virtual void		g_fireParams	(Fvector& P, Fvector& D)	{};
	virtual void		g_WeaponBones	(int &L, int &R1, int &R2)	{};
	virtual	float		ffGetFov		(){return 150.f;}
	virtual	float		ffGetRange		(){return 30.f;}
	virtual	void		OnEvent			(NET_Packet& P, u16 type);
	virtual	void		feel_touch_new	(CObject* O);
	virtual	void		DropItemSendMessage(CObject *O);
	virtual void		Update			(u32 dt);
};