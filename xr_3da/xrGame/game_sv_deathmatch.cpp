#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "ui\UIBuyWeaponWnd.h"
#include "level.h"
#include "xrserver.h"
#include "Inventory.h"

int		SkinID = -1;
u32		g_dwMaxCorpses = 10;

void	game_sv_Deathmatch::Create					(LPSTR &options)
{
	__super::Create					(options);
	fraglimit	= get_option_i		(options,"fraglimit",0);
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
	damgeblocklimit	= get_option_i		(options,"timelimit",5)*1000;	// in (ms)

	/////////////////////////////////////////////////////////////////////////
	LoadTeams();
	/////////////////////////////////////////////////////////////////////////

//	switch_Phase(GAME_PHASE_PENDING);
	switch_Phase(GAME_PHASE_INPROGRESS);

	::Random.seed(GetTickCount());
	/////////////////////////////////////////////////////////////////////////
	m_CorpseList.clear();
}

void	game_sv_Deathmatch::OnRoundStart			()
{
	__super::OnRoundStart	();

	// Respawn all players and some info
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);

		ClearPlayerState		(ps);
		//---------------------------------------
		SetPlayersDefItems		(ps);
		//---------------------------------------
		Money_SetStart			(get_it_2_id(it));
		//---------------------------------------

		if (ps->Skip) continue;
		SpawnActor(get_it_2_id(it), "spectator");
	}
	///////////////////////////////////////////
	m_CorpseList.clear();
}

void	game_sv_Deathmatch::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	if (!ps_killed || !ps_killer) return;
	ps_killed->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	ps_killed->deaths				+=	1;
	TeamStruct* pTeam		= GetTeamData(u8(ps_killer->team));
	if (ps_killer == ps_killed)	
	{
		// By himself
		ps_killer->kills			-=	1;

		if (pTeam)
			ps_killer->money_for_round	=	ps_killer->money_for_round + pTeam->m_iM_KillSelf;
	} else {
		// Opponent killed - frag 
		ps_killer->kills			+=	1;
		if (pTeam)
			ps_killer->money_for_round	=	ps_killer->money_for_round + pTeam->m_iM_KillRival;

		if (fraglimit && (ps_killer->kills >= fraglimit) )OnFraglimitExceed();
	}

	// Send Message About Player Killed
	SendPlayerKilledMessage(id_killer, id_killed);

	if (pTeam)
		if (ps_killer->money_for_round < pTeam->m_iM_Min) ps_killer->money_for_round = pTeam->m_iM_Min;

	signal_Syncronize();
}

void	game_sv_Deathmatch::OnTimelimitExceed		()
{
	OnRoundEnd	("TIME_limit");
}
void	game_sv_Deathmatch::OnFraglimitExceed		()
{
	OnRoundEnd	("FRAG_limit");
}
void	game_sv_Deathmatch::Update					()
{
	__super::Update	();
	switch (phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			if (timelimit)
			{
				if ((Level().timeServer()-start_time)>u32(timelimit))
					OnTimelimitExceed	();
			};

			// remove corpses if their number exceed limit
			while (m_CorpseList.size()>g_dwMaxCorpses)
			{
				u16 CorpseID = m_CorpseList.front();
				m_CorpseList.pop_front();
				//---------------------------------------------
				NET_Packet			P;
				u_EventGen			(P,GE_DESTROY,CorpseID);
				Level().Send(P,net_flags(TRUE,TRUE));
			};
		}
		break;
	case GAME_PHASE_PENDING:
		{
			if ((Level().timeServer()-start_time)>u32(10*1000) && AllPlayers_Ready())
				OnRoundStart();
		}
		break;
	}
}

BOOL	game_sv_Deathmatch::AllPlayers_Ready ()
{
	// Check if all players ready
	u32		cnt		= get_count	();
	u32		ready	= 0;
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState* ps		=	get_it	(it);
		if (ps->flags & GAME_PLAYER_FLAG_READY)	++ready;
		else
			if (ps->Skip) ++ready;
	}

	if (ready == cnt) return TRUE;
	return FALSE;
};
	
