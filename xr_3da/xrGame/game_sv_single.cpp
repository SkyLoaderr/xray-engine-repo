#include "stdafx.h"
#include "game_sv_single.h"

void	game_sv_Single::Create			(LPCSTR options)
{
	switch_Phase	(GAME_PHASE_PENDING);
}

BOOL	game_sv_Single::OnTouch			(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= m_tpServer;
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
//	OBJECT_PAIR_IT			I = m_tpALife->m_tObjectRegistry.find(eid_what);
//	R_ASSERT				(I != m_tpALife->m_tObjectRegistry.end());
//	(*I).second->ID_Parent	= 0xfffe;
	return					(TRUE);
}

void	game_sv_Single::OnRoundStart	()
{
	m_tpALife				= xr_new<CAI_ALife>(m_tpServer);
	m_tpALife->Load			();
}

void	game_sv_Single::Update			()
{
	__super::Update	();
	switch(phase) 	{
		case GAME_PHASE_PENDING : {
			OnRoundStart();
			switch_Phase(GAME_PHASE_INPROGRESS);
			break;
		}
	}
}

