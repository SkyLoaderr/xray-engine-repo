#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "ui\UIBuyWeaponWnd.h"
#include "level.h"
#include "xrserver.h"
#include "Inventory.h"
#include "CustomZone.h"



void	game_sv_Deathmatch::Create					(ref_str& options)
{
	inherited::Create						(options);
	fraglimit			= get_option_i		(*options,"fraglimit",0);
	timelimit			= get_option_i		(*options,"timelimit",0)*60000;	// in (ms)
	damageblocklimit	= get_option_i		(*options,"dmgblock",5)*1000;	// in (ms)

	if (get_option_i(*options,"ans",1) != 0)
		m_bAnomaliesEnabled	= true;
	else 
		m_bAnomaliesEnabled	= false;
	//-----------------------------------------------------------------------
	int		SpectatorMode = -1;
	m_dwSM_CurViewEntity = 0;
	if (!g_pGamePersistent->bDedicatedServer)
	{
		SpectatorMode = get_option_i		(*options,"spectr",-1);	// in (ms)
		if (SpectatorMode > 0) SpectatorMode *= 1000;
	};
	if (SpectatorMode<0)
		m_bSpectatorMode = false;
	else
	{
		m_bSpectatorMode = true;
		m_dwSM_SwitchDelta = SpectatorMode;
		m_dwSM_LastSwitchTime = 0;
	}
	//-----------------------------------------------------------------------
	/////////////////////////////////////////////////////////////////////////
	LoadTeams();
	/////////////////////////////////////////////////////////////////////////
	switch_Phase(GAME_PHASE_PENDING);
//	switch_Phase(GAME_PHASE_INPROGRESS);

	::Random.seed(GetTickCount());
	/////////////////////////////////////////////////////////////////////////
	m_CorpseList.clear();

	m_AnomalySetsList.clear();
	m_AnomalySetID.clear();
	m_dwLastAnomalySetID	= 1001;
}

void	game_sv_Deathmatch::OnRoundStart			()
{
	/////////////////////////////////////////////////////////////////////////
	for (u32 i=0; i<teams.size(); ++i)
	{
		teams[i].score			= 0;
		teams[i].num_targets	= 0;
	};

	inherited::OnRoundStart	();

	// Respawn all players and some info
	u32		cnt = get_players_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
//		if (!l_pC->net_Ready) continue;

		ps->clear();
//		ClearPlayerState		(ps);
		//---------------------------------------
		SetPlayersDefItems		(ps);
		//---------------------------------------
		Money_SetStart			(get_it_2_id(it));
		//---------------------------------------

		if (ps->Skip) continue;
		SpawnPlayer(get_it_2_id(it), "spectator");
	}
	///////////////////////////////////////////
}

void	game_sv_Deathmatch::OnPlayerKillPlayer		(ClientID id_killer, ClientID id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);

	if(ps_killed){
		ps_killed->setFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
		ps_killed->deaths				+=	1;
	};

	signal_Syncronize();

	if (!ps_killed || !ps_killer) return;
	
	TeamStruct* pTeam		= GetTeamData(u8(ps_killer->team));
	if (ps_killer == ps_killed)	
	{
		// By himself
		ps_killer->kills			-=	1;

		if (pTeam)
			Player_AddMoney(ps_killer, pTeam->m_iM_KillSelf);
	} else {
		// Opponent killed - frag 
		ps_killer->kills			+=	1;
		if (pTeam)
			Player_AddMoney(ps_killer, pTeam->m_iM_KillRival);

//		if (fraglimit && (ps_killer->kills >= fraglimit) )OnFraglimitExceed();
	}

	// Send Message About Player Killed
	SendPlayerKilledMessage(id_killer, id_killed);

	ps_killed->lasthitter			= 0;
	ps_killed->lasthitweapon		= 0;
	ClearPlayerItems		(ps_killed);
	SetPlayersDefItems		(ps_killed);
}


void	game_sv_Deathmatch::OnTimelimitExceed		()
{
	OnRoundEnd	("TIME_limit");
}
void	game_sv_Deathmatch::OnFraglimitExceed		()
{
	OnRoundEnd	("FRAG_limit");
}

