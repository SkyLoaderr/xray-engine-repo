#include "stdafx.h"
#include "game_sv_ArtefactHunt.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrserver.h"
#include "Level.h"
#include "LevelGameDef.h"
#include "Actor.h"
#include "game_cl_base.h"

void	game_sv_ArtefactHunt::Create					(shared_str& options)
{
	inherited::Create					(options);

	m_delayedRoundEnd = false;
	m_eAState = NONE;
	m_bPDAHunt = FALSE;
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
	m_iAfBearerMenaceID = 0;
	teamInPossession = 0;
	m_dwArtefactID = 0;

	bNoLostMessage = false;
	m_bArtefactWasBringedToBase = true;
	//---------------------------------------------------------------
	Set_RankUp_Allowed(false);
}

void	game_sv_ArtefactHunt::OnRoundStart			()
{
	inherited::OnRoundStart	();
	
	m_delayedRoundEnd = false;
	
	m_ArtefactsSpawnedTotal = 0;

	m_dwNextReinforcementTime	= Level().timeServer();
	
	Artefact_PrepareForSpawn();
}

KILL_RES	game_sv_ArtefactHunt::GetKillResult			(game_PlayerState* pKiller, game_PlayerState* pVictim)
{
	KILL_RES Res = inherited::GetKillResult(pKiller, pVictim);
	switch (Res)
	{
	case KR_TEAMMATE:
		{
			if (pVictim->GameID == artefactBearerID)
				Res = KR_TEAMMATE_CRITICAL;
		}break;
	case KR_RIVAL:
		{
			if (pVictim->GameID == artefactBearerID)
				Res = KR_RIVAL_CRITICAL;
		}break;
	default:
		{
		}break;
	};
	return Res;
};

bool	game_sv_ArtefactHunt::OnKillResult			(KILL_RES KillResult, game_PlayerState* pKiller, game_PlayerState* pVictim)
{	
	bool res = true;	
	TeamStruct* pTeam		= GetTeamData(u8(pKiller->team));
	switch (KillResult)
	{
	case KR_TEAMMATE_CRITICAL:
		{
			pKiller->kills -= 1;
			if (pTeam) Player_AddMoney(pKiller, pTeam->m_iM_TargetTeam);
			res = false;
		}break;
	case KR_RIVAL_CRITICAL:
		{
			pKiller->kills += 1;
			pKiller->m_iKillsInRow ++;
			if (pTeam)
			{
				u32 ResMoney = pTeam->m_iM_TargetRival;
				if (pKiller->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))
					ResMoney = s32(ResMoney * pTeam->m_fInvinsibleKillModifier);
				Player_AddMoney(pKiller, ResMoney);
			};
			res = true;
		}break;
	default:
		{
			res = inherited::OnKillResult(KillResult, pKiller, pVictim);
		}break;
	}
	return res;
}


void				game_sv_ArtefactHunt::OnGiveBonus				(KILL_RES KillResult, game_PlayerState* pKiller, game_PlayerState* pVictim, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA)
{
	if (!pKiller) return;	
	switch (KillResult)
	{
	case KR_RIVAL:
		{
			if (pVictim->GameID == m_iAfBearerMenaceID)
				Player_AddExperience(pKiller, READ_IF_EXISTS(pSettings, r_float, "mp_bonus_exp", "assist_kill",0));

			inherited::OnGiveBonus(KR_RIVAL, pKiller, pVictim, KillType, SpecialKillType, pWeaponA);
		}break;
	case KR_RIVAL_CRITICAL:
		{
			inherited::OnGiveBonus(KR_RIVAL, pKiller, pVictim, KillType, SpecialKillType, pWeaponA);
		}break;
	default:
		{
			inherited::OnGiveBonus(KillResult, pKiller, pVictim, KillType, SpecialKillType, pWeaponA);
		}break;
	}	
}