BOOL	game_sv_Deathmatch::OnTouch			(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	CSE_Abstract*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A)
	{
		// Actor touches something
		CSE_ALifeItemWeapon*	W			=	dynamic_cast<CSE_ALifeItemWeapon*> (e_what);
		if (W) 
		{
			// Weapon
			xr_vector<u16>&	C			=	A->children;
			u8 slot						=	W->get_slot	();
			for (u32 it=0; it<C.size(); ++it)
			{
				CSE_Abstract*		Et	= S->ID_to_entity				(C[it]);
				if (0==Et)				continue;
				CSE_ALifeItemWeapon*		T	= dynamic_cast<CSE_ALifeItemWeapon*>	(Et);
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
		
		CSE_ALifeItemAmmo* pIAmmo			=	dynamic_cast<CSE_ALifeItemAmmo*> (e_what);
		if (pIAmmo)
		{
			//Ammo
			return TRUE;
		};

		CSE_ALifeItemGrenade* pIGrenade		=	dynamic_cast<CSE_ALifeItemGrenade*> (e_what);
		if (pIGrenade)
		{
			//Grenade
			return TRUE;
		};

		CSE_ALifeItemCustomOutfit* pOutfit		= dynamic_cast<CSE_ALifeItemCustomOutfit*> (e_what);
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

void	game_sv_Deathmatch::OnPlayerReady			(u32 id)
{
//	if	(GAME_PHASE_INPROGRESS == phase) return;
	switch (phase)
	{
	case GAME_PHASE_PENDING:
		{
			game_PlayerState*	ps	=	get_id	(id);
			if (ps)
			{
				if (ps->flags & GAME_PLAYER_FLAG_READY)	
				{
					ps->flags &= ~GAME_PLAYER_FLAG_READY;
				} 
				else 
				{
					ps->flags |= GAME_PLAYER_FLAG_READY;

					if (AllPlayers_Ready())
					{
						OnRoundStart	();
					};
				};
			};
		}break;
	case GAME_PHASE_INPROGRESS:
		{
//			LPCSTR	options			=	get_name_id	(id);
			game_PlayerState*	ps	=	get_id	(id);
			if (ps->Skip) break;
			
			//------------------------------------------------------------
			xrClientData* xrCData	=	Level().Server->ID_to_client(id);
			if (!xrCData || !xrCData->owner) break;
			CSE_Abstract* pOwner = xrCData->owner;
			CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(pOwner);
			CSE_Spectator			*pS =	dynamic_cast<CSE_Spectator*>(pOwner);

			if (pA)
			{
				//------------------------------------------------------------			
				AllowDeadBodyRemove(id);
				//------------------------------------------------------------
				m_CorpseList.push_back(pOwner->ID);
				//------------------------------------------------------------
				SpawnActor(id, "spectator");
				//------------------------------------------------------------
			}
			else
			{
				R_ASSERT2	(pS,"Respawned Client is not Actor nor Spectator");
				if (pS)
				{
					//------------------------------------------------------------
					if (pS->owner != Level().Server->GetServer_client())
					{
						pS->owner = Level().Server->GetServer_client();
					};
					//------------------------------------------------------------
					//remove spectator entity
					NET_Packet			P;
					u_EventGen			(P,GE_DESTROY,pS->ID);
					//		pObject->u_EventSend		(P);
					Level().Send(P,net_flags(TRUE,TRUE));
					//------------------------------------------------------------
					SpawnActor(id, "mp_actor");
					//------------------------------------------------------------
					SpawnWeaponsForActor(xrCData->owner, ps);
					//------------------------------------------------------------
				};				
			};			
		}break;
	};
}

void game_sv_Deathmatch::OnPlayerConnect	(u32 id_who)
{
	__super::OnPlayerConnect	(id_who);

	xrClientData* xrCData	=	Level().Server->ID_to_client(id_who);
	game_PlayerState*	ps_who	=	get_id	(id_who);

	ClearPlayerState(ps_who);
	ps_who->team				=	0;	
	
	if (g_pGamePersistent->bDedicatedServer && (xrCData == Level().Server->GetServer_client()) )
	{
		ps_who->Skip = true;
		return;
	}
	ps_who->Skip = false;
	SpawnActor(id_who, "spectator");

	// Send Message About Client Connected
	if (xrCData)
	{
		NET_Packet			P;
		P.w_begin			(M_GAMEMESSAGE);
		P.w_u32				(GMSG_PLAYER_CONNECTED);
		P.w_string			(get_option_s(*xrCData->Name,"name",*xrCData->Name));

		u_EventSend(P);
	};

	Money_SetStart(id_who);
	SetPlayersDefItems(ps_who);
}


void game_sv_Deathmatch::OnPlayerDisconnect		(u32 id_who)
{
	__super::OnPlayerDisconnect	(id_who);

	LPCSTR	Name = NULL;

	Name = get_name_id(id_who);
	Name = get_option_s(Name,"name",Name);


	xrServer*	S					=	Level().Server;
	
	// Remove everything	
	xrClientData* xrCData	=	S->ID_to_client(id_who);
	xrClientData* xrSCData	=	S->GetServer_client();	

	if (xrCData != xrSCData)
	{
		KillPlayer	(id_who);
		AllowDeadBodyRemove(id_who);

		// Send Message About Client DisConnected
		if (xrCData)
		{
			NET_Packet			P;
			P.w_begin			(M_GAMEMESSAGE);
			P.w_u32				(GMSG_PLAYER_DISCONNECTED);
			P.w_string			(get_option_s(*xrCData->Name,"name",*xrCData->Name));

			u_EventSend(P);
		};
	}
	else
	{	

		CSE_Abstract*		from		= S->ID_to_entity(get_id_2_eid(id_who));
		if (from) S->Perform_destroy				(from,net_flags(TRUE, TRUE), FALSE);
	};
};

void	game_sv_Deathmatch::AllowDeadBodyRemove		(u32 id)
{
	xrClientData* xrCData	=	Level().Server->ID_to_client(id);
	if (!xrCData) return;
	if (!xrCData->owner) return;
	
	if (xrCData->owner->owner != Level().Server->GetServer_client())
	{
		xrCData->owner->owner = Level().Server->GetServer_client();
	};

	CObject* pObject =  Level().Objects.net_Find(xrCData->owner->ID);

//	R_ASSERT2	((pObject && pObject->SUB_CLS_ID == CLSID_OBJECT_ACTOR),"Dead Player is not Actor");

	if (pObject && pObject->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
	{
		CActor* pActor = dynamic_cast <CActor*>(pObject);
		if (pActor)
		{
			pActor->set_death_time		();
			pActor->m_bAllowDeathRemove = true;
		};
	};	
};

void	game_sv_Deathmatch::SpawnActor				(u32 id, LPCSTR N)
{
	xrClientData* CL	= Level().Server->ID_to_client(id);
	game_PlayerState*	ps_who	=	&CL->ps;//get_id	(id);
	ps_who->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	
	// Spawn "actor"
	LPCSTR	options			=	get_name_id	(id);
	CSE_Abstract			*E	=	spawn_begin	(N);													// create SE
	strcpy					(E->s_name_replace,get_option_s(options,"name","Player"));					// name

	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);	// flags

	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	CSE_Spectator			*pS	=	dynamic_cast<CSE_Spectator*>(E);

	R_ASSERT2	(pA || pS,"Respawned Client is not Actor nor Spectator");
	
	if (pA) 
	{
		pA->s_team				=	u8(ps_who->team);
		assign_RP				(pA);
		SetSkin(E, pA->s_team, ps_who->m_skin);
		ps_who->flags &= ~(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
		ps_who->m_RespawnTime = Device.dwTimeGlobal;
	}
	else
		if (pS)
		{
			Fvector Pos, Angle;
			ps_who->flags		|= GAME_PLAYER_FLAG_CS_SPECTATOR;
			if (!GetPosAngleFromActor(id, Pos, Angle)) assign_RP				(E);
			else
			{
				E->o_Angle.set(Angle);
				E->o_Position.set(Pos);
			}
		};
	
	Msg		("* %s respawned as %s",get_option_s(options,"name","Player"), (0 == pA) ? "spectator" : "actor");
	spawn_end				(E,id);

	ps_who->GameID = CL->owner->ID;

	signal_Syncronize();
}

#include "../CameraBase.h"

bool	game_sv_Deathmatch::GetPosAngleFromActor				(u32 id, Fvector& Pos, Fvector &Angle)
{
	xrClientData* xrCData	=	Level().Server->ID_to_client(id);
	if (!xrCData || !xrCData->owner) return false;
	
	CObject* pObject =  Level().Objects.net_Find(xrCData->owner->ID);
///	R_ASSERT2	((pObject && pObject->SUB_CLS_ID == CLSID_OBJECT_ACTOR),"Dead Player is not Actor");

	if (!pObject || pObject->SUB_CLS_ID != CLSID_OBJECT_ACTOR) return false;

	CActor* pActor = dynamic_cast <CActor*>(pObject);
	if (!pActor) return false;

	Angle.set(-pActor->cam_Active()->pitch, -pActor->cam_Active()->yaw, 0);
	Pos.set(pActor->cam_Active()->vPosition);
	return true;
};

void	game_sv_Deathmatch::KillPlayer				(u32 id_who)
{
	xrServer*	S					=	Level().Server;

	// Remove everything	
	xrClientData* xrCData	=	S->ID_to_client(id_who);
	if (!xrCData) return;
	// Kill Player on all clients
	NET_Packet			P;
	P.w_begin			(M_EVENT);
	P.w_u32				(Level().timeServer());
	P.w_u16				(GE_DIE);
	P.w_u16				(xrCData->owner->ID);
	P.w_u16				(0);
	P.w_u32				(xrCData->ID);
	S->SendBroadcast	(0xffffffff,P,net_flags(TRUE, TRUE, TRUE));

};

void	game_sv_Deathmatch::OnPlayerWantsDie		(u32 id_who)
{
	KillPlayer(id_who);
};

u32		game_sv_Deathmatch::RP_2_Use				(CSE_Abstract* E)
{
	return 0;
};

void	game_sv_Deathmatch::assign_RP				(CSE_Abstract* E)
{
	VERIFY				(E);
	u32		Team		= RP_2_Use(E);
	VERIFY				(rpoints[Team].size());

	CSE_Spectator		*pSpectator = dynamic_cast<CSE_Spectator*>(E);
	if (pSpectator)
	{
		game_sv_GameState::assign_RP(E);
		return;
	};

	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	if (!pA)
	{
		game_sv_GameState::assign_RP(E);
		return;
	};
//-------------------------------------------------------------------------------
	//create Enemies list
	xr_vector <u32>					pEnemies;
	xr_vector <u32>					pFriends;
	
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->flags & GAME_PLAYER_FLAG_VERY_VERY_DEAD) continue;
		if (ps->team == pA->s_team && !teams.empty()) pFriends.push_back(it);
		else pEnemies.push_back(it);
	};

	if (pEnemies.empty()) 
	{
		u8 OldTeam = pA->s_team;
		pA->s_team = u8(Team);
		game_sv_GameState::assign_RP(E);
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
			xrClientData* xrCData	=	Level().Server->ID_to_client(get_it_2_id(pEnemies[p]));
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
		CWeapon		*pWeapon	=	dynamic_cast<CWeapon*>(pItem);
		if (pWeapon)
		{
			u8 OldAddons  = pWeapon->GetAddonsState();
			u8 NewAddons  = u8(ItemID&0x00ff)>>0x05;
			if (OldAddons != NewAddons)
			{
				CSE_ALifeItemWeapon* pSWeapon = dynamic_cast<CSE_ALifeItemWeapon*>(Level().Server->game->get_entity_from_eid(pWeapon->ID()));
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


void	game_sv_Deathmatch::OnPlayerBuyFinished		(u32 id_who, NET_Packet& P)
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

	CSE_ALifeCreatureActor*		e_Actor	= dynamic_cast<CSE_ALifeCreatureActor*>(Level().Server->game->get_entity_from_eid	(ps->GameID));
/*	
	if (e_Actor)
	{
		//-------------------------------------------------------------------------
		xr_vector<u16>				ItemsToDelete;

		xr_vector<u16>::const_iterator	I = e_Actor->children.begin	();
		xr_vector<u16>::const_iterator	E = e_Actor->children.end		();
		for ( ; I != E; ++I) 
		{
			PIItem pItem = dynamic_cast<PIItem> (Level().Objects.net_Find(*I));
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
	CActor* pActor = dynamic_cast<CActor*>(Level().Objects.net_Find	(ps->GameID));
	if (pActor)
	{
		PIItem pItem = NULL;
		xr_vector<u16>				ItemsToDelete;

		//��������� ����
		TIItemList::const_iterator	IBelt = pActor->inventory().m_belt.begin();
		TIItemList::const_iterator	EBelt = pActor->inventory().m_belt.end();

		for ( ; IBelt != EBelt; ++IBelt) 
		{
			pItem = (*IBelt);
			CheckItem(ps, pItem, &ItemsDesired, &ItemsToDelete);
		};

		//��������� �����
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
	ps->m_lasthitter		= 0;
	ps->m_lasthitweapon		= 0;

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

void	game_sv_Deathmatch::SpawnWeapon4Actor		(u32 actorId,  LPCSTR N, u8 Addons)
{
	if (!N) return;
	
	CSE_Abstract			*E	=	spawn_begin	(N);
	E->ID_Parent = u16(actorId);

	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL);	// flags

	/////////////////////////////////////////////////////////////////////////////////
	//���� ��� ������ - ������� ��� � ������ ���������
	CSE_ALifeItemWeapon		*pWeapon	=	dynamic_cast<CSE_ALifeItemWeapon*>(E);
	if (pWeapon)
	{
		pWeapon->a_elapsed = pWeapon->get_ammo_magsize();

		pWeapon->m_addon_flags.assign(Addons);
	};
	/////////////////////////////////////////////////////////////////////////////////

	spawn_end				(E,Level().Server->GetServer_client()->ID);
};

void	game_sv_Deathmatch::SpawnWeaponsForActor(CSE_Abstract* pE, game_PlayerState*	ps)
{
	CSE_ALifeCreatureActor* pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(pE);
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

	ps->money_for_round = ps->money_for_round + ps->LastBuyAcount;
};
void	game_sv_Deathmatch::LoadWeaponsForTeam		(char* caSection, TEAM_WPN_LIST *pTeamWpnList)
{
	
	R_ASSERT(xr_strcmp(caSection,""));

	pTeamWpnList->clear();

	for (int i = 1; i < 20; ++i)
	{
		// ��� ����
		string16			wpnSection;	
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(caSection, wpnSection)) 
		{
//			pTeamWpnList->push_back(wpnOneType);
			continue;
		}

		string256			wpnNames, wpnSingleName;
		// ������ ������ ����� ����
		std::strcpy(wpnNames, pSettings->r_string(caSection, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
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

	// ���� strSectionName ������ ��������� ��� ������
	R_ASSERT(xr_strcmp(caSection,""));

	pTeamSkins->clear();

	// ��� ����
	if (!pSettings->line_exist(caSection, "skins")) return;

	// ������ ������ ����� ����
	std::strcpy(Skins, pSettings->r_string(caSection, "skins"));
	u32 count	= _GetItemCount(Skins);
	// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
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

	// ���� strSectionName ������ ��������� ��� ������
	R_ASSERT(xr_strcmp(caSection,""));

	pDefItems->clear();

	// ��� ����
	if (!pSettings->line_exist(caSection, "default_items")) return;

	// ������ ������ ����� ����
	std::strcpy(DefItems, pSettings->r_string(caSection, "default_items"));
	u32 count	= _GetItemCount(DefItems);
	// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
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
	CSE_Visual* pV = dynamic_cast<CSE_Visual*>(E);
	if (!pV) return;
	//-------------------------------------------
	string256 SkinName = {"actors\\Different_stalkers\\Mp_Skins\\"};
	//��������� �� ����� ��� ���� ��������
	if (SkinID != -1) ID = u16(SkinID);

	if (!TeamList.empty()	&&
		TeamList.size() > Team	&&
		!TeamList[Team].aSkins.empty())
	{
		//��������� �� ���������� ������ ��� ���� ��������
		if (TeamList[Team].aSkins.size() > ID)
		{
			std::strcat(SkinName, TeamList[Team].aSkins[ID].c_str());
		}
		else
			std::strcat(SkinName, TeamList[Team].aSkins[0].c_str());
	}
	else
	{
		//����� ��� ����� �������� �� ���������
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

	CSE_ALifeCreatureActor*		a_hitter		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_ALifeCreatureActor*		a_hitted		= dynamic_cast <CSE_ALifeCreatureActor*> (e_hitted);

	if (!a_hitter || !a_hitted) return;

//	game_PlayerState*	ps_hitter = &a_hitter->owner->ps;
	game_PlayerState*	ps_hitted = &a_hitted->owner->ps;

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
	if (Device.dwTimeGlobal<ps_hitted->m_RespawnTime + damgeblocklimit)
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
		ps_hitted->m_lasthitter = a_hitter->ID;
		ps_hitted->m_lasthitweapon = WeaponID;
	};
	//---------------------------------------
	P.B.count	= BCount;
};

void	game_sv_Deathmatch::SendPlayerKilledMessage	(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	if (!ps_killed || !ps_killer) return;

	NET_Packet			P;
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_PLAYER_KILLED);
	P.w_u16				(ps_killed->GameID);
	if (ps_killer->GameID == ps_killed->m_lasthitter)
	{
		P.w_u16				(ps_killer->GameID);
		P.w_u16				(ps_killed->m_lasthitweapon);
	}
	else
	{
		P.w_u16				(ps_killer->GameID);
		P.w_u16				(0);
	};

	u_EventSend(P);

	//---------------------------------------------------------
	ps_killed->m_lasthitter			= 0;
	ps_killed->m_lasthitweapon		= 0;
	ClearPlayerItems		(ps_killed);
	//---------------------------------------------------------
	SetPlayersDefItems		(ps_killed);
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

void	game_sv_Deathmatch::LoadTeamData			(char* caSection)
{
	TeamStruct	NewTeam;

	std::strcpy(NewTeam.caSection, caSection);

	LoadWeaponsForTeam	(caSection, &NewTeam.aWeapons);
	LoadSkinsForTeam	(caSection, &NewTeam.aSkins);
	LoadDefItemsForTeam	(caSection, &NewTeam.aWeapons, &NewTeam.aDefaultItems);	
	//-------------------------------------------------------------
	if (pSettings->section_exist(caSection))
	{
		NewTeam.m_iM_Start				= pSettings->r_s16(caSection, "money_start");
		NewTeam.m_iM_Min				= pSettings->r_s16(caSection, "money_min");

		NewTeam.m_iM_KillRival			= pSettings->r_s16(caSection, "kill_rival");
		NewTeam.m_iM_KillSelf			= pSettings->r_s16(caSection, "kill_self");
		NewTeam.m_iM_KillTeam			= pSettings->r_s16(caSection, "kill_team");

		NewTeam.m_iM_TargetRival		= pSettings->r_s16(caSection, "target_rival");
		NewTeam.m_iM_TargetTeam			= pSettings->r_s16(caSection, "target_team");
		NewTeam.m_iM_TargetSucceed		= pSettings->r_s16(caSection, "target_succeed");
		NewTeam.m_iM_TargetSucceedAll	= pSettings->r_s16(caSection, "target_succeed_all");

		NewTeam.m_iM_RoundWin			= pSettings->r_s16(caSection, "round_win");
		NewTeam.m_iM_RoundLoose			= pSettings->r_s16(caSection, "round_loose");
		NewTeam.m_iM_RoundDraw			= pSettings->r_s16(caSection, "round_draw");
	};
	//-------------------------------------------------------------
	TeamList.push_back(NewTeam);
};

void game_sv_Deathmatch::OnPlayerChangeSkin(u32 id_who, u8 skin) 
{
	game_PlayerState*	ps_who	=	get_id	(id_who);
	if (!ps_who) return;
	ps_who->m_skin = skin;

	if (OnServer())
	{
		KillPlayer(id_who);
	};
	signal_Syncronize();
}

void game_sv_Deathmatch::OnDestroyObject			(u16 eid_who)
{
	for (u32 i=0; i<m_CorpseList.size();)
	{
		if (m_CorpseList[i] == eid_who)
		{
			m_CorpseList.erase(m_CorpseList.begin()+i);
		}
		else i++;
	};
};

game_sv_Deathmatch::TeamStruct* game_sv_Deathmatch::GetTeamData				(u8 Team)
{
	VERIFY(TeamList.size());
	if (TeamList.empty()) return NULL;
	
	VERIFY(TeamList.size()>Team);
	if (TeamList.size()<=Team) return NULL;

	return &(TeamList[Team]);
};

void game_sv_Deathmatch::Money_SetStart			(u32	id_who)
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
void	game_sv_Deathmatch::RemoveItemFromActor		(CSE_Abstract* pItem)
{
	if (!pItem) return;
	//-------------------------------------------------------------
	CSE_ALifeItemWeapon* pWeapon = dynamic_cast<CSE_ALifeItemWeapon*> (pItem);
	if (pWeapon)
	{
	};
	//-------------------------------------------------------------
	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,pItem->ID);
	Level().Send(P,net_flags(TRUE,TRUE));
};

void	game_sv_Deathmatch::OnTeamScore				(u32 Team)
{
	TeamStruct* pTeam		= GetTeamData(u8(Team));
	if (!pTeam) return;
	
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->Skip) continue;		

		if (ps->team == s16(Team))
			ps->money_for_round = ps->money_for_round + pTeam->m_iM_RoundWin;
		else
			ps->money_for_round = ps->money_for_round + pTeam->m_iM_RoundLoose;
	}
}