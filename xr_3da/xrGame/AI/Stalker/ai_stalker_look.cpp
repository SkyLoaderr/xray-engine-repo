////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

static BOOL __fastcall StalkerQualifier(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else {
		CEntity* E = dynamic_cast<CEntity*> (O);
		if (!E) return FALSE;
		if (!E->IsVisibleForAI()) return FALSE; 
		return TRUE;
	}
}

objQualifier* CAI_Stalker::GetQualifier	()
{
	return(&StalkerQualifier);
}

void CAI_Stalker::OnVisible	()
{
	inherited::OnVisible	();

	CWeapon* W				= Weapons->ActiveWeapon();
	if (W)					W->OnVisible		();
}
