#include "stdafx.h"
#include "pseudo_gigant.h"
#include "../../../CharacterPhysicsSupport.h"
#include "../../../phmovementcontrol.h"
#include "../../../xrserver_objects_alife_monsters.h"


CMonsterPseudoGigant::CMonsterPseudoGigant() 
{
	Init							();

	m_pPhysics_support				= xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);

}

CMonsterPseudoGigant::~CMonsterPseudoGigant()
{
	xr_delete						(m_pPhysics_support);
}


void CMonsterPseudoGigant::Init()
{
	inherited::Init();

}

void CMonsterPseudoGigant::reinit()
{
	inherited::reinit();
	m_pPhysics_support->in_NetSpawn	();

	cur_anim	= 0;
}

void CMonsterPseudoGigant::Load(LPCSTR section)
{
	inherited::Load(section);

	m_pPhysics_support->in_Load		(section);
}

void CMonsterPseudoGigant::reload(LPCSTR	section)
{
	inherited::reload(section);
}


BOOL CMonsterPseudoGigant::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterAbstract		*tpMonster = dynamic_cast<CSE_ALifeMonsterAbstract*>(e);
	R_ASSERT						(tpMonster);

	m_PhysicMovementControl->SetPosition	(Position());
	m_PhysicMovementControl->SetVelocity	(0,0,0);


	return TRUE;
}

void CMonsterPseudoGigant::UpdateCL()
{
	inherited::UpdateCL();

	m_pPhysics_support->in_UpdateCL();
}

void CMonsterPseudoGigant::net_Destroy()
{
	inherited::net_Destroy	();
	m_pPhysics_support->in_NetDestroy();
}

void CMonsterPseudoGigant::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	m_pPhysics_support->in_shedule_Update(dt);
}

void CMonsterPseudoGigant::Think()
{

}

void CMonsterPseudoGigant::Die()
{
	inherited::Die();
}


static void __stdcall AnimEndCallback(CBlend* B)
{
	CMonsterPseudoGigant *pC = (CMonsterPseudoGigant*)B->CallbackParam;
	pC->cur_anim = 0;
}


void CMonsterPseudoGigant::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	if (cur_anim == 0) {
		cur_anim = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_idle_0");
		PSkeletonAnimated(Visual())->PlayCycle(cur_anim,TRUE,AnimEndCallback,this);
	}
}

BOOL CMonsterPseudoGigant::feel_vision_isRelevant(CObject* O)
{
	if (CLSID_ENTITY!=O->CLS_ID) return FALSE;

	CEntityAlive* E = dynamic_cast<CEntityAlive*> (O);
	if (!E) return FALSE;
	if (E->g_Team() == g_Team() && E->g_Alive()) return FALSE;

	return TRUE;
}



void CMonsterPseudoGigant::net_Export(NET_Packet& P) 
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16			(fEntityHealth,-1000,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);

	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1 = 0;
	if (ai().game_graph().valid_vertex_id(l_game_vertex_id)) {
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
	}
	else {
		P.w					(&f1,						sizeof(f1));
		P.w					(&f1,						sizeof(f1));
	}
}

void CMonsterPseudoGigant::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;

	float health;
	P.r_float_q16		(health,-1000,1000);
	fEntityHealth = health;

	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);

	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
	P.r						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.r						(&l_game_vertex_id,			sizeof(l_game_vertex_id));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	P.r						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.r						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1 = 0;
	if (ai().game_graph().valid_vertex_id(l_game_vertex_id)) {
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.r					(&f1,						sizeof(f1));
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.r					(&f1,						sizeof(f1));
	}
	else {
		P.r					(&f1,						sizeof(f1));
		P.r					(&f1,						sizeof(f1));
	}

}
