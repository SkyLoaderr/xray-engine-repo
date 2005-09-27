#include "stdafx.h"
#include "psy_dog.h"
#include "../../../level_navigation_graph.h"
#include "../../../ai_space.h"
#include "../../../alife_simulator.h"
#include "../../../xrServer_Object_Base.h"
#include "../../../xrserver.h"
#include "../../../ai_object_location.h"
#include "../../../level.h"
#include "../control_movement_base.h"
#include "../monster_velocity_space.h"
#include "../../../restricted_object.h"
#include "../../../actor.h"
#include "../ai_monster_effector.h"
#include "../../../ActorEffector.h"


CPsyDog::CPsyDog()
{
}
CPsyDog::~CPsyDog()
{
}

void CPsyDog::Load(LPCSTR section)
{
	inherited::Load(section);
}

BOOL CPsyDog::net_Spawn(CSE_Abstract *dc)
{
	if (!inherited::net_Spawn(dc)) return FALSE;

	return TRUE;
}
void CPsyDog::reinit()
{
	inherited::reinit();
}
void CPsyDog::reload(LPCSTR section)
{
	inherited::reload(section);
}

void CPsyDog::register_phantom(CPsyDogPhantom *phantom)
{
	m_storage.push_back(phantom);
}

bool CPsyDog::spawn_phantom()
{
	u32 node;
	if (!control().path_builder().get_node_in_radius(ai_location().level_vertex_id(), 4,8,5,node)) return false;
	
	Level().spawn_item("psy_dog_phantom", ai().level_graph().vertex_position(node), node, ID());
	return true;
}

void CPsyDog::delete_phantom(CPsyDogPhantom *phantom)
{
	xr_vector<CPsyDogPhantom*>::iterator it = find(m_storage.begin(),m_storage.end(), phantom);
	VERIFY(it != m_storage.end());
	m_storage.erase(it);

	phantom->mark_destroyed();

	if (ai().get_alife()) {
		CALifeSimulator *p_sim = const_cast<CALifeSimulator *>(ai().get_alife());
		CSE_Abstract* e_entity	= Level().Server->game->get_entity_from_eid	(phantom->ID()); 
		VERIFY(e_entity);
		p_sim->release(e_entity);
	} else {
		NET_Packet		P;
		u_EventGen		(P,GE_DESTROY,phantom->ID());
		u_EventSend		(P);
	}
}

void CPsyDog::Think()
{
	inherited::Think();
	if (!g_Alive()) return;
	
	static u32 time_last_change = 0;
	
	if ((m_storage.size() < 3) && (time_last_change + 5000 < time()))
		if (spawn_phantom()) time_last_change = time();
}

void CPsyDog::delete_all_phantoms()
{
	for (xr_vector<CPsyDogPhantom*>::iterator it = m_storage.begin(); it != m_storage.end(); it++) {
		CPsyDogPhantom *phantom = *it;
		
		phantom->mark_destroyed();

		if (ai().get_alife()) {
			CALifeSimulator *p_sim = const_cast<CALifeSimulator *>(ai().get_alife());
			CSE_Abstract* e_entity	= Level().Server->game->get_entity_from_eid	(phantom->ID()); 
			VERIFY(e_entity);
			p_sim->release(e_entity);
		} else {
			NET_Packet		P;
			u_EventGen		(P,GE_DESTROY,phantom->ID());
			u_EventSend		(P);
		}
	}
	
	m_storage.clear();

}

void CPsyDog::net_Destroy()
{
	delete_all_phantoms	();
	inherited::net_Destroy();
}

void CPsyDog::Die(CObject* who)
{
	delete_all_phantoms();
	inherited::Die(who);
}



