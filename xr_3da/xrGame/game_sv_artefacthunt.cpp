#include "stdafx.h"
#include "game_sv_ArtefactHunt.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrserver.h"
#include "Level.h"
#include "LevelGameDef.h"

void	game_sv_ArtefactHunt::Create					(ref_str& options)
{
	inherited::Create					(options);

	m_dwArtefactRespawnDelta			= get_option_i(*options,"ardelta",0)*1000;
	artefactsNum						= u8(get_option_i(*options,"anum",1));
	m_dwArtefactStayTime				= get_option_i(*options,"astime",3)*60000;
	fraglimit = 0;	
	//----------------------------------------------------------------------------
	m_iReinforcementTime = 0;
	m_iReinforcementTime				= get_option_i(*options,"reinf",0)*1000;
	if (m_iReinforcementTime<0)	m_iReinforcementTime = -1;
	//----------------------------------------------------------------------------

	m_delayedRoundEnd = false;
	m_eAState = NONE;
	//---------------------------------------------------
	// loading respawn points for artefacts
	m_LastRespawnPointID = 0;
	ArtefactsRPoints_ID.clear();
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
	Artefact_PrepareForSpawn();

	m_ArtefactsSpawnedTotal = 0;
	//---------------------------------------------------------------
	artefactBearerID = 0;
	teamInPossession = 0;
	m_dwArtefactID = 0;

	bNoLostMessage = false;
}

void	game_sv_ArtefactHunt::OnRoundStart			()
{
	inherited::OnRoundStart	();
	
	m_delayedRoundEnd = false;
	
	m_ArtefactsSpawnedTotal = 0;

	m_dwNextReinforcementTime	= Level().timeServer();
	
	Artefact_PrepareForSpawn();
}

void	game_sv_ArtefactHunt::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	if (!ps_killed || !ps_killer) return;

//	ps_killed->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
//	ps_killed->deaths				+=	1;

	TeamStruct* pTeam		= GetTeamData(u8(ps_killer->team));

	if (ps_killer == ps_killed || ps_killed->team == ps_killer->team)	
	{
		// By himself
		ps_killer->kills			-=	1;

		if (pTeam)
		{
			if (ps_killer == ps_killed)
				ps_killer->money_for_round	=	ps_killer->money_for_round + pTeam->m_iM_KillSelf;
			else
				if (ps_killed->GameID == artefactBearerID)
					ps_killer->money_for_round	=	ps_killer->money_for_round + pTeam->m_iM_TargetTeam;
				else
					ps_killer->money_for_round	=	ps_killer->money_for_round + pTeam->m_iM_KillTeam;
		}
	} else {
		// Opponent killed - frag 
		ps_killer->kills			+=	1;

		if (pTeam)
			if (ps_killed->GameID == artefactBearerID)
				ps_killer->money_for_round	=	ps_killer->money_for_round + pTeam->m_iM_TargetRival;
			else
				ps_killer->money_for_round	=	ps_killer->money_for_round + pTeam->m_iM_KillRival;

		if (m_iReinforcementTime<0 && !CheckAlivePlayersInTeam(ps_killed->team))
		{
			int x=0;
			x=x;
		}
	}
	// Send Message About Player Killed
	SendPlayerKilledMessage(id_killer, id_killed);

	if (pTeam)
		if (ps_killer->money_for_round < pTeam->m_iM_Min) ps_killer->money_for_round = pTeam->m_iM_Min;

	signal_Syncronize();
}

void	game_sv_ArtefactHunt::OnPlayerReady			(u32 id)
{
	//	if	(GAME_PHASE_INPROGRESS == phase) return;
	switch (phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			xrClientData* xrCData	=	m_server->ID_to_client(id);
			if (!xrCData || !xrCData->owner) return;
			CSE_Abstract* pOwner	= xrCData->owner;
			CSE_Spectator* pS		= dynamic_cast<CSE_Spectator*>(pOwner);

			if (pS && m_iReinforcementTime != 0) 
			{
				return;
			}
		}break;
	};
	inherited::OnPlayerReady(id);
}