#include "UIGameDM.h"
void	game_sv_Deathmatch::Update					()
{
	inherited::Update	();

	switch ( Phase() )
	{
	case GAME_PHASE_INPROGRESS:
		{
			checkForRoundEnd();
/* //moved to game_sv_mp
			// remove corpses if their number exceed limit
			while (m_CorpseList.size()>g_dwMaxCorpses)
			{
				u16 CorpseID = m_CorpseList.front();
				m_CorpseList.pop_front();
				//---------------------------------------------
				NET_Packet			P;
				u_EventGen			(P,GE_DESTROY,CorpseID);
				Level().Send(P,net_flags(TRUE,TRUE));
			};*/
			//-----------------------------------------------------
			if (m_bSpectatorMode)
			{
				if (m_dwSM_LastSwitchTime<Level().timeServer())
					SM_SwitchOnNextActivePlayer();
				
				CUIGameDM* GameDM = smart_cast<CUIGameDM*>(HUD().GetUI()->UIGame());
				if (GameDM) GameDM->SetSpectrModeMsgCaption("");

				CObject* pObject = Level().CurrentViewEntity();
				if (pObject && pObject->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
				{
					char Text[1024];
					sprintf(Text, "Following %s", pObject->cName().c_str());

					if (GameDM) GameDM->SetSpectrModeMsgCaption(Text);
				};
			};
		}
		break;
	case GAME_PHASE_PENDING:
		{
			checkForRoundStart();
		}
		break;
	}
}

bool game_sv_Deathmatch::checkForRoundStart()
{

	if( (Level().timeServer())>u32(10*1000) && AllPlayers_Ready() ){
		OnRoundStart();
		return true;
	};

	return false;
}

bool game_sv_Deathmatch::checkForTimeLimit()
{
	if (timelimit && ((Level().timeServer()-start_time)) > u32(timelimit) )
	{
		OnTimelimitExceed();
		return true;
	};
	return false;
}

bool game_sv_Deathmatch::checkForFragLimit()
{
	if( fraglimit )
	{
		u32		cnt		= get_players_count	();
		for		(u32 it=0; it<cnt; ++it)	
		{
			game_PlayerState* ps		=	get_it	(it);
			if (ps->kills >= fraglimit ){
				OnFraglimitExceed();
				return true;
			}
		}
	}
	return false;
}

bool game_sv_Deathmatch::checkForRoundEnd()
{

	if( checkForTimeLimit() )
			return true;

	if( checkForFragLimit() )
			return true;

	return false;
}


void	game_sv_Deathmatch::SM_SwitchOnNextActivePlayer()
{
	if (!m_bSpectatorMode) return;
	u32		PossiblePlayers[32];
	u32		cnt		= get_players_count	();
	u32		PPlayersCount = 0;

	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready) continue;
		if (ps->Skip) continue;
		if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
		PossiblePlayers[PPlayersCount++] = it;
	};
	
	
	CObject* pNewObject = NULL;
	if (!PPlayersCount)



	{
		xrClientData*	C = NULL;
		C	= m_server->GetServer_client();
		pNewObject =  Level().Objects.net_Find(C->ps->GameID);
	}
	else
	{
		it	= PossiblePlayers[::Random.randI((int)PPlayersCount)];
		xrClientData*	C = NULL;
		C	= (xrClientData*)m_server->client_Get			(it);	
		pNewObject =  Level().Objects.net_Find(C->ps->GameID);
		CActor* pActor = smart_cast<CActor*>(pNewObject);
		if (!pActor || !pActor->g_Alive()) return;
	};
	SM_SwitchOnPlayer(pNewObject);
};

#include "WeaponHUD.h"
void	game_sv_Deathmatch::SM_SwitchOnPlayer(CObject* pNewObject)
{
//	CObject* pNewObject =  Level().Objects.net_Find(ps->GameID);
	if (!pNewObject || !m_bSpectatorMode) return;

//	CObject* pOldObject = Level().CurrentViewEntity();
	Level().SetEntity(pNewObject);
	/*
	if (pOldObject)
	{
		Engine.Sheduler.Unregister	(pOldObject);
		Engine.Sheduler.Register	(pOldObject);
	};
	Engine.Sheduler.Unregister	(pNewObject);
	Engine.Sheduler.Register	(pNewObject, TRUE);
	*/
	CActor* pActor = smart_cast<CActor*> (pNewObject);
	if (pActor)
	{
		CHudItem* pHudItem = smart_cast<CHudItem*>(pActor->inventory().ActiveItem());
		if (pHudItem) 
		{
			pHudItem->GetHUD()->SetCurrentEntityHud(true);
			pHudItem->StartIdleAnim();
		}
		CWeapon* pWeapon = smart_cast<CWeapon*>(pActor->inventory().ActiveItem());
		if (pWeapon)
		{
			pWeapon->InitAddons();
			pWeapon->UpdateAddonsVisibility();



		}
	}
	m_dwSM_CurViewEntity = pNewObject->ID();
	m_dwSM_LastSwitchTime = Level().timeServer() + m_dwSM_SwitchDelta;


}


BOOL	game_sv_Deathmatch::AllPlayers_Ready ()
{
	// Check if all players ready
	u32		cnt		= get_players_count	();
	u32		ready	= 0;
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready)
		{
			if (l_pC->ID == m_server->GetServer_client()->ID)
			{
				continue;
			}
			++ready;
		};
		if (ps->testFlag(GAME_PLAYER_FLAG_READY) )	++ready;
		else
			if (ps->Skip) ++ready;
	}

	if (ready == cnt) return TRUE;
	return FALSE;
};
	

void	game_sv_Deathmatch::OnPlayerReady			(ClientID id)
{
//	if	(GAME_PHASE_INPROGRESS == phase) return;
	switch (phase)
	{
	case GAME_PHASE_PENDING:
		{
			game_PlayerState*	ps	=	get_id	(id);
			if (ps)
			{
				if (ps->testFlag(GAME_PLAYER_FLAG_READY) )	
				{
					ps->resetFlag(GAME_PLAYER_FLAG_READY);
				} 
				else 
				{
					ps->setFlag(GAME_PLAYER_FLAG_READY);
				};
			};
		}break;
	case GAME_PHASE_INPROGRESS:
		{
//			LPCSTR	options			=	get_name_id	(id);
			xrClientData*	xrCData	= (xrClientData*)m_server->ID_to_client	(id);
			game_PlayerState*	ps	=	get_id	(id);
			if (ps->Skip) break;
			if (!(ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))) break;
			xrClientData* xrSCData	=	m_server->GetServer_client();
			if (xrSCData && xrSCData->ID == id && m_bSpectatorMode) 
			{
				SM_SwitchOnNextActivePlayer();
				return;
			}
			//------------------------------------------------------------
			RespawnPlayer(id, false);
			CSE_Abstract* pOwner = xrCData->owner;
			CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(pOwner);
			if(pA)
			{
				SpawnWeaponsForActor(pOwner, ps);
			}
		}break;
	};
}


