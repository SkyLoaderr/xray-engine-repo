#include "stdafx.h"
#include "poltergeist.h"

void CPoltergeist::LoadFlame(LPCSTR section)
{
	m_flame_prepare.sound.create	(TRUE, pSettings->r_string(section,"sound_fireball_perpare"),	SOUND_TYPE_WORLD);
	m_flame_fire.sound.create		(TRUE, pSettings->r_string(section,"sound_fireball_fire"),		SOUND_TYPE_WORLD);
	m_flame_finalize.sound.create	(TRUE, pSettings->r_string(section,"sound_fireball_finalize"),	SOUND_TYPE_WORLD);

	m_flame_prepare.particles		= pSettings->r_string(section,"particle_fireball_perpare");
	m_flame_fire.particles			= pSettings->r_string(section,"particle_fireball_fire");
	m_flame_finalize.particles		= pSettings->r_string(section,"particle_fireball_finalize");

	m_flame_prepare.time			= pSettings->r_u32(section,"time_fireball_perpare");
	m_flame_fire.time				= pSettings->r_u32(section,"time_fireball_fire");
	m_flame_finalize.time			= pSettings->r_u32(section,"time_fireball_finalize");
}

void CPoltergeist::StartFlame(const Fvector &position)
{
	m_flame_position	= position;
	m_flame_state		= flamePrepare;
	flame_state_started	= Level().timeServer();

	m_flame_prepare.sound.play_at_pos(this,m_flame_position, sm_Looped);
	
	PlayFlameParticles(m_flame_prepare.ps_object, m_flame_prepare.particles, m_flame_position);
}

void CPoltergeist::FireFlame()
{
	m_flame_state		= flameFire;
	flame_state_started	= Level().timeServer();

	m_flame_fire.sound.play_at_pos(this,m_flame_position, sm_Looped);

	PlayFlameParticles(m_flame_fire.ps_object, m_flame_fire.particles, m_flame_position);
}

void CPoltergeist::FinalizeFlame()
{
	m_flame_state		= flameFinalize;
	flame_state_started	= Level().timeServer();

	m_flame_finalize.sound.play_at_pos(this,m_flame_position, sm_Looped);
	PlayFlameParticles(m_flame_finalize.ps_object, m_flame_finalize.particles, m_flame_position);
}

void CPoltergeist::UpdateFlame()
{
	if (m_flame_state == flameNone) return;

	if (m_flame_state == flamePrepare) {
		if (flame_state_started + m_flame_prepare.time < Level().timeServer())	{
			
			if (m_flame_prepare.sound.feedback) m_flame_prepare.sound.stop();
			
			FireFlame();
		}
		return;
	}

	if (m_flame_state == flameFire) {
		if (flame_state_started + m_flame_fire.time < Level().timeServer())	{

			if (m_flame_fire.sound.feedback) m_flame_fire.sound.stop();
			// stop previous particles
			
			m_flame_prepare.ps_object->Stop			();
			m_flame_prepare.ps_object->PSI_destroy	();

			m_flame_fire.ps_object->Stop			();
			m_flame_fire.ps_object->PSI_destroy		();

			FinalizeFlame();
		}
		return;
	}

	if (m_flame_state == flameFinalize) {
		if (flame_state_started + m_flame_finalize.time < Level().timeServer())	{

			// stop everything
			if (m_flame_finalize.sound.feedback) m_flame_finalize.sound.stop();

			m_flame_finalize.ps_object->Stop			();
			m_flame_finalize.ps_object->PSI_destroy		();

			m_flame_state = flameNone;
		}

		return;
	}
}

void CPoltergeist::PlayFlameParticles(CParticlesObject *&obj, LPCSTR name, const Fvector &position) 
{
	obj = xr_new<CParticlesObject>(name, FALSE);
	
	// вычислить позицию и направленность партикла
	Fmatrix particles_pos; 
	particles_pos.identity();
	particles_pos.k.set(Fvector().set(0.f,1.f,0.f));
	Fvector::generate_orthonormal_basis_normalized(particles_pos.k,particles_pos.j,particles_pos.i);

	particles_pos.translate_over(position);
	obj->SetXFORM(particles_pos);

	obj->Play();
}

