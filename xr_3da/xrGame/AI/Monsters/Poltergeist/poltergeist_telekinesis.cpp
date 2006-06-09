#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShellHolder.h"
#include "../../../level.h"
#include "../../../actor.h"

void CPoltergeist::initailize_telekinesis() 
{
	m_tele_state			= eWait;
	m_tele_time				= 0;
	m_tele_time_next		= 0;
}

void CPoltergeist::update_telekinesis()
{
	if (!g_Alive() || !Actor() || !Actor()->g_Alive()) return;
	if (Actor()->Position().distance_to(Position()) > m_pmt_tele_distance) return;

	switch (m_tele_state) {
	case eStartRaiseObjects:	
		if (m_tele_time + m_tele_time_next < time()) {
			if (!tele_raise_objects())
				m_tele_state	= eRaisingObjects;
			
			m_tele_time			= time();
			m_tele_time_next	= m_pmt_tele_raise_time_to_wait_in_objects / 2 + Random.randI(m_pmt_tele_raise_time_to_wait_in_objects / 2);
		}

		if (m_tele_state == eStartRaiseObjects) {
			if (CTelekinesis::get_objects_count() >= m_pmt_tele_object_count) {
				m_tele_state	= eRaisingObjects;
				m_tele_time		= time();
			}
		}

		break;
	case eRaisingObjects:
		if (m_tele_time + m_pmt_tele_time_to_hold > time()) break;
		
		m_tele_time				= time();
		m_tele_time_next		= 0;
		m_tele_state			= eFireObjects;
	case eFireObjects:			
		if (m_tele_time + m_tele_time_next < time()) {
			tele_fire_objects	();
			
			m_tele_time			= time();
			m_tele_time_next	= m_pmt_tele_time_to_wait_in_objects / 2 + Random.randI(m_pmt_tele_time_to_wait_in_objects / 2);
		}
		
		if (CTelekinesis::get_objects_count() == 0) {
			m_tele_state		= eWait;
			m_tele_time			= time();
		}
		break;
	case eWait:				
		if (m_tele_time + m_pmt_tele_time_to_wait < time()) {
			m_tele_time_next	= 0;
			m_tele_state		= eStartRaiseObjects;
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
// Выбор подходящих объектов для телекинеза
//////////////////////////////////////////////////////////////////////////
class best_object_predicate {
	Fvector enemy_pos;
	Fvector monster_pos;
public:
	best_object_predicate(const Fvector &m_pos, const Fvector &pos) {
		monster_pos = m_pos;
		enemy_pos	= pos;
	}

	bool operator()	 (const CGameObject *tpObject1, const CGameObject *tpObject2) const
	{

		float dist1 = monster_pos.distance_to(tpObject1->Position());
		float dist2 = enemy_pos.distance_to(tpObject2->Position());
		float dist3 = enemy_pos.distance_to(monster_pos);

		return ((dist1 < dist3) && (dist2 > dist3));
	};
};

class best_object_predicate2 {
	Fvector enemy_pos;
	Fvector monster_pos;
public:
	best_object_predicate2(const Fvector &m_pos, const Fvector &pos) {
		monster_pos = m_pos;
		enemy_pos	= pos;
	}

	bool operator()	 (const CObject *tpObject1, const CObject *tpObject2) const
	{
		float dist1 = enemy_pos.distance_to(tpObject1->Position());
		float dist2 = enemy_pos.distance_to(tpObject2->Position());

		return (dist1 < dist2);		
	};
};

//////////////////////////////////////////////////////////////////////////


void CPoltergeist::tele_find_objects(xr_vector<CObject*> &objects, const Fvector &pos) 
{
	xr_vector<CObject*> tp_objects;
	Level().ObjectSpace.GetNearest	(tp_objects, pos, m_pmt_tele_radius, NULL);

	for (u32 i=0;i<tp_objects.size();i++) {
		CPhysicsShellHolder *obj			= smart_cast<CPhysicsShellHolder *>(tp_objects[i]);
		CCustomMonster		*custom_monster	= smart_cast<CCustomMonster *>(tp_objects[i]);
		if (!obj || 
			!obj->PPhysicsShell() || 
			!obj->PPhysicsShell()->isActive()|| 
			custom_monster ||
			(obj->spawn_ini() && obj->spawn_ini()->section_exist("ph_heavy")) || 
			(obj->m_pPhysicsShell->getMass() < m_pmt_tele_object_min_mass) || 
			(obj->m_pPhysicsShell->getMass() > m_pmt_tele_object_max_mass) || 
			(obj == this) || 
			CTelekinesis::is_active_object(obj) || 
			!obj->m_pPhysicsShell->get_ApplyByGravity()) continue;

		objects.push_back(obj);
	}
}


bool CPoltergeist::tele_raise_objects()
{
	// find objects near actor
	xr_vector<CObject*>		tele_objects;
	tele_objects.reserve	(20);

	// получить список объектов вокруг врага
	tele_find_objects	(tele_objects, Actor()->Position());

	// получить список объектов вокруг монстра
	tele_find_objects	(tele_objects, Position());

	// получить список объектов между монстром и врагом
	float dist			= Actor()->Position().distance_to(Position());
	Fvector dir;
	dir.sub				(Actor()->Position(), Position());
	dir.normalize		();

	Fvector pos;
	pos.mad				(Position(), dir, dist / 2.f);
	tele_find_objects	(tele_objects, pos);	

	// сортировать и оставить только необходимое количество объектов
	sort(tele_objects.begin(),tele_objects.end(),best_object_predicate2(Position(), Actor()->Position()));
	
	// оставить уникальные объекты
	xr_vector<CObject*>::iterator I = unique(tele_objects.begin(),tele_objects.end());
	tele_objects.erase(I,tele_objects.end());

	// оставить необходимое количество объектов
	//if (tele_objects.size() > m_pmt_tele_object_count) tele_objects.resize	(m_pmt_tele_object_count);

	//// активировать
	//for (u32 i=0; i<tele_objects.size(); i++) {
	//	CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tele_objects[i]);

	//	// применить телекинез на объект
	//	bool	rotate = false;

	//	CTelekinesis::activate		(obj, m_pmt_tele_raise_speed, m_pmt_tele_object_height, m_pmt_tele_time_object_keep, rotate);
	//}

	if (!tele_objects.empty()) {
		CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tele_objects[0]);

		// применить телекинез на объект
		bool	rotate = false;

		CTelekinesis::activate		(obj, m_pmt_tele_raise_speed, m_pmt_tele_object_height, m_pmt_tele_time_object_keep, rotate);
		return true;
	}

	return false;
}

void CPoltergeist::tele_fire_objects()
{
	for (u32 i=0; i < CTelekinesis::get_objects_total_count();i++) {
		CTelekineticObject tele_object = CTelekinesis::get_object_by_index(i);
		//if (tele_object.get_state() != TS_Fire) {
		if ((tele_object.get_state() == TS_Raise) || (tele_object.get_state() == TS_Keep))  {
			Fvector					enemy_pos;
			enemy_pos				= get_head_position(Actor());
			CTelekinesis::fire_t	(tele_object.get_object(),enemy_pos, tele_object.get_object()->Position().distance_to(enemy_pos) / m_tele_fly_velocity);
			return;
		}
	}
}



