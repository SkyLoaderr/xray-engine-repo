#include "stdafx.h"
#include "controller.h"
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
#include "../monster_velocity_space.h"
#include "../../../level_debug.h"
#include "../../../game_object_space.h"
#include "../../../detail_path_manager.h"
#include "../../../ai_space.h"
#include "../../../cover_point.h"
#include "../../../cover_manager.h"

#include "controller_animation.h"
#include "controller_direction.h"

#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../control_path_builder_base.h"

#include "../../../level_navigation_graph.h"
#include "../../../ai_object_location.h"

const u32	_pmt_psy_attack_delay		= 2000;
const float	_pmt_psy_attack_min_angle	= deg(5);


CController::CController()
{
	StateMan = xr_new<CStateManagerController>(this);
	CPsyAuraController::init_external(this);
	time_control_hit_started = 0;
}

CController::~CController()
{
	xr_delete(StateMan);
	xr_delete(m_ce_best);
}

void CController::Load(LPCSTR section)
{
	inherited::Load	(section);

	// Load Control FX texture
	m_UIControlFX.Init(pSettings->r_string(section, "control_fx_texture"), "hud\\default",0,0,0);
	m_UIControlFX2.Init(pSettings->r_string(section, "control_fx_texture2"), "hud\\default",0,0,0);

	m_max_controlled_number			= pSettings->r_u8(section,"Max_Controlled_Count");
	m_controlled_objects.reserve	(m_max_controlled_number);

	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	::Sound->create(control_start_sound,TRUE, pSettings->r_string(section,"sound_control_start"),	SOUND_TYPE_WORLD);
	::Sound->create(control_hit_sound,	TRUE, pSettings->r_string(section,"sound_control_hit"),		SOUND_TYPE_WORLD);

	anim().AddReplacedAnim(&m_bDamaged, eAnimStandIdle,	eAnimStandDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

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

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eVelocityParameterIdle);	
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eVelocityParameterStand);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	//SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);


	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSitIdle,		"sit_idle_",			-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimEat,			"sit_eat_",				-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND);
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_dmg_",		-1, &velocity_walk_dmg,	PS_STAND);
	anim().AddAnim(eAnimRun,			"stand_run_fwd_",		-1,	&velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &velocity_run_dmg,	PS_STAND);
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND);
	anim().AddAnim(eAnimCheckCorpse,	"stand_check_corpse_",	-1,	&velocity_none,		PS_STAND);
	anim().AddAnim(eAnimDie,			"stand_die_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,		PS_STAND);	
	anim().AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimSleep,			"sit_sleep_",			-1, &velocity_none,		PS_SIT);

	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkFwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimSitIdle);
	anim().LinkAction(ACT_DRAG,			eAnimStandIdle);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

	anim().AddTransition(PS_STAND,	PS_SIT,		eAnimStandSitDown,	false);
	anim().AddTransition(PS_SIT,	PS_STAND,	eAnimSitStandUp,	false);

#ifdef DEBUG	
	anim().accel_chain_test		();
#endif


	m_ce_best = xr_new<CControllerCoverEvaluator>(&control().path_builder().restrictions());

	m_velocity_move_fwd.Load	(section, "Velocity_MoveFwd");
	m_velocity_move_bkwd.Load	(section, "Velocity_MoveBkwd");

}

BOOL CController::net_Spawn(CSE_Abstract *DC)
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	if (!g_Alive()) {
		CPsyAuraController::deactivate();
		CPsyAuraController::set_auto_activate(false);
	}
	
	return (TRUE);
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
		com_man().seq_run(anim().get_motion_id(eAnimCheckCorpse));
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
	Fvector pos = EnemyMan.get_enemy()->Position();
	pos.y += 1.5f;

	if (control_start_sound._feedback()) control_start_sound.stop();
	control_start_sound.play_at_pos(const_cast<CEntityAlive*>(EnemyMan.get_enemy()),pos);
}

void CController::play_control_sound_hit()
{
	Fvector pos = EnemyMan.get_enemy()->Position();
	pos.y += 1.5f;
	
	if (control_hit_sound._feedback()) control_hit_sound.stop();
	control_hit_sound.play_at_pos(const_cast<CEntityAlive*>(EnemyMan.get_enemy()),pos);
}

void CController::reload(LPCSTR section)
{
	inherited::reload			(section);
	CPsyAuraController::reload	(section);

	com_man().ta_fill_data(anim_triple_control,	"stand_sit_down_attack_0",	"control_attack_0",	"sit_stand_up_attack_0", true, false);
}

void CController::reinit()
{
	// must be before inherited call because of its use in ControlAnimation com
	m_mental_state = eStateIdle;
	
	inherited::reinit();
	CPsyAuraController::reinit();
	
	int_need_deactivate = false;

	m_psy_fire_start_time	= 0;
	m_psy_fire_delay		= _pmt_psy_attack_delay;

	control().path_builder().detail().add_velocity(MonsterMovement::eControllerVelocityParameterMoveFwd,	CDetailPathManager::STravelParams(m_velocity_move_fwd.velocity.linear,	m_velocity_move_fwd.velocity.angular_path,	m_velocity_move_fwd.velocity.angular_real));
	control().path_builder().detail().add_velocity(MonsterMovement::eControllerVelocityParameterMoveBkwd,	CDetailPathManager::STravelParams(m_velocity_move_bkwd.velocity.linear,	m_velocity_move_bkwd.velocity.angular_path, m_velocity_move_bkwd.velocity.angular_real));
}

