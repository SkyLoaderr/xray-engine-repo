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
#include "../ai_monster_movement.h"
#include "../ai_monster_movement_space.h"
#include "../../../entitycondition.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../ui/UIMainIngameWnd.h"
#include "../state_manager.h"
#include "../controlled_entity.h"

CBaseMonster::CBaseMonster()
{
	m_movement_manager				= 0;

	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);
	
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
	m_controlled					= 0;
}


CBaseMonster::~CBaseMonster()
{
	xr_delete(m_pPhysics_support);
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

		// ќбновить линейную скорости движени€
		movement().update_velocity			();

		// ќбновить направление объекта, его угловые скорости движени€
		DirMan.update_frame					();

		CStepManager::update				();
	}

	m_pPhysics_support->in_UpdateCL();
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

#ifdef DEBUG	
	show_debug_info();
#endif
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////


void CBaseMonster::Die(CObject* who)
{
	inherited::Die(who);

	if (StateMan) StateMan->critical_finalize();
	
	if (is_special_killer(who))
		sound().play			(MonsterSpace::eMonsterSoundDieInAnomaly);
	else
		sound().play			(MonsterSpace::eMonsterSoundDie);

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);
	
	if (m_controlled)			m_controlled->on_die();
}


void CBaseMonster::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CBaseMonster::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*=ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

CPHDestroyable*	CBaseMonster::	ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

bool CBaseMonster::useful(const CItemManager *manager, const CGameObject *object) const
{
	const CEntityAlive *pCorpse = smart_cast<const CEntityAlive *>(object); 
	if (!pCorpse) return false;
	
	if (!pCorpse->g_Alive()) return true;
	return false;
}

float CBaseMonster::evaluate(const CItemManager *manager, const CGameObject *object) const
{
	return (0.f);
}

//////////////////////////////////////////////////////////////////////////

void CBaseMonster::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;
	VERIFY2(g_Alive(), "Try to change team of a dead object");

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

//void CBaseMonster::PitchCorrection()
//{
//	bool b_need_pitch_correction = true;
//	
//	if (ability_can_jump()) {
//		if (m_jumping && m_jumping->IsActive()) b_need_pitch_correction = false;
//	}
//	if (b_need_pitch_correction) inherited::PitchCorrection();
//}

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
		bEnablePath = false;
		break;

	case ACT_WALK_FWD:
		if (m_bDamaged) {
			vel_mask = MonsterMovement::eVelocityParamsWalkDamaged;
			des_mask = MonsterMovement::eVelocityParameterWalkDamaged;
		} else {
			vel_mask = MonsterMovement::eVelocityParamsWalk;
			des_mask = MonsterMovement::eVelocityParameterWalkNormal;
		}
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		if (m_bDamaged) {
			vel_mask = MonsterMovement::eVelocityParamsRunDamaged;
			des_mask = MonsterMovement::eVelocityParameterRunDamaged;
		} else {
			vel_mask = MonsterMovement::eVelocityParamsRun;
			des_mask = MonsterMovement::eVelocityParameterRunNormal;
		}
		break;
	case ACT_DRAG:
		vel_mask = MonsterMovement::eVelocityParamsDrag;
		des_mask = MonsterMovement::eVelocityParameterDrag;

		MotionMan.SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = MonsterMovement::eVelocityParamsSteal;
		des_mask = MonsterMovement::eVelocityParameterSteal;
		break;
	}

	if (state_invisible) {
		vel_mask = MonsterMovement::eVelocityParamsInvisible;
		des_mask = MonsterMovement::eVelocityParameterInvisible;
	}

	if (m_force_real_speed) vel_mask = des_mask;

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
	HUD().GetUI()->UIMainIngameWnd->AddMonsterClawsEffect ("monster", "controller\\controller_blood_01");
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

