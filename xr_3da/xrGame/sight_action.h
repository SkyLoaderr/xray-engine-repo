////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_action.h
//	Created 	: 27.12.2003
//  Modified 	: 03.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sight action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sight_manager_space.h"
#include "control_action.h"

namespace MemorySpace {
	struct CMemoryInfo;
};

class CAI_Stalker;
class CGameObject;

class CSightAction : public CControlAction {
public:
	typedef SightManager::ESightType	ESightType;
	typedef MemorySpace::CMemoryInfo	CMemoryInfo;

protected:
	ESightType			m_sight_type;
	bool				m_torso_look;
	bool				m_path;
	Fvector				m_vector3d;
	const CGameObject	*m_object_to_look;
	const CMemoryInfo	*m_memory_object;
	LPCSTR				m_bone_name;

public:
	IC					CSightAction		();
	IC					CSightAction		(const ESightType &sight_type, bool	torso_look = false, bool path = false);
	IC					CSightAction		(const ESightType &sight_type, const Fvector &vector3d, bool torso_look = false);
	IC					CSightAction		(const ESightType &sight_type, const Fvector *vector3d);
	IC					CSightAction		(const CGameObject *object_to_look, bool torso_look = false);
	IC					CSightAction		(const CGameObject *object_to_look, bool torso_look, LPCSTR bone_name);
	IC					CSightAction		(const CMemoryInfo *memory_object, bool	torso_look = false);
			void		execute				();
	IC		void		set_vector3d		(const Fvector &position);
	IC		void		set_object_to_look	(const CGameObject *object_to_look);
	IC		void		set_memory_object	(const CMemoryInfo *object_to_look);
	IC		bool		operator==			(const CSightAction &sight_action) const;
			void		remove_links		(CObject *object);
};

#include "sight_action_inline.h"