u32		game_sv_ArtefactHunt::RP_2_Use				(CSE_Abstract* E)
{
	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA) return 0;

	return u32(pA->g_team());
};

void	game_sv_ArtefactHunt::LoadTeams			()
{
	m_sBaseWeaponCostSection._set("artefacthunt_base_cost");
	if (!pSettings->section_exist(m_sBaseWeaponCostSection))
	{
		R_ASSERT2(0, "No section for base weapon cost for this type of the Game!");
		return;
	};

	LoadTeamData("artefacthunt_team0");
	LoadTeamData("artefacthunt_team1");
	LoadTeamData("artefacthunt_team2");
};

/*
bool	game_sv_ArtefactHunt::IsBuyableItem				(CSE_Abstract* pItem)
{
	if (!pItem) return false;
	CSE_ALifeItemWeapon* pWeapon = dynamic_cast<CSE_ALifeItemWeapon*> (pItem);
	if (pWeapon) return true;
	CSE_ALifeItemGrenade* pGrenade = dynamic_cast<CSE_ALifeItemGrenade*> (pItem);
	if (pGrenade) return true;
	CSE_ALifeItemCustomOutfit* pOutfit = dynamic_cast<CSE_ALifeItemCustomOutfit*> (pItem);
	if (pOutfit) return true;
	CSE_ALifeItemTorch* pTorch = dynamic_cast<CSE_ALifeItemTorch*> (pItem);
	if (pTorch) return true;
	//-----------------------------------------------------------------------------
	CSE_ALifeObject*	pAlifeObject = dynamic_cast<CSE_ALifeObject*> (pItem);
	if (!pAlifeObject) return false;
	if (pAlifeObject->m_tClassID == CLSID_OBJECT_A_VOG25) return true;
	if (pAlifeObject->m_tClassID == CLSID_OBJECT_A_OG7B) return true;
	if (pAlifeObject->m_tClassID == CLSID_OBJECT_A_M209) return true;
	return false;
};
/*
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

	xr_vector<u16>::const_iterator	I = pItem->children.begin	();
	xr_vector<u16>::const_iterator	E = pItem->children.end		();
	for ( ; I != E; ++I) {
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
	pWeapon->m_addon_flags.assign(IItem >> 0x05);
	return true;
};
/*
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
		if (SectID == OUTFIT_SLOT) SectID = APPARATUS_SLOT;
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
*/
BOOL	game_sv_ArtefactHunt::OnTouch				(u16 eid_who, u16 eid_what)
{
	CSE_Abstract*		e_who	= m_server->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= m_server->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		CSE_ALifeItemArtefact* pIArtefact	=	dynamic_cast<CSE_ALifeItemArtefact*> (e_what);
		if (pIArtefact)
		{
			artefactBearerID = eid_who;
			teamInPossession = A->g_team();
			signal_Syncronize();

			m_eAState = IN_POSSESSION;
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

		// Actor touches something
		CSE_ALifeItemWeapon*	W			=	dynamic_cast<CSE_ALifeItemWeapon*> (e_what);
		if (W) 
		{
			//---------------------------------------------------------------
			if (IsBuyableItem(e_what->s_name)) return TRUE;
			//---------------------------------------------------------------
		}
	}
	return inherited::OnTouch(eid_who, eid_what);
};

BOOL	game_sv_ArtefactHunt::OnDetach				(u16 eid_who, u16 eid_what)
{
	CSE_Abstract*		e_who	= m_server->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= m_server->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		CSE_ALifeItemArtefact* pIArtefact	=	dynamic_cast<CSE_ALifeItemArtefact*> (e_what);
		if (pIArtefact)
		{
			artefactBearerID = 0;
			teamInPossession = 0;
			signal_Syncronize();
			m_eAState = ON_FIELD;

			xrClientData* xrCData	= e_who->owner;
			game_PlayerState*	ps_who	=	&xrCData->ps;
			if (ps_who && !bNoLostMessage)
			{
				NET_Packet			P;
				P.w_begin			(M_GAMEMESSAGE);
				P.w_u32				(GMSG_ARTEFACT_DROPPED);
				P.w_u16				(ps_who->GameID);
				P.w_u16				(ps_who->team);
				u_EventSend(P);
			};
			Artefact_PrepareForRemove();

			return TRUE;
		};
	}
	return inherited::OnDetach(eid_who, eid_what);
};