void game_sv_Deathmatch::OnPlayerDisconnect		(ClientID id_who, LPSTR Name, u16 GameID)
{
//	__super::OnPlayerDisconnect	(id_who);
	inherited::OnPlayerDisconnect	(id_who, Name, GameID);
	
	KillPlayer	(id_who, GameID);
	{
		NET_Packet			P;
		GenerateGameMessage (P);
		P.w_u32				(GAME_EVENT_PLAYER_DISCONNECTED);
		P.w_stringZ			(Name);
		u_EventSend(P);
	};
};




/*
void	game_sv_Deathmatch::OnPlayerWantsDie		(ClientID id_who)
{
	KillPlayer(id_who);
};*/

u32		game_sv_Deathmatch::RP_2_Use				(CSE_Abstract* E)
{
	return 0;
};

void	game_sv_Deathmatch::assign_RP				(CSE_Abstract* E)
{
	VERIFY				(E);
	u32		Team		= RP_2_Use(E);
	VERIFY				(rpoints[Team].size());

	CSE_Spectator		*pSpectator = smart_cast<CSE_Spectator*>(E);
	if (pSpectator)
	{
		inherited::assign_RP(E);
		return;
	};

	CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA)
	{
		inherited::assign_RP(E);
		return;
	};
//-------------------------------------------------------------------------------
	//create Enemies list
	xr_vector <u32>					pEnemies;
	xr_vector <u32>					pFriends;
	
	u32		cnt = get_players_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) ) continue;
		if (ps->team == pA->s_team && !teams.empty()) pFriends.push_back(it);
//		else pEnemies.push_back(it);
	};
	

	if (pEnemies.empty()) 
	{
		u8 OldTeam = pA->s_team;
		pA->s_team = u8(Team);
		inherited::assign_RP(E);
		pA->s_team = OldTeam;
		return;
	};
//-------------------------------------------------------------------------------	
	xr_vector<RPoint>&	rp	= rpoints[Team];
	xr_deque<RPointData>			pRPDist;
	pRPDist.clear();

	u32 NumRP = rp.size();
	Fvector DistVect;
	for (it=0; it < NumRP; it++)
	{
		RPoint&				r	= rp[it];
		pRPDist.push_back(RPointData(it, 1000000.0f));

		for (u32 p=0; p<pEnemies.size(); p++)
		{
			xrClientData* xrCData	=	m_server->ID_to_client(get_it_2_id(pEnemies[p]));
			if (!xrCData || !xrCData->owner) continue;

			CSE_Abstract* pOwner = xrCData->owner;
			DistVect.sub(pOwner->o_Position, r.P);
			float Dist = DistVect.square_magnitude();
			if (pRPDist[it].MinEnemyDist > Dist) pRPDist[it].MinEnemyDist = Dist;
		};
	};
	std::sort(pRPDist.begin(), pRPDist.end());

	RPoint&				r	= rp[(pRPDist.back()).PointID];
	E->o_Position.set	(r.P);
	E->o_Angle.set		(r.A);
};

bool	game_sv_Deathmatch::IsBuyableItem			(LPCSTR	ItemName)
{
	for (u8 i=0; i<TeamList.size(); i++)
	{
		TEAM_WPN_LIST	WpnList = TeamList[i].aWeapons;
		TEAM_WPN_LIST_it pWpnI	= std::find(WpnList.begin(), WpnList.end(), ItemName);
		if (pWpnI != WpnList.end() && ((*pWpnI) == ItemName)) return true;
	};
	return false;
};

void	game_sv_Deathmatch::CheckItem		(game_PlayerState*	ps, PIItem pItem, xr_vector<s16> *pItemsDesired, xr_vector<u16> *pItemsToDelete)
{
	if (!pItem || !pItemsDesired || !pItemsToDelete) return;

//	CSE_Abstract* pItem = Level().Server->game->get_entity_from_eid(*I);
//	R_ASSERT(pItem);
	WeaponDataStruct* pWpnS = NULL;

	TEAM_WPN_LIST	WpnList = TeamList[ps->team].aWeapons;
	TEAM_WPN_LIST_it pWpnI	= std::find(WpnList.begin(), WpnList.end(), *(pItem->cNameSect()));
	if (pWpnI == WpnList.end() || !((*pWpnI) == *(pItem->cNameSect()))) return;
	pWpnS = &(*pWpnI);
	//-------------------------------------------
	bool	found = false;
	for (u32 it = 0; it < pItemsDesired->size(); it++)
	{
		s16 ItemID = (*pItemsDesired)[it];
		if ((ItemID & 0xff1f) != pWpnS->SlotItem_ID) continue;

		found = true;
		pItemsDesired->erase(pItemsDesired->begin()+it);
		//----- Check for Addon Changes ---------------------
		CWeapon		*pWeapon	=	smart_cast<CWeapon*>(pItem);
		if (pWeapon)
		{
			u8 OldAddons  = pWeapon->GetAddonsState();
			u8 NewAddons  = u8(ItemID&0x00ff)>>0x05;
			if (OldAddons != NewAddons)
			{
				CSE_ALifeItemWeapon* pSWeapon = smart_cast<CSE_ALifeItemWeapon*>(get_entity_from_eid(pWeapon->ID()));
				if (pSWeapon)
				{
					pSWeapon->m_addon_flags.zero();
					pSWeapon->m_addon_flags.set(NewAddons, TRUE);
				}

				NET_Packet	P;
				u_EventGen(P, GE_ADDON_CHANGE, pWeapon->ID());
				P.w_u8(NewAddons);
				u_EventSend(P);
			}
		};
		//---------------------------------------------------
		break;
	};
	if (found) return;
	pItemsToDelete->push_back(pItem->ID());
};