void	game_sv_ArtefactHunt::OnPlayerKillPlayer		(game_PlayerState* ps_killer, game_PlayerState* ps_killed, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA)
{
	inherited::OnPlayerKillPlayer(ps_killer, ps_killed, KillType, SpecialKillType, pWeaponA);
	if (ps_killed && ps_killed->GameID == m_iAfBearerMenaceID)
		m_iAfBearerMenaceID = 0;
};

void	game_sv_ArtefactHunt::OnPlayerReady			(ClientID id)
{
	//	if	(GAME_PHASE_INPROGRESS == phase) return;
	switch (phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			xrClientData* xrCData	=	m_server->ID_to_client(id);
			if (!xrCData || !xrCData->owner) return;
			CSE_Abstract* pOwner	= xrCData->owner;
			CSE_Spectator* pS		= smart_cast<CSE_Spectator*>(pOwner);

			if (pS && m_iReinforcementTime != 0) 
			{
				return;
			}
		}break;
	};
	inherited::OnPlayerReady(id);
}

void	game_sv_ArtefactHunt::assign_RP				(CSE_Abstract* E, game_PlayerState* ps_who)
{
	CSE_Spectator		*pSpectator = smart_cast<CSE_Spectator*>(E);
	if (pSpectator)
	{
		inherited::assign_RP(E, ps_who);
		return;
	};

	CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA)
	{
		inherited::assign_RP(E, ps_who);
		return;
	};
	//-----------------------------------------------------------------------------
	u32		Team		= RP_2_Use(E);
	VERIFY				(rpoints[Team].size());
	
	xr_vector<RPoint>&	rps	= rpoints[Team];
	xr_vector<u32>	rpID; rpID.clear();
	xr_vector<u32>	rpIDEnemy; rpID.clear();
	xr_vector<u32>	EnemyIt; EnemyIt.clear();
	for (u32 p=0; p<rps.size(); p++)
	{
		RPoint rp = rps[p];
		
		bool Blocked = false;
		for (u32 p_it=0; p_it<get_players_count(); ++p_it)
		{
			game_PlayerState* PS		=	get_it			(p_it);
			if (!PS) continue;
			if (PS->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
//			if (u32(PS->team) != Team) continue;
			CObject* pPlayer = Level().Objects.net_Find(PS->GameID);
			if (!pPlayer) continue;

			if (rp.P.distance_to(pPlayer->Position())<=0.4f)
			{
				Blocked = true;
				if(ps_who->team != PS->team && !teams.empty())
				{
					rpIDEnemy.push_back(p);
					EnemyIt.push_back(p_it);
				}
				break;
			}
		};

		if (Blocked || rp.Blocked) 
		{
			continue;
		};
		rpID.push_back(p);
	}
	
	if (rpID.empty() && !rpIDEnemy.empty())
	{
		u32 PointID = ::Random.randI(rpIDEnemy.size());;
		RPoint&	r	= rps[rpIDEnemy[PointID]];
		SetRP(E, &r);
		//---------------------------------------------------------------------
		game_PlayerState* PSE		=	get_it			(EnemyIt[PointID]);
		R_ASSERT2(PSE, "Where is Enemy!!!");
		CGameObject* pPlayer = smart_cast<CGameObject*>(Level().Objects.net_Find(PSE->GameID));
		R_ASSERT2(pPlayer, "Where is Enemy Object!!!");

		NET_Packet		P;
		pPlayer->u_EventGen		(P,GE_GAME_EVENT,pPlayer->ID()	);
		P.w_u16(GAME_EVENT_PLAYER_KILL);
		P.w_u16			(u16(pPlayer->ID())	);
		pPlayer->u_EventSend		(P);
	}
	else
	{		
		R_ASSERT2(rpID.size()>0, "No free Respawn Points!");
		u32 PointID = ::Random.randI(rpID.size());
		RPoint&	r = rps[rpID[PointID]];
		SetRP(E, &r);		
		//-------------------------------------------------------------------
	}

};

