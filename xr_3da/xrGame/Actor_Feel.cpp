#include "stdafx.h"
#include "actor.h"
#include "weapon.h"
#include "xr_weapon_list.h"
#include "targetassault.h"
#include "mercuryball.h"
#include "targetcsbase.h"
#include "targetcs.h"
#include "targetcscask.h"

void CActor::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;

	// Now, test for game specific logical objects to minimize traffic
	CWeapon*		W	= dynamic_cast<CWeapon*>		(O);
	CTargetAssault*	At	= dynamic_cast<CTargetAssault*>	(O);
	CTargetCSBase*	CSb	= dynamic_cast<CTargetCSBase*>	(O);
	CTargetCS*		CSt	= dynamic_cast<CTargetCS*>		(O);
	CTargetCSCask*	CSca= dynamic_cast<CTargetCSCask*>	(O);

	if (W || At || CSb || CSt || CSca)
	{
		// Generate event
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(O->ID()));
		u_EventSend		(P);
	}
}

void CActor::feel_touch_delete	(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;

	CTargetCSBase*	CSt	= dynamic_cast<CTargetCSBase*>	(O);
	if (T)
	{
		// Generate event
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16			(u16(T->ID()));
		u_EventSend		(P);
		return;
	}
}
