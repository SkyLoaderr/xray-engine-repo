#pragma once
#include "ai_monster_defs.h"

class CAI_Biting;

class CMonsterHitMemory {
	CAI_Biting				*monster;
	TTime					time_memory;

	MONSTER_HIT_VECTOR		m_hits;

public:
						CMonsterHitMemory		();
						~CMonsterHitMemory		();

	void				init_external			(CAI_Biting *M, TTime mem_time);
	void				update					();

	// -----------------------------------------------------
	bool				is_hit					() {return !m_hits.empty();}
	bool				is_hit					(CObject *pO);

	void				add_hit					(CObject *who);

private:
	void				remove_non_actual		();
};
