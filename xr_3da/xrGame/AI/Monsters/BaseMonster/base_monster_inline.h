#pragma once

/////////////////////////////////////////////////////////////////////////////////////
// Attack Stops
/////////////////////////////////////////////////////////////////////////////////////

IC void CBaseMonster::AS_Init() {
	_as.active	= false;
}
IC void CBaseMonster::AS_Load(LPCSTR section) {
	_as.min_dist	= pSettings->r_float(section,"as_min_dist");
	_as.step		= pSettings->r_float(section,"as_step");
}
IC void CBaseMonster::AS_Start() {
	_as.active			= true;
	_as.prev_prev_hit	= true;
	_as.prev_hit		= true;
}
IC void CBaseMonster::AS_Stop() {
	_as.active				= false;
	m_fCurMinAttackDist		= get_sd()->m_fMinAttackDist;
}
IC void CBaseMonster::AS_Check(bool hit_success) {
	if (!_as.active) return;

	_as.prev_prev_hit	= _as.prev_hit;
	_as.prev_hit		= hit_success;

	if ((!_as.prev_prev_hit && !_as.prev_prev_hit) && ((m_fCurMinAttackDist >= _as.min_dist) && (m_fCurMinAttackDist >= _as.min_dist + _as.step))) {
		m_fCurMinAttackDist -= _as.step;
	} else if (_as.prev_prev_hit && _as.prev_prev_hit && (m_fCurMinAttackDist < get_sd()->m_fMinAttackDist - _as.step)) {
		m_fCurMinAttackDist += _as.step;
	}
}

IC bool CBaseMonster::AS_Active() {return _as.active;}

/////////////////////////////////////////////////////////////////////////////////////

IC void	CBaseMonster::set_action(EAction action)
{
	MotionMan.m_tAction = action;
}
