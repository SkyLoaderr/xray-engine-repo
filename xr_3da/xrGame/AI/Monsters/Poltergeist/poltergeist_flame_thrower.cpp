#include "stdafx.h"
#include "poltergeist.h"

void CPoltergeist::LoadFlame(LPCSTR section)
{
	m_flame_prepare.sound.create	(TRUE, pSettings->r_string(section,"sound_fireball_perpare"),	SOUND_TYPE_WORLD);
	m_flame_fire.sound.create		(TRUE, pSettings->r_string(section,"sound_fireball_fire"),		SOUND_TYPE_WORLD);
	m_flame_finalize.sound.create	(TRUE, pSettings->r_string(section,"sound_fireball_finalize"),	SOUND_TYPE_WORLD);

	m_flame_prepare.particle	= pSettings->r_string(section,"particle_fireball_perpare");
	m_flame_fire.particle		= pSettings->r_string(section,"particle_fireball_fire");
	m_flame_finalize.particle	= pSe
		ttings->r_string(section,"particle_fireball_finalize");
}

void CPoltergeist::StartFlame()
{
	if (!m_flame_prepare.sound.feedback) {
		m_flame_prepare.sound.play_at_pos(this,m_flame_position, sm_Looped,0.2f);
	} 
}

void CPoltergeist::FireFlame()
{
	if (m_flame_prepare.sound.feedback) {
		m_flame_prepare.sound.stop();
	} 

	if (!m_flame_fire.sound.feedback){
		m_flame_fire.sound.play_at_pos(this,m_flame_position, sm_Looped, 0.2f);
	}
}
