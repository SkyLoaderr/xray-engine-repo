#include "stdafx.h"
#include "actor.h"
#include "weapon.h"
#include "xr_weapon_list.h"

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CActor::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			// Test for weapon
			CWeapon* W	= dynamic_cast<CWeapon*>	(O);
			if (W) 
			{
				R_ASSERT							(BE(Local(),W->Local()));	// remote can't take local
				W->H_SetParent						(this);
				int id	= Weapons->weapon_add		(W);
				Weapons->ActivateWeaponID			(id);
				return;
			}
		}
		break;
	case GE_OWNERSHIP_REJECT:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			// Test for weapon
			CWeapon* W	= dynamic_cast<CWeapon*>	(O);
			if (W) 
			{
				R_ASSERT							(BE(Local(),W->Local()));	// remote can't eject local
				int id	= Weapons->weapon_remove	(W);
				Weapons->ActivateWeaponHistory		();
				W->H_SetParent						(0);
				feel_touch.push_back				(W);
				return;
			}
		}
		break;
	case GE_TRANSFER_AMMO:
		{
			u16			from,to;
			P.r_u16		(from);
			P.r_u16		(to);

			CObject* Ofrom	= Level().Objects.net_Find	(from);
			CObject* Oto	= Level().Objects.net_Find	(to);
			if (!(Ofrom && Oto))	break;

			// Test for weapon
			CWeapon* Wfrom	= dynamic_cast<CWeapon*>	(Ofrom);
			CWeapon* Wto	= dynamic_cast<CWeapon*>	(Oto);
			if (Wfrom && Wto)
			{
				R_ASSERT							(BE(Local(),Wfrom->Local()));
				R_ASSERT							(BE(Local(),Wto->Local()));
				Wto->Ammo_add	(Wfrom->Ammo_eject());
			}
		}
		break;
	}
}
