#include "stdafx.h"
#include "psy_dog_aura.h"

//CPPEffectorPsyDogAura::CPPEffectorPsyDogAura(const SPPInfo &ppi)
//: inherited(ppi, type, true)
//{
//	m_time_to_fade			= time_to_fade;
//	m_effector_state		= eStateFadeIn;
//	m_factor				= 0;
//	m_time_state_started	= Device.dwTimeGlobal;
//
//}
//
//void CPPEffectorPsyDogAura::switch_off()
//{
//	m_effector_state		= eStateFadeOut;		
//	m_time_state_started	= Device.dwTimeGlobal;
//}
//
//
//BOOL CPPEffectorPsyDogAura::update()
//{
//	// update factor
//	if (m_effector_state == eStatePermanent) {
//		m_factor = 1.f;
//	} else {
//		m_factor = (Device.dwTimeGlobal - m_time_state_started) / m_time_to_fade;
//		if (m_effector_state == eStateFadeOut) m_factor = 1 - m_factor;
//
//		if (m_factor > 1) {
//			m_effector_state	= eStatePermanent;
//			m_factor			= 1.f;
//		} else if (m_factor < 0) {
//			return FALSE;
//		}
//	}
//
//	return TRUE;
//}
//
////////////////////////////////////////////////////////////////////////////
////
////////////////////////////////////////////////////////////////////////////
//
//void CPsyDogAura::load(LPCSTR section)
//{
//	// load effector
//	inherited::load(section);
//	
//}
//
//void CPsyDogAura::reinit()
//{
//	m_actor			= 0;
//}
//
//void CPsyDogAura::check_start_condition()
//{
//	// check memory of actor and check memory of phantoms
//	//m_actor->memory();
//}
//
//void CPsyDogAura::check_completion()
//{
//	// check memory of actor and check memory of phantoms
//	//m_actor->memory();
//}
