#include "stdafx.h"
#include "radioactivezone.h"
#include "hudmanager.h"
#include "level.h"
#include "xrmessages.h"

CRadioactiveZone::CRadioactiveZone(void) 
{
	m_dwDeltaTime = 0;
	m_fHitImpulseScale = 1.f;
}

CRadioactiveZone::~CRadioactiveZone(void) 
{
}

void CRadioactiveZone::Load(LPCSTR section) 
{
	inherited::Load(section);
	m_fHitImpulseScale = pSettings->r_float(section,"hit_impulse_scale");

	m_pHitEffect = pSettings->r_string(section,"hit_effect");
	m_fHitImpulseScale = 0.01f;
}

void CRadioactiveZone::Affect(CObject* O) 
{
	CGameObject *l_pO = dynamic_cast<CGameObject*>(O);
	
	if(l_pO) 
	{
		Fvector P; 
		XFORM().transform_tiny(P,CFORM()->getSphere().P);
		char l_pow[255]; 
		sprintf(l_pow, "zone hit. %.1f", Power(l_pO->Position().distance_to(P)));
		if(bDebug) HUD().outMessage(0xffffffff,l_pO->cName(), l_pow);
		
		Fvector l_dir; 
		l_dir.set(0,0,0);
		/*l_dir.set(::Random.randF(-.5f,.5f), 
				  ::Random.randF(.0f,1.f), 
				  ::Random.randF(-.5f,.5f)); 
		l_dir.normalize();*/
		
		//Fvector l_dir; l_dir.sub(l_pO->Position(), P); l_dir.normalize();
		//l_pO->ph_Movement.ApplyImpulse(l_dir, 50.f*Power(l_pO->Position().distance_to(P)));
		
		Fvector position_in_bone_space;
		float power = Power(l_pO->Position().distance_to(P))*m_fHitImpulseScale;
		//float impulse = m_fHitImpulseScale*power*l_pO->GetMass();
		float impulse = 0;
		if(power > 0.01f) 
		{
			m_dwDeltaTime = 0;
			position_in_bone_space.set(0.f,0.f,0.f);
			NET_Packet		l_P;
			l_pO->u_EventGen	(l_P,GE_HIT,l_pO->ID());
			l_P.w_u16			(u16(ID()));
			l_P.w_u16			(ID());
			l_P.w_dir			(l_dir);
			l_P.w_float			(power);
			l_P.w_s16			((s16)0);
			//l_P.w_s16			((s16)-1);  while not handle -1
			l_P.w_vec3			(position_in_bone_space);
			l_P.w_float			(impulse);
			l_P.w_u16			(ALife::eHitTypeRadiation);
			l_pO->u_EventSend	(l_P);

//			CPGObject* pStaticPG = xr_new<CPGObject>(m_pHitEffect,l_pO->Sector()); 
//			pStaticPG->play_at_pos(l_pO->Position());
		}
	}
}

//void CRadioactiveZone::Update(u32 dt) {
void CRadioactiveZone::UpdateCL() 
{
//	m_dwPeriod = 50;

	u32 dt = Device.dwTimeDelta;
	m_dwDeltaTime += dt;

	if(m_dwDeltaTime > m_dwPeriod) 
	{
		m_dwDeltaTime = m_dwPeriod;
		//while(m_dwDeltaTime > m_dwPeriod) m_dwDeltaTime -= m_dwPeriod;
		m_bZoneReady = true;
	}
	
	inherited::UpdateCL();
	//inherited::Update(dt);
}

void CRadioactiveZone::Postprocess(f32 /**val/**/) 
{
}
