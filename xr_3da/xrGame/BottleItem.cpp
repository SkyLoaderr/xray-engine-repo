///////////////////////////////////////////////////////////////
// BottleItem.cpp
// BottleItem - бутылка с напитком, которую можно разбить
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "BottleItem.h"


#define PLAYING_ANIM_TIME 10000

CBottleItem::CBottleItem(void) 
{
}

CBottleItem::~CBottleItem(void) 
{
}


BOOL CBottleItem::net_Spawn(LPVOID DC) 
{
	BOOL res = inherited::net_Spawn(DC);

	return res;
}

void CBottleItem::Load(LPCSTR section) 
{
	inherited::Load(section);
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
			Explode();
			break;
	}
}

void CBottleItem::Explode()
{

}

void CBottleItem::Hit (float P, Fvector &dir,	
					   CObject* who, s16 element,
					   Fvector position_in_object_space, 
					   float impulse, 
					   ALife::EHitType hit_type)
{
	inherited::Hit(P, dir, who, element, position_in_object_space, impulse, hit_type);
	
	if(P>5.f) Explode();

}

/*void CBottleItem::Destroy() 
{
	//Generate Expode event
	if (Local()) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	};
}
*/