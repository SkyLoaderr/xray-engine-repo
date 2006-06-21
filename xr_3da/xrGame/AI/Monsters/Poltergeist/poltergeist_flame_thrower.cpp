#include "stdafx.h"
#include "poltergeist.h"
#include "../../../xrmessages.h"
#include "../../../ai_object_location.h"
#include "../../../level_graph.h"
#include "../../../level.h"
#include "../../../net_utils.h"
#include "../../../ai_space.h"

#define FIND_POINT_ATTEMPT_COUNT	5
#define MIN_FLAME_DIST				2.f
#define MAX_FLAME_DIST				8.f
#define MIN_FLAME_HEIGHT			1.2f
#define MAX_FLAME_HEIGHT			2.6f


void CPoltergeist::LoadFlame(LPCSTR section)
{
	m_flame_sound.create		(pSettings->r_string(section,"flame_sound"), st_Effect,SOUND_TYPE_WORLD);
	
	m_flame_particles_prepare	= pSettings->r_string(section,"flame_particles_prepare");
	m_flame_particles_fire		= pSettings->r_string(section,"flame_particles_fire");
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
	element->time_started		= Device.dwTimeGlobal;
	element->sound.clone		(m_flame_sound, st_Effect,SOUND_TYPE_WORLD);
	element->sound.play_at_pos	(this,element->position);
	
	m_flames.push_back			(element);

	Fvector target_point = get_head_position(const_cast<CObject*>(target_object));
	element->target_dir.sub(target_point, element->position);
	element->target_dir.normalize();

	PlayParticles(m_flame_particles_prepare,element->position,element->target_dir,TRUE);
}


struct remove_predicate {
	bool	operator() (CPoltergeist::SFlameElement *element)	{
		
		if ((element->time_started == 0) && !element->sound._feedback()) {
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

		if (element->time_started + m_flame_fire_delay < time()) {
			//launch the second part particles
			
			Fvector target_point = get_head_position(const_cast<CObject*>(element->target_object));
			element->target_dir.sub(target_point, element->position);
			element->target_dir.normalize();
			
			PlayParticles(m_flame_particles_fire,element->position,element->target_dir,TRUE);

			// Test hit
			collide::rq_result rq;

			if (Level().ObjectSpace.RayPick(element->position, element->target_dir, m_flame_length, collide::rqtBoth, rq, NULL)) {
				if ((rq.O == element->target_object) && (rq.range < m_flame_length)) {

					float			hit_value;
					hit_value		= m_flame_hit_value - m_flame_hit_value * rq.range / m_flame_length;

					NET_Packet		P;
					SHit			HS;
					HS.GenHeader		(GE_HIT, element->target_object->ID());	//					u_EventGen		(P,GE_HIT, element->target_object->ID());
					HS.whoID			= (ID());								//					P.w_u16			(ID());
					HS.weaponID			= (ID());								//					P.w_u16			(ID());
					HS.dir				= (element->target_dir);				//					P.w_dir			(element->target_dir);
					HS.power			= (hit_value);							//					P.w_float		(m_flame_hit_value);
					HS.boneID			= (BI_NONE);							//					P.w_s16			(BI_NONE);
					HS.p_in_bone_space	= (Fvector().set(0.f,0.f,0.f));			//					P.w_vec3		(Fvector().set(0.f,0.f,0.f));
					HS.impulse			= (0.f);								//					P.w_float		(0.f);
					HS.hit_type			= (ALife::eHitTypeBurn);				//					P.w_u16			(u16(ALife::eHitTypeBurn));

					HS.Write_Packet	(P);
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

		vertex_position = ai().level_graph().vertex_position(Obj->ai_location().level_vertex_id());
		new_pos.mad(vertex_position, dir, Random.randF(MIN_FLAME_DIST, MAX_FLAME_DIST));

		u32 node = ai().level_graph().check_position_in_direction(Obj->ai_location().level_vertex_id(), vertex_position, new_pos);
		if (node != u32(-1)) {
			res_pos = ai().level_graph().vertex_position(node);
			res_pos.y += Random.randF(MIN_FLAME_HEIGHT, MAX_FLAME_HEIGHT);
			return (true);
		}
	}


	float angle = ai().level_graph().vertex_cover_angle(Obj->ai_location().level_vertex_id(),PI_DIV_6,std::less<float>());

	dir.set(1.f,0.f,0.f);
	dir.setHP(angle + PI, 0.f);
	dir.normalize();

	vertex_position = ai().level_graph().vertex_position(Obj->ai_location().level_vertex_id());
	new_pos.mad(vertex_position, dir, Random.randF(MIN_FLAME_DIST, MAX_FLAME_DIST));

	u32 node = ai().level_graph().check_position_in_direction(Obj->ai_location().level_vertex_id(), vertex_position, new_pos);
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
		if ((*I)->sound._feedback()) (*I)->sound.stop();
		xr_delete((*I));
	}
	
	m_flames.clear();
}


