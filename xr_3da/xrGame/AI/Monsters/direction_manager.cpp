#include "stdafx.h"
#include "direction_manager.h"
#include "basemonster/base_monster.h"
#include "../../detail_path_manager.h"
#include "ai_monster_movement.h"

void CDirectionManager::reinit()
{
	m_delay				= 0;
	m_time_last_faced	= 0;
}

void CDirectionManager::face_target(const Fvector &position, u32 delay)
{
	if (m_time_last_faced + delay > Device.dwTimeGlobal) return;
	
	m_delay = delay;

	float	yaw, pitch;
	Fvector dir;

	dir.sub		(position, m_object->Position());
	dir.getHP	(yaw,pitch);
	yaw			*= -1;
	yaw			= angle_normalize(yaw);
	
	m_object->movement().m_body.target.yaw	= yaw;

	m_time_last_faced			= Device.dwTimeGlobal;
}

void CDirectionManager::use_path_direction(bool reversed)
{
	float yaw,pitch;
	m_object->movement().detail().direction().getHP	(yaw,pitch);

	if (fsimilar(yaw,0.f,EPS_S)) return;

	m_object->movement().m_body.target.yaw = angle_normalize((reversed) ? (-yaw + PI) : (-yaw));
}

bool CDirectionManager::is_face_target(const Fvector &position, float eps_angle)
{
	float						h, p;
	Fvector().sub				(position, m_object->Position()).getHP(h,p);
	
	float						my_h;
	m_object->Direction().getHP	(my_h, p);

	if (angle_difference(h,my_h) > eps_angle) return false;
	
	return true;
}

void CDirectionManager::face_target(const CObject *obj,	u32 delay) 
{
	face_target	(obj->Position(), delay);
}

bool CDirectionManager::is_face_target(const CObject *obj, float eps_angle) 
{
	return is_face_target(obj->Position(), eps_angle);
}

void CDirectionManager::force_direction(const Fvector &dir)
{
	float	yaw, pitch;
	
	dir.getHP	(yaw,pitch);
	yaw			*= -1;
	yaw			= angle_normalize(yaw);

	m_object->movement().m_body.current.yaw		= yaw;
	m_object->movement().m_body.target.yaw		= yaw;
}