void	game_sv_ArtefactHunt::SetRP					(CSE_Abstract* E, RPoint* pRP)
{
	E->o_Position.set	(pRP->P);
	E->o_Angle.set		(pRP->A);

	pRP->Blocked = true;
	pRP->BlockedByID = E->ID;
	pRP->BlockTime = Level().timeServer();
	rpointsBlocked.push_back(pRP);
}

void	game_sv_ArtefactHunt::CheckRPUnblock			()
{
	if (rpointsBlocked.empty()) return;
	for (u32 b=0; b<rpointsBlocked.size(); )
	{
		RPoint* pRP = rpointsBlocked[b];
		if (!pRP->Blocked || pRP->BlockTime+1000 < Level().timeServer())
		{
			pRP->Blocked = false;
			rpointsBlocked.erase(rpointsBlocked.begin()+b);
			continue;
		};
		CObject* pPlayer = Level().Objects.net_Find(pRP->BlockedByID);
		if (!pPlayer || pRP->P.distance_to(pPlayer->Position())<=0.4f)
		{
			pRP->Blocked = false;
			continue;
		};
		b++;
	}
};

u32		game_sv_ArtefactHunt::RP_2_Use				(CSE_Abstract* E)
{
	CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(E);
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
	CSE_ALifeItemWeapon* pWeapon = smart_cast<CSE_ALifeItemWeapon*> (pItem);
	if (pWeapon) return true;
	CSE_ALifeItemGrenade* pGrenade = smart_cast<CSE_ALifeItemGrenade*> (pItem);
	if (pGrenade) return true;
	CSE_ALifeItemCustomOutfit* pOutfit = smart_cast<CSE_ALifeItemCustomOutfit*> (pItem);
	if (pOutfit) return true;
	CSE_ALifeItemTorch* pTorch = smart_cast<CSE_ALifeItemTorch*> (pItem);
	if (pTorch) return true;
	//-----------------------------------------------------------------------------
	CSE_ALifeObject*	pAlifeObject = smart_cast<CSE_ALifeObject*> (pItem);
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
	CSE_ALifeItemWeapon* pWeapon = smart_cast<CSE_ALifeItemWeapon*> (pItem);
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
	CSE_ALifeItemWeapon* pWeapon = smart_cast<CSE_ALifeItemWeapon*> (pItem);
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
	CSE_ALifeCreatureActor*		e_Actor	= smart_cast<CSE_ALifeCreatureActor*>(Level().Server->game->get_entity_from_eid	(ps->GameID));
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

	CSE_ALifeCreatureActor*			A		= smart_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		CSE_ALifeItemArtefact* pIArtefact	=	smart_cast<CSE_ALifeItemArtefact*> (e_what);
		if (pIArtefact)
		{
			artefactBearerID = eid_who;
			m_iAfBearerMenaceID = 0;
			teamInPossession = A->g_team();
			signal_Syncronize();

			m_eAState = IN_POSSESSION;
			xrClientData* xrCData	= e_who->owner;
			game_PlayerState*	ps_who	=	xrCData->ps;
			if (ps_who)
			{
				NET_Packet			P;
				//P.w_begin			(M_GAMEMESSAGE);
				GenerateGameMessage	(P);
				P.w_u32				(GAME_EVENT_ARTEFACT_TAKEN);
				P.w_u16				(ps_who->GameID);
				P.w_u16				(ps_who->team);
				u_EventSend(P);
				//-- Artefact is taken for first time
				if (!m_bWasTaken)
				{
					m_bWasTaken = true;
					TeamStruct* pTeam		= GetTeamData(u8(ps_who->team));
					if (pTeam)
					{					
						u32		cnt = get_players_count();
						for		(u32 it=0; it<cnt; ++it)	
						{
							// init
							xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
							game_PlayerState* pstate	= l_pC->ps;
							if (!l_pC->net_Ready || pstate->Skip || pstate->team != ps_who->team) continue;

							Player_AddExperience(pstate, READ_IF_EXISTS(pSettings, r_float, "mp_bonus_exp", "af_first_take_all",0));
						}
					}
				}
			};
			return TRUE;
		};

		// Actor touches something
		CSE_ALifeItemWeapon*	W			=	smart_cast<CSE_ALifeItemWeapon*> (e_what);
		if (W) 
		{
			//---------------------------------------------------------------
			if (IsBuyableItem(*e_what->s_name)) return TRUE;
			//---------------------------------------------------------------
		}
	}
	return inherited::OnTouch(eid_who, eid_what);
};

