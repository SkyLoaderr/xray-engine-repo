////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action_manager.h"
#include "member_order.h"

class CEntity;
class CAI_Stalker;
class CSetupAction;

class CAgentManager :
	public CMotivationActionManager<CAgentManager>,
	public ISheduled
{
	enum {
		DANGER_INTERVAL = u32(30000),
	};
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
			return			(order.object() == m_object);
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
		CCoverPoint		*m_cover;
		u32				m_level_time;

		IC	bool	operator==	(CCoverPoint *cover) const
		{
			return		(m_cover == cover);
		}
	};

	struct CRemoveOldDangerCover {
		IC	bool	operator()	(const CAgentManager::CDangerCover &cover) const
		{
			return						(Level().timeServer() > cover.m_level_time + CAgentManager::DANGER_INTERVAL);
		}
	};

public:
	typedef xr_vector<CMemberOrder>					MEMBER_STORAGE;
	typedef MEMBER_STORAGE::iterator				iterator;
	typedef MEMBER_STORAGE::const_iterator			const_iterator;

protected:
	typedef CMotivationActionManager<CAgentManager>	inherited;
	
	using inherited::add_condition;

protected:
	MEMBER_STORAGE							m_members;
	xr_vector<CVisibleObject>				*m_visible_objects;
	xr_vector<CSoundObject>					*m_sound_objects;
	xr_vector<CHitObject>					*m_hit_objects;
	xr_vector<CEnemy>						m_enemies;
	mutable xr_vector<CDangerCover>			m_danger_covers;

protected:
			void							add_motivations		();
			void							add_evaluators		();
			void							add_actions			();
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

public:
											CAgentManager		();
	virtual									~CAgentManager		();
	virtual float							shedule_Scale		();
	virtual void							shedule_Update		(u32 time_delta);	
	virtual BOOL							shedule_Ready		();
	virtual void							reload				(LPCSTR section);	
			void							add					(CEntity *member);
			void							remove				(CEntity *member, bool no_assert = false);
			void							distribute_enemies	();
			void							distribute_locations();
			void							setup_actions		();
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
			void							add_danger_cover	(CCoverPoint *cover, u32 time) const;
	IC		CDangerCover					*danger_cover		(CCoverPoint *cover) const;
			float							cover_danger		(CCoverPoint *cover) const;
	IC		void							clear_danger_covers	();
};

#include "agent_manager_inline.h"