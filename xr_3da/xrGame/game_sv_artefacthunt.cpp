#include "stdafx.h"
#include "game_sv_ArtefactHunt.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"

void	game_sv_ArtefactHunt::Create					(LPSTR &options)
{
	inherited::Create					(options);
}

void	game_sv_ArtefactHunt::OnRoundStart			()
{
	inherited::OnRoundStart	();
/*
	// Respawn all players and some info
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		ClearPlayerState(ps);

		SpawnActor(get_it_2_id(it), "spectator");
	}
	*/
}

void	game_sv_ArtefactHunt::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	if (!ps_killed || !ps_killer) return;

	ps_killed->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	ps_killed->deaths				+=	1;
	if (ps_killer == ps_killed || ps_killed->team == ps_killer->team)	
	{
		// By himself
		ps_killer->kills			-=	1;
	} else {
		// Opponent killed - frag 
		ps_killer->kills			+=	1;
	}
	
	teams[ps_killer->team-1].score = 0;
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->team != ps_killer->team) continue;

		teams[ps_killer->team-1].score += ps->kills;
	};	

	if (fraglimit && (teams[ps_killer->team-1].score >= fraglimit) )OnFraglimitExceed();

	// Send Message About Player Killed
	SendPlayerKilledMessage(id_killer, id_killed);

	signal_Syncronize();
}

u32		game_sv_ArtefactHunt::RP_2_Use				(CSE_Abstract* E)
{
	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA) return 0;

	return u32(pA->g_team());
};

void	game_sv_ArtefactHunt::LoadTeams			()
{
	LoadTeamData("artefacthunt_team0");
	LoadTeamData("artefacthunt_team1");
	LoadTeamData("artefacthunt_team2");
};

bool	game_sv_ArtefactHunt::IsBuyableItem				(CSE_Abstract* pItem)
{
	if (!pItem) return false;
	CSE_ALifeItemWeapon* pWeapon = dynamic_cast<CSE_ALifeItemWeapon*> (pItem);
	if (pWeapon) return true;
	CSE_ALifeItemGrenade* pGrenade = dynamic_cast<CSE_ALifeItemGrenade*> (pItem);
	if (pGrenade) return true;
	return false;
};

void	game_sv_ArtefactHunt::RemoveItemFromActor		(CSE_Abstract* pItem)
{
	if (!pItem) return;
	//-------------------------------------------------------------
	CSE_ALifeItemWeapon* pWeapon = dynamic_cast<CSE_ALifeItemWeapon*> (pItem);
	if (pWeapon)
	{
	};
	//-------------------------------------------------------------
	NET_Packet			P;
	u_EventGen			(P,GE_OWNERSHIP_REJECT,pItem->ID_Parent);
	P.w_u16				(pItem->ID);
	Level().Send(P,net_flags(TRUE,TRUE));
/*
	u_EventGen			(P,GE_DESTROY,pItem->ID);
	Level().Send(P,net_flags(TRUE,TRUE));
//	*/
//	Level().Server->Perform_destroy(pItem, net_flags(TRUE, TRUE), TRUE);
	xr_vector<u16>::const_iterator	I = pItem->children.begin	();
	xr_vector<u16>::const_iterator	E = pItem->children.end		();
	for ( ; I != E; ++I) {
//		CSE_Abstract	*e_dest	= get_entity_from_eid(*I);
		u_EventGen			(P,GE_DESTROY,*I);
		Level().Send(P,net_flags(TRUE,TRUE));
	}
	u_EventGen			(P,GE_DESTROY,pItem->ID);
	Level().Send(P,net_flags(TRUE,TRUE));
};

BOOL	game_sv_ArtefactHunt::CheckUpgrades			(CSE_Abstract* pItem, u8 IItem)
{
	CSE_ALifeItemWeapon* pWeapon = dynamic_cast<CSE_ALifeItemWeapon*> (pItem);
	if (!pWeapon) return true;

//	u8 Addons = pWeapon->m_addon_flags.get();
//	u8 Upgrades = IItem >> 0x05;
//	return Addons == Upgrades;
	pWeapon->m_addon_flags.set(IItem >> 0x05);
	return true;
};

