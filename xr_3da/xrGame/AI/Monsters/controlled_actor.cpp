#include "stdafx.h"
#include "controlled_actor.h"
#include "../../actor.h"
#include "../../../CameraBase.h"
#include "../../xr_level_controller.h"
#include "../../level.h"
#include "ai_monster_utils.h"

#define DIR_RIGHT		0
#define DIR_LEFT		1
#define DIR_NONE		2

#define DIR_BOTTOM		0
#define DIR_UP			1


void CControlledActor::reinit()
{
	reset();
}


void CControlledActor::take_control()
{
	m_actor = smart_cast<CActor*>	(Level().CurrentEntity());
	
	m_actor->SetControlled			(true);
	m_actor->SetMouseScaleFactor	(flt_max);
}

void CControlledActor::free_from_control()
{
	if (!is_controlled())			return;

	m_actor->SetControlled			(false);
	reset();
}

void CControlledActor::frame_update()
{
	if (m_active_turn) update_turn();
}

void CControlledActor::look_point(float speed, const Fvector &point)
{
	m_speed_yaw		= speed;
	m_speed_pitch	= speed;

	Fvector	P,D,N;
	m_actor->cam_Active()->Get			(P,D,N);
	Fvector().sub(point, P).getHP		(target_yaw, target_pitch);
	target_yaw		= angle_normalize	(-target_yaw);
	target_pitch	= angle_normalize	(-target_pitch);

	// define directions
	float	h,p;
	D.getHP	(h,p);	
	h		= angle_normalize(-h);
	p		= angle_normalize(-p);

	yaw_dir		= (from_right(target_yaw,h) ? DIR_RIGHT : DIR_LEFT);
	pitch_dir	= (from_right(target_pitch,p) ? DIR_UP	: DIR_BOTTOM);	
	
	m_active_turn	= true;
}

void CControlledActor::update_turn()
{
	float	h,p;
	Fvector	P,D,N;
	m_actor->cam_Active()->Get(P,D,N);
	
	D.getHP	(h,p);	
	h		= angle_normalize(-h);
	p		= angle_normalize(-p);

	if (angle_difference(target_yaw,h) < PI / (5*180))		yaw_dir		= DIR_NONE;
	if (angle_difference(target_pitch,p) < PI / (5*180))	pitch_dir	= DIR_NONE;

	if ((pitch_dir == DIR_NONE) && (yaw_dir == DIR_NONE)) 
		m_active_turn	= false;

	if (yaw_dir != DIR_NONE) {
		if (yaw_dir == DIR_RIGHT) {
			if (from_right(target_yaw,h)) {
				m_actor->cam_Active()->Move	(kRIGHT,	m_speed_yaw * Device.fTimeDelta);
			} else {
				yaw_dir		= DIR_LEFT;
				m_speed_yaw /= 2;
			}
		} else {
			if (!from_right(target_yaw,h)) {
				m_actor->cam_Active()->Move	(kLEFT,		m_speed_yaw * Device.fTimeDelta);
			} else {
				yaw_dir		= DIR_RIGHT;
				m_speed_yaw /= 2;
			}
		}
	}
	
	if (pitch_dir != DIR_NONE) {
		if (pitch_dir == DIR_UP) {
			if (from_right(target_pitch,p)) {
				m_actor->cam_Active()->Move	(kUP,	m_speed_pitch * Device.fTimeDelta);
			} else {
				pitch_dir		= DIR_BOTTOM;
				m_speed_pitch	/= 2;
			}
		} else {
			if (!from_right(target_pitch,p)) {
				m_actor->cam_Active()->Move	(kDOWN,	m_speed_pitch * Device.fTimeDelta);
			} else {
				pitch_dir		= DIR_UP;
				m_speed_pitch	/= 2;
			}
		}
	}
	
}

void CControlledActor::reset()
{
	m_actor				= 0;
	m_active_turn		= false;

	yaw_dir				= DIR_NONE;
	pitch_dir			= DIR_NONE;
}

bool CControlledActor::is_controlled()
{
	return (m_actor && m_actor->IsControlled());
}

void CControlledActor::update_look_point(const Fvector &point)
{
	Fvector	P,D,N;
	m_actor->cam_Active()->Get			(P,D,N);
	
	Fvector().sub(point, P).getHP		(target_yaw, target_pitch);
	target_yaw		= angle_normalize	(-target_yaw);
	target_pitch	= angle_normalize	(-target_pitch);
}

