#include "stdafx.h"
#include "mincer.h"
#include "hudmanager.h"

static
f32 g_pp_fade = 2000.f;

CMincer::CMincer(void) 
{
	m_dwDeltaTime = 0;
	m_pp_time = 0;
	//m_fHitImpulseScale = 1.f;
}

CMincer::~CMincer(void) 
{
}

void CMincer::Load(LPCSTR section) 
{
	inherited::Load(section);

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
	m_fPowerMin = m_fMaxPower*0.9f;
	m_bDischarging = false;
	m_bZoneReady = false;
	m_fSpecificDischarge = m_fMaxPower*0.01f;


	return bOk;
}

float CMincer::Power(float dist, float)// mass)
{
	float radius = CFORM()->getRadius()*3/4.f;
	//f32 l_pow = l_r < dist ? 0 : m_fCurrentPower * (1.f - m_fAttenuation*(dist/l_r)*(dist/l_r));
	
	
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
	CEntityAlive *pObject = dynamic_cast<CEntityAlive*>(O);

	//�������� ������ �� ����� ��������
	if(pObject /*&& l_pO->g_Alive()*/) 
	{
		//���� ���������� ����
		m_bDischarging = true;

		Fvector position; 
		XFORM().transform_tiny(position,CFORM()->getSphere().P);
		
		char pow[255]; 
		sprintf(pow, "zone hit. %.1f", Power(pObject->Position().distance_to(position),
											 pObject->GetMass()));
		if(bDebug) HUD().outMessage(0xffffffff,*pObject->cName(), pow);
		
		Fvector dir; 
		dir.set(::Random.randF(-.5f,.5f), 
				  ::Random.randF(-.5f,.2f), 
				  ::Random.randF(-.5f,.5f)); 
		dir.normalize();
		
		//l_pO->m_PhysicMovementControl.ApplyImpulse(l_dir, 50.f*Power(l_pO->Position().distance_to(P)));
		float power = Power(pObject->Position().distance_to(position), pObject->GetMass());
		float impulse = power*pObject->GetMass()/**m_fHitImpulseScale*/;
		//float impulse = Impulse(power, l_pO->GetMass());

		
		Fvector position_in_bone_space;
		
		if(power > 0.01f) 
		{
			position_in_bone_space.set(0.f,0.f,0.f);
			NET_Packet	P;
			pObject->u_EventGen		(P,GE_HIT,pObject->ID());
			P.w_u16					(u16(pObject->ID()));
			P.w_dir					(dir);
			P.w_float				(power);
			P.w_s16					((s16)0);
			P.w_vec3				(position_in_bone_space);
			P.w_float				(impulse);
			P.w_u16					(eHitTypeWound);
			pObject->u_EventSend	(P);
		}
	}
}

//void CMincer::Update(u32 dt) {
void CMincer::UpdateCL() 
{
	u32 dt = Device.dwTimeDelta;
	m_dwDeltaTime += dt;
	
	if(m_dwDeltaTime > m_dwPeriod) 
	{
		while(m_dwDeltaTime > m_dwPeriod) m_dwDeltaTime -= m_dwPeriod;
//		m_bZoneReady = true;
	}

	if(m_fCurrentPower<m_fMaxPower && !m_bDischarging)
	{
		m_fCurrentPower += dt*(m_fMaxPower/m_dwPeriod);
		if(m_fCurrentPower>m_fMaxPower) m_fCurrentPower = m_fMaxPower;
	}

	//���� ����������� ���������
	if(m_fCurrentPower<=0)
	{
		m_fCurrentPower = 0;
		m_bDischarging = false;
		m_bZoneReady = false;
	}
	
	//������ �� ���� � �������
	if(m_fCurrentPower<m_fPowerMin && m_bDischarging==false)
		m_bZoneReady = false;
	else
		m_bZoneReady = true;

	
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
		T->set_color_base		(color_rgba(m_pp.r,m_pp.g,m_pp.b,m_pp.a));
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
		T->set_color_base		(color_rgba(m_pp.r,m_pp.g,m_pp.b,m_pp.a));
		T->set_noise_fps		(10.f);
	}
	//inherited::Update(dt);
	inherited::UpdateCL();
}

void CMincer::Postprocess(f32)// val) 
{
}
