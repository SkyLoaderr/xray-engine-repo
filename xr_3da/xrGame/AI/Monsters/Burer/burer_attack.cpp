#include "stdafx.h"
#include "../../ai_monster_state.h"
#include "burer_attack.h"
#include "burer.h"
#include "../../../PhysicsShell.h"
#include "../../ai_monster_utils.h"

#define MAX_HANDLED_OBJECTS		3
#define CHECK_OBJECTS_RADIUS	10.f
#define MINIMAL_MASS			40.f
#define MAXIMAL_MASS			5000.f


#define MIN_DIST_FOR_GRAVI		3.f
#define MAX_DIST_FOR_GRAVI		20.f



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
	
	time_next_gravi_available	= 0;
	time_gravi_started			= 0;
	
	LastTimeRebuild				= 0;
	m_dwFaceEnemyLastTime		= 0;
	
	selected_random_point		= pMonster->Position();

	run_around_time_started		= 0;
}



void CBurerAttack::Run()
{
	// Если враг изменился, инициализировать состояние
	if (pMonster->EnemyMan.get_enemy() != enemy) Init();

	//-----------------------------------------------------------
	// Выбор действия
	
	// проверка на активацию телекинеза
	if (CheckTele())		m_tAction	= ACTION_TELE_STARTED;

	// проверка на активацию грави удара
	if (CheckGravi())		m_tAction	= ACTION_GRAVI_STARTED;
	
	
	// Если ничего не выбрали
	float m_fDistMin, m_fDistMax;
	float dist;
	dist = pMonster->GetEnemyDistances(m_fDistMin, m_fDistMax);

	if ((m_tAction == ACTION_DEFAULT) || (m_tAction == ACTION_MELEE) || (m_tAction == ACTION_RUN)) {
		// получить минимальную и максимальную дистанции до врага
		if ((m_tPrevAction == ACTION_MELEE) && (dist < m_fDistMax)) m_tAction = ACTION_MELEE;
		else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_MELEE);
	}


	//-----------------------------------------------------------
	// Выполнение действия
	
	bool bNeedRebuild = false;

	switch (m_tAction) {
		/*************************/		
		case ACTION_GRAVI_STARTED: 
		case ACTION_GRAVI_CONTINUE:
		case ACTION_GRAVI_FIRE:
		/*************************/		
			LOG_EX("gravi attack");
			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

			Execute_Gravi(); 
			
			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, 1200);
				pMonster->FaceTarget(enemy);
			DO_IN_TIME_INTERVAL_END();

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);

			break;

		/*************************/		
		case ACTION_TELE_STARTED: 
		case ACTION_TELE_CONTINUE:
		case ACTION_TELE_FIRE:
		/*************************/		
			LOG_EX("EXECUTE_TELE");	
			pMonster->MotionMan.m_tAction= ACT_STAND_IDLE;

			Execute_Tele();
			
			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, 500);
				pMonster->FaceTarget(enemy);
			DO_IN_TIME_INTERVAL_END();

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);

			break;
		/*******************/
		case ACTION_DEFAULT:
		/*******************/
			LOG_EX("ACTION_DEFAULT");
			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			
			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, 500);
				pMonster->FaceTarget(enemy);
			DO_IN_TIME_INTERVAL_END();

			break;
		
		/****************************/
		case ACTION_WAIT_TRIPLE_END:	// если сотояние ожидать окончания triple animation
		/****************************/
			LOG_EX("WAIT_TRIPLE_END");			

			if (!pMonster->MotionMan.TA_IsActive()) {
				m_tAction					= ACTION_RUN_AROUND;
				run_around_time_started		= m_dwCurrentTime;

				Fvector dir;

				// select point random_point
				if (dist > 20.f) {
					dir.sub(enemy->Position(),pMonster->Position());
					dir.normalize();
					selected_random_point.mad(pMonster->Position(),dir,10.f);
				} else if ((dist < 20.f) && (dist > 10.f)) {
					dir.sub(pMonster->Position(), enemy->Position());
					dir.normalize();
					selected_random_point.mad(pMonster->Position(),dir,10.f);
				} else 
					selected_random_point = random_position(pMonster->Position(), 10.f);

			}
			
			break;

		// *********************
		case ACTION_MELEE:		
		// *********************
			
			LOG_EX("ATTACK: ATTACK_MELEE");
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;

			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(m_dwFaceEnemyLastTime, 1200);
				pMonster->FaceTarget(enemy);
			DO_IN_TIME_INTERVAL_END();

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);

			break;

		// ************
		case ACTION_RUN:		 // бежать на врага
		// ************	
			LOG_EX("ATTACK: RUN");
			pMonster->MotionMan.m_tAction	= ACT_RUN;
			pMonster->CMonsterMovement::set_try_min_time(false);

			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);


			DO_IN_TIME_INTERVAL_BEGIN(LastTimeRebuild,100 + 50.f * dist);
				bNeedRebuild = true; 
			DO_IN_TIME_INTERVAL_END();
			if (IS_NEED_REBUILD()) bNeedRebuild = true;

			if (bNeedRebuild) pMonster->MoveToTarget(enemy);

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);

			break;
		/*********************/
		case ACTION_RUN_AROUND:
		/*********************/
			LOG_EX("ATTACK: RUN AROUND");			

			pMonster->MotionMan.m_tAction = ACT_RUN;			
			pMonster->MoveToTarget(selected_random_point);
			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->_sd->m_dwAttackSndDelay);
			
			dist = pMonster->Position().distance_to(selected_random_point);
			if ((run_around_time_started + 3000 < m_dwCurrentTime) || (dist < 2.f))
				m_tAction = ACTION_DEFAULT;

			break;
	}

	if (m_tAction != ACTION_RUN_AROUND) run_around_time_started = 0;


	m_tPrevAction = m_tAction;
}