void		game_sv_ArtefactHunt::OnObjectEnterTeamBase	(u16 id, u16 id_zone)
{
	CSE_Abstract*		e_who	= m_server->ID_to_entity(id);		VERIFY(e_who	);
	CSE_Abstract*		e_zone	= m_server->ID_to_entity(id_zone);	VERIFY(e_zone	);

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
	CSE_Abstract*		e_who	= m_server->ID_to_entity(id);		
	if (!e_who)			return;

	CSE_Abstract*		e_zone	= m_server->ID_to_entity(id_zone);	VERIFY(e_zone	);

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

	TeamStruct* pTeam		= GetTeamData(u8(ps->team));
	if (pTeam)
	{
		ps->money_for_round = ps->money_for_round + pTeam->m_iM_TargetSucceed;

		// Add money to players in this team
		u32		cnt = get_count();
		for		(u32 it=0; it<cnt; ++it)	
		{
			// init
			game_PlayerState*	pstate	=	get_it	(it);
			if (pstate->Skip || pstate == ps || pstate->team != ps->team) continue;		
			
			pstate->money_for_round = pstate->money_for_round + pTeam->m_iM_TargetSucceedAll;			
		}
	}

//	teams[ps->team-1].score++;
	SetTeamScore( ps->team-1, GetTeamScore(ps->team-1)+1 );
	//-----------------------------------------------
	bNoLostMessage = true;
	//-----------------------------------------------
	//remove artefact from player
	NET_Packet			P;
	P.w_begin			(M_EVENT);
	P.w_u32				(Device.dwTimeGlobal);
	P.w_u16				(GE_DESTROY);
	P.w_u16				(m_dwArtefactID);

	Level().Send(P,net_flags(TRUE,TRUE));
	//-----------------------------------------------
	bNoLostMessage = false;
	//-----------------------------------------------
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_ARTEFACT_ONBASE);
	P.w_u16				(ps->GameID);
	P.w_u16				(ps->team);
	u_EventSend(P);
	//-----------------------------------------------
	CActor* pActor = dynamic_cast<CActor*> (Level().Objects.net_Find(ps->GameID));
	if (pActor)
	{
		pActor->SetfHealth(100.0f);
	};
	//-----------------------------------------------
	signal_Syncronize();
	//-----------------------------------------------
	Artefact_PrepareForSpawn();

	if ( GetTeamScore(ps->team-1) >= artefactsNum) 
	{
		OnTeamScore(ps->team-1);
		phase = u16((ps->team-1)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
		switch_Phase		(phase);
		OnDelayedRoundEnd("Team Final Score");
	};
};

void	game_sv_ArtefactHunt::SpawnArtefact			()
{
//	if (OnClient()) return;
	CSE_Abstract			*E	=	spawn_begin	("af_magnet");
	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL);	// flags

	Assign_Artefact_RPoint	(E);

	CSE_Abstract*  af =  spawn_end				(E, m_server->GetServer_client()->ID);
	m_dwArtefactID = af->ID;
	//-----------------------------------------------
	NET_Packet P;
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_ARTEFACT_SPAWNED);
	u_EventSend(P);
	//-----------------------------------------------
	m_eAState = ON_FIELD;

	Artefact_PrepareForRemove();

	signal_Syncronize();
	//-------------------------------------------------
	if (m_iReinforcementTime == -1) RespawnAllNotAlivePlayers();
	//-------------------------------------------------
	if (m_bAnomaliesEnabled)	StartAnomalies();
	//-------------------------------------------------
};

