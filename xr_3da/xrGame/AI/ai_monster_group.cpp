#include "stdafx.h"
#include "ai_monster_group.h"
#include "..\\entity.h"
#include "ai_monster_defs.h"

//////////////////////////////////////////////////////////////////////////
// SQUAD MANAGER Implementation
//////////////////////////////////////////////////////////////////////////

CSquadManager::CSquadManager	()
{
}
CSquadManager::~CSquadManager	()
{
	for (SQUADS_MAP_IT it = squads.begin(); it != squads.end(); it++) xr_delete(it->second);
	squads.clear();
}

void CSquadManager::RegisterMember(u8 squad_id, CEntity *e)
{
	// искать группу с данным id
	CMonsterSquad *pSquad = GetSquad(squad_id);

	if (!pSquad) {
		pSquad = xr_new<CMonsterSquad>(squad_id);
		squads.insert(mk_pair(squad_id,pSquad));
	}
	pSquad->RegisterMember(e);
}

void CSquadManager::RemoveMember(u8 squad_id, CEntity *e)
{
	CMonsterSquad *pSquad = GetSquad(squad_id);
	R_ASSERT(pSquad);

	pSquad->RemoveMember(e);
}

CMonsterSquad *CSquadManager::GetSquad(u8 squad_id)
{
	// искать группу с данным id
	CMonsterSquad *pSquad = 0;

	SQUADS_MAP_IT it = squads.find(squad_id);
	if (it != squads.end()) pSquad = it->second;
	
	return pSquad;
}

void CSquadManager::Dump()
{
	for (SQUADS_MAP_IT it=squads.begin(); it != squads.end(); it++) {
		it->second->Dump();
	}
}

u8 CSquadManager::TransformPriority(ESquadCommand com)
{	
	u8 ret_val = 0;

	switch (com) {
	case SC_EXPLORE:		ret_val = 15;		break;
	case SC_ATTACK:			ret_val = 30;		break;
	case SC_THREATEN:		ret_val = 30;		break;
	case SC_COVER:			ret_val = 20;		break;
	case SC_FOLLOW:			ret_val = 15;		break;
	case SC_FEEL_DANGER:	ret_val = 20;		break;
	}

	return ret_val;
}

//////////////////////////////////////////////////////////////////////////
// MONSTER SQUAD Implementation
//////////////////////////////////////////////////////////////////////////

void CMonsterSquad::RegisterMember(CEntity *pE)
{
	GTask blank_task;
	InitTask(&blank_task);
	squad.insert(mk_pair(pE, blank_task));

	if (!leader) leader = pE;
}

void CMonsterSquad::RemoveMember(CEntity *pE)
{
	SQUAD_MAP_IT it = squad.find(pE);
	R_ASSERT(it != squad.end());
	
	squad.erase(it);

	if (leader == pE)  {
		if (squad.empty()) leader = 0;
		else leader = squad.begin()->first;
	}
}

void CMonsterSquad::InitTask(GTask *task)
{
	task->state.ttl		= 0;
	task->target.entity	= 0;
}

GTask &CMonsterSquad::GetTask(CEntity *pE)
{
	SQUAD_MAP_IT	it = squad.find(pE);
	R_ASSERT2((it != squad.end()), "Try to get task of non-existent memeber");

	return (it->second);
}

void CMonsterSquad::ProcessGroupIntel()
{
	ENTITY_VEC	enemies, members;

	// получить тактические данные о группе
	for (SQUAD_MAP_IT it = squad.begin(); it != squad.end(); it++) 
		if (it->first->g_Alive()) members.push_back(it->first);

	//if (members.size() < 2) return;

	// получить тактические данные о противнике
	//objVisible &temp_enemies = GetKnownEnemies();
	objVisible &temp_enemies = Level().Teams[leader->g_Team()].Squads[leader->g_Squad()].KnownEnemys;

	for (int i=0, n=temp_enemies.size(); i<n; i++) {
		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(temp_enemies[i].key);
		if (pE->g_Alive()) enemies.push_back(pE);
	}

	// распределить задачи
	CommonAttack(enemies,members);

}


