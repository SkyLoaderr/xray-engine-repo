#include "stdafx.h"
#include "mincer.h"
#include "hudmanager.h"
#include "xrmessages.h"
#include "level.h"

CMincer::CMincer(void) 
{
	m_dwDeltaTime = 0;
	//m_fHitImpulseScale = 1.f;
}

CMincer::~CMincer(void) 
{
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
	//CGameObject *l_pO = smart_cast<CGameObject*>(O);
	CEntityAlive *pObject = smart_cast<CEntityAlive*>(O);

	//разрядка только на живые существа
	if(pObject /*&& l_pO->g_Alive()*/) 
	{
		//флаг активности зоны
		m_bDischarging = true;

		Fvector position; 
		XFORM().transform_tiny(position,CFORM()->getSphere().P);
		
		char pow[255]; 
		sprintf(pow, "zone hit. %.1f", Power(pObject->Position().distance_to(position),
											 pObject->GetMass()));
		if(bDebug) HUD().outMessage(0xffffffff,pObject->cName(), pow);
		
		Fvector dir; 
		dir.set(::Random.randF(-.5f,.5f), 
				  ::Random.randF(-.5f,.2f), 
				  ::Random.randF(-.5f,.5f)); 
		dir.normalize();
		
		//l_pO->m_PhysicMovementControl->ApplyImpulse(l_dir, 50.f*Power(l_pO->Position().distance_to(P)));
		float power = Power(pObject->Position().distance_to(position), pObject->GetMass());
		float impulse = power*pObject->GetMass()/**m_fHitImpulseScale*/;
		//float impulse = Impulse(power, l_pO->GetMass());

		
		Fvector position_in_bone_space;
		
		if(power > 0.01f) 
		{
			position_in_bone_space.set(0.f,0.f,0.f);
			if (OnServer())
			{
				NET_Packet	P;
				pObject->u_EventGen		(P,GE_HIT,pObject->ID());
				P.w_u16					(ID());
				P.w_u16					(ID());
				P.w_dir					(dir);
				P.w_float				(power);
				P.w_s16					((s16)0);
				P.w_vec3				(position_in_bone_space);
				P.w_float				(impulse);
				P.w_u16					(ALife::eHitTypeWound);
				pObject->u_EventSend	(P);
			};
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

	//зона разрядилась полностью
	if(m_fCurrentPower<=0)
	{
		m_fCurrentPower = 0;
		m_bDischarging = false;
		m_bZoneReady = false;
	}
	
	//готова ли зона к разряду
	if(m_fCurrentPower<m_fPowerMin && m_bDischarging==false)
		m_bZoneReady = false;
	else
		m_bZoneReady = true;

	
	if(m_inZone.empty())
		m_bDischarging = false;


	//inherited::Update(dt);
	inherited::UpdateCL();
}

void CMincer::Postprocess(f32)// val) 
{
}
