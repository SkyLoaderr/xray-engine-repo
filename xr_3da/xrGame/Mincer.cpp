#include "stdafx.h"
#include "mincer.h"
#include "hudmanager.h"

static
f32 g_pp_fade = 5000.f;

CMincer::CMincer(void) {
	m_time = 0;
	m_pp_time = 0;
	//m_hitImpulseScale = 1.f;
}

CMincer::~CMincer(void) {}

void CMincer::Load(LPCSTR section) {
	inherited::Load(section);
	//m_hitImpulseScale = pSettings->ReadFLOAT(section,"hit_impulse_scale");

	LPCSTR l_PP = pSettings->ReadSTRING(section,"postprocess");
	m_pp.duality_h = pSettings->ReadFLOAT(l_PP,"duality_h");
	m_pp.duality_v = pSettings->ReadFLOAT(l_PP,"duality_v");
	m_pp.blur = pSettings->ReadFLOAT(l_PP,"blur");
	m_pp.gray = pSettings->ReadFLOAT(l_PP,"gray");
	m_pp.noise = pSettings->ReadFLOAT(l_PP,"noise");
	m_pp.noise_scale = pSettings->ReadFLOAT(l_PP,"noise_scale");
	sscanf(pSettings->ReadSTRING(l_PP,"noise_color"), "%d,%d,%d,%d", &m_pp.r, &m_pp.g, &m_pp.b, &m_pp.a);
}

void CMincer::Affect(CObject* O) {
	CGameObject *l_pO = dynamic_cast<CGameObject*>(O);
	if(l_pO) {
		Fvector P; clXFORM().transform_tiny(P,cfModel->getSphere().P);
		char l_pow[255]; sprintf(l_pow, "zone hit. %.1f", Power(l_pO->Position().distance_to(P)));
		if(bDebug) Level().HUD()->outMessage(0xffffffff,l_pO->cName(), l_pow);
		Fvector l_dir; l_dir.set(::Random.randF(-.5f,.5f), ::Random.randF(.0f,1.f), ::Random.randF(-.5f,.5f)); l_dir.normalize();
		//l_pO->ph_Movement.ApplyImpulse(l_dir, 50.f*Power(l_pO->Position().distance_to(P)));
		Fvector position_in_bone_space;
		float power = Power(l_pO->Position().distance_to(P)), impulse = 0;
		if(power > 0.01f) {
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
}

//void CMincer::Update(u32 dt) {
void CMincer::UpdateCL() {
	u32 dt = Device.dwTimeDelta;
	m_time += dt;
	if(m_time > m_period) {
		while(m_time > m_period) m_time -= m_period;
		m_ready = true;
	}
	if(m_pLocalActor && m_pLocalActor->g_Alive()) {
		CRender_target*		T	= ::Render->getTarget();
		f32 l_h = m_pp_time < g_pp_fade ? m_pp.duality_h * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_h;
		f32 l_v = m_pp_time < g_pp_fade ? m_pp.duality_v * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_v;
		T->set_duality_h		(l_h*sinf(1.f*Device.fTimeGlobal));
		T->set_duality_v		(l_v*cosf(1.1f*Device.fTimeGlobal));
		//T->set_duality_h		(.0f);
		//T->set_duality_v		(.0f);
		T->set_blur				(m_pp_time < g_pp_fade ? m_pp.blur * ((f32)m_pp_time / g_pp_fade) : m_pp.noise);
		T->set_noise			(m_pp_time < g_pp_fade ? m_pp.noise * ((f32)m_pp_time / g_pp_fade) : m_pp.noise);
		T->set_noise_scale		(m_pp.noise_scale);
		T->set_noise_color		(color_rgba(m_pp.r,m_pp.g,m_pp.b,m_pp.a));
		T->set_noise_fps		(10.f);
		m_pp_time += dt;
	} else if(m_pp_time>0) {
		if(m_pp_time > g_pp_fade) m_pp_time = (u32)g_pp_fade;
		m_pp_time = m_pp_time < dt ? 0 : m_pp_time - dt;
		CRender_target*		T	= ::Render->getTarget();
		f32 l_h = m_pp_time < g_pp_fade ? m_pp.duality_h * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_h;
		f32 l_v = m_pp_time < g_pp_fade ? m_pp.duality_v * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_v;
		T->set_duality_h		(l_h*sinf(1.f*Device.fTimeGlobal));
		T->set_duality_v		(l_v*cosf(1.1f*Device.fTimeGlobal));
		T->set_blur				(m_pp_time < g_pp_fade ? m_pp.blur * ((f32)m_pp_time / g_pp_fade) : m_pp.blur);
		T->set_noise			(m_pp_time < g_pp_fade ? m_pp.noise * ((f32)m_pp_time / g_pp_fade) : m_pp.noise);
		T->set_noise_scale		(m_pp.noise_scale);
		T->set_noise_color		(color_rgba(m_pp.r,m_pp.g,m_pp.b,m_pp.a));
		T->set_noise_fps		(10.f);
	}
	//inherited::Update(dt);
	inherited::UpdateCL();
}

void CMincer::Postprocess(f32 val) {
}
