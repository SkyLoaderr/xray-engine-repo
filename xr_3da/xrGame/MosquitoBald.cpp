#include "stdafx.h"
#include "mosquitobald.h"
#include "hudmanager.h"
#include "ParticlesObject.h"

static f32 g_pp_fade = 2000.f;

CMosquitoBald::CMosquitoBald(void) 
{
	m_dwDeltaTime = 0;
	m_pp_time = 0;
	m_fHitImpulseScale = 1.f;
}

CMosquitoBald::~CMosquitoBald(void) 
{
}

void CMosquitoBald::Load(LPCSTR section) 
{
	inherited::Load(section);

	LPCSTR l_PP = pSettings->r_string(section,"postprocess");
	m_pp.duality_h = pSettings->r_float(l_PP,"duality_h");
	m_pp.duality_v = pSettings->r_float(l_PP,"duality_v");
	m_pp.gray = pSettings->r_float(l_PP,"gray");
	m_pp.blur = pSettings->r_float(l_PP,"blur");
	m_pp.gray = pSettings->r_float(l_PP,"gray");
	m_pp.noise = pSettings->r_float(l_PP,"noise");
	m_pp.noise_scale = pSettings->r_float(l_PP,"noise_scale");
	sscanf(pSettings->r_string(l_PP,"noise_color"), "%d,%d,%d,%d", &m_pp.r, &m_pp.g, &m_pp.b, &m_pp.a);

	m_pHitEffect = pSettings->r_string(section,"hit_effect");
}


//void CMosquitoBald::Update(u32 dt) {
void CMosquitoBald::UpdateCL() 
{
	u32 dt = Device.dwTimeDelta;
	m_dwDeltaTime += dt;

	if(m_dwDeltaTime > m_dwPeriod) 
	{
		m_dwDeltaTime = m_dwPeriod;
		//while(m_dwDeltaTime > m_dwPeriod) m_dwDeltaTime -= m_dwPeriod;
		m_bZoneReady = true;
	}
	
	if(m_pLocalActor && m_pLocalActor->g_Alive()) 
	{
		IRender_Target*		T	= ::Render->getTarget();
		f32 l_h = m_pp_time < g_pp_fade ? m_pp.duality_h * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_h;
		f32 l_v = m_pp_time < g_pp_fade ? m_pp.duality_v * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_v;
		T->set_duality_h		(l_h*_sin(1.f*Device.fTimeGlobal));
		T->set_duality_v		(l_v*_cos(1.1f*Device.fTimeGlobal));
		//T->set_duality_h		(.0f);
		//T->set_duality_v		(.0f);
		T->set_blur				(m_pp_time < g_pp_fade ? m_pp.blur * ((f32)m_pp_time / g_pp_fade) : m_pp.blur);
		//T->set_blur				(1.f);
		T->set_gray				(m_pp_time < g_pp_fade ? m_pp.gray * ((f32)m_pp_time / g_pp_fade) : m_pp.gray);
		T->set_noise			(m_pp_time < g_pp_fade ? m_pp.noise * ((f32)m_pp_time / g_pp_fade) : m_pp.noise);
		//T->set_noise			(m_pp.noise);
		T->set_noise_scale		(m_pp.noise_scale);
		T->set_noise_color		(color_rgba(m_pp.r,m_pp.g,m_pp.b,m_pp.a));
		T->set_noise_fps		(10.f);
		m_pp_time += dt;
	} 
	else if(m_pp_time>0) 
	{
		if(m_pp_time > g_pp_fade) m_pp_time = (u32)g_pp_fade;
		m_pp_time = m_pp_time < dt ? 0 : m_pp_time - dt;
		IRender_Target*		T	= ::Render->getTarget();
		f32 l_h = m_pp_time < g_pp_fade ? m_pp.duality_h * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_h;
		f32 l_v = m_pp_time < g_pp_fade ? m_pp.duality_v * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_v;
		T->set_duality_h		(l_h*_sin(1.f*Device.fTimeGlobal));
		T->set_duality_v		(l_v*_cos(1.1f*Device.fTimeGlobal));
		T->set_blur				(m_pp_time < g_pp_fade ? m_pp.blur * ((f32)m_pp_time / g_pp_fade) : m_pp.blur);
		T->set_gray				(m_pp_time < g_pp_fade ? m_pp.gray * ((f32)m_pp_time / g_pp_fade) : m_pp.gray);
		T->set_noise			(m_pp_time < g_pp_fade ? m_pp.noise * ((f32)m_pp_time / g_pp_fade) : m_pp.noise);
		//T->set_noise			(m_pp.noise);
		T->set_noise_scale		(m_pp.noise_scale);
		T->set_noise_color		(color_rgba(m_pp.r,m_pp.g,m_pp.b,m_pp.a));
		T->set_noise_fps		(10.f);
	}
	inherited::UpdateCL();
	//inherited::Update(dt);
}

void CMosquitoBald::Postprocess(f32 /**val/**/) 
{
}
