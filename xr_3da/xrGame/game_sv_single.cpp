#include "stdafx.h"
#include "game_sv_single.h"

void	game_sv_Single::Create			(LPCSTR options)
{
	switch_Phase	(GAME_PHASE_PENDING);
}

BOOL	game_sv_Single::OnTouch			(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	xrServerEntity*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	xrServerEntity*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	xrSE_Actor*			A		= dynamic_cast<xrSE_Actor*> (e_who);
	if (A)
	{
		// Actor touches something
		xrSE_Weapon*	W			=	dynamic_cast<xrSE_Weapon*> (e_what);
		if (W) 
		{
			// Weapon
			vector<u16>&	C			=	A->children;
			u8 slot						=	W->get_slot	();
			for (u32 it=0; it<C.size(); it++)
			{
				xrServerEntity*		Et	= S->ID_to_entity				(C[it]);
				if (0==Et)				continue;
				xrSE_Weapon*		T	= dynamic_cast<xrSE_Weapon*>	(Et);
				if (0==T)				continue;
				if (slot == T->get_slot())	
				{
					// We've found same slot occupied - disallow ownership
					return FALSE;
				}
			}

			// Weapon slot empty - ownership OK
			return TRUE;
		}
	}

	// We don't know what the hell is it, so disallow ownership just for safety 
	return FALSE;
}

BOOL	game_sv_Single::OnDetach		(u16 eid_who, u16 eid_what)
{
	return TRUE;
}

void	game_sv_Single::OnRoundStart	()
{
	tALife.Load();
}

void	game_sv_Single::Update			()
{
	__super::Update	();
	switch(phase) 	{
		case GAME_PHASE_PENDING : {
			if ((Device.TimerAsync()-start_time)>u32(5*1000)) {
				OnRoundStart();
				switch_Phase(GAME_PHASE_INPROGRESS);
			}
		}
		break;
	}
}

