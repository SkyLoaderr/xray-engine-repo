#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"

void	game_sv_Deathmatch::Create					(LPSTR &options)
{
	__super::Create					(options);
	fraglimit	= get_option_i		(options,"fraglimit",0);
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
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
		ps->kills				=	0;
		ps->deaths				=	0;

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
			LPCSTR	options			=	get_name_id	(id);
			Msg		("* [%s] respawned ",get_option_s(options,"name","Player"));
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
					SpawnItem4Actor(pA->ID, "wpn_knife");
					SpawnItem4Actor(pA->ID, "wpn_lr300");
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

	ps_who->kills				=	0;
	ps_who->deaths				=	0;
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
	game_PlayerState*	ps_who	=	get_id	(id);
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
	
	spawn_end				(E,id);

	ps_who->flags &= ~(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
}

void	game_sv_Deathmatch::SpawnItem4Actor			(u32 actorId, LPCSTR N)
{
	if (!N) return;
	CSE_Abstract			*E	=	spawn_begin	(N);
	E->ID_Parent = u16(actorId);

	E->s_flags.set			(M_SPAWN_OBJECT_LOCAL);	// flags
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
		game_sv_GameState::assign_RP(E);
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