void	game_sv_Deathmatch::OnPlayerBuyFinished		(ClientID id_who, NET_Packet& P)
{
	game_PlayerState*	ps	=	get_id	(id_who);
	if (!ps || ps->Skip) return;
	
	P.r_s16(ps->LastBuyAcount);

	xr_vector<s16>		ItemsDesired;

	u8 NumItems;
	P.r_u8(NumItems);
	for (u8 i=0; i<NumItems; i++)
	{
		s16	ItemID;
		P.r_s16(ItemID);
		ItemsDesired.push_back(ItemID);
	};

	CSE_ALifeCreatureActor*		e_Actor	= smart_cast<CSE_ALifeCreatureActor*>(get_entity_from_eid	(ps->GameID));
/*	
	if (e_Actor)
	{
		//-------------------------------------------------------------------------
		xr_vector<u16>				ItemsToDelete;

		xr_vector<u16>::const_iterator	I = e_Actor->children.begin	();
		xr_vector<u16>::const_iterator	E = e_Actor->children.end		();
		for ( ; I != E; ++I) 
		{
			PIItem pItem = smart_cast<PIItem> (Level().Objects.net_Find(*I));
			CheckItem(ps, pItem, &ItemsDesired, &ItemsToDelete);
		};
		//-------------------------------------------------------------
		xr_vector<u16>::iterator	IDI = ItemsToDelete.begin();
		xr_vector<u16>::iterator	EDI = ItemsToDelete.end();
		for ( ; IDI != EDI; ++IDI) 
		{
			NET_Packet			P;
			u_EventGen			(P,GE_DESTROY,*IDI);
			Level().Send(P,net_flags(TRUE,TRUE));
		};
		//-------------------------------------------------------------
	}
*/
	CActor* pActor = smart_cast<CActor*>(Level().Objects.net_Find	(ps->GameID));
	if (pActor)
	{
		PIItem pItem = NULL;
		xr_vector<u16>				ItemsToDelete;

		//провер€ем по€с
		TIItemList::const_iterator	IBelt = pActor->inventory().m_belt.begin();
		TIItemList::const_iterator	EBelt = pActor->inventory().m_belt.end();

		for ( ; IBelt != EBelt; ++IBelt) 
		{
			pItem = (*IBelt);
			CheckItem(ps, pItem, &ItemsDesired, &ItemsToDelete);
		};

		//провер€ем слоты
		TISlotArr::const_iterator	ISlot = pActor->inventory().m_slots.begin();
		TISlotArr::const_iterator	ESlot = pActor->inventory().m_slots.end();

		for ( ; ISlot != ESlot; ++ISlot) 
		{
			pItem = (*ISlot).m_pIItem;
			CheckItem(ps, pItem, &ItemsDesired, &ItemsToDelete);
		};
		//-------------------------------------------------------------
		xr_vector<u16>::iterator	IDI = ItemsToDelete.begin();
		xr_vector<u16>::iterator	EDI = ItemsToDelete.end();
		for ( ; IDI != EDI; ++IDI) 
		{
			NET_Packet			P;
			u_EventGen			(P,GE_DESTROY,*IDI);
			Level().Send(P,net_flags(TRUE,TRUE));
		};
		//-------------------------------------------------------------
	};
	//-------------------------------------------------------------
	ps->pItemList.clear();
	for (u32 it = 0; it<ItemsDesired.size(); it++)
	{
		ps->pItemList.push_back(ItemsDesired[it]);
	};
	//-------------------------------------------------------------
	if (!e_Actor) return;

	SpawnWeaponsForActor(e_Actor, ps);
};

void	game_sv_Deathmatch::ClearPlayerState		(game_PlayerState* ps)
{
	if (!ps) return;

	ps->kills				= 0;
	ps->deaths				= 0;
	ps->lasthitter		= 0;
	ps->lasthitweapon		= 0;

	ClearPlayerItems		(ps);
};

void	game_sv_Deathmatch::ClearPlayerItems		(game_PlayerState* ps)
{
	ps->pItemList.clear();
	ps->LastBuyAcount = 0;
};

void	game_sv_Deathmatch::SetPlayersDefItems		(game_PlayerState* ps)
{
	ps->pItemList.clear();
	ps->LastBuyAcount = 0;
	//-------------------------------------------

	//fill player with default items
	if (ps->team < s16(TeamList.size()))
	{
		DEF_ITEMS_LIST	aDefItems = TeamList[ps->team].aDefaultItems;

		for (u16 i=0; i<aDefItems.size(); i++)
		{
//			game_PlayerState::PlayersItem NewItem;
//			NewItem.ItemID		= aDefItems[i];
//			NewItem.ItemCost	= 0;
			ps->pItemList.push_back(aDefItems[i]);
		}
	};
};

//void	game_sv_Deathmatch::SpawnWeapon4Actor		(u32 actorId,  LPCSTR N, u8 Addons)
//{
//	if (!N) return;
//	
//	CSE_Abstract			*E	=	spawn_begin	(N);
//	E->ID_Parent = u16(actorId);
//
//	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL);	// flags
//
//	/////////////////////////////////////////////////////////////////////////////////
//	//если это оружие - спавним его с полным магазином
//	CSE_ALifeItemWeapon		*pWeapon	=	smart_cast<CSE_ALifeItemWeapon*>(E);
//	if (pWeapon)
//	{
//		pWeapon->a_elapsed = pWeapon->get_ammo_magsize();
//
//		pWeapon->m_addon_flags.assign(Addons);
//	};
//	/////////////////////////////////////////////////////////////////////////////////
//
//	spawn_end				(E,m_server->GetServer_client()->ID);
//};

