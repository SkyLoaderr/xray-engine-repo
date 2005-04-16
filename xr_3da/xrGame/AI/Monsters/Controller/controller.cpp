#include "stdafx.h"
#include "controller.h"
#include "../ai_monster_utils.h"
#include "controller_state_manager.h"
#include "../controlled_entity.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../ai_monster_effector.h"
#include "../../../hudmanager.h"
#include "../../../ui.h"
#include "../../../../skeletonanimated.h"
#include "../../../level.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"
#include "../../../ui/UIMainIngameWnd.h"
#include "../ai_monster_movement.h"
#include "../ai_monster_movement_space.h"
#include "../../../level_debug.h"


CController::CController()
{
	StateMan = xr_new<CStateManagerController>(this);

	CJumping::Init(this);
	CPsyAuraController::init_external(this);
	
	time_control_hit_started = 0;
}
CController::~CController()
{
	xr_delete(StateMan);
}

void CController::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);

	// Load Control FX texture
	m_UIControlFX.Init(pSettings->r_string(section, "control_fx_texture"), "hud\\default",0,0,0);
	m_UIControlFX2.Init(pSettings->r_string(section, "control_fx_texture2"), "hud\\default",0,0,0);

	m_max_controlled_number			= pSettings->r_u8(section,"Max_Controlled_Count");
	m_controlled_objects.reserve	(m_max_controlled_number);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	::Sound->create(control_start_sound,TRUE, pSettings->r_string(section,"sound_control_start"),	SOUND_TYPE_WORLD);
	::Sound->create(control_hit_sound,	TRUE, pSettings->r_string(section,"sound_control_hit"),		SOUND_TYPE_WORLD);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimStandIdle,	eAnimStandDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	// Load control postprocess --------------------------------------------------------
	LPCSTR ppi_section = pSettings->r_string(section, "control_effector");
	m_control_effector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
	m_control_effector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
	m_control_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	m_control_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	m_control_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	m_control_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	m_control_effector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(m_control_effector.ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_control_effector.ppi.color_base.r,	&m_control_effector.ppi.color_base.g,	&m_control_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_control_effector.ppi.color_gray.r,	&m_control_effector.ppi.color_gray.g,	&m_control_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_control_effector.ppi.color_add.r,	&m_control_effector.ppi.color_add.g,	&m_control_effector.ppi.color_add.b);

	m_control_effector.time			= pSettings->r_float(ppi_section,"time");
	m_control_effector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_control_effector.time_release	= pSettings->r_float(ppi_section,"time_release");

	m_control_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	m_control_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	m_control_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	m_control_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	if (MotionMan.start_load_shared(CLS_ID)) {

		SVelocityParam &velocity_none		= movement().get_velocity(MonsterMovement::eVelocityParameterIdle);	
		SVelocityParam &velocity_turn		= movement().get_velocity(MonsterMovement::eVelocityParameterStand);
		SVelocityParam &velocity_walk		= movement().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
		SVelocityParam &velocity_run		= movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
		SVelocityParam &velocity_walk_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
		SVelocityParam &velocity_run_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
		SVelocityParam &velocity_steal		= movement().get_velocity(MonsterMovement::eVelocityParameterSteal);
		//SVelocityParam &velocity_drag		= movement().get_velocity(MonsterMovement::eVelocityParameterDrag);


		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &velocity_none,		PS_SIT);
		MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &velocity_none,		PS_SIT);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &velocity_walk_dmg,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&velocity_run,		PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &velocity_run_dmg,	PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND);
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,		PS_STAND);	
		MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,		PS_SIT);
		MotionMan.AddAnim(eAnimSleep,			"sit_sleep_",			-1, &velocity_none,		PS_SIT);

		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

		MotionMan.AddTransition(PS_STAND,	PS_SIT,		eAnimStandSitDown,	false);
		MotionMan.AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,	false);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}

BOOL CController::net_Spawn(CSE_Abstract *DC)
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	if (!g_Alive()) {
		CPsyAuraController::deactivate();
		CPsyAuraController::set_auto_activate(false);
	}
}


