#pragma once

IC void	CBaseMonster::set_action(EAction action)
{
	MotionMan.m_tAction = action;
}

IC	CMonsterMovement &CBaseMonster::movement			() const
{
	VERIFY		(m_movement_manager);
	return		(*m_movement_manager);
}
