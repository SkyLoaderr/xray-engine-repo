#include "stdafx.h"
#include "direction_manager.h"
#include "basemonster/base_monster.h"

void CDirectionManager::reinit()
{
	m_delay				= 0;
	m_time_last_faced	= 0;
}

void CDirectionManager::face_target(const Fvector &position, u32 delay)
{
	if (m_time_last_faced + delay > Level().timeServer()) return;
	
	m_delay = delay;

	float	yaw, pitch;
	Fvector dir;

	dir.sub		(position, m_object->Position());
	dir.getHP	(yaw,pitch);
	yaw			*= -1;
	yaw			= angle_normalize(yaw);
	
	m_object->m_body.target.yaw	= yaw;

	m_time_last_faced			= Level().timeServer();
}

void CDirectionManager::face_target(const CObject *obj, u32 delay)
{
	face_target	(obj->Position(), delay);
}

void CDirectionManager::use_path_direction(bool reversed)
{
	float yaw,pitch;
	m_object->CDetailPathManager::direction().getHP	(yaw,pitch);

	if (fsimilar(yaw,0.f,EPS_S)) return;

	m_object->m_body.target.yaw = angle_normalize((reversed) ? (-yaw + PI) : (-yaw));
}