void	game_sv_ArtefactHunt::RemoveArtefact			()
{
	//-----------------------------------------------
	NET_Packet	P;
	u_EventGen(P, GE_DESTROY, m_dwArtefactID);
	Level().Send(P,net_flags(TRUE,TRUE));
	//-----------------------------------------------
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_ARTEFACT_DESTROYED);
	u_EventSend(P);
	//-----------------------------------------------
	Artefact_PrepareForSpawn();
};

void	game_sv_ArtefactHunt::Update			()
{
	inherited::Update	();

	switch(phase) 	
	{
	case GAME_PHASE_TEAM1_SCORES :
	case GAME_PHASE_TEAM2_SCORES :
	case GAME_PHASE_TEAMS_IN_A_DRAW :
		{
			if(m_delayedRoundEnd && m_roundEndDelay < Device.TimerAsync()) OnRoundEnd("Finish");
		} break;
	case GAME_PHASE_PENDING : 
		{
			//				if ((Device.TimerAsync()-start_time)>u32(30*1000)) OnRoundStart();
		} break;			
	case GAME_PHASE_INPROGRESS:
		{
			//---------------------------------------------------
			if (m_iReinforcementTime > 0)
			{
				u32 CurTime = Level().timeServer();
				if (m_dwNextReinforcementTime < CurTime)
				{
					RespawnAllNotAlivePlayers();
					m_dwNextReinforcementTime = CurTime + m_iReinforcementTime;
				}
			};
			if (m_iReinforcementTime == -1 && m_dwArtefactID != 0)
				CheckForAnyAlivePlayer();
			//---------------------------------------------------
			if (Artefact_NeedToSpawn()) return;
			if (Artefact_NeedToRemove()) return;
			if (Artefact_MissCheck()) return;
		}break;
	}
}
bool	game_sv_ArtefactHunt::ArtefactSpawn_Allowed		()	
{
//	return true;
	// Check if all players ready
	u32		cnt		= get_count	();
	
	u32		TeamAlived[2] = {0, 0};
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
//		game_PlayerState* ps		=	get_it	(it);
		game_PlayerState* ps	= &l_pC->ps;

		
//		if (/*ps->flags & GAME_PLAYER_FLAG_VERY_VERY_DEAD*/!(ps->flags & GAME_PLAYER_FLAG_READY) || ps->Skip)	continue;
		if (!l_pC->net_Ready || ps->Skip) continue;
		else
			TeamAlived[ps->team-1]++;
	}

	if (TeamAlived[0] == 0 || TeamAlived[1] == 0) return FALSE;
	
	return TRUE;
};

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
	xr_vector<u8>&	rpID	= ArtefactsRPoints_ID;
	xr_deque<RPointData>	pRPDist;
	RPoint				r;

	if (rpID.empty())
	{
		for (u8 i=0; i<rp.size(); i++)
		{
			if (m_LastRespawnPointID == i) continue;
			rpID.push_back(i);
		}
	};

	u8 ID = u8(::Random.randI((int)rpID.size()));
	m_LastRespawnPointID = rpID[ID];
	r	= rp[m_LastRespawnPointID];
	rpID.erase(rpID.begin()+ID);
/*
	xr_vector <u32>					pEnemies;

	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->flags & GAME_PLAYER_FLAG_VERY_VERY_DEAD) continue;
		//pEnemies.push_back(it);
	};

	if (pEnemies.empty())
	{
		r	= rp[::Random.randI((int)rp.size())];
	}
	else
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
*/
	E->o_Position.set	(r.P);
	E->o_Angle.set		(r.A);
};

