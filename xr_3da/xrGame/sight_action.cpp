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
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"

//#define SIGHT_TEST

void CSightAction::execute		()
{
	switch (m_sight_type) {
		case SightManager::eSightTypeCurrentDirection : {
			object().movement().m_head.target	= object().movement().m_head.current;
#ifdef SIGHT_TEST
			Msg					("%6d eSightTypeCurrentDirection",Device.dwTimeGlobal);
#endif
			break;
		}
		case SightManager::eSightTypePathDirection : {
			object().sight().SetDirectionLook();
#ifdef SIGHT_TEST
			Msg					("%6d eSightTypePathDirection",Device.dwTimeGlobal);
#endif
			break;
		}
		case SightManager::eSightTypeDirection : {
			m_vector3d.getHP	(object().movement().m_head.target.yaw,object().movement().m_head.target.pitch);
			object().movement().m_head.target.yaw		*= -1;
			object().movement().m_head.target.pitch	*= -1;
#ifdef SIGHT_TEST
			Msg					("%6d eSightTypeDirection",Device.dwTimeGlobal);
#endif
			break;
		}
		case SightManager::eSightTypePosition : {
			if (m_torso_look)
				object().sight().SetFirePointLookAngles	(m_vector3d,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch);
			else
				object().sight().SetPointLookAngles		(m_vector3d,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch);
#ifdef SIGHT_TEST
			Msg					("%6d %s",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFirePosition" : "eSightTypePosition");
#endif
			break;
		}
		case SightManager::eSightTypeObject : {
			Fvector					look_pos;
			m_object_to_look->Center(look_pos);
			look_pos.x				= m_object_to_look->Position().x;
			look_pos.z				= m_object_to_look->Position().z;
			if (m_torso_look)
				object().sight().SetFirePointLookAngles	(look_pos,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch,m_object_to_look);
			else
				object().sight().SetPointLookAngles		(look_pos,object().movement().m_head.target.yaw,object().movement().m_head.target.pitch,m_object_to_look);
#ifdef SIGHT_TEST
			Msg					("%6d %s",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFireObject" : "eSightTypeObject");
#endif
			break;
		}
		case SightManager::eSightTypeCover : {
			if (m_torso_look)
				object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),PI,m_path);
			else
				object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),m_path);
#ifdef SIGHT_TEST
			Msg					("%6d %s",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFireCover" : "eSightTypeCover");
#endif
			break;
		}
		case SightManager::eSightTypeSearch : {
			m_torso_look					= false;
			if (m_torso_look)
				object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),PI,m_path);
			else
				object().sight().SetLessCoverLook(m_object->ai_location().level_vertex(),m_path);
			object().movement().m_head.target.pitch	= PI_DIV_4;
#ifdef SIGHT_TEST
			Msg					("%6d %s",Device.dwTimeGlobal,m_torso_look ? "eSightTypeFireSearch" : "eSightTypeSearch");
#endif
			break;
		}
		default	: NODEFAULT;
	}
}