#include "stdafx.h"
#include "monster_hit_memory.h"
#include "biting/ai_biting.h"

CMonsterHitMemory::CMonsterHitMemory()
{
	monster			= 0;
	time_memory		= 10000; 
}

CMonsterHitMemory::~CMonsterHitMemory()
{
}

void CMonsterHitMemory::init_external(CAI_Biting *M, TTime mem_time) 
{
	monster = M; 
	time_memory = mem_time;
}


void CMonsterHitMemory::update()
{
	// удалить устаревшие hits
	remove_non_actual();
}

bool CMonsterHitMemory::is_hit(CObject *pO)
{
	MONSTER_HIT_VECTOR_IT it = find(m_hits.begin(), m_hits.end(), pO);
	
	return (it != m_hits.end());
}

void CMonsterHitMemory::add_hit(CObject *who)
{
	SMonsterHit			new_hit_info;
	new_hit_info.object = who;
	new_hit_info.time	= Level().timeServer();

	MONSTER_HIT_VECTOR_IT it = find(m_hits.begin(), m_hits.end(), who);

	if (it == m_hits.end()) m_hits.push_back(new_hit_info);
	else *it = new_hit_info;
}

struct predicate_old_hit {
	TTime cur_time;
	TTime mem_time;

	predicate_old_hit(TTime mem_time, TTime cur_time){
		this->cur_time = cur_time;
		this->mem_time = mem_time;
	}

	IC bool	operator() (const SMonsterHit &hit_info) {
		return ((mem_time + hit_info.time) < cur_time);
	}

};

void CMonsterHitMemory::remove_non_actual() 
{
	MONSTER_HIT_VECTOR_IT it = remove_if(m_hits.begin(), m_hits.end(), predicate_old_hit(time_memory, Level().timeServer()));
	m_hits.erase(it, m_hits.end());
}
