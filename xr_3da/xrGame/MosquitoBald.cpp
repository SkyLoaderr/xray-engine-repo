#include "stdafx.h"
#include "mosquitobald.h"
#include "hudmanager.h"

CMosquitoBald::CMosquitoBald(void) {
	m_time = 0;
	m_hitImpulseScale = 1.f;
}

CMosquitoBald::~CMosquitoBald(void) {}

void CMosquitoBald::Load(LPCSTR section) {
	inherited::Load(section);
	m_hitImpulseScale = pSettings->ReadFLOAT(section,"hit_impulse_scale");
}

void CMosquitoBald::Affect(CObject* O) {
	CGameObject *l_pO = dynamic_cast<CGameObject*>(O);
	if(l_pO) {
		Fvector P; clXFORM().transform_tiny(P,cfModel->getSphere().P);
		char l_pow[255]; sprintf(l_pow, "zone hit. %.1f", Power(l_pO->Position().distance_to(P)));
		Level().HUD()->outMessage(0xffffffff,l_pO->cName(), l_pow);
		Fvector l_dir; l_dir.set(::Random.randF(-.5f,.5f), ::Random.randF(.0f,1.f), ::Random.randF(-.5f,.5f)); l_dir.normalize();
		//l_pO->ph_Movement.ApplyImpulse(l_dir, 50.f*Power(l_pO->Position().distance_to(P)));
		Fvector position_in_bone_space;
		float power = Power(l_pO->Position().distance_to(P)), impulse = m_hitImpulseScale*power;
		position_in_bone_space.set(0.f,0.f,0.f);
		NET_Packet		l_P;
		l_pO->u_EventGen		(l_P,GE_HIT,l_pO->ID());
		l_P.w_u16			(u16(l_pO->ID()));
		l_P.w_dir			(l_dir);
		l_P.w_float			(power);
		l_P.w_s16			((s16)0);
		l_P.w_vec3			(position_in_bone_space);
		l_P.w_float			(impulse);
		l_pO->u_EventSend		(l_P);
	}
}

void CMosquitoBald::Update(u32 dt) {
	m_time += dt;
	if(m_time > m_period) {
		while(m_time > m_period) m_time -= m_period;
		m_ready = true;
	}
	inherited::Update(dt);
}