void	game_sv_ArtefactHunt::OnPlayerBuyFinished		(u32 id_who, NET_Packet& P)
{

	game_PlayerState*	ps	=	get_id	(id_who);
	if (!ps) return;

	//-------------------------------------------------------------
	u8 NumItems;
	P.r_u8		(NumItems);
	xr_vector<game_PlayerState::BeltItem>	ItemsDesired;
	xr_vector<CSE_Abstract*>				ItemsToDelete;
	xr_vector<game_PlayerState::BeltItem>	ItemsDesiredNew;
	for (u8 i=0; i<NumItems; i++)
	{
		u8 SectID, ItemID;
		P.r_u8(SectID);
		P.r_u8(ItemID);
		ItemsDesired.push_back(game_PlayerState::BeltItem(SectID, ItemID));
	};
	//-------------------------------------------------------------
	CSE_ALifeCreatureActor*		e_Actor	= dynamic_cast<CSE_ALifeCreatureActor*>(Level().Server->game->get_entity_from_eid	(ps->GameID));
	if (!e_Actor)
	{
		ps->BeltItems.clear();
		for (u32 it = 0; it<ItemsDesired.size(); it++)
		{
			ps->BeltItems.push_back(ItemsDesired[it]);
		};
		return;
	};
	xr_vector<u16>::const_iterator	I = e_Actor->children.begin	();
	xr_vector<u16>::const_iterator	E = e_Actor->children.end		();
	for ( ; I != E; ++I) 
	{
		CSE_Abstract* pItem = Level().Server->game->get_entity_from_eid(*I);
		R_ASSERT(pItem);
		if (!IsBuyableItem(pItem)) continue;
		
		bool	found = false;
		xr_vector<game_PlayerState::BeltItem>::iterator	II = ItemsDesired.begin();
		xr_vector<game_PlayerState::BeltItem>::iterator	EI = ItemsDesired.end();
		for ( ; II != EI; ++II) 
		{
			game_PlayerState::BeltItem	DItem = *II; 
			const char* Name = GetItemForSlot(DItem.SlotID, DItem.ItemID, ps);
			if (!Name) continue;
			if (!xr_strcmp(Name, pItem->s_name))
			{
				CheckUpgrades(pItem, DItem.ItemID);
				found = true;
				ItemsDesired.erase(II);
				break;
			};
		};
		if (found) continue;
		ItemsToDelete.push_back(pItem);
	};
	//-------------------------------------------------------------
	xr_vector<CSE_Abstract*>::iterator	IDI = ItemsToDelete.begin();
	xr_vector<CSE_Abstract*>::iterator	EDI = ItemsToDelete.end();
	for ( ; IDI != EDI; ++IDI) 
	{
		CSE_Abstract* pItem = *IDI;

		RemoveItemFromActor	(pItem);
	};
	//-------------------------------------------------------------
	for (u32 it = 0; it<ItemsDesired.size(); it++)
	{
		game_PlayerState::BeltItem	DItem = ItemsDesired[it];
		SpawnWeapon4Actor(e_Actor->ID, GetItemForSlot(DItem.SlotID, DItem.ItemID,  ps), GetItemAddonsForSlot(DItem.SlotID, DItem.ItemID,  ps));
	};
};

