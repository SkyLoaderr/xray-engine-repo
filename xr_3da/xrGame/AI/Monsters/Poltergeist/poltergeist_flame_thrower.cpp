#include "stdafx.h"
#include "poltergeist.h"
#include "../../../xrmessages.h"

void CPoltergeist::LoadFlame(LPCSTR section)
{
	m_flame_sound.create		(TRUE, pSettings->r_string(section,"flame_sound"), SOUND_TYPE_WORLD);
	m_flame_particles			= pSettings->r_string(section,"flame_particles");
	m_flame_fire_delay			= pSettings->r_u32(section,"flame_fire_delay");
	m_flame_length				= pSettings->r_float(section,"flame_length");
	m_flame_hit_value			= pSettings->r_float(section,"flame_hit_value");
}

void CPoltergeist::FireFlame(const Fvector &position, CObject *target_object)
{
	m_flame_position		= position;
	m_time_flame_started	= Level().timeServer();
	m_flame_target_object	 = target_object;

	m_flame_sound.play_at_pos(this,position);

	// Start Particles
	CParticlesObject *ps = xr_new<CParticlesObject>(m_flame_particles);
	
	// вычислить позицию и направленность партикла
	Fvector p_center;
	m_flame_target_object->Center(p_center);
	m_flame_target_dir.sub(p_center, m_flame_position);
	m_flame_target_dir.normalize();

	Fmatrix pos; 
	pos.identity();
	pos.k.set(m_flame_target_dir);
	Fvector::generate_orthonormal_basis_normalized(pos.k,pos.j,pos.i);
	// установить позицию
	pos.translate_over(m_flame_position);

	ps->UpdateParent(pos, zero_vel);
	ps->Play();	
}

void CPoltergeist::UpdateFlame()
{
	if (m_time_flame_started == 0) return;
	
	if (m_time_flame_started + m_flame_fire_delay < Level().timeServer()) {
		// ray pick;

		Collide::rq_result rq;
		if (Level().ObjectSpace.RayPick(m_flame_position, m_flame_target_dir, m_flame_length, Collide::rqtBoth, rq)) {
			if ((rq.O == m_flame_target_object) && rq.range < m_flame_length) {
				
				NET_Packet		P;
				u_EventGen		(P,GE_HIT, m_flame_target_object->ID());
				P.w_u16			(ID());
				P.w_u16			(ID());
				P.w_dir			(m_flame_target_dir);
				P.w_float		(m_flame_hit_value);
				P.w_s16			(BI_NONE);
				P.w_vec3		(Fvector().set(0.f,0.f,0.f));
				P.w_float		(0.f);
				P.w_u16			(u16(ALife::eHitTypeBurn));
				u_EventSend		(P);
			}
		}

		m_time_flame_started = 0;
	}
}
