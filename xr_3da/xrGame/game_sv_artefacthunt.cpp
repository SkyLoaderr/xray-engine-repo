#include "stdafx.h"
#include "game_sv_ArtefactHunt.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "LevelGameDef.h"

void	game_sv_ArtefactHunt::Create					(LPSTR &options)
{
	inherited::Create					(options);

	m_dwArtefactRespawnDelta = get_option_i		(options,"artefactrdelta",0);
	m_ArtefactsNum	= u8(get_option_i		(options,"numberartefacts",1));
	fraglimit = 0;	

	m_delayedRoundEnd = false;
	//---------------------------------------------------
	// loading respawn points for artefacts
	Artefact_rpoints.clear();

	string256	fn_game;
	if (FS.exist(fn_game, "$level$", "level.game")) 
	{
		IReader *F = FS.r_open	(fn_game);
		IReader *O = 0;

		// Load RPoints
		if (0!=(O = F->open_chunk	(RPOINT_CHUNK)))
		{ 
			for (int id=0; O->find_chunk(id); ++id)
			{
				RPoint					R;
				u8						team;
				u8						type;

				O->r_fvector3			(R.P);
				O->r_fvector3			(R.A);
				team					= O->r_u8	();	VERIFY(team>=0 && team<4);
				type					= O->r_u8	();
				u16 res					= O->r_u16	();
				switch (type)
				{
				case rptArtefactSpawn:
					{
						Artefact_rpoints.push_back	(R);
					}break;
				};
				res = 0;
			}
			O->close();
		}

		FS.r_close	(F);
	}
	R_ASSERT2 (!Artefact_rpoints.empty(), "No points to spawn ARTEFACT");
	//---------------------------------------------------------------
	m_dwArtefactSpawnTime = 0;
	m_ArtefactBearerID = 0;
	m_TeamInPosession = 0;
}

void	game_sv_ArtefactHunt::OnRoundStart			()
{
	inherited::OnRoundStart	();
	
	m_delayedRoundEnd = false;
	m_dwArtefactSpawnTime = 0;

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
	/*
	teams[ps_killer->team-1].score = 0;
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->team != ps_killer->team) continue;

		teams[ps_killer->team-1].score += ps->kills;
	};
	*/

//	if (fraglimit && (teams[ps_killer->team-1].score >= fraglimit) )OnFraglimitExceed();

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
			m_ArtefactBearerID = eid_who;
			m_TeamInPosession = A->g_team();
			signal_Syncronize();

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
			m_ArtefactBearerID = 0;
			m_TeamInPosession = 0;
			signal_Syncronize();

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
	CSE_Abstract*		e_who	= S->ID_to_entity(id);		
	if (!e_who)			return;

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
	teams[ps->team-1].score++;
	//-----------------------------------------------
	//remove artefact from player
	NET_Packet			P;
	P.w_begin			(M_EVENT);
	P.w_u32				(Device.dwTimeGlobal);
	P.w_u16				(GE_DESTROY);
	P.w_u16				(m_dwArtefactID);

	Level().Send(P,net_flags(TRUE,TRUE));
	//-----------------------------------------------
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_ARTEFACT_ONBASE);
	P.w_u16				(ps->GameID);
	P.w_u16				(ps->team);
	u_EventSend(P);
	//-----------------------------------------------
	m_ArtefactBearerID	= 0;
	m_TeamInPosession	= 0;
	signal_Syncronize();
	//-----------------------------------------------
	m_dwArtefactSpawnTime = 0;
	if (teams[ps->team-1].score >= m_ArtefactsNum) 
	{
		OnTeamScore(ps->team-1);
		phase = u16((ps->team-1)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
		switch_Phase		(phase);
		OnDelayedRoundEnd("Team Final Score");
	};
};

void	game_sv_ArtefactHunt::SpawnArtefact			()
{
	if (OnClient()) return;
	CSE_Abstract			*E	=	spawn_begin	("af_magnet");
	E->s_flags.set			(M_SPAWN_OBJECT_LOCAL);	// flags

	Assign_Artefact_RPoint	(E);

	spawn_end				(E,Level().Server->GetServer_client()->ID);

	//-----------------------------------------------
	NET_Packet P;
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_ARTEFACT_SPAWNED);
	u_EventSend(P);
};

