#include "stdafx.h"
#include "base_monster.h"
#include "../../../PhysicsShell.h"
#include "../../../CharacterPhysicsSupport.h"
#include "../../../game_level_cross_table.h"
#include "../../../game_graph.h"
#include "../../../phmovementcontrol.h"
#include "../ai_monster_squad_manager.h"
#include "../../../xrserver_objects_alife_monsters.h"
#include "../ai_monster_jump.h"
#include "../ai_monster_utils.h"
#include "../ai_monster_debug.h"
#include "../corpse_cover.h"
#include "../../../cover_evaluators.h"
#include "../../../seniority_hierarchy_holder.h"
#include "../../../team_hierarchy_holder.h"
#include "../../../squad_hierarchy_holder.h"
#include "../../../group_hierarchy_holder.h"
#include "../../../phdestroyable.h"

CBaseMonster::CBaseMonster()
{
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);

#ifdef DEBUG
	HDebug							= xr_new<CMonsterDebug>(this, Fvector().set(0.0f,2.0f,0.0f), 20.f);
#endif
	
	m_pPhysics_support				->in_Init();

	m_dwFrameLoad		= u32(-1);
	m_dwFrameReload		= u32(-1);
	m_dwFrameReinit		= u32(-1);
	m_dwFrameSpawn		= u32(-1);
	m_dwFrameDestroy	= u32(-1);
	m_dwFrameClient		= u32(-1);

	// Components external init 

	CMonsterMovement::InitExternal	(this);
	EnemyMemory.init_external		(this, 20000);
	SoundMemory.init_external		(this, 20000);
	CorpseMemory.init_external		(this, 20000);
	HitMemory.init_external			(this, 50000);

	EnemyMan.init_external			(this);
	CorpseMan.init_external			(this);

	// Инициализация параметров анимации	
	MotionMan.Init					(this);

	m_fGoingSpeed					= 0.f;

	// Attack-stops init
	AS_Init							();

	m_corpse_cover_evaluator		= xr_new<CMonsterCorpseCoverEvaluator>	(this);
	m_enemy_cover_evaluator			= xr_new<CCoverEvaluatorFarFromEnemy>	(this);
	m_cover_evaluator_close_point	= xr_new<CCoverEvaluatorCloseToEnemy>	(this);

	StateMan						= 0;

	CStepManager::init_external		(this);
}

CBaseMonster::~CBaseMonster()
{
	xr_delete(m_pPhysics_support);
#ifdef DEBUG
	xr_delete(HDebug);
#endif	

	xr_delete(m_corpse_cover_evaluator);
	xr_delete(m_enemy_cover_evaluator);
	xr_delete(m_cover_evaluator_close_point);
}


void CBaseMonster::UpdateCL()
{
	if (!frame_check(m_dwFrameClient))
		return;

	inherited::UpdateCL();
	
	
	if (g_Alive()) {
		// обновить анимации и установить параметры скорости
		MotionMan.FrameUpdate				();

		// Проверка состояния анимации (атака)
		AA_CheckHit							();

		// Поправка Pitch
		PitchCorrection						();

		// Обновить угловую и линейную скорости движения
		CMonsterMovement::update_velocity	();

		Exec_Look							(Device.fTimeDelta);
		
		CStepManager::update				();
	}

	m_pPhysics_support->in_UpdateCL();

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug))
		HDebug->M_Update	();
#endif

}

void CBaseMonster::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	
	m_pPhysics_support->in_shedule_Update(dt);
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////


void CBaseMonster::Die()
{
	inherited::Die( );

	CSoundPlayer::play(MonsterSpace::eMonsterSoundDie);
	MotionMan.ForceAnimSelect();

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);
}


void CBaseMonster::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CBaseMonster::PHHit(float P,Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*=ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,element,p_in_object_space,impulse,hit_type);
}
CBoneInstance *CBaseMonster::GetBoneInstance(LPCTSTR bone_name)
{
	int bone = smart_cast<CKinematics*>(Visual())->LL_BoneID(bone_name);
	return (&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(bone)));
}

CPHDestroyable*	CBaseMonster::	ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}
CBoneInstance *CBaseMonster::GetBoneInstance(int bone_id)
{
	return (&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(bone_id)));
}

CBoneInstance *CBaseMonster::GetEatBone()
{
	int bone = smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_ponytail2");
	return (&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(bone)));
}


void CBaseMonster::MoraleBroadcast(float fValue)
{
	CGroupHierarchyHolder &Group = Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group());
	for (u32 i=0; i<Group.members().size(); ++i) {
		CEntityAlive *pE = smart_cast<CEntityAlive *>(Group.members()[i]);
		if (!pE) continue;
		
		if (pE->g_Alive() && (pE->Position().distance_to(Position()) < get_sd()->m_fMoraleBroadcastDistance)) pE->ChangeEntityMorale(fValue);
	}
}

