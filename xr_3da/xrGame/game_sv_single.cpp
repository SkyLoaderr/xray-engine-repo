#include "stdafx.h"
#include "game_sv_single.h"

void	game_sv_Single::Create			(LPCSTR options)
{
	m_bALife			= !!strstr(options,"/alife");
	switch_Phase		(GAME_PHASE_PENDING);
}
xrServerEntity*		game_sv_Single::get_entity_from_eid		(u16 id)
{
	if (m_tpALife) {
		OBJECT_PAIR_IT	I = m_tpALife->m_tObjectRegistry.find(id);
		if (I != m_tpALife->m_tObjectRegistry.end())
			return((*I).second);
		else
			return(inherited::get_entity_from_eid(id));
	}
	else
		return(inherited::get_entity_from_eid(id));
}

BOOL	game_sv_Single::OnTouch			(u16 eid_who, u16 eid_what)
{
	xrServerEntity*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
	xrServerEntity*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

	xrSE_Actor*			A		= dynamic_cast<xrSE_Actor*> (e_who);
	if (A)
	{
		// Actor touches something
		xrSE_Weapon*	W			=	dynamic_cast<xrSE_Weapon*> (e_what);
		if (W) 
		{
			// Weapon
			//vector<u16>&	C			=	A->children;
			//u8 slot						=	W->get_slot	();
			//for (u32 it=0; it<C.size(); it++)
			//{
			//	xrServerEntity*		Et	= get_entity_from_eid			(C[it]);
			//	if (0==Et)				continue;
			//	xrSE_Weapon*		T	= dynamic_cast<xrSE_Weapon*>	(Et);
			//	if (0==T)				continue;
			//	if (slot == T->get_slot())	
			//	{
			//		// We've found same slot occupied - disallow ownership
			//		return FALSE;
			//	}
			//}

			// Weapon slot empty - ownership OK
			return TRUE;
		}
	}
	else {
		xrSE_Human*			A		= dynamic_cast<xrSE_Human*> (e_who);
		if (A) {
			// Actor touches something
			xrSE_Weapon*	W			=	dynamic_cast<xrSE_Weapon*> (e_what);
			if (W) 
			{
//				// Weapon
//				vector<u16>&	C			=	A->children;
//				u8 slot						=	W->get_slot	();
//				for (u32 it=0; it<C.size(); it++)
//				{
//					xrServerEntity*		Et	= get_entity_from_eid			(C[it]);
//					if (0==Et)				continue;
//					xrSE_Weapon*		T	= dynamic_cast<xrSE_Weapon*>	(Et);
//					if (0==T)				continue;
//					if (slot == T->get_slot())	
//					{
//						// We've found same slot occupied - disallow ownership
//						return FALSE;
//					}
//				}
//
				// Weapon slot empty - ownership OK
				return TRUE;
			}
		}
	}

	// We don't know what the hell is it, so disallow ownership just for safety 
	return TRUE;
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
//	if (m_bALife) {
//		m_tpALife				= xr_new<CAI_ALife>(m_tpServer);
//		m_tpALife->Load			();
//	}
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

void	game_sv_Single::OnPlayerKillPlayer	(u32 id_killer, u32 id_killed)
{
	//xrServer*	S					=	Level().Server;
	//// Drop everything
	//vector<u16>*	C				=	get_children(id_killed);
	//if (0==C)						return;
	//while(C->size())
	//{
	//	u16		eid						= (*C)[0];

	//	xrServerEntity*		from		= S->get_entity_from_eid(get_id_2_eid(id_killed));
	//	xrServerEntity*		what		= S->get_entity_from_eid(eid);
	//	S->Perform_reject				(what,from);
	//}
}