BOOL	game_sv_ArtefactHunt::OnDetach				(u16 eid_who, u16 eid_what)
{
	CSE_Abstract*		e_who	= m_server->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= m_server->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= smart_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		CSE_ALifeItemArtefact* pIArtefact	=	smart_cast<CSE_ALifeItemArtefact*> (e_what);
		if (pIArtefact)
		{
			artefactBearerID = 0;
			m_iAfBearerMenaceID = 0;
			teamInPossession = 0;
			signal_Syncronize();
			m_eAState = ON_FIELD;

			xrClientData* xrCData	= e_who->owner;
			game_PlayerState*	ps_who	=	xrCData->ps;
			if (ps_who && !bNoLostMessage)
			{
				NET_Packet			P;
				//P.w_begin			(M_GAMEMESSAGE);
				GenerateGameMessage (P);
				P.w_u32				(GAME_EVENT_ARTEFACT_DROPPED);
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

void		game_sv_ArtefactHunt::OnObjectEnterTeamBase	(u16 id, u16 zone_team)
{
	CSE_Abstract*		e_who	= m_server->ID_to_entity(id);		VERIFY(e_who	);
	CSE_ALifeCreatureActor* eActor = smart_cast<CSE_ALifeCreatureActor*> (e_who);
	if (eActor)
	{
		if (eActor->g_team() == zone_team)
		{
			game_PlayerState* ps = eActor->owner->ps;
			if (ps) ps->setFlag(GAME_PLAYER_FLAG_ONBASE);

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

void		game_sv_ArtefactHunt::OnObjectLeaveTeamBase	(u16 id, u16 zone_team)
{
	CSE_Abstract*		e_who	= m_server->ID_to_entity(id);		
	if (!e_who)			return;

//	CSE_Abstract*		e_zone	= m_server->ID_to_entity(id_zone);	VERIFY(e_zone	);

	CSE_ALifeCreatureActor* eActor = smart_cast<CSE_ALifeCreatureActor*> (e_who);
//	CSE_ALifeTeamBaseZone*	eZoneBase = smart_cast<CSE_ALifeTeamBaseZone*> (e_zone);
	if (eActor /*&& eZoneBase*/)
	{
		if (eActor->g_team() == zone_team)
		{
			game_PlayerState* ps = eActor->owner->ps;
			if (ps) ps->resetFlag(GAME_PLAYER_FLAG_ONBASE);
		}
		else
		{
		};
	};
};

BOOL	g_bAfReturnPlayersToBases = FALSE;

void		game_sv_ArtefactHunt::OnArtefactOnBase		(ClientID id_who)
{
	if (m_iReinforcementTime == -1 || g_bAfReturnPlayersToBases) 
	{
		MoveAllAlivePlayers();
	};
	if (m_iReinforcementTime > 0 || m_iReinforcementTime == -1)
	{
		RespawnAllNotAlivePlayers();
	};
	//-----------------------------------------------------------
	game_PlayerState*	ps	=	get_id	(id_who);
	if (!ps) return;
	//-----------------------------------------------
	//add player's points
	
	Set_RankUp_Allowed(true);
	TeamStruct* pTeam		= GetTeamData(u8(ps->team));
	if (pTeam)
	{
		Player_AddMoney(ps, pTeam->m_iM_TargetSucceed);
		Player_AddExperience(ps, READ_IF_EXISTS(pSettings, r_float, "mp_bonus_exp","target_succeed",0));
		ps->af_count++;

		// Add money to players in this team
		u32		cnt = get_players_count();
		for		(u32 it=0; it<cnt; ++it)	
		{
			// init
			xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
			game_PlayerState* pstate	= l_pC->ps;
			if (!l_pC->net_Ready || pstate->Skip || pstate == ps) continue;
			if (pstate->team == ps->team)
			{
				Player_AddMoney(pstate, pTeam->m_iM_TargetSucceedAll);				
				Player_AddExperience(pstate, READ_IF_EXISTS(pSettings, r_float, "mp_bonus_exp", "target_succeed_all",0));
			}
			else
			{
				Player_AddMoney(pstate, pTeam->m_iM_TargetFailed);
				pstate->experience_New *= READ_IF_EXISTS(pSettings, r_float, "mp_bonus_exp", "target_failed_all_mul",1.0f);				
			};
			
			Player_AddExperience(pstate, 0);
			Player_ExperienceFin(pstate);
		}
	}
	Set_RankUp_Allowed(false);

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
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_ARTEFACT_ONBASE);
	P.w_u16				(ps->GameID);
	P.w_u16				(ps->team);
	u_EventSend(P);
	//-----------------------------------------------
	CActor* pActor = smart_cast<CActor*> (Level().Objects.net_Find(ps->GameID));
	if (pActor)
	{
		pActor->SetfHealth(pActor->g_MaxHealth());
		//-------------------------------------------
		u_EventGen(P, GE_ACTOR_MAX_POWER, ps->GameID);
		m_server->SendTo(id_who,P,net_flags(TRUE,TRUE));	
	};
	//-----------------------------------------------
	signal_Syncronize();
	//-----------------------------------------------
	Artefact_PrepareForSpawn();
	//-----------------------------------------------
	m_bArtefactWasBringedToBase = true;
};

void	game_sv_ArtefactHunt::SpawnArtefact			()
{
//	if (OnClient()) return;

	CSE_Abstract			*E = NULL;
	if (pSettings->line_exist("artefacthunt_gamedata", "artefact"))
		E	=	spawn_begin	(pSettings->r_string("artefacthunt_gamedata", "artefact"));
	else
		return;

	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL);	// flags

	Assign_Artefact_RPoint	(E);

	CSE_Abstract*  af =  spawn_end				(E, m_server->GetServerClient()->ID);
	m_dwArtefactID = af->ID;
	//-----------------------------------------------
	NET_Packet P;
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_ARTEFACT_SPAWNED);
	u_EventSend(P);
	//-----------------------------------------------
	m_eAState = ON_FIELD;

	Artefact_PrepareForRemove();

	signal_Syncronize();
	//-------------------------------------------------
	if (m_bAnomaliesEnabled)	StartAnomalies();
	//-------------------------------------------------
	m_bWasTaken = false;
};

void	game_sv_ArtefactHunt::RemoveArtefact			()
{
	if (m_dwArtefactID != 0)
	{
		NET_Packet	P;
		//-----------------------------------------------
		GenerateGameMessage (P);
		P.w_u32				(GAME_EVENT_ARTEFACT_DESTROYED);
		P.w_u16				(m_dwArtefactID);
		u_EventSend(P);
		//-----------------------------------------------
		u_EventGen(P, GE_DESTROY, m_dwArtefactID);
		Level().Send(P,net_flags(TRUE,TRUE));
		//-----------------------------------------------
	};
	Artefact_PrepareForSpawn();
};

void	game_sv_ArtefactHunt::Update			()
{
	inherited::Update	();

	switch(Phase()) 	
	{
	case GAME_PHASE_TEAM1_ELIMINATED :
	case GAME_PHASE_TEAM2_ELIMINATED :
		{
			if(m_delayedRoundEnd && m_roundEndDelay < Device.TimerAsync())
			{
				switch_Phase	(GAME_PHASE_INPROGRESS);
			};
		}break;
	case GAME_PHASE_PENDING : 
		{
		} break;			
	case GAME_PHASE_INPROGRESS:
		{
			CheckRPUnblock();
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
			{
				CheckForAnyAlivePlayer();
				CheckForTeamElimination();
			};
			CheckForTeamWin();
			//---------------------------------------------------
			if (Artefact_NeedToSpawn()) return;
			if (Artefact_NeedToRemove()) return;
			if (Artefact_MissCheck()) return;
		}break;
	}
}

#ifdef DEBUG
BOOL	g_SV_Force_Artefact_Spawn = FALSE;
#endif

bool	game_sv_ArtefactHunt::ArtefactSpawn_Allowed		()	
{
#ifdef DEBUG
	if (g_SV_Force_Artefact_Spawn) return true;
#endif
///	return true;
	// Check if all players ready
	u32		cnt		= get_players_count	();
	
	u32		TeamAlived[2] = {0, 0};
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;

		if (!l_pC->net_Ready || ps->Skip) continue;
		else
			TeamAlived[ps->team-1]++;
	}

	if (TeamAlived[0] == 0 || TeamAlived[1] == 0) return FALSE;
	
	return TRUE;
};

void	game_sv_ArtefactHunt::OnCreate				(u16 id_who)
{
	inherited::OnCreate(id_who);

	CSE_Abstract	*pEntity	= get_entity_from_eid(id_who);
	if (!pEntity) return;
	CSE_ALifeItemArtefact* pIArtefact	=	smart_cast<CSE_ALifeItemArtefact*> (pEntity);
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

	E->o_Position.set	(r.P);
	E->o_Angle.set		(r.A);
};

void				game_sv_ArtefactHunt::OnTimelimitExceed		()
{
	if ( GetTeamScore(0) == GetTeamScore(1) ) return;
	u8 winning_team = (GetTeamScore(0) < GetTeamScore(1))? 1 : 0;
	OnTeamScore(winning_team, false);
	phase = u16((winning_team)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd("Team Final Score");
};

BOOL	g_bBearerCantSprint = TRUE;
void				game_sv_ArtefactHunt::net_Export_State		(NET_Packet& P, ClientID id_to)
{
	inherited::net_Export_State(P, id_to);
	P.w_u8			(u8(m_dwArtefactsNum));
	P.w_u16			(artefactBearerID);
	P.w_u8			(teamInPossession);
	P.w_u16			(m_dwArtefactID);
	P.w_u8			((u8)g_bBearerCantSprint);

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
	m_iAfBearerMenaceID = 0;
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
	u32		cnt		= get_players_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;

		if (!l_pC->net_Ready || ps->Skip) continue;

		if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) )
		{
			RespawnPlayer(l_pC->ID, true);
			SpawnWeaponsForActor(l_pC->owner, ps);
			Check_ForClearRun(ps);
		};
	}
	signal_Syncronize();

	m_dwNextReinforcementTime = Level().timeServer() + m_iReinforcementTime;
};

