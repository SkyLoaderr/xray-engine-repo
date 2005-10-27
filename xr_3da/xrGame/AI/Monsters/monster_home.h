#pragma once

class CBaseMonster;
class CPatrolPath;

class CMonsterHome {
	CBaseMonster		*m_object;
	const CPatrolPath	*m_path;

	float				m_radius_min;
	float				m_radius_max;

public:
			CMonsterHome	(CBaseMonster *obj) {m_object = obj;}

	void	load				(LPCSTR line);
	void	setup				(LPCSTR path_name, float min_radius, float max_radius);
	void	remove_home			();

	u32		get_place			();
	u32		get_place_in_cover	();
	bool	at_home				();
	bool	at_home				(const Fvector &pos);
};