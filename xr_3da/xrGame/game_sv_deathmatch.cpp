#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "ui\UIBuyWeaponWnd.h"

int		SkinID = -1;

void	game_sv_Deathmatch::Create					(LPSTR &options)
{
	__super::Create					(options);
	fraglimit	= get_option_i		(options,"fraglimit",0);
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)

	/////////////////////////////////////////////////////////////////////////
	LoadWeapons						();
	/////////////////////////////////////////////////////////////////////////
	LoadSkins						();
	/////////////////////////////////////////////////////////////////////////

//	switch_Phase(GAME_PHASE_PENDING);
	switch_Phase(GAME_PHASE_INPROGRESS);
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

		ClearPlayerState(ps);

		SpawnActor(get_it_2_id(it), "spectator");
	}
}

void	game_sv_Deathmatch::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	if (!ps_killed || !ps_killer) return;
	ps_killed->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	ps_killed->deaths				+=	1;
	if (ps_killer == ps_killed)	
	{
		// By himself
		ps_killer->kills			-=	1;
	} else {
		// Opponent killed - frag 
		ps_killer->kills			+=	1;
		if (fraglimit && (ps_killer->kills >= fraglimit) )OnFraglimitExceed();
	}

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
				if ((Device.TimerAsync()-start_time)>u32(timelimit))
					OnTimelimitExceed	();
			}
		}
		break;
	case GAME_PHASE_PENDING:
		{
			if ((Device.TimerAsync()-start_time)>u32(10*1000) && AllPlayers_Ready())
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

		CSE_ALifeItemGrenade* pIGrenade			=	dynamic_cast<CSE_ALifeItemGrenade*> (e_what);
		if (pIGrenade)
		{
			//Ammo
			return TRUE;
		};
	}

	// We don't know what the hell is it, so disallow ownership just for safety 
	return TRUE;
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
					SpawnActor(id, "actor");
					//------------------------------------------------------------
					pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(xrCData->owner);
					R_ASSERT2(pA, "Owner not a Actor");

					SpawnItem4Actor(pA->ID, GetItemForSlot(KNIFE_SLOT, 0xff, ps));
					SpawnItem4Actor(pA->ID, GetItemForSlot(PISTOL_SLOT, 0xff,  ps));
					SpawnItem4Actor(pA->ID, GetItemForSlot(RIFLE_SLOT, 0xff,  ps));
					SpawnItem4Actor(pA->ID, GetItemForSlot(GRENADE_SLOT, 0xff,  ps));

					for (u32 i = 0; i<ps->BeltItems.size(); i++)
					{
						SpawnItem4Actor(pA->ID, GetItemForSlot(ps->BeltItems[i].SlotID, ps->BeltItems[i].ItemID,  ps));
					};
					//------------------------------------------------------------
				};				
			};			
		}break;
	};
}

void game_sv_Deathmatch::OnPlayerConnect	(u32 id_who)
{
	__super::OnPlayerConnect	(id_who);

	game_PlayerState*	ps_who	=	get_id	(id_who);

	ClearPlayerState(ps_who);
	ps_who->team				=	0;	

	SpawnActor(id_who, "spectator");
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
	}
	else
	{	
		/*
		xr_vector<u16>*	C				=	get_children(id_who);

		if (C)
		{
			xr_vector<u16>::iterator i,e;
			i=C->begin();
			e=C->end();
			for(;i!=e;++i)
				//	while(C->size())
			{
				u16		eid						= (*i);

				CSE_Abstract*		what		= S->ID_to_entity(eid);
				if (!what) continue;
				S->Perform_destroy				(what,net_flags(TRUE, TRUE));
			}
		};
		*/
		CSE_Abstract*		from		= S->ID_to_entity(get_id_2_eid(id_who));
		S->Perform_destroy				(from,net_flags(TRUE, TRUE));
	}