void				game_sv_ArtefactHunt::CheckForAnyAlivePlayer()
{
	u32		cnt		= get_players_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;

		if (!l_pC->net_Ready || ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) || ps->Skip)	continue;
		// found at least one alive player
		return;
	};

	// no alive players found
	RespawnAllNotAlivePlayers();
}

bool	game_sv_ArtefactHunt::CheckAlivePlayersInTeam	(s16 Team)
{
	u32		cnt		= get_players_count	();
	u32		cnt_alive = 0;
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (ps->team != Team) continue;
		if (!l_pC->net_Ready || ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) || ps->Skip)	continue;
		cnt_alive++;
	};
	return cnt_alive != 0;
};

void	game_sv_ArtefactHunt::MoveAllAlivePlayers			()
{
	u32		cnt		= get_players_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready || ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) || ps->Skip)	continue;
		if (ps->testFlag(GAME_PLAYER_FLAG_ONBASE)) continue;
		CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(l_pC->owner);
		CActor* pActor = smart_cast<CActor*> (Level().Objects.net_Find(ps->GameID));
		if (!pA || !pActor) continue;

		Fvector Pos = pA->o_Position;
		Fvector Angle = pA->o_Angle;

		assign_RP(l_pC->owner, ps);
		//-----------------------------------------------
		NET_Packet	P2;
		P2.w_begin	(M_EVENT_PACK);
		//-----------------------------------------------
		NET_Packet	P;
		u_EventGen(P, GE_MOVE_ACTOR, ps->GameID);
		P.w_vec3(pA->o_Position);
		P.w_vec3(pA->o_Angle);

		pA->o_Position	= Pos;
		pA->o_Angle		= Angle;
		//------------------------------------------------
		pActor->SetfHealth(pActor->g_MaxHealth());
		//------------------------------------------------
		P2.w_u8(u8(P.B.count));
		P2.w(&P.B.data, P.B.count);
		//------------------------------------------------
		u_EventGen(P, GE_ACTOR_MAX_POWER, ps->GameID);
		//------------------------------------------------
		P2.w_u8(u8(P.B.count));
		P2.w(&P.B.data, P.B.count);
		//------------------------------------------------
		m_server->SendTo(l_pC->ID,P2,net_flags(TRUE,TRUE));		
	};
};