BOOL	game_sv_ArtefactHunt::OnTouch				(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	CSE_Abstract*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		CSE_ALifeItemArtefact* pIArtefact	=	dynamic_cast<CSE_ALifeItemArtefact*> (e_what);
		if (pIArtefact)
		{
			m_dwArtefactID = eid_what;
			xrClientData* xrCData	= e_who->owner;
			game_PlayerState*	ps_who	=	&xrCData->ps;
			if (ps_who)
			{
				NET_Packet			P;
				P.w_begin			(M_GAMEMESSAGE);
				P.w_u32				(GMSG_ARTEFACT_TAKEN);
				P.w_u16				(ps_who->GameID);
				P.w_u16				(ps_who->team);
				u_EventSend(P);
			};
			return TRUE;
		};
	}
	return inherited::OnTouch(eid_who, eid_what);
};

BOOL	game_sv_ArtefactHunt::OnDetach				(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	CSE_Abstract*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		CSE_ALifeItemArtefact* pIArtefact	=	dynamic_cast<CSE_ALifeItemArtefact*> (e_what);
		if (pIArtefact)
		{	
			m_dwArtefactID = eid_what;
			xrClientData* xrCData	= e_who->owner;
			game_PlayerState*	ps_who	=	&xrCData->ps;
			if (ps_who)
			{
				NET_Packet			P;
				P.w_begin			(M_GAMEMESSAGE);
				P.w_u32				(GMSG_ARTEFACT_DROPPED);
				P.w_u16				(ps_who->GameID);
				P.w_u16				(ps_who->team);
				u_EventSend(P);
			};
			return TRUE;
		};
	}
	return inherited::OnDetach(eid_who, eid_what);
};

void		game_sv_ArtefactHunt::OnObjectEnterTeamBase	(u16 id, u16 id_zone)
{
	xrServer*			S		= Level().Server;
	CSE_Abstract*		e_who	= S->ID_to_entity(id);		VERIFY(e_who	);
	CSE_Abstract*		e_zone	= S->ID_to_entity(id_zone);	VERIFY(e_zone	);

	CSE_ALifeCreatureActor* eActor = dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	CSE_ALifeTeamBaseZone*	eZoneBase = dynamic_cast<CSE_ALifeTeamBaseZone*> (e_zone);
	if (eActor && eZoneBase)
	{
		if (eActor->g_team() == eZoneBase->m_team)
		{
			xr_vector<u16>& C			= eActor->children;
			xr_vector<u16>::iterator c	= std::find	(C.begin(),C.end(),m_dwArtefactID);
			if (C.end()!=c)
			{
				OnArtefactOnBase(eActor->owner->ID);
			};
		}
		else
		{
		};
	};
};

void		game_sv_ArtefactHunt::OnObjectLeaveTeamBase	(u16 id, u16 id_zone)
{
	xrServer*			S		= Level().Server;
	CSE_Abstract*		e_who	= S->ID_to_entity(id);		VERIFY(e_who	);
	CSE_Abstract*		e_zone	= S->ID_to_entity(id_zone);	VERIFY(e_zone	);

	CSE_ALifeCreatureActor* eActor = dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	CSE_ALifeTeamBaseZone*	eZoneBase = dynamic_cast<CSE_ALifeTeamBaseZone*> (e_zone);
	if (eActor && eZoneBase)
	{
		if (eActor->g_team() == eZoneBase->m_team)
		{
		}
		else
		{
		};
	};
};

void		game_sv_ArtefactHunt::OnArtefactOnBase		(u32 id_who)
{
	game_PlayerState*	ps	=	get_id	(id_who);
	if (!ps) return;
	//-----------------------------------------------
	//add player's points
	ps->kills += 5;
	teams[ps->team].score++;
	//-----------------------------------------------
	//remove artefact from player
	NET_Packet	P;
	P.w_begin				(M_EVENT);
	P.w_u32					(Device.dwTimeGlobal);
	P.w_u16					(GE_OWNERSHIP_REJECT);
	P.w_u16					(ps->GameID);
	P.w_u16					(m_dwArtefactID);
	u_EventSend(P);
	//-----------------------------------------------
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_ARTEFACT_ONBASE);
	P.w_u16				(ps->GameID);
	P.w_u16				(ps->team);
	u_EventSend(P);
};