///////////////////////////////////////////////////////////////
// BottleItem.cpp
// BottleItem - бутылка с напитком, которую можно разбить
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BottleItem.h"
#include "xrmessages.h"
#include "net_utils.h"

#define BREAK_POWER 5.f

CBottleItem::CBottleItem(void) 
{
}

CBottleItem::~CBottleItem(void) 
{
	sndBreaking.destroy();
}


BOOL CBottleItem::net_Spawn(LPVOID DC) 
{
	BOOL res = inherited::net_Spawn(DC);

	return res;
}

void CBottleItem::Load(LPCSTR section) 
{
	inherited::Load(section);

	if(pSettings->line_exist(section, "break_particles"))
		m_sBreakParticles = pSettings->r_string(section, "break_particles");

	if(pSettings->line_exist(section, "break_sound"))
		sndBreaking.create(TRUE, pSettings->r_string(section, "break_sound"));
}

void CBottleItem::net_Destroy() 
{
	inherited::net_Destroy();
}

void CBottleItem::UpdateCL() 
{
	inherited::UpdateCL();
}


void CBottleItem::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CBottleItem::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CBottleItem::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);

	switch (type) 
	{
		case GE_GRENADE_EXPLODE : 
			BreakToPieces();
			break;
	}
}

void CBottleItem::BreakToPieces()
{
	//играем звук
	sndBreaking.play_at_pos(0, Position(), false);

	//отыграть партиклы разбивания
	if(*m_sBreakParticles)
	{
		//показываем эффекты
		CParticlesObject* pStaticPG; 
		pStaticPG = xr_new<CParticlesObject>(*m_sBreakParticles,Sector()); 
		pStaticPG->play_at_pos(Position());
	}

	//ликвидировать сам объект 
	if (Local())
	{
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,ID());
//		Msg					("ge_destroy: [%d] - %s",ID(),*cName());
		u_EventSend			(P);
	}
}

void CBottleItem::Hit (float P, Fvector &dir,	
					   CObject* who, s16 element,
					   Fvector position_in_object_space, 
					   float impulse, 
					   ALife::EHitType hit_type)
{
	inherited::Hit(P, dir, who, element, position_in_object_space, impulse, hit_type);
	
	if(P>BREAK_POWER)
	{
		//Generate Expode event
		if (Local()) 
		{
			NET_Packet		P;
			u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
			u_EventSend		(P);
		};
	}
}