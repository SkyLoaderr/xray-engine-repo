#include "stdafx.h"
#include "ai_monster_group.h"
#include "../entity_alive.h"
#include "ai_monster_defs.h"
#include "../movement_manager.h"

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
	case SC_NONE:			ret_val = 0;		break;
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
	
	SEntityState S;
	S.pEnemy = 0;
	states.insert(mk_pair(pE, S));

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
	// Получить тактические данные:
	// 1. Информация о лидере (его состояние, цель)
	// 2. Список и расположение членов группы
	// 3. Список и расположение врагов
	
	
	// получить целевую ноду лидера
	CMovementManager *pM = dynamic_cast<CMovementManager *>(leader);
	if (!pM) return;
	
	if ((pM->speed() < EPS_L) || pM->CDetailPathManager::completed(pM->Position()))
		dest_node = pM->level_vertex_id();
	else {
		dest_node = pM->CDetailPathManager::path()[pM->CDetailPathManager::path().size()-1].vertex_id;
	}
	
	// получить всех членов группы
	u32 num = members.size();
	members.clear();
	members.reserve(num);

	// получить тактические данные о группе
	for (SQUAD_MAP_IT it = squad.begin(); it != squad.end(); it++) 
		if (it->first->g_Alive()) members.push_back(it->first);

	// Выбрать действие
	
	switch (leader_state) {
	case LS_IDLE:
		TaskIdle();
		break;
	}

}

bool CMonsterSquad::ActiveTask(GTask *task)
{
	if (task->state.type == TS_PROGRESS) {
		if (task->state.ttl > Level().timeServer()) return true;
		else task->state.ttl = 0;
}

	return false;	
}

