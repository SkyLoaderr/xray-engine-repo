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

//#define SIGHT_TEST

void CSightAction::execute		()
{
	switch (m_sight_type) {
		case SightManager::eSightTypeCurrentDirection : {
			m_object->m_head.target	= m_object->m_head.current;
#ifdef SIGHT_TEST
			Msg					("%6d eSightTypeCurrentDirection",Level().timeServer());
#endif
			break;
		}
		case SightManager::eSightTypePathDirection : {
			m_object->SetDirectionLook();
#ifdef SIGHT_TEST
			Msg					("%6d eSightTypePathDirection",Level().timeServer());
#endif
			break;
		}
		case SightManager::eSightTypeDirection : {
			m_vector3d.getHP	(m_object->m_head.target.yaw,m_object->m_head.target.pitch);
			m_object->m_head.target.yaw		*= -1;
			m_object->m_head.target.pitch	*= -1;
#ifdef SIGHT_TEST
			Msg					("%6d eSightTypeDirection",Level().timeServer());
#endif
			break;
		}
		case SightManager::eSightTypePosition : {
			if (m_torso_look)
				m_object->SetFirePointLookAngles	(m_vector3d,m_object->m_head.target.yaw,m_object->m_head.target.pitch);
			else
				m_object->SetPointLookAngles		(m_vector3d,m_object->m_head.target.yaw,m_object->m_head.target.pitch);
#ifdef SIGHT_TEST
			Msg					("%6d %s",Level().timeServer(),m_torso_look ? "eSightTypeFirePosition" : "eSightTypePosition");
#endif
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
#ifdef SIGHT_TEST
			Msg					("%6d %s",Level().timeServer(),m_torso_look ? "eSightTypeFireObject" : "eSightTypeObject");
#endif
			break;
		}
		case SightManager::eSightTypeCover : {
			if (m_torso_look)
				m_object->SetLessCoverLook(m_object->level_vertex(),PI,m_path);
			else
				m_object->SetLessCoverLook(m_object->level_vertex(),m_path);
#ifdef SIGHT_TEST
			Msg					("%6d %s",Level().timeServer(),m_torso_look ? "eSightTypeFireCover" : "eSightTypeCover");
#endif
			break;
		}
		case SightManager::eSightTypeSearch : {
			if (m_torso_look)
				m_object->SetLessCoverLook(m_object->level_vertex(),PI,m_path);
			else
				m_object->SetLessCoverLook(m_object->level_vertex(),m_path);
			m_object->m_head.target.pitch	= PI_DIV_4;
#ifdef SIGHT_TEST
			Msg					("%6d %s",Level().timeServer(),m_torso_look ? "eSightTypeFireSearch" : "eSightTypeSearch");
#endif
			break;
		}
		default	: NODEFAULT;
	}
}