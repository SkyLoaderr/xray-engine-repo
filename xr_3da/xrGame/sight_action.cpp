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
			Fvector					look_pos;
			m_object_to_look->Center(look_pos);
			look_pos.x				= m_object_to_look->Position().x;
			look_pos.z				= m_object_to_look->Position().z;
			if (m_torso_look)
				m_object->SetFirePointLookAngles	(look_pos,m_object->m_head.target.yaw,m_object->m_head.target.pitch);
			else
				m_object->SetPointLookAngles		(look_pos,m_object->m_head.target.yaw,m_object->m_head.target.pitch);
			break;
		}
		case SightManager::eSightTypeCover : {
			if (m_torso_look)
				m_object->SetLessCoverLook(m_object->level_vertex(),PI,m_path);
			else
				m_object->SetLessCoverLook(m_object->level_vertex(),m_path);
			break;
		}
		case SightManager::eSightTypeSearch : {
			if (m_torso_look)
				m_object->SetLessCoverLook(m_object->level_vertex(),PI,m_path);
			else
				m_object->SetLessCoverLook(m_object->level_vertex(),m_path);
			m_object->m_head.target.pitch	= PI_DIV_4;
			break;
		}
		default	: NODEFAULT;
	}
}