#ifdef DEBUG
void CBaseMonster::OnRender()
{
	inherited::OnRender();
	
	if (psAI_Flags.test(aiMonsterDebug)) {
		HDebug->L_Update();
		HDebug->HT_Update();
	}
}
#endif

// Получить расстояние от fire_bone до врага
// Выполнить RayQuery от fire_bone в enemy.center
float CBaseMonster::GetRealDistToEnemy(const CEntity *pE)
{
	Fvector enemy_center;
	pE->Center(enemy_center);
	
	Fmatrix global_transform;
	global_transform.set(XFORM());
	
	global_transform.mulB(smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_head")).mTransform);
	
	Fvector dir; 
	dir.sub(enemy_center, global_transform.c);
	Collide::ray_defs	r_query(global_transform.c, dir, 10.f, CDB::OPT_CULL | CDB::OPT_ONLYNEAREST, Collide::rqtObject);
	Collide::rq_results	r_res;

	float ret_val = -1.f;

	setEnabled(false);
	if (pE->CFORM()->_RayQuery(r_query, r_res)) 
		if (r_res.r_begin()->O) ret_val = r_res.r_begin()->range;
	setEnabled(true);		
			
	return ret_val;
}


bool CBaseMonster::useful(const CGameObject *object) const
{
	const CEntityAlive *pCorpse = smart_cast<const CEntityAlive *>(object); 
	if (!pCorpse) return false;
	
	if (!pCorpse->g_Alive()) return true;
	return false;
}

float CBaseMonster::evaluate(const CGameObject *object) const
{
	return (0.f);
}

float CBaseMonster::get_custom_pitch_speed(float def_speed)
{
	float cur_speed = angle_difference(m_body.current.pitch, m_body.target.pitch) * 4.0f;
	clamp(cur_speed, PI_DIV_6, 5 * PI_DIV_6);

	return cur_speed;
}

#define REAL_DIST_THROUGH_TRACE_THRESHOLD	6.0f

float CBaseMonster::GetEnemyDistances(float &min_dist, float &max_dist, const CEntity *enemy)
{
	// обновить минимальную и максимальную дистанции до врага
	min_dist = m_fCurMinAttackDist;
	max_dist = get_sd()->m_fMaxAttackDist - (get_sd()->m_fMinAttackDist - m_fCurMinAttackDist);

	const CEntity *t_enemy = (enemy) ? enemy : EnemyMan.get_enemy();
	// определить расстояние до противника
	float cur_dist = t_enemy->Position().distance_to(Position()); 
	if (cur_dist < REAL_DIST_THROUGH_TRACE_THRESHOLD) cur_dist = GetRealDistToEnemy(t_enemy); 

	return cur_dist;
}

//////////////////////////////////////////////////////////////////////////
// Function for foot processing
//////////////////////////////////////////////////////////////////////////
//Fvector	CBaseMonster::get_foot_position(ELegType leg_type)
//{
//	R_ASSERT2(m_FootBones[leg_type] != BI_NONE, "foot bone had not been set");
//	
//	CKinematics *pK = smart_cast<CKinematics*>(Visual());
//	Fmatrix bone_transform;
//
//	bone_transform = pK->LL_GetBoneInstance(m_FootBones[leg_type]).mTransform;	
//
//	Fmatrix global_transform;
//	global_transform.set(XFORM());
//	global_transform.mulB(bone_transform);
//
//	return global_transform.c;
//}
//
//void CBaseMonster::LoadFootBones()
//{
//
//	CInifile* ini		= smart_cast<CKinematics*>(Visual())->LL_UserData();
//	if(ini&&ini->section_exist("foot_bones")){
//		
//		CInifile::Sect& data		= ini->r_section("foot_bones");
//		for (CInifile::SectIt I=data.begin(); I!=data.end(); I++){
//			CInifile::Item& item	= *I;
//			
//			u16 index = smart_cast<CKinematics*>(Visual())->LL_BoneID(*item.second);
//			VERIFY3(index != BI_NONE, "foot bone not found", *item.second);
//			
//			if (xr_strcmp(*item.first, "front_left") == 0) 			m_FootBones[eFrontLeft]		= index;
//			else if (xr_strcmp(*item.first, "front_right")== 0)		m_FootBones[eFrontRight]	= index;
//			else if (xr_strcmp(*item.first, "back_right")== 0)		m_FootBones[eBackRight]		= index;
//			else if (xr_strcmp(*item.first, "back_left")== 0)		m_FootBones[eBackLeft]		= index;
//		}
//	} else VERIFY("section [foot_bones] not found in monster user_data");
//
//	// проверка на соответсвие
//	int count = 0;
//	for (u32 i = 0; i < eLegsMaxNumber; i++) 
//		if (m_FootBones[i] != BI_NONE) count++;
//
//	VERIFY(count == get_legs_number());
//}

float CBaseMonster::get_current_animation_time()
{
	return MotionMan.GetCurAnimTime();
}

void CBaseMonster::on_animation_start(shared_str anim)
{
	CStepManager::on_animation_start(anim);
}



//////////////////////////////////////////////////////////////////////////

void CBaseMonster::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;
		
	// remove from current team
	monster_squad().remove_member	((u8)g_Team(),(u8)g_Squad(), this);
	inherited::ChangeTeam			(team,squad,group);
	monster_squad().register_member	((u8)g_Team(),(u8)g_Squad(), this);
}