void	game_sv_ArtefactHunt::CheckForTeamElimination()
{
	u8 WinTeam = 0;
	if (!CheckAlivePlayersInTeam(1)) WinTeam = 2;
	else if (!CheckAlivePlayersInTeam(2)) WinTeam = 1;
	if (!WinTeam) return;
	
	SetTeamScore( WinTeam - 1, GetTeamScore(WinTeam-1)+1 );
	//			OnTeamScore(ps_killer->team, false);
	//-----------------------------------------------------------------------------
	u32		cnt = get_players_count();
	TeamStruct* pWTeam		= GetTeamData(WinTeam);
	if (pWTeam)
	{
		for		(u32 it=0; it<cnt; ++it)	
		{
			// init
			xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
			game_PlayerState* pstate	= l_pC->ps;
			if (!l_pC->net_Ready || pstate->Skip || pstate->team != WinTeam) continue;
			Player_AddMoney(pstate, pWTeam->m_iM_RivalsWipedOut);
		};
	};
	//-----------------------------------------------------------------------------
	phase = u16((WinTeam == 1)?GAME_PHASE_TEAM2_ELIMINATED:GAME_PHASE_TEAM1_ELIMINATED);
	switch_Phase(phase);
	OnDelayedRoundEnd("Team Eliminated");
	RemoveArtefact();
}

