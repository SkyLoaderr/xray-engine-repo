//////////////////////////////////////////////////////////////////////
// ExplosiveItem.cpp:	класс для вещи которая взрывается под 
//						действием различных хитов (канистры,
//						балоны с газом и т.д.)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ExplosiveItem.h"


CExplosiveItem::CExplosiveItem(void)
{
	m_HitTypeK.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;

}
CExplosiveItem::~CExplosiveItem(void)
{
}

void CExplosiveItem::Load(LPCSTR section)
{
	
	m_HitTypeK[ALife::eHitTypeBurn]			= pSettings->r_float(section,"burn_immunity");
	m_HitTypeK[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_immunity");
	m_HitTypeK[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_immunity");
	m_HitTypeK[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_immunity");
	m_HitTypeK[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_immunity");
	m_HitTypeK[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_immunity");
	m_HitTypeK[ALife::eHitTypeChemicalBurn] = pSettings->r_float(section,"chemical_burn_immunity");
	m_HitTypeK[ALife::eHitTypeFireWound]	= pSettings->r_float(section,"fire_wound_immunity");
	m_HitTypeK[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_immunity");


	inherited::Load(section);
}

void CExplosiveItem::Hit(float P, Fvector &dir,	CObject* who, s16 element,
						Fvector position_in_object_space, float impulse, 
						ALife::EHitType hit_type)
{
	float hit_power = P/100.f;
	hit_power *= m_HitTypeK[hit_type];

	m_fHealth -= hit_power;

	if (Local() && m_fHealth<0.f) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	};

}

void CExplosiveItem::renderable_Render()
{
	CInventoryItem::renderable_Render();
}


BOOL CExplosiveItem::net_Spawn		(LPVOID DC)
{
	return CInventoryItem::net_Spawn(DC);
}
void CExplosiveItem::net_Destroy	()
{
	CInventoryItem::net_Destroy();
}
void CExplosiveItem::net_Export		(NET_Packet& P)
{
	CInventoryItem::net_Export(P);
	P.w_float					(m_fHealth);
}
void CExplosiveItem::net_Import		(NET_Packet& P)
{
	CInventoryItem::net_Import(P);
	P.r_float					(m_fHealth);
}