void CController::UpdateControlled()
{
	// если есть враг, проверить может ли быть враг взят под контроль
	if (EnemyMan.get_enemy()) {
		CControlledEntityBase *entity = smart_cast<CControlledEntityBase *>(const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
		if (entity) {
			if (!entity->is_under_control() && (m_controlled_objects.size() < m_max_controlled_number)) {
				// взять под контроль
				entity->set_under_control	(this);
				entity->set_task_follow		(this);
				m_controlled_objects.push_back(const_cast<CEntityAlive *>(EnemyMan.get_enemy()));
			}
		}
	}
}

void CController::set_controlled_task(u32 task)
{
	if (!HasUnderControl()) return;
	
	const CEntity *object = ((((ETask)task) == eTaskNone) ? 0 : ((((ETask)task) == eTaskFollow) ? this : EnemyMan.get_enemy()));
	
	for	(u32 i=0; i<m_controlled_objects.size(); i++) {
		CControlledEntityBase *entity = smart_cast<CControlledEntityBase *>(m_controlled_objects[i]);		
		entity->get_data().m_object = object;
		entity->get_data().m_task	= (ETask)task;
	}
}

void CController::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

} 

void CController::InitThink()
{
	for	(u32 i=0; i<m_controlled_objects.size(); i++) {	
		CBaseMonster *base = smart_cast<CBaseMonster*>(m_controlled_objects[i]);
		if (!base) continue;
		if (base->EnemyMan.get_enemy()) 
			EnemyMemory.add_enemy  (base->EnemyMan.get_enemy(), 
									base->EnemyMan.get_enemy_position(),
									base->EnemyMan.get_enemy_vertex(),
									base->EnemyMan.get_enemy_time_last_seen()
			);
	}
}

void CController::play_control_sound_start()
{
	CActor *pA = smart_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;
	
	Fvector pos = pA->Position();
	pos.y += 1.5f;

	if (control_start_sound.feedback) control_start_sound.stop();
	control_start_sound.play_at_pos(pA,pos);
}

void CController::play_control_sound_hit()
{
	CActor *pA = smart_cast<CActor*>(Level().CurrentEntity());
	if (!pA) return;

	Fvector pos = pA->Position();
	pos.y += 1.5f;
	
	if (control_hit_sound.feedback) control_hit_sound.stop();
	control_hit_sound.play_at_pos(pA,pos);
}

void CController::reload(LPCSTR section)
{
	inherited::reload			(section);
	CPsyAuraController::reload	(section);

	// Load triple gravi animations
	MotionID					def1, def2, def3;
	def1						= smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("stand_sit_down_attack_0");
	VERIFY						(def1);

	def2						= smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("control_attack_0");
	VERIFY						(def2);

	def3						= smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("sit_stand_up_attack_0");
	VERIFY						(def3);

	anim_triple_control.reinit_external	(&EventMan, def1, def2, def3);

	SVelocityParam &velocity_run = movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	CJumping::AddState			(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("jump_glide_0"), JT_GLIDE, false,	0.f, velocity_run.velocity.angular_real);
}

void CController::reinit()
{
	inherited::reinit();
	CPsyAuraController::reinit();
	
	int_need_deactivate = false;
}

void CController::control_hit()
{
	play_control_sound_hit();
	
	// start postprocess
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
	if (pA) {
		pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(m_control_effector.ce_time,m_control_effector.ce_amplitude,m_control_effector.ce_period_number,m_control_effector.ce_power));
		Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_control_effector.ppi, m_control_effector.time, m_control_effector.time_attack, m_control_effector.time_release));
	}

	active_control_fx			= true;
	time_control_hit_started	= Device.dwTimeGlobal;

}

#define TEXTURE_SIZE_PERCENT 2.f

