//////////////////////////////////////////////////////////////////////
// ExplosiveRocket.cpp:	ракета, которой стреляет RocketLauncher 
//						взрывается при столкновении
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ExplosiveRocket.h"


CExplosiveRocket::CExplosiveRocket() 
{
}

CExplosiveRocket::~CExplosiveRocket() 
{
}




void CExplosiveRocket::Load(LPCSTR section) 
{
	inherited::Load(section);
	CExplosive::Load(section);
}

BOOL CExplosiveRocket::net_Spawn(LPVOID DC) 
{
	BOOL result = inherited::net_Spawn(DC);
	result |= CExplosive::net_Spawn(DC);

	return result;
}

void CExplosiveRocket::Contact(const Fvector &pos, const Fvector &normal)
{
	if(eCollide == m_eState) return;



	CExplosive::ExplodeParams(pos,normal);

	//взорвать гранату
	if (Local()) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	}

	inherited::Contact(pos, normal);
}

void CExplosiveRocket::net_Destroy() 
{
	CExplosive::net_Destroy();
	inherited::net_Destroy();
}

void CExplosiveRocket::OnH_A_Independent() 
{
	inherited::OnH_A_Independent();
	CExplosive::OnH_A_Independent();
}

void CExplosiveRocket::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	CExplosive::OnH_B_Independent();
}

void CExplosiveRocket::UpdateCL() 
{
	if(eCollide == m_eState)
		CExplosive::UpdateCL();
	else
		inherited::UpdateCL();
}


void  CExplosiveRocket::OnEvent (NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	CExplosive::OnEvent(P, type);
}


void CExplosiveRocket::make_Interpolation ()
{
	inherited::make_Interpolation();
}

void CExplosiveRocket::PH_B_CrPr			()
{
	inherited::PH_B_CrPr		();
}

void CExplosiveRocket::PH_I_CrPr			()
{
	inherited::PH_I_CrPr		();
}

void CExplosiveRocket::PH_A_CrPr			()
{
	inherited::PH_A_CrPr		();
}

void CExplosiveRocket::reinit				()
{
	inherited::reinit			();
	CExplosive::reinit			();
}

void CExplosiveRocket::reload					(LPCSTR section)
{
	inherited::reload			(section);
	CExplosive::reload			(section);
}

void CExplosiveRocket::activate_physic_shell	()
{
	inherited::activate_physic_shell();
}

void CExplosiveRocket::setup_physic_shell		()
{
	inherited::setup_physic_shell();
}

void CExplosiveRocket::create_physic_shell		()
{
	inherited::create_physic_shell();
}