void	game_sv_Deathmatch::SpawnWeaponsForActor(CSE_Abstract* pE, game_PlayerState*	ps)
{
	CSE_ALifeCreatureActor* pA	=	smart_cast<CSE_ALifeCreatureActor*>(pE);
	R_ASSERT2(pA, "Owner not a Actor");
	if (!pA) return;

	if (!(ps->team < s16(TeamList.size()))) return;

	TEAM_WPN_LIST	WpnList = TeamList[ps->team].aWeapons;
	
	for (u32 i = 0; i<ps->pItemList.size(); i++)
	{
		u16 ItemID = ps->pItemList[i];
		TEAM_WPN_LIST_it pWpnI	= std::find(WpnList.begin(), WpnList.end(), (ItemID & 0xFF1f));

		if (pWpnI == WpnList.end() || !((*pWpnI) == (ItemID & 0xFF1f))) continue;

		SpawnWeapon4Actor(pA->ID, (*pWpnI).WeaponName.c_str(), u8(ItemID & 0x00FF)>>0x05);
	};
	
	Player_AddMoney(ps, ps->LastBuyAcount);
};
void	game_sv_Deathmatch::LoadWeaponsForTeam		(char* caSection, TEAM_WPN_LIST *pTeamWpnList)
{
	
	R_ASSERT(xr_strcmp(caSection,""));

	pTeamWpnList->clear();

	for (int i = 1; i < 20; ++i)
	{
		// »м€ пол€
		string16			wpnSection;	
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(caSection, wpnSection)) 
		{
//			pTeamWpnList->push_back(wpnOneType);
			continue;
		}

		string256			wpnNames, wpnSingleName;
		// „итаем данные этого пол€
		std::strcpy(wpnNames, pSettings->r_string(caSection, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		// теперь дл€ каждое им€ оружи€, разделенные зап€тыми, заносим в массив
		for (u32 j = 0; j < count; ++j)
		{
			_GetItem(wpnNames, j, wpnSingleName);
			R_ASSERT2(pSettings->line_exist(m_sBaseWeaponCostSection, wpnSingleName), "No base item cost!");

			WeaponDataStruct	NewWpnData;

			NewWpnData.SlotItem_ID = (s16(i-1) << 8) | s16(j);
			NewWpnData.WeaponName = wpnSingleName;			
			NewWpnData.Cost = pSettings->r_s16(m_sBaseWeaponCostSection, wpnSingleName);

			std::strcat(wpnSingleName, "_cost");
			if (pSettings->line_exist(caSection, wpnSingleName))
				NewWpnData.Cost = pSettings->r_s16(caSection, wpnSingleName);
			
			pTeamWpnList->push_back(NewWpnData);
		}
	}
};

void	game_sv_Deathmatch::LoadSkinsForTeam		(char* caSection, TEAM_SKINS_NAMES* pTeamSkins)
{
	string256			SkinSingleName;
	string4096			Skins;

	// ѕоле strSectionName должно содержать им€ секции
	R_ASSERT(xr_strcmp(caSection,""));

	pTeamSkins->clear();

	// »м€ пол€
	if (!pSettings->line_exist(caSection, "skins")) return;

	// „итаем данные этого пол€
	std::strcpy(Skins, pSettings->r_string(caSection, "skins"));
	u32 count	= _GetItemCount(Skins);
	// теперь дл€ каждое им€ оружи€, разделенные зап€тыми, заносим в массив
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(Skins, i, SkinSingleName);
		pTeamSkins->push_back(SkinSingleName);
	};
};


void	game_sv_Deathmatch::LoadDefItemsForTeam	(char* caSection, TEAM_WPN_LIST *pWpnList, DEF_ITEMS_LIST* pDefItems)
{
	string256			ItemName;
	string4096			DefItems;

	// ѕоле strSectionName должно содержать им€ секции
	R_ASSERT(xr_strcmp(caSection,""));

	pDefItems->clear();

	// »м€ пол€
	if (!pSettings->line_exist(caSection, "default_items")) return;

	// „итаем данные этого пол€
	std::strcpy(DefItems, pSettings->r_string(caSection, "default_items"));
	u32 count	= _GetItemCount(DefItems);
	// теперь дл€ каждое им€ оружи€, разделенные зап€тыми, заносим в массив
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(DefItems, i, ItemName);
		TEAM_WPN_LIST_it pWpnI	= std::find(pWpnList->begin(), pWpnList->end(), ItemName);
		if (pWpnI == pWpnList->end() || !((*pWpnI) == ItemName)) continue;
		
		pDefItems->push_back((*pWpnI).SlotItem_ID);
	};
};


