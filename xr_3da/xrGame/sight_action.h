////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_action.h
//	Created 	: 27.12.2003
//  Modified 	: 03.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sight action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sight_manager_space.h"
#include "memory_space.h"

class CAI_Stalker;
class CGameObject;

class CSightAction {
public:
	typedef SightManager::ESightType ESightType;
	typedef MemorySpace::CMemoryInfo CMemoryInfo;

protected:
	CAI_Stalker			*m_object;
	ESightType			m_sight_type;
	bool				m_torso_look;
	bool				m_path;
	Fvector				m_vector3d;
	const CGameObject	*m_object_to_look;
	const CMemoryInfo	*m_memory_object;
	u32					m_inertia_time;
	u32					m_start_time;
	float				m_weight;

public:
						CSightAction		(const ESightType &sight_type, bool	torso_look = false, u32 inertia_time = u32(-1), float weight = 1.f, bool path = false);
						CSightAction		(const ESightType &sight_type, const Fvector &vector3d, bool torso_look = false, u32 inertia_time = u32(-1), float weight = 1.f);
						CSightAction		(const CGameObject *object_to_look, bool torso_look = false, u32 inertia_time = u32(-1), float weight = 1.f);
						CSightAction		(const CMemoryInfo *memory_object, bool	torso_look = false, u32 inertia_time = u32(-1), float weight = 1.f);
	virtual				~CSightAction		();
			void		set_object			(CAI_Stalker *object);
			bool		applicable			() const;
			bool		completed			() const;
			void		initialize			();
			void		execute				();
			void		finalize			();
	IC		bool		use_torso_look		() const;
	IC		float		weight				() const;
	IC		ESightType	sight_type			() const;
	IC		const Fvector &vector3d			() const;
};

#include "sight_action_inline.h"