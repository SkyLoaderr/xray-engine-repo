///////////////////////////////////////////////////////////////
// BottleItem.cpp
// BottleItem - ������� � ��������, ������� ����� �������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BottleItem.h"
#include "xrmessages.h"
#include "net_utils.h"
#include "entity_alive.h"
#include "EntityCondition.h"

#define BREAK_POWER 5.f

CBottleItem::CBottleItem(void) 
{
}

CBottleItem::~CBottleItem(void) 
{
	sndBreaking.destroy();
}


BOOL CBottleItem::net_Spawn(CSE_Abstract* DC) 
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

	m_alcohol = READ_IF_EXISTS(pSettings, r_float, section, "eat_alcohol", 0.0f);
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
	//������ ����
	sndBreaking.play_at_pos(0, Position(), false);

	//�������� �������� ����������
	if(*m_sBreakParticles)
	{
		//���������� �������
		CParticlesObject* pStaticPG; 
		pStaticPG = CParticlesObject::Create(*m_sBreakParticles,TRUE); 
		pStaticPG->play_at_pos(Position());
	}

	//������������� ��� ������ 
	if (Local())
	{
		DestroyObject	();
//		NET_Packet			P;
//		u_EventGen			(P,GE_DESTROY,ID());
////		Msg					("ge_destroy: [%d] - %s",ID(),*cName());
//		u_EventSend			(P);
	}
}

//void CBottleItem::Hit (float P, Fvector &dir,	
//					   CObject* who, s16 element,
//					   Fvector position_in_object_space, 
//					   float impulse, 
//					   ALife::EHitType hit_type)
void	CBottleItem::Hit					(SHit* pHDS)
{
//	inherited::Hit(P, dir, who, element, position_in_object_space, impulse, hit_type);
	inherited::Hit(pHDS);
	
	if(pHDS->P>BREAK_POWER)
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

void CBottleItem::UseBy				(CEntityAlive* entity_alive)
{
	inherited::UseBy					(entity_alive);

	entity_alive->conditions().ChangeAlcohol			(m_alcohol);
}