void	game_sv_Deathmatch::SetSkin					(CSE_Abstract* E, u16 Team, u16 ID)
{
	if (!E) return;
	//-------------------------------------------
	CSE_Visual* pV = smart_cast<CSE_Visual*>(E);
	if (!pV) return;
	//-------------------------------------------
	string256 SkinName;
	std::strcpy(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
	//загружены ли скины дл€ этой комманды
//	if (SkinID != -1) ID = u16(SkinID);

	if (!TeamList.empty()	&&
		TeamList.size() > Team	&&
		!TeamList[Team].aSkins.empty())
	{
		//загружено ли достаточно скинов дл€ этой комманды
		if (TeamList[Team].aSkins.size() > ID)
		{
			std::strcat(SkinName, TeamList[Team].aSkins[ID].c_str());
		}
		else
			std::strcat(SkinName, TeamList[Team].aSkins[0].c_str());
	}
	else
	{
		//скины дл€ такой комманды не загружены
		switch (Team)
		{
		case 0:
			std::strcat(SkinName, "stalker_hood_multiplayer");
			break;
		case 1:
			std::strcat(SkinName, "soldat_beret");
			break;
		case 2:
			std::strcat(SkinName, "stalker_black_mask");
			break;
		default:
			R_ASSERT2(0,"Unknown Team");
			break;
		};
	};
	std::strcat(SkinName, ".ogf");
	Msg("* Skin - %s", SkinName);
	int len = xr_strlen(SkinName);
	R_ASSERT2(len < 64, "Skin Name is too LONG!!!");
	pV->set_visual(SkinName);
	//-------------------------------------------
};

void	game_sv_Deathmatch::OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	CSE_Abstract*		e_hitter		= get_entity_from_eid	(id_hitter	);
	CSE_Abstract*		e_hitted		= get_entity_from_eid	(id_hitted	);

	if (!e_hitter || !e_hitted) return;

	CSE_ALifeCreatureActor*		a_hitter		= smart_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_ALifeCreatureActor*		a_hitted		= smart_cast <CSE_ALifeCreatureActor*> (e_hitted);

	if (!a_hitter || !a_hitted) return;

//	game_PlayerState*	ps_hitter = &a_hitter->owner->ps;
	game_PlayerState*	ps_hitted = a_hitted->owner->ps;

	u32 BCount = P.B.count;
	//---------------------------------------
	// read hit event
	u32 PowRPos, ImpRPos;

	u16				WeaponID;
	Fvector			dir;
	float			power, impulse;
	s16				element;
	Fvector			position_in_bone_space;
	u16				hit_type;

	u32	RPos = P.r_pos;
	P.r_u16			(WeaponID);
	P.r_dir			(dir);						PowRPos = P.r_pos;
	P.r_float		(power);
	P.r_s16			(element);
	P.r_vec3		(position_in_bone_space);	ImpRPos = P.r_pos;
	P.r_float		(impulse);
	P.r_u16			(hit_type);	//hit type
	P.r_pos = RPos;
	
	//---------------------------------------
	if (Device.dwTimeGlobal<ps_hitted->RespawnTime + damageblocklimit)
	{
		power = 0;
		impulse = 0;
	}
	//---------------------------------------
	P.B.count	= PowRPos;	P.w_float(power);
	P.B.count	= ImpRPos;	P.w_float(impulse);
	//---------------------------------------
	if (power > 0)
	{
		ps_hitted->lasthitter = a_hitter->ID;
		ps_hitted->lasthitweapon = WeaponID;
	};
	//---------------------------------------
	P.B.count	= BCount;
};


void	game_sv_Deathmatch::LoadTeams			()
{
	m_sBaseWeaponCostSection._set("deathmatch_base_cost");
	if (!pSettings->section_exist(m_sBaseWeaponCostSection))
	{
		R_ASSERT2(0, "No section for base weapon cost for this type of the Game!");
		return;
	};

	LoadTeamData("deathmatch_team0");
};

s16		game_sv_Deathmatch::GetMoneyAmount			(char* caSection, char* caMoneyStr)
{
	if (pSettings->line_exist(caSection, caMoneyStr))
		return pSettings->r_s16(caSection, caMoneyStr);

	return 0;
};

void	game_sv_Deathmatch::LoadTeamData			(char* caSection)
{
	TeamStruct	NewTeam;

	std::strcpy(NewTeam.caSection, caSection);

	LoadWeaponsForTeam	(caSection, &NewTeam.aWeapons);
	LoadSkinsForTeam	(caSection, &NewTeam.aSkins);
	LoadDefItemsForTeam	(caSection, &NewTeam.aWeapons, &NewTeam.aDefaultItems);	
	//-------------------------------------------------------------
	if( pSettings->section_exist(caSection) )//money
	{
		NewTeam.m_iM_Start				= GetMoneyAmount(caSection, "money_start");
		NewTeam.m_iM_OnRespawn			= GetMoneyAmount(caSection, "money_respawn");
		NewTeam.m_iM_Min				= GetMoneyAmount(caSection, "money_min");

		NewTeam.m_iM_KillRival			= GetMoneyAmount(caSection, "kill_rival");
		NewTeam.m_iM_KillSelf			= GetMoneyAmount(caSection, "kill_self");
		NewTeam.m_iM_KillTeam			= GetMoneyAmount(caSection, "kill_team");

		NewTeam.m_iM_TargetRival		= GetMoneyAmount(caSection, "target_rival");
		NewTeam.m_iM_TargetTeam			= GetMoneyAmount(caSection, "target_team");
		NewTeam.m_iM_TargetSucceed		= GetMoneyAmount(caSection, "target_succeed");
		NewTeam.m_iM_TargetSucceedAll	= GetMoneyAmount(caSection, "target_succeed_all");

		NewTeam.m_iM_RoundWin			= GetMoneyAmount(caSection, "round_win");
		NewTeam.m_iM_RoundLoose			= GetMoneyAmount(caSection, "round_loose");
		NewTeam.m_iM_RoundDraw			= GetMoneyAmount(caSection, "round_draw");

		NewTeam.m_iM_RoundWin_Minor		= GetMoneyAmount(caSection, "round_win_minor");
		NewTeam.m_iM_RoundLoose_Minor	= GetMoneyAmount(caSection, "round_loose_minor");

		NewTeam.m_iM_RivalsWipedOut		= GetMoneyAmount(caSection, "rivals_wiped_out");
	};
	//-------------------------------------------------------------
	TeamList.push_back(NewTeam);
};

void game_sv_Deathmatch::OnDestroyObject			(u16 eid_who)
{
	if (eid_who == m_dwSM_CurViewEntity && m_bSpectatorMode)
	{
		SM_SwitchOnNextActivePlayer();
	};

	for (u32 i=0; i<m_CorpseList.size();)
	{
		if (m_CorpseList[i] == eid_who)
		{
			m_CorpseList.erase(m_CorpseList.begin()+i);
		}
		else i++;
	};
};



