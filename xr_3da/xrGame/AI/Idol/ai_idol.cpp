////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_idol.h"
#include "..\\..\\bolt.h"

CAI_Idol::CAI_Idol					()
{
	m_tpCurrentBlend				= 0;
	m_bPlaying						= false;
	m_dwCurrentAnimationIndex		= 0;
}

CAI_Idol::~CAI_Idol					()
{
}

void CAI_Idol::Load					(LPCSTR section)
{
	setEnabled						(false);
	inherited::Load					(section);
}

BOOL CAI_Idol::net_Spawn			(LPVOID DC)
{
	xrSE_Idol						*tpIdol = (xrSE_Idol*)(DC);
	R_ASSERT						(tpIdol);
	cNameVisual_set					(tpIdol->caModel);
	
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	m_dwAnyPlayType					= tpIdol->m_dwAniPlayType;
	m_tpaAnims.clear				();
	u32								N = _GetItemCount(tpIdol->m_caAnimations);
	string32						I;
	for (u32 i=0; i<N; i++)
		m_tpaAnims.push_back		(PKinematics(pVisual)->ID_Cycle(_GetItem(tpIdol->m_caAnimations,i,I)));

	return							TRUE;
}

void CAI_Idol::SelectAnimation		(const Fvector& _view, const Fvector& _move, float speed)
{
	//R_ASSERT						(!m_tpaAnims.empty());
	if (m_tpaAnims.empty())
		return;
	if (g_Alive()) {
		switch (m_dwAnyPlayType) {
			case 0 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= PKinematics(pVisual)->PlayCycle	(m_tpaAnims[::Random.randI(0,m_tpaAnims.size())],TRUE,AnimCallback,this);
					m_bPlaying				= true;
				}
				break;
			}
			case 1 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= PKinematics(pVisual)->PlayCycle	(m_tpaAnims[m_dwCurrentAnimationIndex],TRUE,AnimCallback,this);
					m_bPlaying				= true;
					m_dwCurrentAnimationIndex = (m_dwCurrentAnimationIndex + 1) % m_tpaAnims.size();
				}
				break;
			}
			case 2 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= PKinematics(pVisual)->PlayCycle	(m_tpaAnims[m_dwCurrentAnimationIndex],TRUE,AnimCallback,this);
					m_bPlaying				= true;
					if (m_dwCurrentAnimationIndex < m_tpaAnims.size() - 1)
						m_dwCurrentAnimationIndex++;
				}
				break;
			}
			default : NODEFAULT;
		}
	}
}

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CAI_Idol::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			Log("Trying to take - ", O->cName());
			if(g_Alive() && m_inventory.Take(dynamic_cast<CGameObject*>(O))) {
				O->H_SetParent(this);
				Log("TAKE - ", O->cName());
//				if(PIItem(O)->m_slot < 0xffffffff) {
//					m_inventory.Slot(PIItem(O));
//					m_inventory.Activate(PIItem(O)->m_slot);
//				}
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

void CAI_Idol::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= dynamic_cast<CInventoryItem*>	(O);
	CBolt				*E	= dynamic_cast<CBolt*>			(O);

	if (I && !E) {
		Msg("Taking item %s!",I->cName());
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->ID()));
		u_EventSend		(P);
	}
}

void CAI_Idol::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (O->H_Parent() != this))
		return;

	Msg("Dropping item!");
	// We doesn't have similar weapon - pick up it
	NET_Packet				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);
}

void CAI_Idol::Update		(u32 dt)
{
	inherited::Update		(dt);
	m_inventory.Update		(dt);
}

void CAI_Idol::g_WeaponBones	(int &L, int &R1, int &R2)
{
	CKinematics *V	= PKinematics(Visual());
	R1				= V->LL_BoneID("bip01_r_hand");
	R2				= V->LL_BoneID("bip01_r_finger2");
	L				= V->LL_BoneID("bip01_l_finger1");
}