void CMonsterSquad::CommonAttack(ENTITY_VEC &enemies, ENTITY_VEC &members)
{
//	for (ENTITY_VEC_IT it = members.begin();it != members.end(); it++) {
//		GTask &pTask= GetTask(*it);
//
//		if ((pTask.state.type == TS_REQUEST) || (pTask.state.ttl < Level().timeServer())) {
//			pTask.state.command		= SC_ATTACK;
//			pTask.state.type		= TS_REQUEST;
//			pTask.state.ttl			= Level().timeServer() + 3000;
//			pTask.target.entity		= GetNearestEnemy(*it,&enemies);
//			if (pTask.target.entity) pTask.target.pos = pTask.target.entity->Position();  
//		}
//	}

	for (ENTITY_VEC_IT it = members.begin();it != members.end(); it++) {
		GTask &pTask= GetTask(*it);

		if ((pTask.state.type == TS_REQUEST) || (pTask.state.ttl < Level().timeServer())) {
			
			if (pTask.state.type == TS_REQUEST) 
				LOG_EX2("_______________ TS_REQUEST, TTL = [%u], LTS = [%u] ", *"*/ pTask.state.ttl, Level().timeServer() /*"*)
			else if (pTask.state.type == TS_PROGRESS)
				LOG_EX2("_______________ TS_REQUEST, TTL = [%u], LTS = [%u] ", *"*/ pTask.state.ttl, Level().timeServer() /*"*);
			
			pTask.state.command		= SC_FOLLOW;
			
			pTask.state.type		= TS_REQUEST;
			pTask.state.ttl			= Level().timeServer() + 3000;
			//pTask.target.entity		= GetNearestEnemy(*it,&enemies);
			//if (pTask.target.entity) pTask.target.pos = pTask.target.entity->Position();  
			pTask.target.pos = Level().CurrentEntity()->Position();
		}
	}

}

CEntity	*CMonsterSquad::GetNearestEnemy(CEntity *t, ENTITY_VEC *ev)
{
	float	best_dist = flt_max;
	CEntity *best_entity = 0;

	for (ENTITY_VEC_IT it = ev->begin(); it != ev->end(); it++) {
		float cur_dist = (*it)->Position().distance_to((*it)->Position());
		if (cur_dist < best_dist) best_entity = (*it);
	}

	return best_entity;
}

void CMonsterSquad::Dump()
{
	LOG_EX2("SQUAD:: ----- BEGIN Dump for squad #%u,  Time = [%u]-------", *"*/ id, Level().timeServer() /*"*);
	if (leader) LOG_EX2("SQUAD:: Team size = [%u] Leader = [%s] ", *"*/ squad.size(), leader->cName() /*"*)
	else LOG_EX2("SQUAD:: Team size = [%u] No leader ", *"*/ squad.size() /*"*);
	
	int i=0;
	for (SQUAD_MAP_IT it=squad.begin(); it != squad.end(); it++, i++) {
		LOG_EX2("SQUAD:: -- Member [%u], Name = [%s] Task desc:", *"*/ i+1, it->first->cName() /*"*);

		string32	s_temp;
		switch (it->second.state.type) {
			case TS_REQUEST:	strcpy(s_temp, "TS_REQUEST");	break;
			case TS_PROGRESS:	strcpy(s_temp, "TS_PROGRESS");	break;
			case TS_REFUSED:	strcpy(s_temp, "TS_REFUSED");	break;
		}

		if (!it->second.state.ttl) LOG_EX("SQUAD:: Task:: No Command")
		else LOG_EX2("SQUAD:: Task:: Command = [%s], Time = [%u]", *"*/ s_temp, it->second.state.ttl /*"*); 
		
		if (!it->second.target.entity)	LOG_EX("SQUAD:: Task:: No target")
		else LOG_EX2("SQUAD:: Task:: Target: pos = [%f,%f,%f], name = [%s]", *"*/ VPUSH(it->second.target.pos), it->second.target.entity->cName() /*"*); 
	}

	LOG_EX2("SQUAD:: ----- END Dump for squad #%u -------", *"*/ id /*"*);
}