void CController::control_hit()
{
	PsyHit						(EnemyMan.get_enemy(), 30.f);
	
	// start postprocess
	CActor *pA = const_cast<CActor *>(smart_cast<const CActor *>(EnemyMan.get_enemy()));
	if (!pA) return;
	
	pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(m_control_effector.ce_time,m_control_effector.ce_amplitude,m_control_effector.ce_period_number,m_control_effector.ce_power));
	Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_control_effector.ppi, m_control_effector.time, m_control_effector.time_attack, m_control_effector.time_release));

	play_control_sound_hit		();

	active_control_fx			= true;
	time_control_hit_started	= Device.dwTimeGlobal;
}

#define TEXTURE_SIZE_PERCENT 2.f

void CController::UpdateCL()
{
	inherited::UpdateCL();
	
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

			HUD().GetUI()->UIMainIngameWnd->AddStaticItem(&m_UIControlFX2,x1,y1,x2,y2);
		} else if (percent2 > 0){
			float x1 = Device.dwWidth  / 2 - ((Device.dwWidth	/ 2) * percent2);
			float y1 = Device.dwHeight / 2 - ((Device.dwHeight	/ 2) * percent2);
			float x2 = Device.dwWidth  / 2 + ((Device.dwWidth	/ 2) * percent2);
			float y2 = Device.dwHeight / 2 + ((Device.dwHeight	/ 2) * percent2);

			HUD().GetUI()->UIMainIngameWnd->AddStaticItem(&m_UIControlFX,x1,y1,x2,y2);

		} else active_control_fx = false;
	}
}

void CController::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CPsyAuraController::schedule_update();

	UpdateControlled();
	
	// DEBUG
	test_covers();
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

void CController::net_Relcase(CObject *O)
{
	inherited::net_Relcase(O);
	CPsyAuraController::on_relcase(O);
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

//////////////////////////////////////////////////////////////////////////

void CController::draw_fire_particles()
{
	if (!EnemyMan.get_enemy()) return;
	//if (!EnemyMan.see_enemy_now()) return;
	CEntityAlive *enemy	= const_cast<CEntityAlive*>(EnemyMan.get_enemy());

	// вычислить позицию и направленность партикла
	Fvector my_head_pos;
	my_head_pos.set	(get_head_position(this));
	
	Fvector position;
	position.set	(get_head_position(enemy));
	position.y		-= 0.5f;
	
	Fvector			dir;
	dir.sub			(position, my_head_pos);
	dir.normalize	();

	PlayParticles("weapons\\generic_weapon07", my_head_pos, dir);

	NET_Packet	l_P;
	u_EventGen	(l_P,GE_HIT, enemy->ID());
	l_P.w_u16	(ID());
	l_P.w_u16	(ID());
	l_P.w_dir	(dir);
	l_P.w_float	(20.f);
	l_P.w_s16	(smart_cast<CKinematics*>(enemy->Visual())->LL_GetBoneRoot());
	l_P.w_vec3	(Fvector().set(0.f,0.f,0.f));
	l_P.w_float	(200.f);
	l_P.w_u16	( u16(ALife::eHitTypeWound) );
	u_EventSend	(l_P);
	
}

void CController::psy_fire()
{
	if (!EnemyMan.get_enemy())	return;

	draw_fire_particles			();
	play_control_sound_hit		();

	m_psy_fire_start_time		= time();
}

bool CController::can_psy_fire()
{
	if (m_psy_fire_start_time + m_psy_fire_delay > time ())			return false;
	if (!EnemyMan.get_enemy())										return false;
	if (!EnemyMan.see_enemy_now())									return false;

	float cur_yaw	= custom_dir().get_head_orientation().current.yaw;
	float dir_yaw	= Fvector().sub(EnemyMan.get_enemy()->Position(), Position()).getH();
	dir_yaw			= angle_normalize(-dir_yaw);
	if (angle_difference(cur_yaw,dir_yaw) > _pmt_psy_attack_min_angle) return false;

	return true;
}

void CController::set_psy_fire_delay_zero()
{
	m_psy_fire_delay = 0;
}
void CController::set_psy_fire_delay_default()
{
	m_psy_fire_delay = _pmt_psy_attack_delay;
}

//////////////////////////////////////////////////////////////////////////


const MonsterSpace::SBoneRotation &CController::head_orientation	() const
{
	return m_custom_dir_base->get_head_orientation();
}

void CController::test_covers()
{
	//////////////////////////////////////////////////////////////////////////
	// update covers
	//////////////////////////////////////////////////////////////////////////
}

void CController::create_base_controls()
{
	m_custom_anim_base	= xr_new<CControllerAnimation>		(); 
	m_custom_dir_base	= xr_new<CControllerDirection>		(); 
	
	m_anim_base			= m_custom_anim_base;
	m_dir_base			= m_custom_dir_base;

	m_move_base			= xr_new<CControlMovementBase>		();
	m_path_base			= xr_new<CControlPathBuilderBase>	();
}

void CController::TranslateActionToPathParams()
{
	if (m_mental_state == eStateIdle) {
		inherited::TranslateActionToPathParams();
		return;
	}
	
	custom_anim().set_path_params();
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


