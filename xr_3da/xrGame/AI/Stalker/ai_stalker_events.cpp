////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_events.cpp
//	Created 	: 26.02.2003
//  Modified 	: 26.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Events handling for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\weapon.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\targetcs.h"
#include "..\\..\\customdetector.h"
#include "..\\..\\bolt.h"

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CAI_Stalker::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

#ifndef SILENCE
			Msg("Trying to take - %s (%d)", O->cName(),O->ID());
#endif
			if(g_Alive() && m_inventory.Take(dynamic_cast<CGameObject*>(O))) {
				O->H_SetParent(this);
#ifndef SILENCE
				Msg("TAKE - %s (%d)", O->cName(),O->ID());
#endif
				////if(m_inventory.m_activeSlot == 0xffffffff) {
				////	if(PIItem(O)->m_slot < 0xffffffff) {
				////		m_inventory.Slot(PIItem(O)); 
				////		m_inventory.Activate(PIItem(O)->m_slot);
				////	}
				////}
				//if(PIItem(O)->m_slot < 0xffffffff && !m_inventory.m_slots[PIItem(O)->m_slot].m_pIItem)) { m_inventory.Slot(PIItem(O)); }
				//if(m_inventory.Slot(PIItem(O)) && PIItem(O)->m_slot == 1) m_inventory.Activate(PIItem(O)->m_slot);
			}


			//// Test for Detector
			//CCustomDetector* D	= dynamic_cast<CCustomDetector*>	(O);
			//if (D) 
			//{
			//	R_ASSERT							(BE(Local(),D->Local()));	// remote can't take local
			//	D->H_SetParent(this);
			//	return;
			//}

			//// Test for Artifact
			//CTargetCS* A	= dynamic_cast<CTargetCS*>	(O);
			//if (A) 
			//{
			//	if(!BE(Local(),A->Local())) Log("TAKE ERROR: BE(Local(),A->Local()))");
			//	R_ASSERT							(BE(Local(),A->Local()));	// remote can't take local
			//	A->H_SetParent(this);
			//	
			//	// 
			//	m_pArtifact			= A;
			//	// 
			//	return;
			//}

			//// Test for weapon
			//CWeapon* W	= dynamic_cast<CWeapon*>	(O);
			//if (W) 
			//{
			//	if(!BE(Local(),W->Local())) Log("TAKE ERROR: BE(Local(),W->Local()))");
			//	R_ASSERT							(BE(Local(),W->Local()));				// remote can't take local
			//	R_ASSERT							(Weapons->isSlotEmpty(W->GetSlot()));
			//	W->H_SetParent						(this);
			//	Weapons->weapon_add					(W);
			//	Weapons->ActivateWeaponID			(W->GetSlot());
			//	return;
			//}
		}
		break;
	case GE_OWNERSHIP_REJECT:
		{
			// Log			("CActor::OnEvent - REJECT - : ", cName());

			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			
			if(m_inventory.Drop(dynamic_cast<CGameObject*>(O))) {
				O->H_SetParent(0);
				feel_touch_deny(O,2000);
			}

			//// Test for Detector
			//CCustomDetector* D	= dynamic_cast<CCustomDetector*>	(O);
			//if (D) 
			//{
			//	D->H_SetParent						(0);
			//	feel_touch_deny						(D,1000);
			//	return;
			//}

			//// Test for weapon
			//CWeapon* W	= dynamic_cast<CWeapon*>	(O);
			//if (W) 
			//{
			//	//R_ASSERT							(BE(Local(),W->Local()));	// remote can't eject local
			//	Weapons->weapon_remove				(W);
			//	Weapons->ActivateWeaponHistory		();
			//	W->H_SetParent						(0);
			//	feel_touch_deny						(W,1000);
			//	return;
			//}

			//// Test for Artifact
			//CTargetCS* A	= dynamic_cast<CTargetCS*>	(O);
			//if (A)
			//{
			//	// R_ASSERT							(BE(Local(),A->Local()));	// remote can't eject local
			//	A->H_SetParent						(0);
			//	feel_touch_deny						(A,1000);
			//	return;
			//}
			//Log			("CActor::OnEvent - REJECT - Processed.");
		}
		break;
	case GE_TRANSFER_AMMO:
		{
			//u16					from;
			//P.r_u16				(from);

			//CObject* Ofrom		= Level().Objects.net_Find	(from);
			//if (0==Ofrom)		break;
			//CWeapon* Wfrom		= dynamic_cast<CWeapon*>	(Ofrom);
			//if (0==Wfrom)		break;
			//CWeapon* Wto		= Weapons->getWeaponByWeapon(Wfrom);
			//if (0==Wto)			break;

			//// Test for locality
			//Wto->Ammo_add		(Wfrom->Ammo_eject());
			//Wfrom->setDestroy	(TRUE);
		}
		break;
	}
}

void CAI_Stalker::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= dynamic_cast<CInventoryItem*>	(O);
	CBolt				*E	= dynamic_cast<CBolt*>			(O);

	if (I && !E) {
#ifndef SILENCE
		Msg("Taking item %s (%d)!",I->cName(),I->ID());
#endif
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->ID()));
		u_EventSend		(P);
	}
}

void CAI_Stalker::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (O->H_Parent() != this))
		return;

#ifndef SILENCE
	Msg("Dropping item!");
#endif
	// We doesn't have similar weapon - pick up it
	NET_Packet				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);
}
