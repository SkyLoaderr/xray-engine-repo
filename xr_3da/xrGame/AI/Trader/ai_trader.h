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
#include "../../script_callback.h"
#include "../../sound_player.h"
#include "../../AI_PhraseDialogManager.h"

class CInventoryItem;

class CAI_Trader : public CEntityAlive, 
				   public CInventoryOwner, 
				   public CScriptMonster,
				   public CAI_PhraseDialogManager,
				   public CSoundPlayer
{
private:
	typedef CEntityAlive inherited;

	CScriptCallback		m_OnStartCallback;
	CScriptCallback		m_OnStopCallback;
	CScriptCallback		m_OnTradeCallback;

	struct SAnimInfo {
		ref_str		name;			// "talk_"
		u8			count;			// ���������� �������� ������� ����
	};

	DEFINE_MAP(u32, SAnimInfo, MOTION_MAP, MOTION_MAP_IT);

	MOTION_MAP head_anims;

	CMotionDef*			m_tpHeadDef;	
	CMotionDef*			m_tpGlobalDef;

	MonsterSpace::EMonsterHeadAnimType m_cur_head_anim_type;


public:
	
	ALife::ARTEFACT_TRADER_ORDER_MAP	m_tpOrderedArtefacts;


public:
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

	static void	__stdcall AnimGlobalCallback	(CBlend* B);
	static void	__stdcall AnimHeadCallback		(CBlend* B);

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
private:
			// Animation management

			void			add_head_anim			(u32 index, LPCSTR anim_name);
			u8				get_anim_count			(LPCSTR anim);
			void			select_head_anim		(u32 type);
			void			AssignHeadAnimation		();


	virtual	bool			bfAssignSound			(CScriptEntityAction *tpEntityAction);

};
