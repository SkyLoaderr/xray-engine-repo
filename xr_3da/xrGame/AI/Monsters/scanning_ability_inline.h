#pragma once

#include "ai_monster_effector.h"
//#include "../../actor.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CScanningAbilityAbstract CScanningAbility<_Object>

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::load(LPCSTR section)
{
	::Sound->create(sound_scan,	TRUE, pSettings->r_string(section,"scan_sound"), SOUND_TYPE_WORLD);

	critical_value			= pSettings->r_float(section,"scan_critical_value");
	scan_radius				= pSettings->r_float(section,"scan_radius");
	velocity_threshold		= pSettings->r_float(section,"scan_velocity_threshold");
	decrease_value			= pSettings->r_float(section,"scan_decrease_value");
	scan_trace_time_freq	= pSettings->r_float(section,"scan_trace_time_freq");

	VERIFY					(!fis_zero(scan_trace_time_freq));

	// load scan effector
	LPCSTR ppi_section = pSettings->r_string(section, "scan_effector_section");
	m_effector_info.duality.h			= pSettings->r_float(ppi_section,"duality_h");
	m_effector_info.duality.v			= pSettings->r_float(ppi_section,"duality_v");
	m_effector_info.gray				= pSettings->r_float(ppi_section,"gray");
	m_effector_info.blur				= pSettings->r_float(ppi_section,"blur");
	m_effector_info.noise.intensity		= pSettings->r_float(ppi_section,"noise_intensity");
	m_effector_info.noise.grain			= pSettings->r_float(ppi_section,"noise_grain");
	m_effector_info.noise.fps			= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(m_effector_info.noise.fps));
	
	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_effector_info.color_base.r,	&m_effector_info.color_base.g,	&m_effector_info.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_effector_info.color_gray.r,	&m_effector_info.color_gray.g,	&m_effector_info.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_effector_info.color_add.r,	&m_effector_info.color_add.g,	&m_effector_info.color_add.b);

	m_effector_time			= pSettings->r_float(ppi_section,"time");
	m_effector_time_attack	= pSettings->r_float(ppi_section,"time_attack");
	m_effector_time_release	= pSettings->r_float(ppi_section,"time_release");
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::reinit()
{
	state				= eStateDisabled;
	scan_value			= 0.f;

	time_last_trace		= 0;
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::schedule_update()
{
	// ���������������� ���������� ������� ������������

	if (state == eStateDisabled) return;

	CObject *scan_obj	= Level().CurrentEntity();

	// �������� �� ����������
	if (state == eStateNotActive) {
		if (scan_obj->Position().distance_to(object->Position()) < scan_radius) state = eStateScanning;
	}

	if (state == eStateNotActive) return;

	if (state == eStateScanning) {
		// �������� scan_value
		float vel = get_velocity(scan_obj);
		if ( vel > velocity_threshold) {
			
			// �������� �� ����, ��� scan_trace_time_freq
			if (time_last_trace + u32(1000 / scan_trace_time_freq) < Device.dwTimeGlobal) {
				time_last_trace = Device.dwTimeGlobal;
				scan_value		+= vel;
			}
			
			if (sound_scan.feedback) sound_scan.set_position(scan_obj->Position());
			else {
				// ������ ����
				::Sound->play_at_pos(sound_scan, 0, scan_obj->Position());
				
				// �����������
				Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_effector_info, m_effector_time, m_effector_time_attack, m_effector_time_release));
			}
			on_scanning();
		}
	}

	if (scan_value > critical_value) {
		on_scan_success();
		state = eStateDisabled;
	}

}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::frame_update(u32 dt)
{
	if (state != eStateScanning) return;

	if (scan_value < 0) scan_value = 0.f;
	else if (scan_value > 0) {
		scan_value -= decrease_value * float(dt) / 1000;
	}
}


TEMPLATE_SPECIALIZATION
float CScanningAbilityAbstract::get_velocity(CObject *obj)
{
	//if (obj->ps_Size() < 2) return (0.f);

	//CObject::SavedPosition	pos0 = obj->ps_Element(obj->ps_Size() - 2);
	//CObject::SavedPosition	pos1 = obj->ps_Element(obj->ps_Size() - 1);

	//return	(pos1.vPosition.distance_to(pos0.vPosition) /
 //            (float(pos1.dwTime)/1000.f - float(pos0.dwTime)/1000.f));


	CActor *actor = smart_cast<CActor *>(obj);
	return (actor->movement_control()->GetVelocityActual()); 
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::enable() 
{
	if (state != eStateDisabled) return;

	state		= eStateNotActive;
	scan_value	= 0.f; 
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::disable()
{
	state		= eStateDisabled;
	scan_value	= 0.f; 
}


#undef TEMPLATE_SPECIALIZATION