//////////////////////////////////////////////////////////////////////////
// Дополнительные функции
//////////////////////////////////////////////////////////////////////////

class best_object_predicate {
	Fvector enemy_pos;
	Fvector monster_pos;
public:
	best_object_predicate(const Fvector &m_pos, const Fvector &pos) {
		monster_pos = m_pos;
		enemy_pos	= pos;
	}

	bool operator()	 (const CGameObject *tpObject1, const CGameObject *tpObject2) const
	{
		float dist1 = monster_pos.distance_to(tpObject1->Position());
		float dist2 = enemy_pos.distance_to(tpObject2->Position());
		float dist3 = enemy_pos.distance_to(monster_pos);

		return ((dist1 < dist3) && (dist2 > dist3));
	};
};


void CBurerAttack::find_tele_objects()
{
	pMonster->tele_objects.clear();
	
	// получить список объектов вокруг монстра
	Level().ObjectSpace.GetNearest	(pMonster->Position(), CHECK_OBJECTS_RADIUS);
	xr_vector<CObject*> &tpObjects	= Level().ObjectSpace.q_nearest;

	for (u32 i=0;i<tpObjects.size();i++) {
		CGameObject *obj = dynamic_cast<CGameObject *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell || (obj->m_pPhysicsShell->getMass() < MINIMAL_MASS) || (obj->m_pPhysicsShell->getMass() > MAXIMAL_MASS) || (obj == pMonster) || pMonster->CTelekinesis::is_active_object(obj)) continue;

		pMonster->tele_objects.push_back(obj);
	}

	float dist = enemy->Position().distance_to(pMonster->Position());
	Fvector dir;
	dir.sub(enemy->Position(), pMonster->Position());
	dir.normalize();
	
	Fvector pos;
	pos.mad(pMonster->Position(), dir, dist / 2.f);
	Level().ObjectSpace.GetNearest(pos, CHECK_OBJECTS_RADIUS); 
	tpObjects = Level().ObjectSpace.q_nearest;
	
	for (u32 i=0;i<tpObjects.size();i++) {
		CGameObject *obj = dynamic_cast<CGameObject *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell || (obj->m_pPhysicsShell->getMass() < MINIMAL_MASS) || (obj->m_pPhysicsShell->getMass() > MAXIMAL_MASS) || (obj == pMonster) || pMonster->CTelekinesis::is_active_object(obj)) continue;

		pMonster->tele_objects.push_back(obj);
	}
			
	
	sort(pMonster->tele_objects.begin(),pMonster->tele_objects.end(),best_object_predicate(pMonster->Position(), enemy->Position()));
	
	// выбрать объект
	for (u32 i=0; i<pMonster->tele_objects.size(); i++) {
		CGameObject *obj = pMonster->tele_objects[i];
		
		// применить телекинез на объект
		pMonster->CTelekinesis::activate(obj, 3.f, 2.f, 10000);
		pMonster->StartTeleObjectParticle(obj);

		// удалить из списка
		pMonster->tele_objects[i] = pMonster->tele_objects[pMonster->tele_objects.size()-1];
		pMonster->tele_objects.pop_back();

		if (pMonster->CTelekinesis::get_objects_count() >= MAX_HANDLED_OBJECTS) break;
	}
}


void CBurerAttack::Done()
{
	inherited::Done();
	
	pMonster->StopGraviPrepare();
	pMonster->MotionMan.TA_Deactivate();
}