void				game_sv_ArtefactHunt::OnTimelimitExceed		()
{
	if ( GetTeamScore(0) == GetTeamScore(1) ) return;
	u8 winning_team = (GetTeamScore(0) < GetTeamScore(1))? 1 : 0;
	OnTeamScore(winning_team);
	phase = u16((winning_team)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd("Team Final Score");
};

void				game_sv_ArtefactHunt::net_Export_State		(NET_Packet& P, u32 id_to)
{
	inherited::net_Export_State(P, id_to);
	P.w_u8			(artefactsNum);
	P.w_u16			(artefactBearerID);
	P.w_u8			(teamInPossession);
	P.w_u16			(m_dwArtefactID);

	if (m_iReinforcementTime > 0)
	{
		u32		CurTime = Level().timeServer();
		u32		dTime = m_dwNextReinforcementTime - CurTime;
		P.w_u8			(1);
		P.w_s32			(dTime);
	}
	else
		P.w_u8			(0);
};

void				game_sv_ArtefactHunt::Artefact_PrepareForSpawn	()
{
	m_dwArtefactID			= 0;

	m_eAState = NOARTEFACT;

	m_dwArtefactSpawnTime = Device.dwTimeGlobal + m_dwArtefactRespawnDelta;

	artefactBearerID	= 0;
	teamInPossession	= 0;

	signal_Syncronize();
};

void				game_sv_ArtefactHunt::Artefact_PrepareForRemove	()
{
	m_dwArtefactRemoveTime = Device.dwTimeGlobal + m_dwArtefactStayTime;
	m_dwArtefactSpawnTime = 0;
};

bool				game_sv_ArtefactHunt::Artefact_NeedToSpawn	()
{
	if (m_eAState == ON_FIELD || m_eAState == IN_POSSESSION) return false;

	if (m_dwArtefactID != 0) return false;
	
	VERIFY(m_dwArtefactID == 0);
	
	if (m_dwArtefactSpawnTime < Device.dwTimeGlobal)
	{
		if (ArtefactSpawn_Allowed() || 0 != m_ArtefactsSpawnedTotal  )
		{
			m_dwArtefactSpawnTime = 0;
			//time to spawn Artefact;
			SpawnArtefact();
			return true;
		};
	};
	return false;
};
bool				game_sv_ArtefactHunt::Artefact_NeedToRemove	()
{
	if (m_eAState == IN_POSSESSION) return false;
	if (m_eAState == NOARTEFACT) return false;


	if (m_dwArtefactStayTime == 0) return false;

	if (m_dwArtefactRemoveTime < Device.dwTimeGlobal)
	{
//		VERIFY (m_eAState == ON_FIELD);
		RemoveArtefact();
		return true;
	};
	return false;
}

bool				game_sv_ArtefactHunt::Artefact_MissCheck	()
{
	if (m_eAState == NONE) return false;
	
	if (m_dwArtefactID != 0)
	{
		CSE_Abstract*		E	= get_entity_from_eid	(m_dwArtefactID);
		if (!E) 
		{
			Artefact_PrepareForSpawn();
			return true;
		};
	};
	return false;
}

void				game_sv_ArtefactHunt::RespawnAllNotAlivePlayers()
{
	u32		cnt		= get_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= &l_pC->ps;

		if (!l_pC->net_Ready || ps->Skip) continue;

		if (ps->flags & GAME_PLAYER_FLAG_VERY_VERY_DEAD)
		{
			RespawnPlayer(l_pC->ID, true);
		};
	}
	signal_Syncronize();
};

void				game_sv_ArtefactHunt::CheckForAnyAlivePlayer()
{
	u32		cnt		= get_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= &l_pC->ps;

		if (ps->flags & GAME_PLAYER_FLAG_VERY_VERY_DEAD || ps->Skip)	continue;
		// found at least one alive player
		return;
	};

	// no alive players found
	RespawnAllNotAlivePlayers();
}

bool	game_sv_ArtefactHunt::CheckAlivePlayersInTeam	(s16 Team)
{
	u32		cnt		= get_count	();
	u32		cnt_alive = 0;
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= &l_pC->ps;

		if (ps->team != Team) continue;
		if (ps->flags & GAME_PLAYER_FLAG_VERY_VERY_DEAD || ps->Skip)	continue;
		cnt_alive++;
	};
	return cnt_alive != 0;
};