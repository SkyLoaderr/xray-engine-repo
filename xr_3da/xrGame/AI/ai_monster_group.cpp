#include "stdafx.h"
#include "ai_monster_group.h"

CMonsterGroup::CMonsterGroup()
{
	leader = NULL;
}

CMonsterGroup::~CMonsterGroup()
{
}

void	CMonsterGroup::AddMember(TEntity *pE)
{
	GROUP_MAP_IT	member_it = group.find(pE);
	if (member_it != group.end()) return;

	GTask task;
	group.insert(mk_pair(pE, task));

	if (!leader) leader = pE;
}

void	CMonsterGroup::DeleteMember(TEntity *pE)
{
	GROUP_MAP_IT	member_it = group.find(pE);
	R_ASSERT2((member_it != group.end()), "Try to remove a member from a group which has already been removed!");
	
	bool b_need_leader = ((leader == pE) ? true : false);
	group.erase(member_it);

	if (b_need_leader) {
		if (group.empty()) leader = NULL;
		else leader = group.begin()->first;
	}
}




