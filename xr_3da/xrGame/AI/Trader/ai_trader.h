////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_trader.h
//	Created 	: 16.04.2003
//  Modified 	: 16.04.2003
//	Author		: Jim
//	Description : Trader class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../../inventoryowner.h"
#include "../script/ai_script_monster.h"
#include "../script/ai_script_monster.h"
#include "../ai_monster_bones.h"
#include "../../ai_script_callback.h"
#include "../../sound_player.h"

class CInventoryItem;

class CAI_Trader :	public CEntityAlive, 
					public CInventoryOwner, 
					public CScriptMonster,
					public CSoundPlayer
{
	CScriptCallback		m_OnStartCallback;
	CScriptCallback		m_OnStopCallback;
	CScriptCallback		m_OnTradeCallback;

public:
	typedef CEntityAlive inherited;
	CMotionDef*			m_tAnimation;
	bool				m_bPlaying;
	ALife::ARTEFACT_TRADER_ORDER_MAP	m_tpOrderedArtefacts;


						CAI_Trader		();
	virtual				~CAI_Trader		();
	virtual	void		Init			();
	virtual void		Load			( LPCSTR section );
	virtual BOOL		net_Spawn		( LPVOID DC );
	virtual void		net_Export		(NET_Packet& P);
	virtual void		net_Import		(NET_Packet& P);
	virtual	void		net_Destroy		();

	virtual void		Die				();
	virtual void		Think			();
	virtual void		HitSignal		(float /**P/**/, Fvector &/**local_dir/**/,	CObject* /**who/**/, s16 /**element/**/){};
	virtual void		HitImpulse		(float /**P/**/, Fvector &/**vWorldDir/**/, 	Fvector& /**vLocalDir/**/){};
	virtual void		Hit				(float P, Fvector &dir,			CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound) {
		inherited::Hit(P,dir,who,element,position_in_object_space,impulse,hit_type);
	}
	virtual void		SelectAnimation	(const Fvector& _view, const Fvector& _move, float speed);
	
	virtual	void		UpdateCL		();

	static void			AnimCallback	(CBlend* B)
	{
		CAI_Trader		*tpTrader = (CAI_Trader*)B->CallbackParam;
		tpTrader->m_bPlaying = false;
	}

	virtual void		g_fireParams			(const CHudItem* pHudItem, Fvector& P, Fvector& D);
	virtual void		g_WeaponBones			(int &L, int &R1, int &R2);
	virtual	float		ffGetFov				()	const {return 150.f;}
	virtual	float		ffGetRange				()	const {return 30.f;}
	virtual	void		OnEvent					(NET_Packet& P, u16 type);
	virtual	void		feel_touch_new			(CObject* O);
	virtual	void		DropItemSendMessage		(CObject *O);
	virtual void		shedule_Update			(u32 dt);
	virtual void		renderable_Render		();

	virtual BOOL		UsedAI_Locations		();
	///////////////////////////////////////////////////////////////////////
	virtual u16					PHGetSyncItemsNumber	()			{return inherited ::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*		PHGetSyncItem			(u16 item)	{return inherited ::PHGetSyncItem(item);}
	virtual void				PHUnFreeze				()			{return inherited ::PHUnFreeze();}
	virtual void				PHFreeze				()			{return inherited ::PHFreeze();}
	///////////////////////////////////////////////////////////////////////

	virtual void		reinit					();
	virtual void		reload					(LPCSTR section);

	static	void __stdcall	BoneCallback			(CBoneInstance *B);
			void			LookAtActor				(CBoneInstance *B);

			void			set_callback			(const luabind::functor<void> &lua_function, bool bOnStart);
			void			set_callback			(const luabind::object &lua_object, LPCSTR method, bool bOnStart);
			void			clear_callback			(bool bOnStart);

			void			set_trade_callback		(const luabind::functor<void> &lua_function);
			void			set_trade_callback		(const luabind::object &lua_object, LPCSTR method);
			void			clear_trade_callback	();

			void			OnStartTrade			();
			void			OnStopTrade				();
			void			OnTradeAction			(CGameObject *O, bool bSell);
public:	
	virtual	bool			can_attach				(const CInventoryItem *inventory_item) const;
	virtual bool			use_bolts				() const;
	virtual	void			spawn_supplies			();
};
