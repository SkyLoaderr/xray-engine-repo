#include "stdafx.h"
#include "../../ai_monster_state.h"
#include "burer_attack_tele.h"
#include "burer.h"
#include "../../../PhysicsShell.h"

#define GOOD_DISTANCE_FOR_TELE	15.f
#define TELE_DELAY				4000

#define MAX_TIME_CHECK_FAILURE	6000

CBurerAttackTele::CBurerAttackTele(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackTele::Init()
{
	inherited::Init();
	
	UpdateExternal();

	m_tAction		= ACTION_TELE_STARTED;
	selected_object	= 0;

	SelectObjects();

	time_started			= 0;
	time_enemy_last_faced	= 0;
}

//////////////////////////////////////////////////////////////////////////
// ������� ���� ����������
//////////////////////////////////////////////////////////////////////////

void CBurerAttackTele::Run()
{

	switch (m_tAction) {
		/************************/
		case ACTION_TELE_STARTED:
		/************************/
			
			ExecuteTeleStart();
			m_tAction = ACTION_TELE_CONTINUE;

			break;
		/************************/
		case ACTION_TELE_CONTINUE:
		/************************/

			ExecuteTeleContinue();

			break;

		/************************/
		case ACTION_TELE_FIRE:
		/************************/

			ExecuteTeleFire();
			m_tAction = ACTION_WAIT_TRIPLE_END;

			break;
		/***************************/
		case ACTION_WAIT_TRIPLE_END:
		/***************************/
			
			if (!pMonster->MotionMan.TA_IsActive()) {
				if (IsActiveObjects())
					m_tAction = ACTION_TELE_STARTED;
				else 
					m_tAction = ACTION_COMPLETED; 
			}

		/*********************/
		case ACTION_COMPLETED:
		/*********************/

			break;
	}

	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;

	DO_IN_TIME_INTERVAL_BEGIN(time_enemy_last_faced, 700);
		pMonster->FaceTarget(enemy);
	DO_IN_TIME_INTERVAL_END();

}

void CBurerAttackTele::Done()
{
	tele_objects.clear();
}

void CBurerAttackTele::CriticalInterrupt()
{
	pMonster->MotionMan.TA_Deactivate();
	pMonster->CTelekinesis::Deactivate();
	tele_objects.clear();
}

//////////////////////////////////////////////////////////////////////////
// ���������, ����� �� ���������� ���������
//////////////////////////////////////////////////////////////////////////
bool CBurerAttackTele::CheckTeleStart()
{
	// �������� �� ������� ���������� 
	if (IsActiveObjects()) return false;
	
	// ��������� ��������� �� �����
	float dist = pMonster->Position().distance_to(enemy->Position());
	if (dist < GOOD_DISTANCE_FOR_TELE) return false;

	// ����� ���������������� �������
	FindObjects();

	// ���� ��� ��������
	if (tele_objects.empty()) return false;

	// �� �� ����� �������� ���������
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////
// ���������� ��������� '���������� ����������' (�������� ��� ������ ���� ��� ���������)
////////////////////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::ExecuteTeleStart()
{
	pMonster->MotionMan.TA_Activate(&pMonster->anim_triple_tele);
	time_started = m_dwCurrentTime;
}

//////////////////////////////////////////////////////////////////////////
// ���������� ��������� '������� ��������'
//////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::ExecuteTeleContinue()
{
	if (time_started + pMonster->m_tele_time_to_hold > m_dwCurrentTime) return;
	
	// ����� ������ ��� �����
	bool object_found = false;
	CTelekineticObject tele_object;

	u32 i=0;
	while (i < pMonster->CTelekinesis::get_objects_count()) {
		 tele_object = pMonster->CTelekinesis::get_object_by_index(i);

		if ((tele_object.get_state() == TS_Keep) && (tele_object.time_keep_started + 1500 < m_dwCurrentTime)) {

			object_found = true;
			break;

		} else i++;

	}

	if (object_found) {
		m_tAction		= ACTION_TELE_FIRE;
		selected_object = tele_object.get_object();
	} else {
		if (!IsActiveObjects() || (time_started + MAX_TIME_CHECK_FAILURE < m_dwCurrentTime)) {
			pMonster->MotionMan.TA_Deactivate();
			m_tAction = ACTION_COMPLETED;
		} 
	}
}

//////////////////////////////////////////////////////////////////////////
// ���������� ��������� '����'
//////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::ExecuteTeleFire()
{
	pMonster->MotionMan.TA_PointBreak();

	Fvector enemy_pos = enemy->Position();
	enemy_pos.y += 5 * enemy->Radius();

	float dist = selected_object->Position().distance_to(enemy->Position());
	pMonster->CTelekinesis::fire(selected_object, enemy_pos, dist/12);

	pMonster->StopTeleObjectParticle(selected_object);
	
	pMonster->CSoundPlayer::play(eMonsterSoundTeleAttack);
}


//////////////////////////////////////////////////////////////////////////
// ��������, ���� �� ���� ���� ������ ��� ���������
//////////////////////////////////////////////////////////////////////////
bool CBurerAttackTele::IsActiveObjects()
{
	return (pMonster->CTelekinesis::get_objects_count() > 0);
}

//////////////////////////////////////////////////////////////////////////
// ����� �������� ��� ����������
//////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::FindObjects()
{
	tele_objects.clear();

	// �������� ������ �������� ������ �������
	Level().ObjectSpace.GetNearest	(pMonster->Position(), pMonster->m_tele_find_radius);
	xr_vector<CObject*> &tpObjects	= Level().ObjectSpace.q_nearest;

	for (u32 i=0;i<tpObjects.size();i++) {
		CGameObject *obj = dynamic_cast<CGameObject *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell || !obj->m_pPhysicsShell->bActive || (obj->m_pPhysicsShell->getMass() < pMonster->m_tele_object_min_mass) || (obj->m_pPhysicsShell->getMass() > pMonster->m_tele_object_max_mass) || (obj == pMonster) || pMonster->CTelekinesis::is_active_object(obj)) continue;

		tele_objects.push_back(obj);
	}

	
	// �������� ������ �������� ����� �������� � ������
	float dist = enemy->Position().distance_to(pMonster->Position());
	Fvector dir;
	dir.sub(enemy->Position(), pMonster->Position());
	dir.normalize();

	Fvector pos;
	pos.mad(pMonster->Position(), dir, dist / 2.f);
	Level().ObjectSpace.GetNearest(pos, pMonster->m_tele_find_radius); 
	tpObjects = Level().ObjectSpace.q_nearest;

	for (u32 i=0;i<tpObjects.size();i++) {
		CGameObject *obj = dynamic_cast<CGameObject *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell || !obj->m_pPhysicsShell->bActive || (obj->m_pPhysicsShell->getMass() < pMonster->m_tele_object_min_mass) || (obj->m_pPhysicsShell->getMass() > pMonster->m_tele_object_max_mass) || (obj == pMonster) || pMonster->CTelekinesis::is_active_object(obj)) continue;

		tele_objects.push_back(obj);
	}
}
	

//////////////////////////////////////////////////////////////////////////
// ����� ���������� �������� ��� ����������
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
	
void CBurerAttackTele::SelectObjects()
{
	sort(tele_objects.begin(),tele_objects.end(),best_object_predicate(pMonster->Position(), enemy->Position()));

	// ������� ������
	for (u32 i=0; i<tele_objects.size(); i++) {
		CGameObject *obj = tele_objects[i];

		// ��������� ��������� �� ������
		pMonster->CTelekinesis::activate(obj, 3.f, 2.f, 10000);
		pMonster->StartTeleObjectParticle(obj);

		// ������� �� ������
		tele_objects[i] = tele_objects[tele_objects.size()-1];
		tele_objects.pop_back();

		if (pMonster->CTelekinesis::get_objects_count() >= pMonster->m_tele_max_handled_objects) break;
	}
}

//////////////////////////////////////////////////////////////////////////
// �������� �� ���������� �������� ��������� ����������
//////////////////////////////////////////////////////////////////////////
bool CBurerAttackTele::IsCompleted()
{
	return (m_tAction == ACTION_COMPLETED);
}

bool CBurerAttackTele::CheckStartCondition()
{
	return CheckTeleStart();
}

void CBurerAttackTele::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}