void game_sv_Deathmatch::Money_SetStart			(ClientID	id_who)
{
	game_PlayerState*	ps_who	=	get_id	(id_who);
	if (!ps_who) return;
	ps_who->money_for_round = 0;
	if (ps_who->team < 0) return;
	TeamStruct*	pTeamData = GetTeamData(u8(ps_who->team));
	if (!pTeamData) return;
	ps_who->money_for_round = pTeamData->m_iM_Start;
}

/*
s16 game_sv_Deathmatch::GetItemCost			(u32 id_who, s16 ItemID)
{
	s16	res = 0;
	game_PlayerState*	ps	=	get_id	(id_who);
	if (!ps) return res;

	if (!(ps->team < s16(TeamList.size()))) return res;

	TEAM_WPN_LIST	WpnList = TeamList[ps->team].aWeapons;
	
	TEAM_WPN_LIST_it pWpnI	= std::find(WpnList.begin(), WpnList.end(), (ItemID & 0xFF1f));
	
	if (pWpnI == WpnList.end() || (*pWpnI).SlotItem_ID != (ItemID & 0xFF1f)) return res;

	WeaponDataStruct* pWpnS = &(*pWpnI);
	res = s16(pWpnS->Cost);
	//--------- Addons -----------------------------
	u8 Addons = (u8(ItemID & 0x00ff)) >> 0x05;
	
	string256 AddonName;
	if (Addons & CSE_ALifeItemWeapon::eWeaponAddonScope)
	{
		if (pSettings->line_exist(pWpnS->WeaponName.c_str(), "scope_name"))
		{
			std::strcpy(AddonName, pSettings->r_string(pWpnS->WeaponName.c_str(), "scope_name"));
			TEAM_WPN_LIST_it pWpnAddon	= std::find(WpnList.begin(), WpnList.end(), AddonName);
			if (pWpnAddon != WpnList.end() && ((*pWpnAddon) == AddonName))
			{
				res = res + s16((*pWpnAddon).Cost);
			}
		}
	}
	if (Addons & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)
	{
		if (pSettings->line_exist(pWpnS->WeaponName.c_str(), "wpn_addon_grenade_launcher"))
		{
			std::strcpy(AddonName, pSettings->r_string(pWpnS->WeaponName.c_str(), "wpn_addon_grenade_launcher"));
		}
	}
	if (Addons & CSE_ALifeItemWeapon::eWeaponAddonSilencer)
	{
		if (pSettings->line_exist(pWpnS->WeaponName.c_str(), "silencer_name"))
		{
			std::strcpy(AddonName, pSettings->r_string(pWpnS->WeaponName.c_str(), "silencer_name"));
		}
	}
	return res;
}
*/
void	game_sv_Deathmatch::RemoveItemFromActor	(CSE_Abstract* pItem)
{
	if (!pItem) return;
	//-------------------------------------------------------------
	CSE_ALifeItemWeapon* pWeapon = smart_cast<CSE_ALifeItemWeapon*> (pItem);
	if (pWeapon)
	{
	};
	//-------------------------------------------------------------
	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,pItem->ID);
	Level().Send(P,net_flags(TRUE,TRUE));
};

void	game_sv_Deathmatch::OnTeamScore	(u32 Team, bool Minor)
{
	TeamStruct* pTeam		= GetTeamData(u8(Team));
	if (!pTeam) return;
	
	u32		cnt = get_players_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready) continue;
		if (ps->Skip) continue;		

		if (ps->team == s16(Team))
			Player_AddMoney(ps, ((Minor) ? pTeam->m_iM_RoundWin_Minor : pTeam->m_iM_RoundWin));
		else
			Player_AddMoney(ps, ((Minor) ? pTeam->m_iM_RoundLoose_Minor : pTeam->m_iM_RoundLoose));
	}
}


void game_sv_Deathmatch::net_Export_State		(NET_Packet& P, ClientID id_to)
{
	inherited::net_Export_State(P, id_to);

	P.w_s32			(fraglimit);
	P.w_s32			(timelimit);
	// Teams
	P.w_u16			(u16(teams.size()));
	for (u32 t_it=0; t_it<teams.size(); ++t_it)
	{
		P.w				(&teams[t_it],sizeof(game_TeamState));
	}

}

int game_sv_Deathmatch::GetTeamScore(u32 idx)
{
	VERIFY( (idx>=0) && (idx<teams.size()) );
	return teams[idx].score;
}
void game_sv_Deathmatch::OnPlayerChangeSkin(ClientID id_who, u8 skin) 
{
	game_PlayerState*	ps_who	=	get_id	(id_who);
	if (!ps_who) return;
	ps_who->skin = skin;

	KillPlayer(id_who, ps_who->GameID);
}

void game_sv_Deathmatch::SetTeamScore(u32 idx, int val)
{
	VERIFY( (idx>=0) && (idx<teams.size()) );
	teams[idx].score = val;
}


void	game_sv_Deathmatch::LoadAnomalySets			()
{
	//-----------------------------------------------------------
	if (!m_AnomalySetsList.empty()) return;
	//-----------------------------------------------------------
	char* ASetBaseName = GetAnomalySetBaseName();

	string256 SetName, AnomaliesNames, AnomalyName;
	ANOMALIES		AnomalySingleSet;
	for (u32 i=0; i<20; i++)
	{
		AnomalySingleSet.clear();

		sprintf(SetName, "set%i", i);
		if (!Level().pLevel->line_exist(ASetBaseName, SetName))
			continue;

		std::strcpy(AnomaliesNames, Level().pLevel->r_string(ASetBaseName, SetName));
		u32 count	= _GetItemCount(AnomaliesNames);
		if (!count) continue;

		for (u32 j=0; j<count; j++)
		{
			_GetItem(AnomaliesNames, j, AnomalyName);
			AnomalySingleSet.push_back(AnomalyName);
		};

		if (AnomalySingleSet.empty()) continue;
		m_AnomalySetsList.push_back(AnomalySingleSet);
	};
};