//////////////////////////////////////////////////////////////////////////
// Phantom Psy Dog
//////////////////////////////////////////////////////////////////////////
CPsyDogPhantom::CPsyDogPhantom()
{
}
CPsyDogPhantom::~CPsyDogPhantom()
{
}
BOOL CPsyDogPhantom::net_Spawn(CSE_Abstract *dc)
{
	VERIFY(dc->ID_Parent != 0xffff);
		
	CObject	*O = Level().Objects.net_Find(dc->ID_Parent);
	VERIFY(O);

	CPsyDog *dog = smart_cast<CPsyDog *>(O);
	VERIFY(dog);
	
	dog->register_phantom(this);
	m_parent = dog;

	inherited::net_Spawn(dc);

	setVisible	(FALSE);
	setEnabled	(FALSE);

	m_state = eWaitToAppear;

	// load effector
	
	// Load psi postprocess --------------------------------------------------------
	

	LPCSTR ppi_section = pSettings->r_string(cNameSect(), "appear_effector");
	m_appear_effector.ppi.duality.h			= pSettings->r_float(ppi_section,"duality_h");
	m_appear_effector.ppi.duality.v			= pSettings->r_float(ppi_section,"duality_v");
	m_appear_effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	m_appear_effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	m_appear_effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	m_appear_effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	m_appear_effector.ppi.noise.fps			= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(m_appear_effector.ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_appear_effector.ppi.color_base.r,	&m_appear_effector.ppi.color_base.g,	&m_appear_effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_appear_effector.ppi.color_gray.r,	&m_appear_effector.ppi.color_gray.g,	&m_appear_effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_appear_effector.ppi.color_add.r,	&m_appear_effector.ppi.color_add.g,	&m_appear_effector.ppi.color_add.b);

	m_appear_effector.time				= pSettings->r_float(ppi_section,"time");
	m_appear_effector.time_attack		= pSettings->r_float(ppi_section,"time_attack");
	m_appear_effector.time_release		= pSettings->r_float(ppi_section,"time_release");

	m_appear_effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	m_appear_effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	m_appear_effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	m_appear_effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

	// --------------------------------------------------------------------------------



	return (TRUE);
}

void CPsyDogPhantom::Think()
{
	inherited::Think();
	if (m_state != eWaitToAppear) return;
	
	
	EnemyMan.transfer_enemy(m_parent);
	
	SVelocityParam &velocity_run = move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	if (control().path_builder().speed(movement_control()) < 2*velocity_run.velocity.linear/3) return;
	if (!EnemyMan.get_enemy()) return;
	if (!control().direction().is_face_target(EnemyMan.get_enemy(), PI_DIV_6)) return;

	Fvector target;
	target.mad(Position(),Direction(), 10.f);

	// нода в прямой видимости?
	control().path_builder().restrictions().add_border(Position(), target);
	u32 node = ai().level_graph().check_position_in_direction(ai_location().level_vertex_id(),Position(),target);
	control().path_builder().restrictions().remove_border();

	if (!ai().level_graph().valid_vertex_id(node) || !control().path_builder().accessible(node)) return;

	target.y += 1.f;
	com_man().jump	(target);
	m_state			= eAttack;
	
	setVisible		(TRUE);
	setEnabled		(TRUE);

	CParticlesPlayer::StartParticles("anomaly2\\bloodsucker_shield",Fvector().set(0.0f,0.1f,0.0f),ID());

	CActor *pA = const_cast<CActor *>(smart_cast<const CActor *>(EnemyMan.get_enemy()));
	if (!pA) return;

	pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(m_appear_effector.ce_time,m_appear_effector.ce_amplitude,m_appear_effector.ce_period_number,m_appear_effector.ce_power));
	Level().Cameras.AddEffector(xr_new<CMonsterEffector>(m_appear_effector.ppi, m_appear_effector.time, m_appear_effector.time_attack, m_appear_effector.time_release));
}

void CPsyDogPhantom::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
}

void CPsyDogPhantom::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	if (!getVisible()) return;
	
	if ((who == EnemyMan.get_enemy())  && (who != 0)) {
		m_parent->delete_phantom(this);
		setVisible(FALSE);	
	}
}
void CPsyDogPhantom::net_Destroy()
{
	if (m_parent) m_parent->delete_phantom(this);
	inherited::net_Destroy();
}

void CPsyDogPhantom::Die(CObject* who)
{
	m_parent->delete_phantom(this);
	inherited::Die(who);
}

