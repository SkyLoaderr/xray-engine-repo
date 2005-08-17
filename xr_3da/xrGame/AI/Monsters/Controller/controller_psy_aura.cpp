#include "stdafx.h"
#include "controller_psy_aura.h"
#include "../../../actor.h"
#include "../../../ai_sounds.h"
#include "../../../xrmessages.h"
#include "../../../net_utils.h"
#include "../../../CustomMonster.h"

CPsyAuraController::CPsyAuraController()
{
	set_radius(30.f);
	set_auto_activate();
	set_auto_deactivate();
	m_effector.SetRadius(get_radius());
}

CPsyAuraController::~CPsyAuraController()
{
}

void CPsyAuraController::reload(LPCSTR section)
{
	inherited::reload(section, "PsyAura_");
	
	m_effector.Load(pSettings->r_string(section,"PsyAura_Postprocess_Section"));
	::Sound->create(m_sound,TRUE, pSettings->r_string(section,"PsyAura_HeadSound"), SOUND_TYPE_WORLD);

	power_down_vel = pSettings->r_float(section,"PsyAura_Power_Down_Velocity");
}

void CPsyAuraController::reinit()
{
	inherited::reinit();
	m_actor				= 0;
	m_current_radius	= get_radius() + 1.f;
}

void CPsyAuraController::on_activate()
{
	if (m_actor) {
		if (m_current_radius > get_radius()) m_current_radius = get_radius()-0.05f;
	}
}

void CPsyAuraController::on_deactivate()
{
}

void CPsyAuraController::schedule_update()
{
	inherited::schedule_update();
	
	// Падение энергии у врагов
	if (is_active()) {
		if (m_actor && !m_sound._feedback()) m_sound.play_at_pos(m_actor,Fvector().set(0,0,0), sm_2D);

		for (ENEMY_VECTOR_IT it = m_enemies.begin(); it != m_enemies.end(); it++) {
			
			float				power_percent = (*it)->Position().distance_to(get_object()->Position()) / get_radius();

			Fvector				hit_dir;
			hit_dir.sub			(get_object()->Position(), (*it)->Position());
			hit_dir.normalize	();

			NET_Packet					P;
			get_object()->u_EventGen	(P,GE_HIT, (*it)->ID());
			P.w_u16						((*it)->ID());
			P.w_u16						((*it)->ID());
			P.w_dir						(hit_dir);
			P.w_float					(power_down_vel * power_percent);
			P.w_s16						(BI_NONE);
			P.w_vec3					(Fvector().set(0.f,0.f,0.f));
			P.w_float					(0.f);
			P.w_u16						(u16(ALife::eHitTypeTelepatic));
			get_object()->u_EventSend	(P);
		}
	}
}

void CPsyAuraController::frame_update() 
{
	bool b_updated = false;
	
	if (is_active()) {
		if (m_actor) {
			
			m_effector.Update(m_current_radius);

			if (m_current_radius < m_actor->Position().distance_to(get_object()->Position())) 
				m_current_radius += Device.fTimeDelta * 1.2f;
			else 
				m_current_radius -= Device.fTimeDelta * 1.2f;
			
			b_updated = true;
		}
	} else {
		// реализация плавного увядания, если энергия поля закончилась, а актер находится в радиусе
		if (m_actor) {
			m_effector.Update(m_current_radius);
			m_current_radius += Device.fTimeDelta * 1.2f;
			b_updated = true;
		}
	}

	if (!b_updated) 
		m_effector.Update(get_radius() + 1.f);
}


//////////////////////////////////////////////////////////////////////////
// Feel::Touch Routines
BOOL CPsyAuraController::feel_touch_contact(CObject* O)
{
	// реагировать только на актера и на монстров
	if ((O != get_object()) && (smart_cast<CActor*>(O) || smart_cast<CCustomMonster*>(O))) return TRUE;
	
	return FALSE;
}

void CPsyAuraController::feel_touch_new(CObject* O)
{
	m_actor = smart_cast<CActor*>(O);

	ENEMY_VECTOR_IT element = std::find( m_enemies.begin(), m_enemies.end(), O );
	VERIFY(element == m_enemies.end());
	
	m_enemies.push_back(O);
}

void CPsyAuraController::feel_touch_delete(CObject* O)
{
	if (smart_cast<CActor*>(O)) m_actor = 0;

	ENEMY_VECTOR_IT element = std::find( m_enemies.begin(), m_enemies.end(), O );
	VERIFY	(element != m_enemies.end());

	m_enemies.erase(element);
}
//////////////////////////////////////////////////////////////////////////

void CPsyAuraController::on_relcase(CObject *O)
{
	if (m_actor == O) m_actor = 0;
	
	ENEMY_VECTOR_IT element = std::find( m_enemies.begin(), m_enemies.end(), O );
	if (element != m_enemies.end())
		m_enemies.erase(element);
}
