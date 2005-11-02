#pragma once
#include "actor_statistic_defs.h"
#include "alife_registry_wrappers.h"

class CActorStatisticMgr
{
private:
	CActorStatisticsWrapper*		m_actor_stats_wrapper;
	vStatSectionData&				GetStorage		();
public:
							CActorStatisticMgr		();
							~CActorStatisticMgr		();
	SStatSectionData&		GetSection				(int key);
	
	void					AddPoints				(int key, const shared_str& detail_key, s32 cnt, s32 pts);
	s32						GetSectionPoints		(int key);
};