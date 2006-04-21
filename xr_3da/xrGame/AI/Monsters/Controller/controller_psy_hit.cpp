#include "stdafx.h"
#include "controller_psy_hit.h"
#include "../BaseMonster/base_monster.h"
#include "controller.h"
#include "../control_animation_base.h"
#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../../../level.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../../../CameraBase.h"
#include "../../../CharacterPhysicsSupport.h"
void CControllerPsyHit::load(LPCSTR section)
{
}

void CControllerPsyHit::reinit()
{
	inherited::reinit();

	CKinematicsAnimated	*skel = smart_cast<CKinematicsAnimated *>(m_object->Visual());
	m_stage[0] = skel->ID_Cycle_Safe("psy_attack_0"); VERIFY(m_stage[0]);
	m_stage[1] = skel->ID_Cycle_Safe("psy_attack_1"); VERIFY(m_stage[1]);
	m_stage[2] = skel->ID_Cycle_Safe("psy_attack_2"); VERIFY(m_stage[2]);
	m_stage[3] = skel->ID_Cycle_Safe("psy_attack_3"); VERIFY(m_stage[3]);
	m_current_index		= 0;

	m_sound_state		= eNone;
}

bool CControllerPsyHit::check_start_conditions()
{
	if (is_active())				return false;	
	if (m_man->is_captured_pure())	return false;
	
	if (Actor()->Cameras().GetCamEffector(eCEControllerPsyHit))	
									return false;

	return true;
}

void CControllerPsyHit::activate()
{
	m_man->capture_pure				(this);
	m_man->subscribe				(this, ControlCom::eventAnimationEnd);

	m_man->path_stop				(this);
	m_man->move_stop				(this);

	//////////////////////////////////////////////////////////////////////////
	// set direction
	SControlDirectionData			*ctrl_dir = (SControlDirectionData*)m_man->data(this, ControlCom::eControlDir); 
	VERIFY							(ctrl_dir);
	ctrl_dir->heading.target_speed	= 3.f;
	ctrl_dir->heading.target_angle	= m_man->direction().angle_to_target(Level().CurrentEntity()->Position());

	//////////////////////////////////////////////////////////////////////////
	m_current_index					= 0;
	play_anim						();
}

void CControllerPsyHit::deactivate()
{
	m_man->release_pure				(this);
	m_man->unsubscribe				(this, ControlCom::eventAnimationEnd);
}

void CControllerPsyHit::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	if (type == ControlCom::eventAnimationEnd) {
		if (m_current_index < 3) {
			m_current_index++;
			play_anim			();
			
			switch (m_current_index) {
				case 1: death_glide_start();	break;
				case 2: hit();					break;
				case 3: death_glide_end();		break;
			}
		} else {
			m_man->deactivate	(this);
			return;
		}
	}
}

void CControllerPsyHit::play_anim()
{
	SControlAnimationData		*ctrl_anim = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	VERIFY						(ctrl_anim);

	ctrl_anim->global.motion	= m_stage[m_current_index];
	ctrl_anim->global.actual	= false;
}

void CControllerPsyHit::death_glide_start()
{
	if (!m_object->EnemyMan.see_enemy_now()) {
		m_man->deactivate	(this);
		return;
	}

	// Start effector
	CEffectorCam* ce = Actor()->Cameras().GetCamEffector(eCEControllerPsyHit);
	VERIFY(!ce);
	
	Fvector src_pos		= Actor()->cam_Active()->vPosition;
	Fvector target_pos	= m_object->Position();
	target_pos.y		+= 1.2f;
	
	Fvector				dir;
	dir.sub				(target_pos,src_pos);
	
	float dist			= dir.magnitude();
	dir.normalize		();

	target_pos.mad		(src_pos,dir,dist-4.8f);
	
	Actor()->Cameras().AddCamEffector(xr_new<CControllerPsyHitCamEffector>(eCEControllerPsyHit, src_pos,target_pos, m_man->animation().motion_time(m_stage[1], m_object->Visual())));
	smart_cast<CController *>(m_object)->draw_fire_particles();

	dir.sub(src_pos,target_pos);
	dir.normalize();
	float h,p;
	dir.getHP(h,p);
	dir.setHP(h,p+PI_DIV_3);
	Actor()->character_physics_support()->movement()->ApplyImpulse(dir,Actor()->GetMass() * 530.f);

	set_sound_state					(eStart);
}

void CControllerPsyHit::death_glide_end()
{
	// Stop camera effector

	CEffectorCam* ce = Actor()->Cameras().GetCamEffector(eCEControllerPsyHit);
	VERIFY(ce);
	Actor()->Cameras().RemoveCamEffector(eCEControllerPsyHit);
	CController *monster = smart_cast<CController *>(m_object);
	monster->draw_fire_particles();
}

void CControllerPsyHit::update_frame()
{
	//if (m_sound_state == eStart) {
	//	CController *monster = smart_cast<CController *>(m_object);
	//	if (!monster->m_sound_tube_start._feedback()) {
	//		m_sound_state = ePull;
	//		monster->m_sound_tube_pull.play_at_pos(Actor(), Fvector().set(0.f, 0.f, 0.f), sm_2D);
	//	}
	//}
}

void CControllerPsyHit::set_sound_state(ESoundState state)
{
	CController *monster = smart_cast<CController *>(m_object);
	if (state == eStart) {
		monster->m_sound_tube_start.play_at_pos(Actor(), Fvector().set(0.f, 0.f, 0.f), sm_2D);
		monster->m_sound_tube_pull.play_at_pos(Actor(), Fvector().set(0.f, 0.f, 0.f), sm_2D);
		m_sound_state = eStart;
		return;
	}
	
	if (state == eHit) {
		if (monster->m_sound_tube_start._feedback())	monster->m_sound_tube_start.stop();
		if (monster->m_sound_tube_pull._feedback())		monster->m_sound_tube_pull.stop();
		
		monster->m_sound_tube_hit_left.play_at_pos(Actor(), Fvector().set(-1.f, 0.f, 1.f), sm_2D);
		monster->m_sound_tube_hit_right.play_at_pos(Actor(), Fvector().set(1.f, 0.f, 1.f), sm_2D);
		m_sound_state = eHit;

		return;
	}
}

void CControllerPsyHit::hit()
{
	CController *monster	= smart_cast<CController *>(m_object);
	
	set_sound_state			(eHit);
	m_object->Hit_Psy		(Actor(), monster->m_tube_damage);
}
