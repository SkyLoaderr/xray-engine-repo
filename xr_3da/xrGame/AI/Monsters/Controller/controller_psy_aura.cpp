#include "stdafx.h"
#include "controller_psy_aura.h"
#include "../../../actor.h"
#include "../../../ai_sounds.h"

CPsyAuraController::CPsyAuraController()
{
	set_radius(30.f);
	set_auto_activate();
	m_effector.SetRadius(get_radius());
}

CPsyAuraController::~CPsyAuraController()
{
}

void CPsyAuraController::reload(LPCSTR section)
{
	inherited::reload(section);
	m_effector.Load(pSettings->r_string(section,"PsyAura_Postprocess_Section"));
	::Sound->create(m_sound,TRUE, pSettings->r_string(section,"PsyAura_HeadSound"), SOUND_TYPE_WORLD);
}

void CPsyAuraController::reinit()
{
	inherited::reinit();
	m_actor				= 0;
	m_current_radius	= get_radius() + 1.f;
}

void CPsyAuraController::on_activate()
{
}

void CPsyAuraController::on_deactivate()
{
	if (m_actor) m_current_radius = m_actor->Position().distance_to(get_object()->Position());
}

void CPsyAuraController::schedule_update()
{
	inherited::schedule_update();

	if (m_actor) {
		Fvector pos = m_actor->Position();
		pos.y += 1.8f;

		if (m_sound.feedback) m_sound.set_position(pos);
		else ::Sound->play_at_pos(m_sound,m_actor,pos);
	}

}

void CPsyAuraController::frame_update() 
{
	inherited::frame_update();

	bool b_updated = false;
	
	if (is_active()) {
		if (m_actor) {
			m_effector.Update(m_actor->Position().distance_to(get_object()->Position()));
			b_updated = true;
		}
	} else {
		// реализация плавного увядания, если энергия поля закончилась, а актер находится в радиусе
		if (m_actor) {
			m_effector.Update(m_current_radius);
			m_current_radius += Device.fTimeDelta * 2.f;
			b_updated = true;
		}
	}

	if (!b_updated) m_effector.Update(get_radius() + 1.f);
}


//////////////////////////////////////////////////////////////////////////
// Feel::Touch Routines
BOOL CPsyAuraController::feel_touch_contact(CObject* O)
{
	// реагировать только на актера
	if (dynamic_cast<CActor*>(O)) return TRUE;
	else return FALSE;
}

void CPsyAuraController::feel_touch_new(CObject* O)
{
	m_actor = dynamic_cast<CActor*>(O);
}

void CPsyAuraController::feel_touch_delete(CObject* O)
{
	if (dynamic_cast<CActor*>(O)) m_actor = 0;
}
//////////////////////////////////////////////////////////////////////////


