#include "stdafx.h"
#include "mincer.h"
#include "hudmanager.h"

static
f32 g_pp_fade = 2000.f;

CMincer::CMincer(void) 
{
	m_time = 0;
	m_pp_time = 0;
	//m_hitImpulseScale = 1.f;
}

CMincer::~CMincer(void) 
{
}

void CMincer::Load(LPCSTR section) 
{
	inherited::Load(section);
	m_hitImpulseScale = pSettings->r_float(section,"hit_impulse_scale");

	LPCSTR l_PP = pSettings->r_string(section,"postprocess");
	m_pp.duality_h = pSettings->r_float(l_PP,"duality_h");
	m_pp.duality_v = pSettings->r_float(l_PP,"duality_v");
	m_pp.blur = pSettings->r_float(l_PP,"blur");
	m_pp.gray = pSettings->r_float(l_PP,"gray");
	m_pp.noise = pSettings->r_float(l_PP,"noise");
	m_pp.noise_scale = pSettings->r_float(l_PP,"noise_scale");
	sscanf(pSettings->r_string(l_PP,"noise_color"), "%d,%d,%d,%d", &m_pp.r, &m_pp.g, &m_pp.b, &m_pp.a);

}

BOOL CMincer::net_Spawn(LPVOID DC)
{
	BOOL bOk = inherited::net_Spawn(DC);

	m_fCurrentPower = 0;
	m_fPowerMin = m_maxPower*0.9f;
	m_bDischarging = false;
	m_ready = false;
	m_fSpecificDischarge = m_maxPower*0.01f;


	return bOk;
}

float CMincer::Power(float dist, float mass)
{
	float radius = CFORM()->getRadius()*3/4.f;
	//f32 l_pow = l_r < dist ? 0 : m_fCurrentPower * (1.f - m_attn*(dist/l_r)*(dist/l_r));
	
	
	float power;
	if(dist < radius)
	{
		if(m_fCurrentPower>m_fSpecificDischarge)
			power = m_fSpecificDischarge;
		else
			power = m_fCurrentPower;

		m_fCurrentPower -= power;
	}
	else
		power = 0;
	
	
	return power < 0 ? 0 : power;
}

float CMincer::Impulse(float power, float mass)
{
	float impulse = power*mass;
	return impulse;
}
void CMincer::Affect(CObject* O) 
{
	//CGameObject *l_pO = dynamic_cast<CGameObject*>(O);
	CEntityAlive *l_pO = dynamic_cast<CEntityAlive*>(O);

	//�������� ������ �� ����� ��������
	if(l_pO /*&& l_pO->g_Alive()*/) 
	{
		//���� ���������� ����
		m_bDischarging = true;

		Fvector P; 
		XFORM().transform_tiny(P,CFORM()->getSphere().P);
		
		char l_pow[255]; 
		sprintf(l_pow, "zone hit. %.1f", Power(l_pO->Position().distance_to(P),
							l_pO->GetMass()));
		if(bDebug) HUD().outMessage(0xffffffff,l_pO->cName(), l_pow);
		Fvector l_dir; 
		l_dir.set(::Random.randF(-.5f,.5f), 
				  ::Random.randF(-.5f,.2f), 
				  ::Random.randF(-.5f,.5f)); 
		l_dir.normalize();
		//l_pO->ph_Movement.ApplyImpulse(l_dir, 50.f*Power(l_pO->Position().distance_to(P)));
		float power = Power(l_pO->Position().distance_to(P), l_pO->GetMass());
		float impulse = power*l_pO->GetMass()/**m_hitImpulseScale*/;
		//float impulse = Impulse(power, l_pO->GetMass());

		
		Fvector position_in_bone_space;
		
		if(power > 0.01f) 
		{
			position_in_bone_space.set(0.f,0.f,0.f);
			NET_Packet		l_P;
			l_pO->u_EventGen	(l_P,GE_HIT,l_pO->ID());
			l_P.w_u16			(u16(l_pO->ID()));
			l_P.w_dir			(l_dir);
			l_P.w_float			(power);
			l_P.w_s16			((s16)0);
			l_P.w_vec3			(position_in_bone_space);
			l_P.w_float			(impulse);
			l_P.w_u16			(eHitTypeWound);
			l_pO->u_EventSend	(l_P);
		}
	}
}

//void CMincer::Update(u32 dt) {
void CMincer::UpdateCL() 
{
	u32 dt = Device.dwTimeDelta;
	m_time += dt;
	
	if(m_time > m_period) 
	{
		while(m_time > m_period) m_time -= m_period;
//		m_ready = true;
	}

	if(m_fCurrentPower<m_maxPower && !m_bDischarging)
	{
		m_fCurrentPower += dt*(m_maxPower/m_period);
		if(m_fCurrentPower>m_maxPower) m_fCurrentPower = m_maxPower;
	}

	//���� ����������� ���������
	if(m_fCurrentPower<=0)
	{
		m_fCurrentPower = 0;
		m_bDischarging = false;
		m_ready = false;
	}
	
	//������ �� ���� � �������
	if(m_fCurrentPower<m_fPowerMin && m_bDischarging==false)
		m_ready = false;
	else
		m_ready = true;

	
	if(m_inZone.empty())
		m_bDischarging = false;



	
	if(m_pLocalActor && m_pLocalActor->g_Alive()) 
	{
		IRender_Target*		T	= ::Render->getTarget();
		f32 l_h = m_pp_time < g_pp_fade ? m_pp.duality_h * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_h;
		f32 l_v = m_pp_time < g_pp_fade ? m_pp.duality_v * ((f32)m_pp_time / g_pp_fade) : m_pp.duality_v;
		T->set_duality_h		(l_h*_sin(1.f*Device.fTimeGlobal));
		T->set_duality_v		(l_v*_cos(1.1f*Device.fTimeGlobal));
		//T->set_duality_h		(.0f);
		//T->set_duality_v		(.0f);
		T->set_blur				(m_pp_time < g_pp_fade ? m_pp.blur * ((f32)m_pp_time / g_pp_fade) : m_pp.noise);
		T->set_gray				(m_pp_time < g_pp_fade ? m_pp.gray * ((f32)m_pp_time / g_pp_fade) : m_pp.gray);
		T->set_noise			(m_pp_time < g_pp_fade ? m_pp.noise * ((f32)m_pp_time / g_pp_fade) : m_pp.noise);
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
		T->set_noise_scale		(m_pp.noise_scale);
		T->set_noise_color		(color_rgba(m_pp.r,m_pp.g,m_pp.b,m_pp.a));
		T->set_noise_fps		(10.f);
	}
	//inherited::Update(dt);
	inherited::UpdateCL();
}

void CMincer::Postprocess(f32 val) 
{
}