void	game_sv_Deathmatch::StartAnomalies			()
{
	if (m_AnomalySetsList.empty())
		LoadAnomalySets();
	if (m_AnomalySetsList.empty()) return;	

	xr_vector<u8>&	ASetID	= m_AnomalySetID;
	if (ASetID.empty())
	{
		for (u8 i=0; i<m_AnomalySetsList.size(); i++)
		{
			if (m_dwLastAnomalySetID == i) continue;
			ASetID.push_back(i);
		};
	};

	u8 ID = u8(::Random.randI((int)ASetID.size()));
	u32 NewAnomalySetID = ASetID[ID];
	ASetID.erase(ASetID.begin()+ID);
	///////////////////////////////////////////////////
	if (m_dwLastAnomalySetID < m_AnomalySetsList.size())
	{
		ANOMALIES* OldAnomalies = &(m_AnomalySetsList[m_dwLastAnomalySetID]);
		for (u32 i=0; i<OldAnomalies->size(); i++)
		{
			const char *pName = ((*OldAnomalies)[i]).c_str();
			CCustomZone* pZone = smart_cast<CCustomZone*> (Level().Objects.FindObjectByName(pName));
			if (!pZone) continue;
//			if (pZone->IsEnabled())
				pZone->ZoneDisable();
		};
	};
	///////////////////////////////////////////////////
	m_dwLastAnomalySetID = NewAnomalySetID;

	ANOMALIES* NewAnomalies = &(m_AnomalySetsList[NewAnomalySetID]);
	for (u32 i=0; i<NewAnomalies->size(); i++)
	{
		const char *pName = ((*NewAnomalies)[i]).c_str();
		CCustomZone* pZone = smart_cast<CCustomZone*> (Level().Objects.FindObjectByName(pName));
		if (!pZone) continue;
//		if (!pZone->IsEnabled())
			pZone->ZoneEnable();
	};
};

BOOL	game_sv_Deathmatch::OnTouch			(u16 eid_who, u16 eid_what)
{
	CSE_Abstract*		e_who	= m_server->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= m_server->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= smart_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		// Actor touches something
		CSE_ALifeItemWeapon*	W			=	smart_cast<CSE_ALifeItemWeapon*> (e_what);
		if (W) 
		{
			// Weapon
			xr_vector<u16>&	C			=	A->children;
			u8 slot						=	W->get_slot	();
			for (u32 it=0; it<C.size(); ++it)
			{
				CSE_Abstract*		Et	= m_server->ID_to_entity				(C[it]);
				if (0==Et)				continue;
				CSE_ALifeItemWeapon*		T	= smart_cast<CSE_ALifeItemWeapon*>	(Et);
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
		
		CSE_ALifeItemAmmo* pIAmmo			=	smart_cast<CSE_ALifeItemAmmo*> (e_what);
		if (pIAmmo)
		{
			//Ammo
			return TRUE;
		};

		CSE_ALifeItemGrenade* pIGrenade		=	smart_cast<CSE_ALifeItemGrenade*> (e_what);
		if (pIGrenade)
		{
			//Grenade
			return TRUE;
		};

		CSE_ALifeItemCustomOutfit* pOutfit		= smart_cast<CSE_ALifeItemCustomOutfit*> (e_what);
		if (pOutfit)
		{
			//Possibly Addons and/or Outfits
			return TRUE;
		};

		//---------------------------------------------------------------
		if (IsBuyableItem(e_what->s_name)) return TRUE;
		//---------------------------------------------------------------
	};
	// We don't know what the hell is it, so disallow ownership just for safety 
	return FALSE;
}

BOOL	game_sv_Deathmatch::OnDetach		(u16 eid_who, u16 eid_what)
{	
	return TRUE;
}

void game_sv_Deathmatch::OnPlayerConnect	(ClientID id_who)
{
	inherited::OnPlayerConnect	(id_who);

	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	game_PlayerState*	ps_who	=	get_id	(id_who);

	ps_who->clear();
//	ClearPlayerState(ps_who);
	ps_who->team				=	0;	
	
	if (g_pGamePersistent->bDedicatedServer && (xrCData == m_server->GetServer_client()) )
	{
		ps_who->Skip = true;
		return;
	}
	ps_who->Skip = false;
	SpawnPlayer(id_who, "spectator");

	// Send Message About Client Connected
	if (xrCData)
	{
		NET_Packet			P;
		GenerateGameMessage (P);
		P.w_u32				(GAME_EVENT_PLAYER_CONNECTED);
		P.w_stringZ			(get_option_s(*xrCData->Name,"name",*xrCData->Name));

		u_EventSend(P);
	};

	Money_SetStart(id_who);
	SetPlayersDefItems(ps_who);
}

void	game_sv_Deathmatch::Player_AddMoney			(game_PlayerState* ps, s32 MoneyAmount)
{
	if (!ps) return;
	TeamStruct* pTeam		= GetTeamData(u8(ps->team));

	s64 TotalMoney = ps->money_for_round;

	TotalMoney	+= MoneyAmount;
	
	if (TotalMoney<pTeam->m_iM_Min) 
		TotalMoney = pTeam->m_iM_Min;
	if (TotalMoney > 32767)
		TotalMoney = 32767;

	ps->money_for_round = s16(TotalMoney);
};
