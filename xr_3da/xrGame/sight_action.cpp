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
		default	: NODEFAULT;
	}
}