//	HUD().outMessage			(0xffffffff,"DM","Player '%s' disconnected",Name);
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
			pActor->m_dwDeathTime = Level().GetGameTime();
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

	E->s_flags.set			(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);	// flags

	CSE_ALifeCreatureActor	*pA	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	CSE_Spectator			*pS	=	dynamic_cast<CSE_Spectator*>(E);

	R_ASSERT2	(pA || pS,"Respawned Client is not Actor nor Spectator");
	
	if (pA) 
	{
		pA->s_team				=	u8(ps_who->team);
		assign_RP				(pA);
		SetSkin(E, pA->s_team, 0);
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

	ps_who->flags &= ~(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
	ps_who->GameID = CL->owner->ID;

	signal_Syncronize();
}

void	game_sv_Deathmatch::SpawnItem4Actor			(u32 actorId, LPCSTR N)
{
	if (!N) return;
	CSE_Abstract			*E	=	spawn_begin	(N);
	E->ID_Parent = u16(actorId);

	E->s_flags.set			(M_SPAWN_OBJECT_LOCAL);	// flags

	/////////////////////////////////////////////////////////////////////////////////
	//���� ��� ������ - ������� ��� � ������ ���������
	CSE_ALifeItemWeapon		*pWeapon	=	dynamic_cast<CSE_ALifeItemWeapon*>(E);
	if (pWeapon)
	{
		pWeapon->a_elapsed = pWeapon->get_ammo_magsize();
	};
	/////////////////////////////////////////////////////////////////////////////////

	spawn_end				(E,Level().Server->GetServer_client()->ID);
};

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
//	u_EventSend(P);
//	Level().uEvent
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

void	game_sv_Deathmatch::OnPlayerBuyFinished		(u32 id_who, NET_Packet& P)
{
	game_PlayerState*	ps	=	get_id	(id_who);
	if (!ps) return;

	P.r_u8		(ps->Slots[KNIFE_SLOT]	);
	P.r_u8		(ps->Slots[PISTOL_SLOT]	);
	P.r_u8		(ps->Slots[RIFLE_SLOT]	);
	P.r_u8		(ps->Slots[GRENADE_SLOT]);

	ps->BeltItems.clear();

	u8 NumItemsInBelt;
	P.r_u8(NumItemsInBelt);
	for (u8 i=0; i<NumItemsInBelt; i++)
	{
		u8 SectID, ItemID;
		P.r_u8(SectID);
		P.r_u8(ItemID);
		ps->BeltItems.push_back(game_PlayerState::BeltItem(SectID, ItemID));
	};
};

void	game_sv_Deathmatch::ClearPlayerState		(game_PlayerState* ps)
{
	if (!ps) return;

	ps->kills				= 0;
	ps->deaths				= 0;

	Memory.mem_fill(ps->Slots, 0xff, sizeof(ps->Slots));
};

const char* game_sv_Deathmatch::GetItemForSlot		(u8 SlotNum, u8 ItemID, game_PlayerState* ps)
{
	if (!ps) return NULL;
	
	u8 Item = ps->Slots[SlotNum];
	if (0xff != ItemID) Item = ItemID;
	
	if (0xff == ps->Slots[SlotNum]) return NULL;

	WPN_LISTS	WpnList = wpnTeamsSectStorage[ps->team];

	WPN_SECT_NAMES WpnSectNames = WpnList[SlotNum];

	std::string Wpn = WpnSectNames[Item & 0x1f];

	return wpnTeamsSectStorage[ps->team][SlotNum][Item & 0x1f].c_str();
};

void	game_sv_Deathmatch::LoadWeaponsForTeam		(WPN_LISTS *pTeamList, char* caSection)
{
	WPN_SECT_NAMES		wpnOneType;
	string16			wpnSection;	
	string256			wpnNames, wpnSingleName;

	// ���� strSectionName ������ ��������� ��� ������
	R_ASSERT(xr_strcmp(caSection,""));

	for (int i = 1; i < 20; ++i)
	{
		// ������� �����
		wpnOneType.clear();

		// ��� ����
		sprintf(wpnSection, "slot%i", i);
		if (!pSettings->line_exist(caSection, wpnSection)) break;

		// ������ ������ ����� ����
		std::strcpy(wpnNames, pSettings->r_string(caSection, wpnSection));
		u32 count	= _GetItemCount(wpnNames);
		// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
		for (u32 i = 0; i < count; ++i)
		{
			_GetItem(wpnNames, i, wpnSingleName);
			wpnOneType.push_back(wpnSingleName);
		}

		if (!wpnOneType.empty())
			pTeamList->push_back(wpnOneType);
	}
};

void	game_sv_Deathmatch::LoadWeapons				()
{
	//Loading Weapons List
	WPN_LISTS		DefaultTeam;
	LoadWeaponsForTeam				(&DefaultTeam, "deathmatch");

	wpnTeamsSectStorage.push_back(DefaultTeam);
};

void	game_sv_Deathmatch::LoadSkinsForTeam		(SkinsStruct *pTeamList, char* caSection)
{
	string256			SkinSingleName;
	string4096			Skins;

	// ���� strSectionName ������ ��������� ��� ������
	R_ASSERT(xr_strcmp(caSection,""));

	std::strcpy(pTeamList->caSection, caSection);
	pTeamList->Skins.clear();

	// ��� ����
	if (!pSettings->line_exist(caSection, "skins")) return;

	// ������ ������ ����� ����
	std::strcpy(Skins, pSettings->r_string(caSection, "skins"));
	u32 count	= _GetItemCount(Skins);
	// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(Skins, i, SkinSingleName);
		pTeamList->Skins.push_back(SkinSingleName);
	};
};

void	game_sv_Deathmatch::LoadSkins				()
{
	//Loading Skins List
	SkinsStruct		DefaultTeam;
	LoadSkinsForTeam				(&DefaultTeam, "deathmatch");

	SkinsTeamSectStorage.push_back(DefaultTeam);
};

void	game_sv_Deathmatch::SetSkin					(CSE_Abstract* E, u16 Team, u16 ID)
{
	if (!E) return;
	//-------------------------------------------
	CSE_Visual* pV = dynamic_cast<CSE_Visual*>(E);
	if (!pV) return;
	//-------------------------------------------
	string256 SkinName = {"actors\\Different_stalkers\\"};
	//��������� �� ����� ��� ���� ��������
	if (SkinID != -1) ID = u16(SkinID);

	if (!SkinsTeamSectStorage.empty() && 
		SkinsTeamSectStorage.size() > Team && 
		!SkinsTeamSectStorage[Team].Skins.empty())
	{
		//��������� �� ���������� ������ ��� ���� ��������
		if (SkinsTeamSectStorage[Team].Skins.size() > ID)
		{
			std::strcat(SkinName, SkinsTeamSectStorage[Team].Skins[ID].c_str());
		}
		else
			std::strcat(SkinName, SkinsTeamSectStorage[Team].Skins[0].c_str());
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
	pV->set_visual(SkinName);
	//-------------------------------------------
};
