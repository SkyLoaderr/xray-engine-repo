#include "stdafx.h"
#include "mosquitobald.h"
#include "hudmanager.h"
#include "ParticlesObject.h"
#include "level.h"
#include "physicsshellholder.h"
CMosquitoBald::CMosquitoBald(void) 
{
	m_dwDeltaTime = 0;
	m_fHitImpulseScale = 1.f;
}

CMosquitoBald::~CMosquitoBald(void) 
{
}

void CMosquitoBald::Load(LPCSTR section) 
{
	inherited::Load(section);
}


void CMosquitoBald::Postprocess(f32 /**val/**/) 
{
}

bool CMosquitoBald::BlowoutState()
{
	bool result = inherited::BlowoutState();
	if(!result) UpdateBlowout();

	return result;
}



void CMosquitoBald::Affect(CObject* O) 
{
	CPhysicsShellHolder *pGameObject = smart_cast<CPhysicsShellHolder*>(O);
	if(!pGameObject) return;

	if(m_ObjectInfoMap[O].zone_ignore) return;

	Fvector P; 
	XFORM().transform_tiny(P,CFORM()->getSphere().P);

	char l_pow[255]; 
	sprintf(l_pow, "zone hit. %.1f", Power(pGameObject->Position().distance_to(P)));
	if(bDebug) HUD().outMessage(0xffffffff,pGameObject->cName(), l_pow);

	Fvector hit_dir; 
	hit_dir.set(::Random.randF(-.5f,.5f), 
		::Random.randF(.0f,1.f), 
		::Random.randF(-.5f,.5f)); 
	hit_dir.normalize();


	Fvector position_in_bone_space;

	float power = Power(pGameObject->Position().distance_to(P));
	float impulse = m_fHitImpulseScale*power*pGameObject->GetMass();

	//статистика по объекту
	m_ObjectInfoMap[O].total_damage += power;
	m_ObjectInfoMap[O].hit_num++;

	if(power > 0.01f) 
	{
		m_dwDeltaTime = 0;
		position_in_bone_space.set(0.f,0.f,0.f);
	
		if (OnServer())
		{
			NET_Packet	l_P;
			u_EventGen	(l_P,GE_HIT, pGameObject->ID());
			l_P.w_u16	(ID());
			l_P.w_u16	(ID());
			l_P.w_dir	(hit_dir);
			l_P.w_float	(power);
			l_P.w_s16	(/*(s16)BI_NONE*/0);
			l_P.w_vec3	(position_in_bone_space);
			l_P.w_float	(impulse);
			l_P.w_u16	((u16)m_eHitTypeBlowout);
			u_EventSend	(l_P);
		};


		PlayHitParticles(pGameObject);
	}
}