void CController::UpdateCL()
{
	inherited::UpdateCL();
	CJumping::Update();
	CPsyAuraController::frame_update();

	if (int_need_deactivate && !CPsyAuraController::effector_active()) {
		processing_deactivate();
		int_need_deactivate = false;
	}
	
	if (active_control_fx) {
		u32 time_to_show	= 150;
		float percent		= float((Device.dwTimeGlobal - time_control_hit_started)) / float(time_to_show);
		float percent2		= 1 - (percent - TEXTURE_SIZE_PERCENT) / 2 ;

		
		if (percent < TEXTURE_SIZE_PERCENT ) {
			float x1 = Device.dwWidth  / 2 - ((Device.dwWidth	/ 2) * percent);
			float y1 = Device.dwHeight / 2 - ((Device.dwHeight	/ 2) * percent);
			float x2 = Device.dwWidth  / 2 + ((Device.dwWidth	/ 2) * percent);
			float y2 = Device.dwHeight / 2 + ((Device.dwHeight	/ 2) * percent);

			HUD().GetUI()->UIMainIngameWnd->AddStaticItem(&m_UIControlFX2,int(x1),int(y1),int(x2),int(y2));
		} else if (percent2 > 0){
			float x1 = Device.dwWidth  / 2 - ((Device.dwWidth	/ 2) * percent2);
			float y1 = Device.dwHeight / 2 - ((Device.dwHeight	/ 2) * percent2);
			float x2 = Device.dwWidth  / 2 + ((Device.dwWidth	/ 2) * percent2);
			float y2 = Device.dwHeight / 2 + ((Device.dwHeight	/ 2) * percent2);

			HUD().GetUI()->UIMainIngameWnd->AddStaticItem(&m_UIControlFX,int(x1),int(y1),int(x2),int(y2));

		} else active_control_fx = false;
	}
}

void CController::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CPsyAuraController::schedule_update();

	UpdateControlled();
}

void CController::Jump()
{
	if (CJumping::Check(Position(),EnemyMan.get_enemy()->Position(),EnemyMan.get_enemy())){
		sound().play(MonsterSpace::eMonsterSoundAttackHit);
		MotionMan.ActivateJump();
	}
}

void CController::Die(CObject* who)
{
	inherited::Die(who);
	FreeFromControl();

	CPsyAuraController::deactivate();
	CPsyAuraController::set_auto_activate(false);
	
	processing_activate();
	int_need_deactivate = true;
}

void CController::net_Destroy()
{
	inherited::net_Destroy();
	FreeFromControl();
}

void CController::FreeFromControl()
{
	for	(u32 i=0; i<m_controlled_objects.size(); i++) smart_cast<CControlledEntityBase *>(m_controlled_objects[i])->free_from_control();
	m_controlled_objects.clear();
}

void CController::OnFreedFromControl(const CEntity *entity)
{
	for	(u32 i=0; i<m_controlled_objects.size(); i++) 
		if (m_controlled_objects[i] == entity) {
			m_controlled_objects[i] = m_controlled_objects.back();
			m_controlled_objects.pop_back();
			return;
	}
}

#ifdef DEBUG
CBaseMonster::SDebugInfo CController::show_debug_info()
{
	CBaseMonster::SDebugInfo info = inherited::show_debug_info();
	if (!info.active) return CBaseMonster::SDebugInfo();

	string128 text;
	sprintf(text, "Psy Aura: Radius = [%f]  Energy = [%f]", CPsyAuraController::get_current_radius(), CPsyAuraController::get_value());
	DBG().text(this).add_item(text, info.x, info.y+=info.delta_y, info.color);
	DBG().text(this).add_item("---------------------------------------", info.x, info.y+=info.delta_y, info.delimiter_color);

	
	// Draw Controlled Lines
	DBG().level_info(this).clear();
	
	Fvector my_pos = Position();
	my_pos.y += 1.5f;
		
	
	for (u32 i=0; i < m_controlled_objects.size(); i++) {
		Fvector enemy_pos	= m_controlled_objects[i]->Position();
		
		Fvector dir;
		dir.sub(enemy_pos, Position());
		dir.div(2.f);
		Fvector new_pos;
		new_pos.add(Position(),dir);
		new_pos.y += 10.f;

		enemy_pos.y += 1.0f;
	
		DBG().level_info(this).add_item(my_pos,	new_pos, D3DCOLOR_XRGB(0,255,255));
		DBG().level_info(this).add_item(enemy_pos, new_pos, D3DCOLOR_XRGB(0,255,255));
	}

	return CBaseMonster::SDebugInfo();
}
#endif