void	game_sv_ArtefactHunt::Update			()
{
	inherited::Update	();

	switch(phase) 	{
		case GAME_PHASE_TEAM1_SCORES :
		case GAME_PHASE_TEAM2_SCORES :
		case GAME_PHASE_TEAMS_IN_A_DRAW :
			{
//				if (timelimit) if (s32(Device.TimerAsync()-u32(start_time))>timelimit) OnTimelimitExceed();
				if(m_delayedRoundEnd && m_roundEndDelay < Device.TimerAsync()) OnRoundEnd("Finish");
			} break;
		case GAME_PHASE_PENDING : 
			{
//				if ((Device.TimerAsync()-start_time)>u32(30*1000)) OnRoundStart();
			} break;			
		case GAME_PHASE_INPROGRESS:
			{
				if (m_dwArtefactSpawnTime == 0)
				{
					m_dwArtefactSpawnTime = Device.dwTimeGlobal + m_dwArtefactRespawnDelta;
				}
				else
				{
					if (m_dwArtefactSpawnTime != -1 && u32(m_dwArtefactSpawnTime) < Device.dwTimeGlobal)
					{
						m_dwArtefactSpawnTime = -1;
						//time to spawn Artefact;
						SpawnArtefact();
					};
				};
			}break;
	}

}
void	game_sv_ArtefactHunt::OnDelayedRoundEnd		(LPCSTR /**reason/**/)
{
	m_delayedRoundEnd = true;
	m_roundEndDelay = Device.TimerAsync() + 10000;
}

void	game_sv_ArtefactHunt::OnCreate				(u16 id_who)
{
	CSE_Abstract	*pEntity	= get_entity_from_eid(id_who);
	if (!pEntity) return;
	CSE_ALifeItemArtefact* pIArtefact	=	dynamic_cast<CSE_ALifeItemArtefact*> (pEntity);
	if (pIArtefact)
		m_dwArtefactID = pIArtefact->ID;
};

void	game_sv_ArtefactHunt::Assign_Artefact_RPoint	(CSE_Abstract* E)
{
	xr_vector<RPoint>&	rp	= Artefact_rpoints;
	xr_deque<RPointData>	pRPDist;
	RPoint				r;

	xr_vector <u32>					pEnemies;

	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->flags & GAME_PLAYER_FLAG_VERY_VERY_DEAD) continue;
		pEnemies.push_back(it);
	};

	if (!pEnemies.empty())
	{
		pRPDist.clear();

		u32 NumRP = rp.size();
		Fvector DistVect;
		for (it=0; it < NumRP; it++)
		{
			RPoint&				r	= rp[it];
			pRPDist.push_back(RPointData(it, 1000000.0f));

			for (u32 p=0; p<pEnemies.size(); p++)
			{
				xrClientData* xrCData	=	Level().Server->ID_to_client(get_it_2_id(pEnemies[p]));
				if (!xrCData || !xrCData->owner) continue;

				CSE_Abstract* pOwner = xrCData->owner;
				DistVect.sub(pOwner->o_Position, r.P);
				float Dist = DistVect.square_magnitude();
				if (pRPDist[it].MinEnemyDist > Dist) pRPDist[it].MinEnemyDist = Dist;
			};
		};
		std::sort(pRPDist.begin(), pRPDist.end());
		r	= rp[(pRPDist.back()).PointID];
	}
	else
	{
		r	= rp[::Random.randI((int)rp.size())];
	};

	E->o_Position.set	(r.P);
	E->o_Angle.set		(r.A);
};

void				game_sv_ArtefactHunt::OnTimelimitExceed		()
{
	if (teams[0].score == teams[1].score) return;
	u8 winning_team = (teams[0].score < teams[1].score)? 1 : 0;
	OnTeamScore(winning_team);
	phase = u16((winning_team)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd("Team Final Score");
};

void				game_sv_ArtefactHunt::net_Export_State		(NET_Packet& P, u32 id_to)
{
	inherited::net_Export_State(P, id_to);
	P.w_u8			(m_ArtefactsNum);
	P.w_u16			(m_ArtefactBearerID);
	P.w_u8			(m_TeamInPosession);
};