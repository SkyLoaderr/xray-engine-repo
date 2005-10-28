////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_smart_terrain_task_inline.h
//	Created 	: 20.09.2005
//  Modified 	: 20.09.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife smart terrain task inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef DEBUG
IC	const shared_str &CALifeSmartTerrainTask::patrol_path_name	() const
{
	return					(m_patrol_path_name);
}

IC	const u32 &CALifeSmartTerrainTask::patrol_point_index		() const
{
	return					(m_patrol_point_index);
}
#endif

IC	const CPatrolPoint &CALifeSmartTerrainTask::patrol_point	() const
{
	VERIFY					(m_patrol_point);
	return					(*m_patrol_point);
}
