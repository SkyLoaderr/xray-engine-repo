#include "stdafx.h"
#include "monster_enemy_manager.h"
#include "BaseMonster/base_monster.h"
#include "../ai_monsters_misc.h"

CMonsterEnemyManager::CMonsterEnemyManager()
{
	monster		= 0;
	enemy		= 0;
	flags.zero	();
	forced		= false;
	prev_enemy	= 0;
	danger_type = eNone;
}

CMonsterEnemyManager::~CMonsterEnemyManager()
{

}
void CMonsterEnemyManager::init_external(CBaseMonster *M)
{
	monster = M;
}


void CMonsterEnemyManager::update()
{
	if (forced) {
		// ��������� ���������� force-�������
		if (!enemy || enemy->getDestroy() || !enemy->g_Alive()) {
			enemy = 0;
			return;
		}
	} else {
		enemy = monster->EnemyMemory.get_enemy();
		
		if (enemy) {
			SMonsterEnemy enemy_info	= monster->EnemyMemory.get_enemy_info();
			position					= enemy_info.position;
			vertex						= enemy_info.vertex;
			time_last_seen				= enemy_info.time;
		}
	}
	
	if (!enemy) {
		return;
	}
	
	// ��������� ���������
	enemy_see_me = is_faced(enemy, monster);
	
	// �������� ��������� �����
	danger_type = eNone;

	switch (dwfChooseAction(0, 0.8f, 0.6f, 0.4f, 0.2f,monster->g_Team(),monster->g_Squad(),monster->g_Group(),0,1,2,3,4, monster, 30.f)) {
		case 4 : 	danger_type = eVeryStrong;	break;
		case 3 : 	danger_type = eStrong;		break;
		case 2 : 	danger_type = eNormal;		break;
		case 1 : 
		case 0 : 	danger_type = eWeak;		break;
	}

	// �������� �����
	flags.zero();

	if ((prev_enemy == enemy) && (time_last_seen != Level().timeServer()))	flags.or(FLAG_ENEMY_LOST_SIGHT);		
	if (prev_enemy && !prev_enemy->g_Alive())								flags.or(FLAG_ENEMY_DIE);
	if (!enemy_see_me)														flags.or(FLAG_ENEMY_DOESNT_SEE_ME);
	
	float dist_now, dist_prev;
	if (prev_enemy == enemy) {
		dist_now	= position.distance_to(monster->Position());
		dist_prev	= prev_enemy_position.distance_to(monster->Position());

		if (_abs(dist_now - dist_prev) < 0.2f)								flags.or(FLAG_ENEMY_STANDING);
		else {
			if (dist_now < dist_prev)										flags.or(FLAG_ENEMY_GO_CLOSER);
			else															flags.or(FLAG_ENEMY_GO_FARTHER);

			if (_abs(dist_now - dist_prev) < 1.2f) {
				if (dist_now < dist_prev)									flags.or(FLAG_ENEMY_GO_CLOSER_FAST);
				else														flags.or(FLAG_ENEMY_GO_FARTHER_FAST);
			}
		}

		if (flags.is(FLAG_ENEMY_STANDING) && flags.is(FLAG_ENEMY_DOESNT_SEE_ME)) flags.or(FLAG_ENEMY_DOESNT_KNOW_ABOUT_ME);
	}

	// ��������� �������� �����
	prev_enemy			= enemy;
	prev_enemy_position = position;

	expediency			= true;
}



void CMonsterEnemyManager::force_enemy (const CEntityAlive *enemy)
{
	this->enemy		= enemy;
	position		= enemy->Position();
	vertex			= enemy->level_vertex_id();
	time_last_seen	= Level().timeServer();

	forced			= true;
}

void CMonsterEnemyManager::unforce_enemy()
{
	enemy = monster->EnemyMemory.get_enemy();

	if (enemy) {
		SMonsterEnemy enemy_info	= monster->EnemyMemory.get_enemy_info();
		position					= enemy_info.position;
		vertex						= enemy_info.vertex;
		time_last_seen				= enemy_info.time;
	}

	forced = false;
}


u32	CMonsterEnemyManager::get_enemies_count()
{
	return monster->EnemyMemory.get_enemies_count();
}

void CMonsterEnemyManager::reinit()
{
	enemy			= 0;
	time_last_seen	= 0;
	flags.zero		();
	forced			= false;
	prev_enemy		= 0;
	danger_type		= eNone;
}


void CMonsterEnemyManager::add_enemy(const CEntityAlive *enemy)
{
	monster->EnemyMemory.add_enemy(enemy);
}


bool CMonsterEnemyManager::see_enemy_now()
{
	//return (monster->memory().visual().visible_now(enemy));

	if (time_last_seen == monster->m_current_update) return true;
	return false;
}

bool CMonsterEnemyManager::is_faced(const CEntityAlive *object0, const CEntityAlive *object1)
{
	if (object0->Position().distance_to(object1->Position()) > object0->ffGetRange())
		return		(false);

	float			yaw1, pitch1, yaw2, pitch2, fYawFov, fPitchFov, fRange;
	Fvector			tPosition = object0->Position();

	yaw1			= object0->Orientation().yaw;
	pitch1			= object0->Orientation().pitch;
	fYawFov			= angle_normalize_signed(object0->ffGetFov()*PI/180.f);
	fRange			= object0->ffGetRange();

	fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(1.f/tPosition.distance_to(object1->Position()))))/2.f);
	fPitchFov		= angle_normalize_signed(fYawFov*1.f);
	tPosition.sub	(object1->Position());
	tPosition.mul	(-1);
	tPosition.getHP	(yaw2,pitch2);
	yaw1			= angle_normalize_signed(yaw1);
	pitch1			= angle_normalize_signed(pitch1);
	yaw2			= angle_normalize_signed(yaw2);
	pitch2			= angle_normalize_signed(pitch2);
	if ((angle_difference(yaw1,yaw2) <= fYawFov) && (angle_difference(pitch1,pitch2) <= fPitchFov))
		return		(true);
	return			(false);
}

bool CMonsterEnemyManager::is_enemy(const CEntityAlive *obj) 
{
	return ((monster->g_Team() != obj->g_Team()) && ((monster->tfGetRelationType(obj) == ALife::eRelationTypeEnemy) || (monster->tfGetRelationType(obj) == ALife::eRelationTypeWorstEnemy)) && obj->g_Alive());
}




