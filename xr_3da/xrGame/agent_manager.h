////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "member_order.h"
#include "memory_space.h"

class CEntity;
class CAI_Stalker;
class CGameObject;
class CExplosive;
class CSetupAction;
class CAgentManagerMotivationPlanner;

class CAgentManager : public ISheduled {
public:
	class CMemberPredicate {
	protected:
		const CAI_Stalker	*m_object;

	public:
		IC				CMemberPredicate	(const CAI_Stalker *object) :
							m_object	(object)
		{
		}

		IC		bool	operator()			(const CMemberOrder &order) const
		{
			return			(&order.object() == m_object);
		}
	};

	struct CEnemy {
		const CEntityAlive						*m_object;
		_flags<MemorySpace::squad_mask_type>	m_mask;
		_flags<MemorySpace::squad_mask_type>	m_distribute_mask;
		float									m_probability;
		Fvector									m_enemy_position;
		u32										m_level_time;

		IC				CEnemy			(const CEntityAlive *object, MemorySpace::squad_mask_type	mask)
		{
			m_object					= object;
			m_mask.assign				(mask);
			m_probability				= 1.f;
			m_distribute_mask.zero		();
		}

		IC	bool		operator==		(const CEntityAlive *object) const
		{
			return						(m_object == object);
		}

		IC	bool		operator<		(const CEnemy &enemy) const
		{
			return						(m_probability > enemy.m_probability);
		}
	};

	struct CDangerCover {
		Fvector			m_position;
		u32				m_level_time;
		u32				m_interval;
		float			m_radius;

		IC	bool	operator==	(const Fvector &position) const
		{
			return		(!!m_position.similar(position));
		}
	};

	struct CRemoveOldDangerCover {
		IC	bool	operator()	(const CAgentManager::CDangerCover &cover) const
		{
			return						(Device.dwTimeGlobal > cover.m_level_time + cover.m_interval);
		}
	};

	struct CMemberCorpse {
		CAI_Stalker		*m_member;
		CAI_Stalker		*m_reactor;
		u32				m_time;

		IC			CMemberCorpse	(CAI_Stalker *member, CAI_Stalker *reactor, u32 time)
		{
			m_member	= member;
			m_reactor	= reactor;
			m_time		= time;
		}

		IC	bool	operator==		(CAI_Stalker *member) const
		{
			return		(m_member == member);
		}
	};

	struct CMemberGrenade {
		const CExplosive	*m_grenade;
		const CGameObject	*m_game_object;
		CAI_Stalker			*m_reactor;
		u32					m_time;

		IC			CMemberGrenade	(const CExplosive *grenade, const CGameObject *game_object, CAI_Stalker *reactor, u32 time)
		{
			m_grenade	= grenade;
			m_game_object = game_object;
			VERIFY		(!m_grenade || m_game_object);
			m_reactor	= reactor;
			m_time		= time;
		}

		IC	bool	operator==		(const CExplosive *grenade) const
		{
			return		(m_grenade == grenade);
		}
	};

public:
	typedef xr_vector<CMemberOrder>					MEMBER_STORAGE;
	typedef MEMBER_STORAGE::iterator				iterator;
	typedef MEMBER_STORAGE::const_iterator			const_iterator;

protected:
	MEMBER_STORAGE							m_members;
	xr_vector<CVisibleObject>				*m_visible_objects;
	xr_vector<CSoundObject>					*m_sound_objects;
	xr_vector<CHitObject>					*m_hit_objects;
	xr_vector<CEnemy>						m_enemies;
	mutable xr_vector<CMemberCorpse>		m_corpses;
	mutable xr_vector<CMemberGrenade>		m_grenades;
	mutable xr_vector<CDangerCover>			m_danger_covers;
	CAgentManagerMotivationPlanner			*m_brain;

protected:
	IC		xr_vector<CVisibleObject>		&visibles			() const;
	IC		xr_vector<CSoundObject>			&sounds				() const;
	IC		xr_vector<CHitObject>			&hits				() const;
	IC		iterator						member				(MemorySpace::squad_mask_type mask);

	template <typename T>
	IC		void							setup_mask			(xr_vector<T> &objects, CEnemy &enemy);
	IC		void							setup_mask			(CEnemy &enemy);
	template <typename T>
	IC		void							reset_memory_masks	(xr_vector<T> &objects);
	IC		void							reset_memory_masks	();
			void							fill_enemies		();
			void							compute_enemy_danger();
			void							assign_enemies		();
			void							permutate_enemies	();
			void							assign_enemy_masks	();
			float							evaluate			(const CEntityAlive *object0, const CEntityAlive *object1) const;
			void							exchange_enemies	(CMemberOrder &member0, CMemberOrder &member1);
			void							remove_old_danger_covers	();
			bool							process_corpse		(CMemberOrder &member);
			bool							process_grenade		(CMemberOrder &member);

public:
											CAgentManager		();
	virtual									~CAgentManager		();
	virtual float							shedule_Scale		();
	virtual void							shedule_Update		(u32 time_delta);	
	virtual BOOL							shedule_Ready		();
			void							add					(CEntity *member);
			void							remove				(CEntity *member, bool no_assert = false);
			void							distribute_enemies	();
			void							distribute_locations();
			void							setup_actions		();
			void							react_on_grenades	();
			void							react_on_member_death();
	IC		const CSetupAction				&action				(CAI_Stalker *object) const;
	IC		const CMemberOrder				&member				(CAI_Stalker *object) const;
	IC		const MEMBER_STORAGE			&members			() const;
	IC		MEMBER_STORAGE					&members			();
	IC		MemorySpace::squad_mask_type	mask				(const CAI_Stalker *object) const;
	IC		void							set_squad_objects	(xr_vector<CVisibleObject> *objects);
	IC		void							set_squad_objects	(xr_vector<CSoundObject> *objects);
	IC		void							set_squad_objects	(xr_vector<CHitObject> *objects);
			bool							suitable_location	(CAI_Stalker *object, CCoverPoint *location, bool use_enemy_info) const;
	IC		bool							group_behaviour		() const;
			void							add_danger_cover	(CCoverPoint *cover, u32 time, u32 interval, float radius) const;
			void							add_danger_location	(const Fvector &position, u32 time, u32 interval, float radius) const;
	IC		CDangerCover					*danger_cover		(CCoverPoint *cover) const;
	IC		CDangerCover					*danger_location	(const Fvector &position) const;
			float							cover_danger		(CCoverPoint *cover) const;
	IC		void							clear_danger_covers	();
	IC		shared_str						cName				() const;
	IC		CAgentManagerMotivationPlanner	&brain				() const;

public:
	IC		void							register_corpse		(CAI_Stalker *corpse) const;
	IC		xr_vector<CMemberCorpse>		&member_corpses		();

public:
	IC		void							register_grenade	(const CExplosive *grenade, const CGameObject *game_object) const;
	IC		xr_vector<CMemberGrenade>		&member_grenades	();
};

#include "agent_manager_inline.h"