void CMonsterSquad::ProcessGroupIntel(const GTask &task)
{
	// получить тактические данные о группе
	for (SQUAD_MAP_IT it = squad.begin(); it != squad.end(); it++) {
		if (it->first->g_Alive()) {
			// всем в группе выполнять задачу task
			GTask &pTask= GetTask(it->first);

			if (ActiveTask(&pTask)) {
				u8 p1, p2;

				p1 = Level().SquadMan.TransformPriority(pTask.state.command);
				p2 = Level().SquadMan.TransformPriority(task.state.command);
				
				// проверить приоритет новой задачи и текущей
				if (p1 < p2) {
					pTask = task;
					pTask.state.type = TS_REQUEST;
				}
			} else {
				pTask = task;
				pTask.state.type = TS_REQUEST;
			}
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
	if (leader) LOG_EX2("SQUAD:: Team size = [%u] Leader = [%s] ", *"*/ squad.size(), leader->cName() /*"*);
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

		if (!it->second.state.ttl) LOG_EX("SQUAD:: Task:: No Command");
		else LOG_EX2("SQUAD:: Task:: Command = [%s], Time = [%u]", *"*/ s_temp, it->second.state.ttl /*"*); 
		
		if (!it->second.target.entity)	LOG_EX("SQUAD:: Task:: No target");
		else LOG_EX2("SQUAD:: Task:: Target: pos = [%f,%f,%f], name = [%s]", *"*/ VPUSH(it->second.target.pos), it->second.target.entity->cName() /*"*); 
	}

	LOG_EX2("SQUAD:: ----- END Dump for squad #%u -------", *"*/ id /*"*);
}

// по-возрастанию
class CSortCoverPredicate {
public:
	bool	operator() (u32 node1, u32 node2)
	{
		return	(ai().level_graph().vertex_cover(node1) > ai().level_graph().vertex_cover(node2));
	};
};

void CMonsterSquad::TaskIdle()
{
	u32 member_num = members.size();
	u32 cur_index = 0;

	// Определить центр тяжести  
	Fvector centroid = leader->Position();			

	// вектор к цели
	Fvector dir;
	Fvector dest_pos = ai().level_graph().vertex_position(dest_node);

	dir = dest_pos.sub(leader->Position());
	if (fis_zero(dir.square_magnitude())) dir = leader->Direction();

	// выбрать ковер-ноды
	xr_vector<u32> nodes;
	float radius = 10.0f;

	ai().graph_engine().search( ai().level_graph(), leader->level_vertex_id(), leader->level_vertex_id(), &nodes, CGraphEngine::CFlooder(radius));
	
	// сортировать ноды
	sort(nodes.begin(), nodes.end(), CSortCoverPredicate());

	for (ENTITY_VEC_IT it = members.begin();it != members.end(); it++) {
		cur_index++;
		float part = (cur_index * 100.f) / member_num;

		GTask new_task;

		new_task.state.ttl		= Level().timeServer() + 3000;
		new_task.state.need_reset	= false;

		if (part <= 10) {						
			new_task.state.command	= SC_COVER;
			new_task.target.entity	= leader;
		} else if (part <= 50) {
			new_task.state.command	= SC_EXPLORE;
			Explore(nodes, centroid, dir, *it, &new_task);
		} else	{								
			new_task.state.command	= SC_FOLLOW;
			new_task.target.entity	= leader;
		}

		AskMember(*it,new_task);
	}
}

void CMonsterSquad::Explore(xr_vector<u32> &nodes, const Fvector &centroid, const Fvector &dir, CEntity *pE, GTask *pTask) 
{
	float	best_cost	= 0.f;
	u32		best_node	= nodes[0]; 
	u32		index		= 0;
		
	for (u32 i=0; i<nodes.size(); i++) {
		// cost = cover * distance_to_member * distance_to_centroid
		float cur_cost = ai().level_graph().vertex_cover(nodes[i]) * 
			ai().level_graph().vertex_position(nodes[i]).distance_to(pE->Position()) *
			ai().level_graph().vertex_position(nodes[i]).distance_to(centroid);

		if (cur_cost > best_cost) {
			best_cost	= cur_cost;
			best_node	= nodes[i];
			index		= i;
		}
	}
	
	nodes.erase(nodes.begin() + index);

	// prepare task
	pTask->target.pos		= ai().level_graph().vertex_position(best_node);
	pTask->target.node		= best_node;
	pTask->target.entity	= 0;
}

void CMonsterSquad::AskMember(CEntity *pE, const GTask &new_task) 
{
	GTask &cur_task= GetTask(pE);

	if ((cur_task.state.ttl > Level().timeServer()) && (cur_task.state.command == TS_REFUSED)) return;
	
	if ((ActiveTask(&cur_task) && IsPriorityHigher(new_task.state.command, cur_task.state.command)) || !ActiveTask(&cur_task)) {
		cur_task = new_task;
		cur_task.state.need_reset = true;
		cur_task.state.type = TS_REQUEST;
	}

}

bool CMonsterSquad::IsPriorityHigher(ESquadCommand com_new, ESquadCommand com_old)
{
	return (Level().SquadMan.TransformPriority(com_new) >  Level().SquadMan.TransformPriority(com_old));
}

//////////////////////////////////////////////////////////////////////////
// Decentralized Approach (just general data for monsters)
//////////////////////////////////////////////////////////////////////////
void CMonsterSquad::UpdateMonsterData(CEntity *pE, CEntity *pEnemy)
{
	SEntityState S;
	S.pEnemy = pEnemy;
	S.target_pos.set(0.f,0.f,0.f);
	
	ENTITY_STATE_MAP_IT it = states.find(pE);
	if (it == states.end()) {
		states.insert(mk_pair(pE, S));
	} else {
		it->second.pEnemy = pEnemy;
	}
}


Fvector CMonsterSquad::GetTargetPoint(CEntity *pE)
{
	ENTITY_STATE_MAP_IT it = states.find(pE);
	R_ASSERT(it != states.end());
	
	return it->second.target_pos;
}


void CMonsterSquad::UpdateDecentalized()
{
	
//	ENTITY_STATE_MAP	new_map;
//	new_map				= states;	
//	ENTITY_STATE_MAP	cur_map;
//	
//	while (!new_map.empty()) {
//		ENTITY_STATE_MAP_IT	it	= new_map.begin();
//		CEntity	*pGeneralEnemy	= it->second.pEnemy;
//
//		for (;it != new_map.end(); it++) {
//			if (it->second.pEnemy == pGeneralEnemy) cur_map.insert(*it);
//		}
//		
//		// work with cur_map
//		// ... 
//		
//		
//		//---------------------------------
//		// 
//		cur_map.clear();
//		
//		// remove from new_map all with pGeneralEnemy
//		
////		for (it = new_map.begin(); it != )
////		
////		it = std::remove_if(new_map.begin(), new_map.end(), predicate_remove_members(pGeneralEnemy));
////		new_map.erase(it, new_map.end());

	}


//	xr_vector<CEntity *> used;
//	xr_vector<CEntity *> current;
//	
//	ENTITY_STATE_MAP_IT	current_state_it = states.begin();
//
//	while (used.size() != states.size()) {
//		ENTITY_STATE_MAP_IT I,B,E;
//		B = states.begin();
//		E = states.end();
//		for (I = B; I!=E;I++) {
//			// проверить есть ли уже данный элемент в списке
//			xr_vector<CEntity *>::iterator it = used.find((*I)->first);
//			if (it != used.end()) 
//
//		}
//		
//		
//		current_state_it++;
//	}