void CBaseMonster::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) return;

	EMotionAnim anim = MotionMan.GetCurAnim();

	bool turn_left = true;
	if (from_right(m_body.target.yaw, m_body.current.yaw)) turn_left = false; 

	switch (anim) {
		case eAnimStandIdle: 
			(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			return;
		case eAnimJumpLeft:
		case eAnimJumpRight:
			MotionMan.SetCurAnim(anim);
			return;
		default:
			if (delta_yaw > deg(30)) {
				(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			}
			return;
	}

}

bool CBaseMonster::IsVisibleObject(const CGameObject *object)
{
	return CMemoryManager::visible_now(object);
}


void CBaseMonster::Exec_Look		( float dt )
{
	m_body.current.yaw		= angle_normalize			(m_body.current.yaw);
	m_body.target.yaw		= angle_normalize			(m_body.target.yaw);
	m_body.current.pitch	= angle_normalize_signed	(m_body.current.pitch);
	m_body.target.pitch		= angle_normalize_signed	(m_body.target.pitch);

	float pitch_speed		= get_custom_pitch_speed(m_body.speed);
	angle_lerp_bounds		(m_body.current.yaw,m_body.target.yaw,m_body.speed,dt);
	angle_lerp_bounds		(m_body.current.pitch,m_body.target.pitch,pitch_speed,dt);

	Fvector P				= Position();
	XFORM().setHPB			(-m_body.current.yaw,-m_body.current.pitch,0);
	Position()				= P;

}

void CBaseMonster::PitchCorrection()
{
	bool b_need_pitch_correction = true;
	
	if (ability_can_jump()) {
		CJumping *pJumping = smart_cast<CJumping *>(this);
		if (pJumping && pJumping->IsActive()) b_need_pitch_correction = false;
	}
	if (b_need_pitch_correction) inherited::PitchCorrection();
}

//void CBaseMonster::State_PlaySound(u32 internal_type, u32 max_stop_time)
//{
//	if (m_bAngry && ((internal_type == eMonsterSoundIdle) ||  (internal_type == eMonsterSoundEat)))
//		CSoundPlayer::play(MonsterSpace::eMonsterSoundGrowling, 0, 0, get_sd()->m_dwAttackSndDelay);
//	else 
//		CSoundPlayer::play(internal_type, 0, 0, max_stop_time);
//}

void CBaseMonster::set_state_sound(u32 type, bool once)
{
	if (once) {
	
		CSoundPlayer::play(type);
	
	} else {

		u32 delay = 0;
		switch (type) {
		case MonsterSpace::eMonsterSoundIdle : 
		
			delay = get_sd()->m_dwIdleSndDelay;
			break;
		case MonsterSpace::eMonsterSoundEat:
			delay = get_sd()->m_dwEatSndDelay;
			break;
		case MonsterSpace::eMonsterSoundAttack:
		case MonsterSpace::eMonsterSoundPanic:
			delay = get_sd()->m_dwAttackSndDelay;
			break;
		}

		CSoundPlayer::play(type, 0, 0, delay);
	}
}



//void CBaseMonster::SetState(IState *pS, bool bSkipInertiaCheck)
//{
//	if (CurrentState != pS) {
//		// проверка инерций
//		if (!bSkipInertiaCheck)
//			if (CurrentState->IsInertia()) {
//				if (CurrentState->GetPriority() >= pS->GetPriority()) return;
//			}
//
//			CurrentState->Done();
//			CurrentState->Reset();
//			CurrentState = pS;
//			CurrentState->Activate();
//	}
//}

BOOL CBaseMonster::feel_touch_on_contact	(CObject *O)
{
	return		(inherited::feel_touch_on_contact(O));
}

bool CBaseMonster::can_eat_now()
{
	return (CorpseMan.get_corpse() && ((GetSatiety() < get_sd()->m_fMinSatiety) || flagEatNow));
}
