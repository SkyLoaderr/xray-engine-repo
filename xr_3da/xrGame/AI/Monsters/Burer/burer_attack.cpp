#include "stdafx.h"
#include "../../ai_monster_state.h"
#include "burer_attack.h"
#include "burer.h"
#include "../../../PhysicsShell.h"


CBurerAttack::CBurerAttack(CBurer *p)  
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}


void CBurerAttack::Init()
{
	LOG_EX("attack init");
	IState::Init();

	// Получить врага
	enemy		= pMonster->EnemyMan.get_enemy();
	
	m_tAction	= ACTION_DEFAULT;
}

#define GOOD_DISTANCE			10.f
#define MAX_HANDLED_OBJECTS		3
#define CHECK_OBJECTS_RADIUS	10.f
#define MINIMAL_MASS			20.f
#define MAXIMAL_MASS			5000.f

void CBurerAttack::Run()
{
	// Если враг изменился, инициализировать состояние
	if (pMonster->EnemyMan.get_enemy() != enemy) Init();
	
	update();
	
	m_tAction = ACTION_DEFAULT;
	if (pMonster->CTelekinesis::get_objects_count() > 0)  m_tAction = ACTION_TELE;
	
	if (CheckGraviStart())	m_tAction = ACTION_GRAVI_START;
	if ((m_tAction == ACTION_DEFAULT) && (m_tPrevAction == ACTION_GRAVI_CONTINUE) && CheckGraviContinue())		m_tAction = ACTION_GRAVI_CONTINUE;
	//if ()

		
	if (m_tAction == ACTION_DEFAULT) m_tAction = ACTION_MELEE;

	switch (m_tAction) {
		case ACTION_TELE:				Execute_Telekinetic();		break;
		case ACTION_GRAVI_START:		Execute_Gravi_Start();		break;
		case ACTION_GRAVI_CONTINUE:		Execute_Gravi_Continue();	break;
		case ACTION_MELEE:				Execute_Melee();			break;
	}

	m_tPrevAction = m_tAction;
}


void CBurerAttack::Execute_Telekinetic()
{
	LOG_EX("telekinetic attack");
	pMonster->MotionMan.m_tAction= ACT_STAND_IDLE;

	float dist = enemy->Position().distance_to(pMonster->Position());

	u32 i=0;
	while (i < pMonster->CTelekinesis::get_objects_count()) {
		CTelekineticObject tele_object = pMonster->CTelekinesis::get_object_by_index(i);
		if ((tele_object.get_state() == TS_Keep) && (tele_object.time_keep_started + 2000 < m_dwCurrentTime)) {
			Fvector enemy_pos = enemy->Position();
			enemy_pos.y += enemy->Radius() * dist;
			pMonster->CTelekinesis::fire(tele_object.get_object(), enemy_pos);
		} else i++;
	}
}

void CBurerAttack::Execute_Melee()
{
	LOG_EX("melee attack");
	pMonster->MotionMan.m_tAction= ACT_ATTACK;
}

void CBurerAttack::Execute_Gravi_Start()
{
	LOG_EX("gravi attack");
	pMonster->MotionMan.m_tAction= ACT_WALK_FWD;
}	


u32	CBurerAttack::get_number_available_objects(xr_vector<CObject*> &tpObjects)
{
	u32 ret_val = 0;

	for (u32 i=0;i<tpObjects.size();i++) {
		CGameObject *obj = dynamic_cast<CGameObject *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell || (obj->m_pPhysicsShell->getMass() < MINIMAL_MASS) || (obj->m_pPhysicsShell->getMass() > MAXIMAL_MASS) || (obj == pMonster)) continue;
		ret_val++;
	}

	return ret_val;
}

void CBurerAttack::update()
{
	float dist = pMonster->Position().distance_to(enemy->Position());
	
	// обработать объекты
	if (dist > GOOD_DISTANCE) {
		if (pMonster->CTelekinesis::get_objects_count() < MAX_HANDLED_OBJECTS) {

			// получить список объектов вокруг монстра
			Level().ObjectSpace.GetNearest	(pMonster->Position(), CHECK_OBJECTS_RADIUS);
			xr_vector<CObject*> &tpNearest	= Level().ObjectSpace.q_nearest;

			bool b_objects_found = true;

			if (get_number_available_objects(tpNearest) == 0) {
				// получить список объектов на середине пути от монстра до врага
				Fvector dir;
				dir.sub(enemy->Position(), pMonster->Position());
				dir.normalize();

				Fvector pos;
				pos.mad(pMonster->Position(), dir, dist / 2.f);
				Level().ObjectSpace.GetNearest(pos, CHECK_OBJECTS_RADIUS); 
				tpNearest	= Level().ObjectSpace.q_nearest; 

				if (get_number_available_objects(tpNearest) == 0) {
					b_objects_found = false;
				}
			} 

			if (b_objects_found) {
				// выбрать объект
				for (u32 i=0;i<tpNearest.size();i++) {
					CGameObject *obj = dynamic_cast<CGameObject *>(tpNearest[i]);
					// проверка по объекту и его массе
					if (!obj || !obj->m_pPhysicsShell || (obj->m_pPhysicsShell->getMass() < MINIMAL_MASS) || (obj->m_pPhysicsShell->getMass() > MAXIMAL_MASS) || (obj == pMonster)) continue;

					// проверить, активен ли уже объект
					if (pMonster->CTelekinesis::is_active_object(obj)) continue;

					// применить телекинез на объект
					pMonster->CTelekinesis::activate(obj, 3.f, 2.f, 10000);
					break;
				}
			}
		}
	}
}


void CBurerAttack::Execute_Gravi_Continue()
{
	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;	
}


bool CBurerAttack::CheckGraviStart()
{
	if ((m_tAction != ACTION_DEFAULT) || 
		(m_tPrevAction == ACTION_GRAVI_START) || 
		(m_tPrevAction != ACTION_GRAVI_CONTINUE) || 
		(m_tPrevAction != ACTION_GRAVI_FIRE)) return false;

	return true;
}



bool CBurerAttack::CheckGraviContinue() 
{
	return false;
}