//////////////////////////////////////////////////////////////////////
// ExplosiveItem.cpp:	класс для вещи которая взрывается под 
//						действием различных хитов (канистры,
//						балоны с газом и т.д.)
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
	inherited::Load							(section);

	m_bUsingCondition						= true;
}

void CExplosiveItem::net_Destroy()
{
	CExplosive::net_Destroy();
}

void CExplosiveItem::Hit(float P, Fvector &dir,	CObject* who, s16 element,
						Fvector position_in_object_space, float impulse, 
						ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,position_in_object_space,impulse,hit_type);

	
	if(GetCondition()<=0.f)
	{
		//запомнить того, кто взорвал вещь
		m_iCurrentParentID = who->ID();

		Fvector normal;
		FindNormal(normal);
		CExplosive::GenExplodeEvent(Position(), normal);
	}
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
	CExplosive::renderable_Render();
}