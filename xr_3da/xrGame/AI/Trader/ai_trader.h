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
#include "../../script_entity.h"
#include "../monsters/ai_monster_bones.h"
#include "../../sound_player.h"
#include "../../AI_PhraseDialogManager.h"

namespace MonsterSpace {
	enum EMonsterHeadAnimType;
};

class CInventoryItem;
class CArtefact;
class CMotionDef;
class CBlend;
class CSoundPlayer;

class CAI_Trader : public CEntityAlive, 
				   public CInventoryOwner, 
				   public CScriptEntity,
				   public CAI_PhraseDialogManager
{
private:
	typedef CEntityAlive inherited;

	bool				m_busy_now;

	struct SAnimInfo {
		shared_str		name;			// "talk_"
		u8			count;			// количество анимаций данного типа
	};

	DEFINE_MAP(u32, SAnimInfo, MOTION_MAP, MOTION_MAP_IT);

	MOTION_MAP head_anims;

	MotionID			m_tpHeadDef;	
	MotionID			m_tpGlobalDef;

	MonsterSpace::EMonsterHeadAnimType m_cur_head_anim_type;

public:
						CAI_Trader		();
	virtual				~CAI_Trader		();
public:
	virtual CAttachmentOwner*			cast_attachment_owner	()						{return this;}
	virtual CInventoryOwner*			cast_inventory_owner	()						{return this;}
	virtual CEntityAlive*				cast_entity_alive		()						{return this;}
	virtual CEntity*					cast_entity				()						{return this;}
	virtual CGameObject*				cast_game_object		()						{return this;}
	virtual CPhysicsShellHolder*		cast_physics_shell_holder	()					{return this;}
	virtual CParticlesPlayer*			cast_particles_player	()						{return this;}
	virtual CScriptEntity*				cast_script_entity		()						{return this;}

public:

	virtual DLL_Pure	*_construct		();
	virtual void		Load			( LPCSTR section );
	virtual BOOL		net_Spawn		( CSE_Abstract* DC );
	virtual void		net_Export		(NET_Packet& P);
	virtual void		net_Import		(NET_Packet& P);
	virtual	void		net_Destroy		();

	virtual void		save			(NET_Packet &output_packet);
	virtual void		load			(IReader &input_packet);
	virtual BOOL		net_SaveRelevant()							{return inherited::net_SaveRelevant();}

	virtual void		Die				(CObject* who);
	virtual void		Think			();
	virtual void		HitSignal		(float /**P/**/, Fvector &/**local_dir/**/,	CObject* /**who/**/, s16 /**element/**/){};
	virtual void		HitImpulse		(float /**P/**/, Fvector &/**vWorldDir/**/, 	Fvector& /**vLocalDir/**/){};
	virtual	void		Hit				(SHit* pHDS)
	{
		inherited::Hit(pHDS);
	}
	virtual void		SelectAnimation	(const Fvector& _view, const Fvector& _move, float speed);
	
	virtual	void		UpdateCL		();

	static void			AnimGlobalCallback	(CBlend* B);
	static void			AnimHeadCallback		(CBlend* B);

	virtual void		g_fireParams			(const CHudItem* pHudItem, Fvector& P, Fvector& D);
	virtual void		g_WeaponBones			(int &L, int &R1, int &R2);
	virtual	float		ffGetFov				()	const {return 150.f;}
	virtual	float		ffGetRange				()	const {return 30.f;}
	virtual	void		OnEvent					(NET_Packet& P, u16 type);
	virtual	void		feel_touch_new			(CObject* O);
	virtual	void		DropItemSendMessage		(CObject *O);
	virtual void		shedule_Update			(u32 dt);

	virtual BOOL		UsedAI_Locations		();
	///////////////////////////////////////////////////////////////////////
	virtual u16					PHGetSyncItemsNumber	()			{return inherited ::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*		PHGetSyncItem			(u16 item)	{return inherited ::PHGetSyncItem(item);}
	virtual void				PHUnFreeze				()			{return inherited ::PHUnFreeze();}
	virtual void				PHFreeze				()			{return inherited ::PHFreeze();}
	///////////////////////////////////////////////////////////////////////

	virtual void			reinit					();
	virtual void			reload					(LPCSTR section);

	static	void			BoneCallback			(CBoneInstance *B);
			void			LookAtActor				(CBoneInstance *B);

			void			OnStartTrade			();
			void			OnStopTrade				();

			//игровое имя 
			virtual LPCSTR	Name					() const {return CInventoryOwner::Name();}
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

	virtual	ALife::ERelationType tfGetRelationType	(const CEntityAlive *tpEntityAlive) const;

	//////////////////////////////////////////////////////////////////////////
	//генерируемые задания
public:
	//проверяет список артефактов в заказах
	virtual	u32				ArtefactPrice			(CArtefact* pArtefact);
	//продажа артефакта, с последуещим изменением списка заказов  (true - если артефакт был в списке)
	virtual	bool			BuyArtefact				(CArtefact* pArtefact);

public:
	IC		bool			busy_now				() const
	{
		return				(m_busy_now);
	}

private:
	CSoundPlayer			*m_sound_player;

public:
	IC		CSoundPlayer	&sound					() const
	{
		VERIFY				(m_sound_player);
		return				(*m_sound_player);
	}
	virtual bool			natural_weapon			() const {return false;}
	virtual bool			natural_detector		() const {return false;}
	virtual bool			AllowItemToTrade 		(CInventoryItem const * item, EItemPlace place) const;
};
