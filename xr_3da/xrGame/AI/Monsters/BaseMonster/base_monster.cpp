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
#include "../../../../skeletoncustom.h"
#include "../critical_action_info.h"
#include "../../../detail_path_manager.h"
#include "../../../hudmanager.h"
#include "../../../memory_manager.h"
#include "../../../visual_memory_manager.h"
#include "../../../ai_debug.h"
#include "../ai_monster_movement.h"
#include "../../../entitycondition.h"
#include "../../../sound_player.h"

CBaseMonster::CBaseMonster()
{
	m_movement_manager				= 0;
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);

#ifdef DEBUG
	HDebug							= xr_new<CMonsterDebug>(this, Fvector().set(0.0f,2.0f,0.0f), 20.f);
#endif
	
	m_pPhysics_support				->in_Init();

	// Components external init 

	EnemyMemory.init_external		(this, 20000);
	SoundMemory.init_external		(this, 20000);
	CorpseMemory.init_external		(this, 20000);
	HitMemory.init_external			(this, 50000);

	EnemyMan.init_external			(this);
	CorpseMan.init_external			(this);

	// »нициализаци€ параметров анимации	
	MotionMan.Init					(this);

	StateMan						= 0;

	CriticalActionInfo				= xr_new<CCriticalActionInfo>();

	DirMan.init_external			(this);
	MeleeChecker.init_external		(this);
	Morale.init_external			(this);

	m_jumping						= 0;
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
	
	xr_delete(CriticalActionInfo);
}


void CBaseMonster::UpdateCL()
{
	inherited::UpdateCL();
	
	
	if (g_Alive()) {
		// обновить анимации и установить параметры скорости
		MotionMan.FrameUpdate				();

		// ѕроверка состо€ни€ анимации (атака)
		AA_CheckHit							();

		// ѕоправка Pitch
		PitchCorrection						();

		// ќбновить угловую и линейную скорости движени€
		movement().update_velocity	();

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
	if (!m_first_update_initialized) {
		on_first_update();
		m_first_update_initialized	= true;
	}

	inherited::shedule_Update	(dt);
	Morale.update_schedule		(dt);
	
	m_pPhysics_support->in_shedule_Update(dt);
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////


void CBaseMonster::Die(CObject* who)
{
	inherited::Die(who);

	if (is_special_killer(who))
		sound().play			(MonsterSpace::eMonsterSoundDieInAnomaly);
	else
		sound().play			(MonsterSpace::eMonsterSoundDie);

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);
}


void CBaseMonster::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CBaseMonster::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*=ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
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
	float cur_speed = angle_difference(movement().m_body.current.pitch, movement().m_body.target.pitch) * 4.0f;
	clamp(cur_speed, PI_DIV_6, 5 * PI_DIV_6);

	return cur_speed;
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


void CBaseMonster::SetTurnAnimation(bool turn_left)
{
	(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
}

bool CBaseMonster::IsVisibleObject(const CGameObject *object)
{
	return memory().visual().visible_now(object);
}


void CBaseMonster::Exec_Look		( float dt )
{
	movement().m_body.current.yaw		= angle_normalize			(movement().m_body.current.yaw);
	movement().m_body.target.yaw		= angle_normalize			(movement().m_body.target.yaw);
	movement().m_body.current.pitch	= angle_normalize_signed	(movement().m_body.current.pitch);
	movement().m_body.target.pitch		= angle_normalize_signed	(movement().m_body.target.pitch);

	float pitch_speed		= get_custom_pitch_speed(movement().m_body.speed);
	angle_lerp_bounds		(movement().m_body.current.yaw,movement().m_body.target.yaw,movement().m_body.speed,dt);
	angle_lerp_bounds		(movement().m_body.current.pitch,movement().m_body.target.pitch,pitch_speed,dt);

	Fvector P				= Position();
	XFORM().setHPB			(-movement().m_body.current.yaw,-movement().m_body.current.pitch,0);
	Position()				= P;

}

void CBaseMonster::PitchCorrection()
{
	bool b_need_pitch_correction = true;
	
	if (ability_can_jump()) {
		if (m_jumping && m_jumping->IsActive()) b_need_pitch_correction = false;
	}
	if (b_need_pitch_correction) inherited::PitchCorrection();
}

void CBaseMonster::set_state_sound(u32 type, bool once)
{
	if (once) {
	
		sound().play(type);
	
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

		sound().play(type, 0, 0, delay);
	}
}

BOOL CBaseMonster::feel_touch_on_contact	(CObject *O)
{
	return		(inherited::feel_touch_on_contact(O));
}

bool CBaseMonster::can_eat_now()
{
	return (CorpseMan.get_corpse() && ((conditions().GetSatiety() < get_sd()->m_fMinSatiety) || flagEatNow));
}

void CBaseMonster::TranslateActionToPathParams()
{
	if (!movement().b_enable_movement) return;

	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	switch (MotionMan.m_tAction) {
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
	case ACT_LOOK_AROUND:
	case ACT_ATTACK:
	case ACT_TURN:
		bEnablePath = false;
		break;

	case ACT_WALK_FWD:
		if (m_bDamaged) {
			vel_mask = eVelocityParamsWalkDamaged;
			des_mask = eVelocityParameterWalkDamaged;
		} else {
			vel_mask = eVelocityParamsWalk;
			des_mask = eVelocityParameterWalkNormal;
		}
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		if (m_bDamaged) {
			vel_mask = eVelocityParamsRunDamaged;
			des_mask = eVelocityParameterRunDamaged;
		} else {
			vel_mask = eVelocityParamsRun;
			des_mask = eVelocityParameterRunNormal;
		}
		break;
	case ACT_DRAG:
		vel_mask = eVelocityParameterDrag;
		des_mask = eVelocityParameterDrag;

		MotionMan.SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = eVelocityParameterSteal;
		des_mask = eVelocityParameterSteal;
		break;
	case ACT_JUMP:
		break;
	}

	if (state_invisible) {
		vel_mask = eVelocityParamsInvisible;
		des_mask = eVelocityParameterInvisible;
	}

	if (force_real_speed) vel_mask = des_mask;

	if (bEnablePath) {
		movement().detail().set_velocity_mask	(vel_mask);	
		movement().detail().set_desirable_mask	(des_mask);
		movement().enable_path	();		
	} else {
		movement().disable_path	();
	}
}

u32 CBaseMonster::get_attack_rebuild_time()
{
	float dist = EnemyMan.get_enemy()->Position().distance_to(Position());
	return (100 + u32(50.f * dist));
}

void CBaseMonster::on_kill_enemy(const CEntity *obj)
{
	const CEntityAlive *entity	= smart_cast<const CEntityAlive *>(obj);
	
	// добавить в список трупов	
	CorpseMemory.add_corpse		(entity);
	
	// удалить всю информацию о хитах
	HitMemory.remove_hit_info	(entity);
}


void CBaseMonster::on_first_update()
{
	// HUD уже загружен, подгрузить дополнительные данные
	HUD().GetUI()->UIMainIngameWnd.AddMonsterClawsEffect ("monster", "controller\\controller_blood_01");
}

CMovementManager *CBaseMonster::create_movement_manager	()
{
	return		(m_movement_manager = xr_new<CMonsterMovement>(this));
}

DLL_Pure *CBaseMonster::_construct	()
{
	CCustomMonster::_construct	();
	CStepManager::_construct	();
	return						(this);
}
