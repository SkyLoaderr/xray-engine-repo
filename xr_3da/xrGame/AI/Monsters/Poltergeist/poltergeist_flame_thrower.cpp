#include "stdafx.h"
#include "poltergeist.h"
#include "../../../xrmessages.h"

#define FIND_POINT_ATTEMPT_COUNT	5
#define MIN_FLAME_DIST				2.f
#define MAX_FLAME_DIST				8.f
#define MIN_FLAME_HEIGHT			1.2f
#define MAX_FLAME_HEIGHT			2.6f


void CPoltergeist::LoadFlame(LPCSTR section)
{
	m_flame_sound.create		(TRUE, pSettings->r_string(section,"flame_sound"), SOUND_TYPE_WORLD);
	m_flame_particles			= pSettings->r_string(section,"flame_particles");
	m_flame_fire_delay			= pSettings->r_u32(section,"flame_fire_delay");
	m_flame_length				= pSettings->r_float(section,"flame_length");
	m_flame_hit_value			= pSettings->r_float(section,"flame_hit_value");
}

void CPoltergeist::FireFlame(const CObject *target_object)
{
	Fvector position;
	if (!GetValidFlamePosition(target_object, position)) return;
	
	SFlameElement *element	= xr_new<SFlameElement>();

	element->position			= position;
	element->target_object		= target_object;
	element->time_started		= Level().timeServer();
	element->sound.clone		(m_flame_sound, SOUND_TYPE_WORLD);
	element->sound.play_at_pos	(this,element->position);
	
	m_flames.push_back			(element);

	// Start Particles
	CParticlesObject *ps = xr_new<CParticlesObject>(m_flame_particles);
	
	// вычислить позицию и направленность партикла
	Fvector p_center;
	element->target_object->Center(p_center);
	element->target_dir.sub(p_center, element->position);
	element->target_dir.normalize();

	Fmatrix pos; 
	pos.identity();
	pos.k.set(element->target_dir);
	Fvector::generate_orthonormal_basis_normalized(pos.k,pos.j,pos.i);
	// установить позицию
	pos.translate_over(element->position);

	ps->UpdateParent(pos, zero_vel);
	ps->Play();	

}


struct remove_predicate {
	bool	operator() (CPoltergeist::SFlameElement *element)	{
		
		if ((element->time_started == 0) && !element->sound.feedback) {
			xr_delete(element);
			return true;
		}
		return false;
	}
};


void CPoltergeist::UpdateFlame()
{
	FLAME_ELEMS_IT I = m_flames.begin();
	FLAME_ELEMS_IT E = m_flames.end();
	
	// Пройти по всем объектам и проверить на хит врага
	for ( ;I != E; ++I) {
		SFlameElement *element = *I;		
		
		if (element->time_started == 0) continue;
		if (!element->target_object || element->target_object->getDestroy()) {
			element->time_started = 0;
			continue;
		}

		if (element->time_started + m_flame_fire_delay < Level().timeServer()) {
			// Test hit
			Collide::rq_result rq;
			
			if (Level().ObjectSpace.RayPick(element->position, element->target_dir, m_flame_length, Collide::rqtBoth, rq)) {
				if ((rq.O == element->target_object) && (rq.range < m_flame_length)) {

					NET_Packet		P;
					u_EventGen		(P,GE_HIT, element->target_object->ID());
					P.w_u16			(ID());
					P.w_u16			(ID());
					P.w_dir			(element->target_dir);
					P.w_float		(m_flame_hit_value);
					P.w_s16			(BI_NONE);
					P.w_vec3		(Fvector().set(0.f,0.f,0.f));
					P.w_float		(0.f);
					P.w_u16			(u16(ALife::eHitTypeBurn));
					u_EventSend		(P);
				}
			}
			element->time_started = 0;
		}
	}

	// удалить все элементы, выполнение которых закончено
	I = std::remove_if(m_flames.begin(), m_flames.end(), remove_predicate());
	m_flames.erase(I,m_flames.end());
}

bool CPoltergeist::GetValidFlamePosition(const CObject *target_object, Fvector &res_pos)
{
	const CGameObject *Obj = smart_cast<const CGameObject *>(target_object);
	if (!Obj) return (false);

	Fvector dir;
	float h,p;

	Fvector vertex_position;
	Fvector new_pos;

	for (u32 i=0; i<FIND_POINT_ATTEMPT_COUNT; i++) {
		
		target_object->Direction().getHP(h,p);
		h = Random.randF(0, PI_MUL_2);
		dir.setHP(h,p);
		dir.normalize();

		vertex_position = ai().level_graph().vertex_position(Obj->level_vertex_id());
		new_pos.mad(vertex_position, dir, Random.randF(MIN_FLAME_DIST, MAX_FLAME_DIST));

		u32 node = ai().level_graph().check_position_in_direction(Obj->level_vertex_id(), vertex_position, new_pos);
		if (node != u32(-1)) {
			res_pos = ai().level_graph().vertex_position(node);
			res_pos.y += Random.randF(MIN_FLAME_HEIGHT, MAX_FLAME_HEIGHT);
			return (true);
		}
	}


	float angle = ai().level_graph().vertex_less_cover(Obj->level_vertex_id(),PI_DIV_6);

	dir.set(1.f,0.f,0.f);
	dir.setHP(angle_normalize(angle+PI), 0.f);
	dir.normalize();

	vertex_position = ai().level_graph().vertex_position(Obj->level_vertex_id());
	new_pos.mad(vertex_position, dir, Random.randF(MIN_FLAME_DIST, MAX_FLAME_DIST));

	u32 node = ai().level_graph().check_position_in_direction(Obj->level_vertex_id(), vertex_position, new_pos);
	if (node != u32(-1)) {
		res_pos = ai().level_graph().vertex_position(node);
		res_pos.y += Random.randF(MIN_FLAME_HEIGHT, MAX_FLAME_HEIGHT);
		return (true);
	}

	return (false);
}

void CPoltergeist::RemoveFlames()
{
	FLAME_ELEMS_IT I = m_flames.begin();
	FLAME_ELEMS_IT E = m_flames.end();

	// Пройти по всем объектам и проверить на хит врага
	for ( ;I != E; ++I) {
		if ((*I)->sound.feedback) (*I)->sound.stop();
		xr_delete((*I));
	}
	
	m_flames.clear();
}


