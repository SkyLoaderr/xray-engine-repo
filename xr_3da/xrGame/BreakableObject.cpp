#include "stdafx.h"
#pragma hdrstop

#include "BreakableObject.h"
#include "xrserver_objects_alife.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBreakableObject::CBreakableObject	()
{
	fHealth					= 100.f;
}

CBreakableObject::~CBreakableObject	()
{
}

void CBreakableObject::Load		(LPCSTR section)
{
	inherited::Load			(section);
}

BOOL CBreakableObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectBreakable *obj	= dynamic_cast<CSE_ALifeObjectBreakable*>(e);
	R_ASSERT				(obj);
	inherited::net_Spawn	(DC);

	// set bone id
	R_ASSERT				(Visual()&&PKinematics(Visual()));
//	CKinematics* K			= PKinematics(Visual());
	fHealth					= obj->m_health;
	
	setVisible				(TRUE);
	setEnabled				(TRUE);

	return					(TRUE);
}

void CBreakableObject::shedule_Update	(u32 dt)
{
	inherited::shedule_Update		(dt);
}

void CBreakableObject::UpdateCL	()
{
	inherited::UpdateCL		();
}

void CBreakableObject::renderable_Render()
{
	inherited::renderable_Render	();
}

void CBreakableObject::Hit(float P,Fvector &dir, CObject* who,s16 element,
					   Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/)
{
}

void CBreakableObject::net_Export(NET_Packet& P)
{
	VERIFY					(Local());
}

void CBreakableObject::net_Import(NET_Packet& P)
{
	VERIFY					(Remote());
}

BOOL CBreakableObject::UsedAI_Locations()
{
	return					(FALSE);
}
