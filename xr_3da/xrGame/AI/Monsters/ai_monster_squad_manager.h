#pragma once

class CMonsterSquad;
class CEntity;

class CMonsterSquadManager {
	
	DEFINE_VECTOR(CMonsterSquad*, MONSTER_SQUAD_VEC, MONSTER_SQUAD_VEC_IT);
	DEFINE_VECTOR(MONSTER_SQUAD_VEC, MONSTER_TEAM_VEC,MONSTER_TEAM_VEC_IT);

	MONSTER_TEAM_VEC team;

public:
	CMonsterSquadManager	();
	~CMonsterSquadManager	();

	void			register_member			(u8 team_id, u8 squad_id, CEntity *e);
	void			remove_member			(u8 team_id, u8 squad_id, CEntity *e);

	CMonsterSquad	*get_squad				(u8 team_id, u8 squad_id);
	CMonsterSquad	*get_squad				(const CEntity *entity);

	void			update					(CEntity *entity);

	void			net_relcase				(CObject *O);
};


IC CMonsterSquadManager &monster_squad();
extern CMonsterSquadManager *g_monster_squad;

#include "ai_monster_squad_manager_inline.h"
