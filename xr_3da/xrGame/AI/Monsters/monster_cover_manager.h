#pragma once

class CCoverPoint;
class CCoverEvaluator;
class CBaseMonster;

class CMonsterCoverManager {
	CBaseMonster	*m_object;
	CCoverEvaluator	*m_ce_best;

public:
				CMonsterCoverManager	(CBaseMonster *monster);

	void		load					();
	// найти лучший ковер к относительно "position"
	CCoverPoint *find_cover				(const Fvector &position, float min_pos_distance, float	max_pos_distance, float deviation = 0.f);
};