#include "stdafx.h"
#include "control_manager.h"
#include "control_combase.h"
#include "BaseMonster/base_monster.h"

// DEBUG purpose only
char *dbg_control_name_table[] = {
		"Control_Movement",
		"Control_Path",
		"Control_Dir",
		"Control_Animation",
		"Control_Sequencer",
		"Control_RotationJump",
		"Control_Animation_BASE",
		"Control_Movement_BASE",
		"Control_Path_BASE",
		"Control_Dir_BASE"
};	


CControl_Manager::CControl_Manager(CBaseMonster *obj)
{
	m_object			= obj;

	m_sequencer			= xr_new<CAnimationSequencer>	();
	m_rotation_jump		= xr_new<CControlRotationJump>	();
	m_triple_anim		= xr_new<CAnimationTriple>		();

	m_animation			= xr_new<CControlAnimation>	();
	m_movement			= xr_new<CControlMovement>	();
	m_direction			= xr_new<CControlDirection>	();

	add			(m_sequencer,		ControlCom::eControlSequencer);
	add			(m_rotation_jump,	ControlCom::eControlRotationJump);
	add			(m_triple_anim,		ControlCom::eControlTripleAnimation);

	add			(m_animation,		ControlCom::eControlAnimation);
	add			(m_direction,		ControlCom::eControlDir);
	add			(m_movement,		ControlCom::eControlMovement);
}

CControl_Manager::~CControl_Manager()
{
	xr_delete(m_animation);
	xr_delete(m_sequencer);
	xr_delete(m_triple_anim);
	xr_delete(m_rotation_jump);
	xr_delete(m_direction);
	xr_delete(m_movement);
}

void CControl_Manager::init_external()
{
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)
		it->second->init_external(this, m_object);
}

void CControl_Manager::load(LPCSTR section)
{
	init_external	();

	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)
		it->second->load(section);
}
void CControl_Manager::reload(LPCSTR section)
{
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it) 
		it->second->reload(section);
}

void CControl_Manager::reinit()
{
	// todo: make it simpler
	// reinit pure first, base second, custom third
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  
		if (is_pure(it->second)) it->second->reinit();
	
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  
		if (is_base(it->second)) it->second->reinit();
	
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  
		if (!is_pure(it->second) && !is_base(it->second)) it->second->reinit();
}

void CControl_Manager::update_frame()
{
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  {
		// update coms
		if (it->second->is_active() && !is_locked(it->second)) {
			it->second->update_frame();
		}
	}
}

void CControl_Manager::update_schedule()
{
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it)  {
		// update coms
		if (it->second->is_active() && !is_locked(it->second)) {
			it->second->update_schedule();
		}
	}
}

ControlCom::EContolType CControl_Manager::com_type(CControl_Com *com)
{
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it) 
		if (it->second == com) return it->first;

	return ControlCom::eControlInvalid;
}


void CControl_Manager::notify(ControlCom::EEventType event, ControlCom::IEventData *data)
{
	CONTROLLERS_VECTOR &vect = m_listeners[event];

	for (u32 i = 0; i < vect.size(); i++) {
		VERIFY(vect[i]->cing());
		vect[i]->cing()->on_event(event, data);
	}
}

//////////////////////////////////////////////////////////////////////////
// Messaging
//////////////////////////////////////////////////////////////////////////
void CControl_Manager::subscribe(CControl_Com *com, ControlCom::EEventType type)
{
	m_listeners[type].push_back(com);
}

void CControl_Manager::unsubscribe(CControl_Com *com, ControlCom::EEventType type)
{
	CONTROLLERS_VECTOR &vect = m_listeners[type];

	for (u32 i = 0; i < vect.size(); i++) {
		if (vect[i] == com) {
			vect[i]			= vect.back();
			vect.pop_back	();
			return;
		}
	}
}

ControlCom::IComData *CControl_Manager::data(CControl_Com *who, ControlCom::EContolType type)
{
	CControl_Com *target = m_control_elems[type];

	// get_capturer
	CControl_Com *capturer = target->ced()->capturer();
	if (capturer == who) {
		return target->ced()->data();
	}
	
	return 0;
}

// TODO: check construction of SControl_Element and check init in add-function
void CControl_Manager::add(CControl_Com *com, ControlCom::EContolType type)
{
	m_control_elems[type] = com;
}
void CControl_Manager::set_base_controller(CControl_Com *com, ControlCom::EContolType type)
{
	m_base_elems[type]	= com;
}

void CControl_Manager::install_path_manager(CControlPathBuilder *pman)
{
	m_path	= pman;
}

bool CControl_Manager::is_pure(CControl_Com *com)
{
	return (com->cing() == 0);
}

bool CControl_Manager::is_base(CControl_Com *com)
{
	return (com->ced() == 0);
}
bool CControl_Manager::is_locked(CControl_Com *com)
{
	return (com->ced() && com->ced()->is_locked());
}