void	game_sv_ArtefactHunt::CheckForTeamWin()
{
	u8 WinTeam = 0;
	if (GetTeamScore(0) >= m_dwArtefactsNum) WinTeam = 1;
	else if (GetTeamScore(1) >= m_dwArtefactsNum) WinTeam = 2;
	if (!WinTeam) return;
	
	OnTeamScore(WinTeam, false);
	phase = u16((WinTeam == 2)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd("Team Final Score");
}

BOOL	g_bShildedBases = TRUE;
void	game_sv_ArtefactHunt::check_Player_for_Invincibility	(game_PlayerState* ps)
{
	if (!ps) return;
	inherited::check_Player_for_Invincibility(ps);
	if (g_bShildedBases && ps->testFlag(GAME_PLAYER_FLAG_ONBASE))
		ps->setFlag(GAME_PLAYER_FLAG_INVINCIBLE);
};

void	game_sv_ArtefactHunt::Check_ForClearRun		(game_PlayerState* ps)
{
	if (!ps) return;
	if (!ps->m_bClearRun)
	{
		ps->m_bClearRun = true;
		return;
	};
	TeamStruct* pTeam		= GetTeamData(u8(ps->team));
	if (!pTeam) return;	

	Player_AddMoney(ps, pTeam->m_iM_ClearRunBonus);	
};

void game_sv_ArtefactHunt::ReadOptions				(shared_str &options)
{
	inherited::ReadOptions(options);
	//-------------------------------
	m_dwArtefactRespawnDelta			= get_option_i(*options,"ardelta",0)*1000;
	m_dwArtefactsNum					= get_option_i(*options,"anum",1);
	m_dwArtefactStayTime				= get_option_i(*options,"astime",3)*60000;
	fraglimit = 0;	
	//----------------------------------------------------------------------------
	m_iReinforcementTime = 0;
	m_iReinforcementTime				= get_option_i(*options,"reinf",0)*1000;
	if (m_iReinforcementTime<0)	m_iReinforcementTime = -1;
	//----------------------------------------------------------------------------
}

static bool g_bConsoleCommandsCreated_AHUNT = false;
void game_sv_ArtefactHunt::ConsoleCommands_Create	()
{
	inherited::ConsoleCommands_Create();
	//-------------------------------------
	string1024 Cmnd;
	//-------------------------------------	
	CMD_ADD(CCC_SV_Int,"sv_artefact_respawn_delta", (int*)&m_dwArtefactRespawnDelta,0,1800000,g_bConsoleCommandsCreated_AHUNT,Cmnd);
	CMD_ADD(CCC_SV_Int,"sv_artefacts_count", (int*)&m_dwArtefactsNum, 1,100,g_bConsoleCommandsCreated_AHUNT,Cmnd);
	CMD_ADD(CCC_SV_Int,"sv_artefact_stay_time", (int*)&m_dwArtefactStayTime, 0,1800000,g_bConsoleCommandsCreated_AHUNT,Cmnd);
	CMD_ADD(CCC_SV_Int,"sv_reinforcement_time", (int*)&m_iReinforcementTime, 0,1800000,g_bConsoleCommandsCreated_AHUNT,Cmnd);
	//-------------------------------------
	g_bConsoleCommandsCreated_AHUNT = true;
};

void game_sv_ArtefactHunt::ConsoleCommands_Clear	()
{
	inherited::ConsoleCommands_Clear();
	//-----------------------------------
	CMD_CLEAR("sv_artefact_respawn_delta");
	CMD_CLEAR("sv_artefacts_count");
	CMD_CLEAR("sv_artefact_stay_time");
	CMD_CLEAR("sv_reinforcement_time");
};

	//  [7/5/2005]
#ifdef DEBUG

extern	Flags32	dbg_net_Draw_Flags;

void game_sv_ArtefactHunt::OnRender				()
{

	if (dbg_net_Draw_Flags.test(1<<9))
	{
		Fmatrix T; T.identity();
		Fvector V0, V1;
		for (u32 i=0; i<Artefact_rpoints.size(); i++)
		{
			RPoint rp = Artefact_rpoints[i];
			V1 = V0 = rp.P;
			V1.y +=1.0f;

			T.identity();
			RCache.dbg_DrawLINE(Fidentity, V0, V1, D3DCOLOR_XRGB(0, 255, 255));

			float r = .4f;
			T.identity();
			T.scale(r, r/2, r);
			T.translate_add(rp.P);
			RCache.dbg_DrawEllipse(T, D3DCOLOR_XRGB(0, 255, 255));
		}
	};
	inherited::OnRender();
}
#endif
	//  [7/5/2005]

//  [7/29/2005]
bool	game_sv_ArtefactHunt::Player_Check_Rank		(game_PlayerState* ps)
{	
	if (!inherited::Player_Check_Rank(ps)) return false;
	if (ps->af_count < m_aRanks[ps->rank+1].m_iTerms[1]) return false;
	return true;
}
//  [7/29/2005]

void	game_sv_ArtefactHunt::OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	inherited::OnPlayerHitPlayer(id_hitter, id_hitted, P);

	game_PlayerState*	ps_hitter = get_eid(id_hitter);
	game_PlayerState*	ps_hitted = get_eid(id_hitted);

	if (!ps_hitter || !ps_hitted) return;
	if (ps_hitter->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) || ps_hitted->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) return;
	if (ps_hitted->team == ps_hitter->team) return;

	if (ps_hitted->GameID == artefactBearerID)
		m_iAfBearerMenaceID = ps_hitter->GameID;
};