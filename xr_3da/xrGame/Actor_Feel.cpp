#include "stdafx.h"
#include "actor.h"
#include "weapon.h"
#include "xr_weapon_list.h"
#include "targetassault.h"
#include "mercuryball.h"
#include "targetcsbase.h"
#include "targetcs.h"

void CActor::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;

	NET_Packet	P;

	// Test for Artifact
	CTargetCS* A	= dynamic_cast<CTargetCS*>	(O);
	if (A)
	{
		u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16		(u16(A->ID()));
		u_EventSend	(P);
		return;
	}

	// Test for weapon
	CWeapon* W	= dynamic_cast<CWeapon*>	(O);
	if (W)
	{
		// Search if we have similar type of weapon
		if (Weapons->isSlotEmpty(W->GetSlot()))
		{
			// We doesn't have similar weapon - pick up it
			u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
			P.w_u16		(u16(W->ID()));
			u_EventSend	(P);
			return;
		}
		/*
		CWeapon* T = Weapons->getWeaponByWeapon	(W);
		if (T)	
		{
			// We have similar weapon - just get ammo out of it
			u_EventGen	(P,GE_TRANSFER_AMMO,ID());
			P.w_u16		(u16(W->ID()));
			u_EventSend	(P);
			return;
		} else {
			// We doesn't have similar weapon - pick up it
			u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
			P.w_u16		(u16(W->ID()));
			u_EventSend	(P);
			return;
		}
		return;
		*/
	}

	// Test for GAME-specific events
	switch (GameID())
	{
	case GAME_ASSAULT:
		{
			CTargetAssault*		T	= dynamic_cast<CTargetAssault*>	(O);
			if (g_Team() && T)
			{
				// Assault acomplished?
				u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
				P.w_u16		(u16(T->ID()));
				u_EventSend	(P);
				return;
			}
		}
	case GAME_CS:
		{
			// Test for CS Base
			CTargetCSBase* B	= dynamic_cast<CTargetCSBase*>	(O);
			if (B)
			{
				u_EventGen	(P,GE_OWNERSHIP_TAKE,ID());
				P.w_u16		(u16(B->ID()));
				u_EventSend	(P);
				return;
			}
		}
	}
}

void CActor::feel_touch_delete	(CObject* O)
{
	NET_Packet	P;

	switch (GameID())
	{
	case GAME_CS:
		{
			CTargetCSBase*		T	= dynamic_cast<CTargetCSBase*>	(O);
			if (T)
			{
				u_EventGen	(P,GE_OWNERSHIP_REJECT,ID());
				P.w_u16		(u16(T->ID()));
				u_EventSend	(P);
				return;
			}
		}
	}
}
