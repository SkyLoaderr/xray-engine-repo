////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_action.cpp
//	Created 	: 27.12.2003
//  Modified 	: 03.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sight action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sight_action.h"
#include "ai/stalker/ai_stalker.h"

CSightAction::CSightAction		(const ESightType &sight_type, bool	torso_look, u32 inertia_time, float weight, bool path) :
	m_sight_type		(sight_type),
	m_torso_look		(torso_look),
	m_path				(path),
	m_inertia_time		(inertia_time),
	m_object_to_look	(0),
	m_memory_object		(0),
	m_object			(0),
	m_weight			(weight),
	m_start_time		(u32(-1))
{
}

CSightAction::CSightAction		(const ESightType &sight_type, const Fvector &vector3d, bool torso_look, u32 inertia_time, float weight) :
	m_sight_type		(sight_type),
	m_vector3d			(vector3d),
	m_torso_look		(torso_look),
	m_inertia_time		(inertia_time),
	m_object_to_look	(0),
	m_memory_object		(0),
	m_object			(0),
	m_weight			(weight),
	m_start_time		(u32(-1))
{
}

CSightAction::CSightAction		(const CGameObject *object_to_look, bool torso_look, u32 inertia_time, float weight) :
	m_sight_type		(SightManager::eSightTypeObject),
	m_torso_look		(torso_look),
	m_object_to_look	(object_to_look),
	m_inertia_time		(inertia_time),
	m_memory_object		(0),
	m_object			(0),
	m_weight			(weight),
	m_start_time		(u32(-1))
{
}

CSightAction::CSightAction		(const CMemoryInfo *memory_object, bool torso_look, u32 inertia_time, float weight) :
	m_sight_type		(SightManager::eSightTypeObject),
	m_torso_look		(torso_look),
	m_memory_object		(memory_object),
	m_inertia_time		(inertia_time),
	m_object_to_look	(0),
	m_object			(0),
	m_weight			(weight),
	m_start_time		(u32(-1))
{
}

CSightAction::~CSightAction		()
{
}

void CSightAction::set_object	(CAI_Stalker *object)
{
	m_object			= object;
}

bool CSightAction::applicable	() const
{
	return				(true);
}

bool CSightAction::completed	() const
{
	return				(Level().timeServer() - m_start_time >= m_inertia_time);
}

void CSightAction::initialize	()
{
	m_start_time		= Level().timeServer();
}

void CSightAction::execute		()
{
	switch (m_sight_type) {
		case SightManager::eSightTypeCurrentDirection : {
			m_object->m_head.target	= m_object->m_head.current;
			break;
		}
		case SightManager::eSightTypePathDirection : {
			m_object->SetDirectionLook();
			break;
		}
		case SightManager::eSightTypeDirection : {
			m_vector3d.getHP	(m_object->m_head.target.yaw,m_object->m_head.target.pitch);
			m_object->m_head.target.yaw		*= -1;
			m_object->m_head.target.pitch	*= -1;
			break;
		}
		case SightManager::eSightTypePosition : {
			if (m_torso_look)
				m_object->SetFirePointLookAngles	(m_vector3d,m_object->m_head.target.yaw,m_object->m_head.target.pitch);
			else
				m_object->SetPointLookAngles		(m_vector3d,m_object->m_head.target.yaw,m_object->m_head.target.pitch);
			break;
		}
		case SightManager::eSightTypeObject : {
			break;
		}
		case SightManager::eSightTypeCover : {
			if (m_path) {
				if (m_torso_look)
					m_object->SetLessCoverLook(m_object->level_vertex(),PI,true);
				else
					m_object->SetLessCoverLook(m_object->level_vertex(),true);
			}
			else {
				if (m_torso_look)
					m_object->SetLessCoverLook(m_object->level_vertex(),PI,true);
				else
					m_object->SetLessCoverLook(m_object->level_vertex(),true);
			}
			break;
		}
		case SightManager::eSightTypeSearch : {
			if (m_torso_look) {
			}
			else {
			}
			break;
		}
	}
}

void CSightAction::finalize		()
{
}