// capture
void CControl_Manager::capture(CControl_Com *com, ControlCom::EContolType type)  // who, type
{
	CControl_Com *target = m_control_elems[type];
	
	// 1. Check if can capture
	CControl_Com *capturer = target->ced()->capturer();
	VERIFY(!capturer || is_base(capturer));

	if (target->is_active()) {
		target->ced()->on_release();
		// if there is base capturer - stop control com
		if (capturer) capturer->cing()->on_stop_control(type);
	}

	// 3. 
	target->ced()->set_capturer		(com);

	// 4.
	target->ced()->on_capture		();

	// 5. 
	com->cing()->on_start_control	(type);
}

void CControl_Manager::release(CControl_Com *com, ControlCom::EContolType type)  // who, type
{
	CControl_Com *target = m_control_elems[type];
	CControl_Com *capturer = target->ced()->capturer();
	VERIFY	(capturer == com);

	// select new capture if there is a base controller
	CONTROLLERS_MAP_IT it = m_base_elems.find(type);
	if (it != m_base_elems.end()) { 
		com->cing()->on_stop_control(type);
		target->ced()->set_capturer	(0);

		capture(m_base_elems[type], type);
	} else {
		// if active - finalize
		if (target->is_active()) {
			target->ced()->on_release		();
			target->set_active				(false);
		}

		com->cing()->on_stop_control		(type);
		target->ced()->set_capturer			(0);
	}
}

void CControl_Manager::capture_pure(CControl_Com *com)
{
	capture(com,ControlCom::eControlPath);
	capture(com,ControlCom::eControlAnimation);
	capture(com,ControlCom::eControlMovement);
	capture(com,ControlCom::eControlDir);
}

void CControl_Manager::release_pure(CControl_Com *com)
{
	release(com,ControlCom::eControlPath);
	release(com,ControlCom::eControlAnimation);
	release(com,ControlCom::eControlMovement);
	release(com,ControlCom::eControlDir);
}


void CControl_Manager::activate(ControlCom::EContolType type)
{
	m_control_elems[type]->set_active();
}
void CControl_Manager::deactivate(ControlCom::EContolType type)
{
	m_control_elems[type]->set_active(false);
}

bool CControl_Manager::is_captured(ControlCom::EContolType type)
{
	CControl_Com *capturer = m_control_elems[type]->ced()->capturer();
	if (!capturer || is_base(capturer)) return false;
	
	return true;
}

bool CControl_Manager::is_captured_pure()
{
	return (is_captured(ControlCom::eControlPath) ||
			is_captured(ControlCom::eControlAnimation) ||
			is_captured(ControlCom::eControlMovement) ||
			is_captured(ControlCom::eControlDir));
}


void CControl_Manager::lock(CControl_Com *com, ControlCom::EContolType type)
{
	VERIFY	(is_pure(m_control_elems[type]));
	VERIFY	(m_control_elems[type]->ced()->capturer() == com);
	
	m_control_elems[type]->ced()->set_locked();
}

void CControl_Manager::unlock(CControl_Com *com, ControlCom::EContolType type)
{
	VERIFY	(is_pure(m_control_elems[type]));
	VERIFY	(m_control_elems[type]->ced()->capturer() == com);

	m_control_elems[type]->ced()->set_locked(false);
}

void CControl_Manager::path_stop(CControl_Com *com)
{
	SControlPathBuilderData		*ctrl_path = (SControlPathBuilderData*)data(com, ControlCom::eControlPath); 
	VERIFY						(ctrl_path);
	ctrl_path->enable			= false;
}

void CControl_Manager::move_stop(CControl_Com *com)
{
	SControlMovementData		*ctrl_move = (SControlMovementData*)data(com, ControlCom::eControlMovement); 
	VERIFY						(ctrl_move);
	ctrl_move->velocity_target	= 0;
	ctrl_move->acc				= flt_max;

}
void CControl_Manager::dir_stop(CControl_Com *com)
{
	SControlDirectionData		*ctrl_dir = (SControlDirectionData*)data(com, ControlCom::eControlDir); 
	VERIFY						(ctrl_dir);
	ctrl_dir->heading.target_speed	= 0;
}

bool CControl_Manager::check_start_conditions(ControlCom::EContolType type)
{
	return m_control_elems[type]->check_start_conditions();
}


void CControl_Manager::dump(CControl_Com *com, LPCSTR action, ControlCom::EContolType type)
{
	Msg("---------------------------------------------------------------------------");
	Msg("-- [%s] %s [%s]",dbg_control_name_table[com_type(com)], action, dbg_control_name_table[type]);
	Msg("-- Dump:  -----------------------------------------------------------------");

	u32 index = 0;
	for (CONTROLLERS_MAP_IT it = m_control_elems.begin(); it != m_control_elems.end(); ++it, index++)  {
		string128 st; st[0]=0;
		if (!it->second->is_inited()) continue;

		if (it->second->ced()) {
			string128 str;
			if (it->second->ced()->capturer()) 
				strcpy(str, dbg_control_name_table[com_type(it->second->ced()->capturer())]);
			else 
				strcpy(str, "NONE");

			sprintf(st, "Locked[%u] Capturer[%s]", it->second->ced()->is_locked(), str);
		}

		Msg("[%u] %s: Active[%u] %s",index+1, dbg_control_name_table[it->first], it->second->is_active(),st);
	}
}




