//////////////////////////////////////////////////////////////////////
// ExplosiveItem.cpp:	����� ��� ���� ������� ���������� ��� 
//						��������� ��������� ����� (��������,
//						������ � ����� � �.�.)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ExplosiveItem.h"


CExplosiveItem::CExplosiveItem(void)
{
}
CExplosiveItem::~CExplosiveItem(void)
{
}

void CExplosiveItem::Load(LPCSTR section)
{
	m_bUsingCondition = true;

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

void CExplosiveItem::net_Destroy()
{
	CExplosive::net_Destroy();
	CInventoryItem::net_Destroy();
}

void CExplosiveItem::Hit(float P, Fvector &dir,	CObject* who, s16 element,
						Fvector position_in_object_space, float impulse, 
						ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,position_in_object_space,impulse,hit_type);

	
	
	if (Local() && GetCondition()<=0.f) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
		
		//��������� ����, ��� ������� ����
		m_iCurrentParentID = who->ID();
	};

}

void  CExplosiveItem::OnEvent (NET_Packet& P, u16 type)
{
	CExplosive::OnEvent (P, type);
	CInventoryItem::OnEvent (P, type);

}
void CExplosiveItem::UpdateCL()
{
	CExplosive::UpdateCL();
	CInventoryItem::UpdateCL();
}
void CExplosiveItem::renderable_Render()
{
	CInventoryItem::renderable_Render();
}