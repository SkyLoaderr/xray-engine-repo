#include "stdafx.h"
#include "game_sv_cs.h"
#include "HUDManager.h"

void	game_sv_CS::Create			(LPCSTR options)
{
	__super::Create					(options);

	R_ASSERT2						(rpoints[0].size(), "No respawn point for team 0");
	R_ASSERT2						(rpoints[1].size(), "No respawn point for team 1");
	R_ASSERT2						(rpoints[2].size(), "No respawn point for artifacts");

	teams.resize					(2); // @@@ WT
	timelimit	= get_option_i		(options,"timelimit",0)*10000;	// in (ms)
	switch_Phase					(GAME_PHASE_PENDING);
}

void	game_sv_CS::OnRoundStart	()
{
	__super::OnRoundStart	();
	if (0==round)	
	{
		// give $1000 to everybody
		u32		cnt = get_count();
		for		(u32 it=0; it<cnt; it++)	
		{
			game_PlayerState*	ps	=	get_it	(it);
			ps->money_total			=	1000;
			ps->money_for_round		=	0;
			ps->flags = 0;
			ps->deaths = 0;
			ps->kills = 0;
		}
	} else {
		// sum-up money for round with total
		u32		cnt = get_count();
		for		(u32 it=0; it<cnt; it++)	
		{
			game_PlayerState*	ps	=	get_it	(it);
			ps->money_total			=	ps->money_total + ps->money_for_round;
			ps->money_for_round		=	0;
			ps->flags = 0;
		}
		teams[0].num_targets = teams[1].num_targets = 0;
	}

	// Spawn "artifacts"
	vector<RPoint>&		rp	= rpoints[2];
	srand				( (unsigned)time( NULL ) );
	random_shuffle		( rp.begin( ), rp.end( ) );
	for(s32 i = 0; i < 3; i++) {
		xrServerEntity*		E	=	spawn_begin	("m_target_cs");									// create SE
		xrSE_Target_CS*	A		=	(xrSE_Target_CS*) E;					
		A->s_flags				=	M_SPAWN_OBJECT_ACTIVE  | M_SPAWN_OBJECT_LOCAL;				// flags
		RPoint&				r	= rp[i];
		A->o_Position.set	(r.P);
		A->o_Angle.set		(r.A);
		spawn_end			(A,0);
	}

	// Respawn all players and some info
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; it++)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		ps->kills				=	0;
		ps->deaths				=	0;

		// spawn
		LPCSTR	options			=	get_name_it	(it);
		xrServerEntity*		E	=	spawn_begin	(ps->team?"actor_cs_2":"actor_cs_1");													// create SE
		xrSE_Actor*	A			=	(xrSE_Actor*) E;					
		strcpy					(A->s_name_replace,get_option_s(options,"name","Player"));					// name
		A->s_team				=	u8(ps->team);															// team
		A->s_flags				=	M_SPAWN_OBJECT_ACTIVE  | M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER;// flags
		assign_RP				(A);
		spawn_end				(A,get_it_2_id(it));
	}
}

void	game_sv_CS::OnTeamScore		(u32 team)
{
	// Increment/decrement money
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; it++)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->money_for_round		+=	(s32(team)==ps->team)?+2000:+500;
	}
}

void	game_sv_CS::OnTeamsInDraw	()
{
	// Give $1000 to everybody
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; it++)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->money_for_round		+=	+1000;
	}
}

void	game_sv_CS::OnPlayerKillPlayer	(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	ps_killed->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	ps_killed->deaths				+=	1;
	if (ps_killer->team == ps_killed->team)	
	{
		// Teammate killed - no frag, chop money
//		ps_killer->money_for_round	-=	500;
		ps_killer->money_total	-=	500; if(ps_killer->money_total < 0) ps_killer->money_total = 0;
	} else {
		// Opponent killed - frag + money
//		ps_killer->money_for_round	+=	500;
		ps_killer->money_total	+=	500;
		ps_killer->kills			+=	1;

	}
	// Check if there is no opponents left
	u32 alive					=	get_alive_count	(ps_killed->team);
	if (0==alive) {
		OnTeamScore(ps_killer->team);
		OnRoundEnd("ENEMY_quelled");
	}

	// Drop everything
	xrServer*	S					=	Level().Server;
	vector<u16>*	C				=	get_children(id_killed);
	if (0==C)						return;
	for (u32 it=0; it<C->size(); it++)
	{
		u16		eid						= (*C)[it];

		xrServerEntity*		from		= S->ID_to_entity(get_id_2_eid(id_killed));
		xrServerEntity*		what		= S->ID_to_entity(eid);
		S->Perform_reject				(what,from);
	}
}

void	game_sv_CS::OnTimelimitExceed	()
{
	// Если у команд поровну артефактов, то ничья.
	R_ASSERT(teams.size() == 2);
	if(teams[0].num_targets == teams[1].num_targets) OnTeamsInDraw();
	else OnTeamScore((teams[0].num_targets > teams[1].num_targets) ? 0 : 1);
	OnRoundEnd		("TIME_limit");
}

BOOL	game_sv_CS::OnTouch			(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	xrServerEntity*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	xrServerEntity*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	xrSE_Actor*			A		= dynamic_cast<xrSE_Actor*> (e_who);
	if (A) 	{
		// player_id = entity->owner->ID			-- 
		// entity->owner->owner == entity			-- is this entity player

		game_PlayerState*	ps_who	=	get_id			(e_who->owner->ID);

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

		xrSE_Target_CSBase *l_pCSBase	=	dynamic_cast<xrSE_Target_CSBase*>(e_what);
		if(l_pCSBase) {
			// База
			if(ps_who->team == -1) ps_who->team = l_pCSBase->g_team(); // @@@ WT : Пока не сделан респавн
			if(l_pCSBase->s_team == ps_who->team) {				// Если игрок пришел на свою базу
				ps_who->flags |= GAME_PLAYER_FLAG_CS_ON_BASE;
			} else ps_who->flags |= GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE;
			return false;
		}

		xrSE_Target_CS *l_pMBall =  dynamic_cast<xrSE_Target_CS*>(e_what);
		if(l_pMBall) {
			// Мяч
			if(ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)		return false;
			ps_who->flags |= GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
			return true;
		}

		xrSE_Target_CSCask *l_pCSCask =  dynamic_cast<xrSE_Target_CSCask*>(e_what);
		if(l_pCSCask) {
			// Бочка
			// Если игрок на своей базе и у него есть мяч
			if((ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT) && (ps_who->flags&GAME_PLAYER_FLAG_CS_ON_BASE))		{
				l_pMBall = NULL;									// Отбираем у игрока мяч
				vector<u16>&	C			=	A->children;
				for (u32 it=0; it<C.size(); it++) {
					l_pMBall = dynamic_cast<xrSE_Target_CS*>(S->ID_to_entity(C[it]));
					if (l_pMBall) break;
				}
				R_ASSERT(l_pMBall);
				S->Perform_transfer(l_pMBall, A, l_pCSCask);		// Кладем мяч в бочку
				ps_who->flags &= ~GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
				teams[ps_who->team].num_targets++;
				if(teams[ps_who->team].num_targets == 3) {
					OnTeamScore(ps_who->team);
					u32	cnt = get_count();						// Доп. бонус за выполнение задания
					for(u32 it=0; it<cnt; it++)	{
						game_PlayerState* ps = get_it(it);
						if(ps->team == ps_who->team) ps->money_for_round += 1000;
					}											//
					OnRoundEnd("MISSION_complete");
				}
				signal_Syncronize();
				return false;
			}
			// Если игрок на чужой базе и у него нет мяча
			if(!(ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT) && (ps_who->flags&GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE))		{
				l_pMBall = NULL;									// Достаем мяч из бочки
				vector<u16>&	C			=	l_pCSCask->children;
				for (u32 it=0; it<C.size(); it++) {
					l_pMBall = dynamic_cast<xrSE_Target_CS*>(S->ID_to_entity(C[it]));
					if(l_pMBall) break;
				}
				if (l_pMBall){
					S->Perform_transfer(l_pMBall, l_pCSCask, A);		// Отдаем игроку
					ps_who->flags |= GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
					teams[(ps_who->team+1)%2].num_targets--;
					signal_Syncronize();
				}
				return false;
			}
		}
	}

	// We don't know what the hell is it, so disallow ownership just for safety 
	return FALSE;
}

BOOL	game_sv_CS::OnDetouch		(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	xrServerEntity*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	xrServerEntity*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	xrSE_Actor*			A		= dynamic_cast<xrSE_Actor*> (e_who);
	if (A) 	{
		game_PlayerState*	ps_who	=	get_id			(e_who->owner->ID);


		xrSE_Target_CS *l_pMBall =  dynamic_cast<xrSE_Target_CS*>(e_what);
		if(l_pMBall) {
			// Мяч
			if(ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)	{
				ps_who->flags &= ~GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
				return true;
			}
			return false;
		}

		xrSE_Target_CSBase *l_pCSBase	=	dynamic_cast<xrSE_Target_CSBase*>(e_what);
		if(l_pCSBase) {
			if(l_pCSBase->s_team == ps_who->team) {				// Если игрок пришел на свою базу
				ps_who->flags &= ~GAME_PLAYER_FLAG_CS_ON_BASE;
			} else ps_who->flags &= ~GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE;
			return false;
		}

		xrSE_Target_CSCask *l_pCSCask =  dynamic_cast<xrSE_Target_CSCask*>(e_what);
		if(l_pCSCask) return false;
	}
	return TRUE;
}

void	game_sv_CS::Update			()
{
	switch(phase) 	{
		case GAME_PHASE_INPROGRESS : {
				if (timelimit) if (s32(Device.TimerAsync()-u32(start_time))>timelimit) OnTimelimitExceed();
		} break;
		case GAME_PHASE_PENDING : {
				if ((Device.TimerAsync()-start_time)>u32(60*1000)) OnRoundStart();
		} break;
	}
}

void	game_sv_CS::OnPlayerReady			(u32 id)
{
	if	(GAME_PHASE_INPROGRESS == phase) return;

	game_PlayerState*	ps	=	get_id	(id);
	if (ps)
	{
		if (ps->flags & GAME_PLAYER_FLAG_READY)	
		{
			ps->flags &= ~GAME_PLAYER_FLAG_READY;
		} else {
			ps->flags |= GAME_PLAYER_FLAG_READY;

			// Check if all players ready
			u32		cnt		= get_count	();
			u32		ready	= 0;
			for		(u32 it=0; it<cnt; it++)	
			{
				ps		=	get_it	(it);
				if (ps->flags & GAME_PLAYER_FLAG_READY)	ready++;
			}

			if (ready == cnt)
			{
				OnRoundStart	();
			}
		}
	}
}

void game_sv_CS::OnPlayerConnect	(u32 id_who)
{
	__super::OnPlayerConnect	(id_who);

	LPCSTR	options			=	get_name_id	(id_who);

	// Spawn "actor"
	xrServerEntity*		E	=	spawn_begin	("actor");													// create SE
	xrSE_Actor*	A			=	(xrSE_Actor*) E;					
	strcpy					(A->s_name_replace,get_option_s(options,"name","Player"));					// name
	get_id(id_who)->team = A->s_team = u8(get_option_i(options,"team",AutoTeam()));						// team
	A->s_flags				=	M_SPAWN_OBJECT_ACTIVE  | M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER;// flags
	assign_RP				(A);
	spawn_end				(A,id_who);
}

void game_sv_CS::OnPlayerBuy		(u32 id_who, u16 eid_who, LPCSTR what)
{
	__super::OnPlayerBuy	(id_who,eid_who,what);

	// cost
	int cost				=	get_option_i	(what,"cost",0);
	if (0==cost)			return;
	int iAmmoMagCount		= 	get_option_i	(what,"ammo",0);
	if (iAmmoMagCount)	
	{ 
		// Buy ammo
		int		slot			=	get_option_i(what,"primary") ? 1:2;

		// Search weapon slot
		xrServer*		S		=	Level().Server;
		vector<u16>*	C		=	get_children(id_who);
		if (0==C)				return;
		for (u32 it=0; it<C->size(); it++)
		{
			xrServerEntity*		Et	= S->ID_to_entity				((*C)[it]);
			if (0==Et)				continue;
			xrSE_Weapon*		T	= dynamic_cast<xrSE_Weapon*>	(Et);
			if (0==T)				continue;
			if (slot == T->get_slot())	
			{
				// We've found this slot - buy something :) |  491 05 36 - Andy

				// TO: VITYA: I didn't bother to check all conditions, money, etc. Sorry :)))
				xrSE_Weapon*		W	= T;
				u16		a_limit			= W->get_ammo_limit		();
				u16		a_total			= W->get_ammo_total		();
				u16		a_magsize		= W->get_ammo_magsize	();
				while (iAmmoMagCount && (iAmmoMagCount*a_magsize+a_total)>(a_limit+a_magsize-1))	iAmmoMagCount--;
				if		(0==iAmmoMagCount)	return;
				int		a_cost			= iAmmoMagCount*cost;

				// check if has money to pay
				game_PlayerState*	ps_who	=	get_id	(id_who);
				if(ps_who->money_total < cost)	return;
				ps_who->money_total		= ps_who->money_total - s16(a_cost);

				// Event
				NET_Packet			P;
				u_EventGen			(P, GE_ADD_AMMO, W->ID);	   
				P.w_u16				(u16(iAmmoMagCount) * a_magsize);	// Amount of ammo to add
				u_EventSend			(P);
				return;
			}
		}
	} 
	else 
	{
		// Buy weapon
		// entity-name
		string64				name;
		strcpy					(name,what);
		if ( strchr(name,'/') )	*strchr(name,'/') = 0;

		// initialize spawn
		xrServerEntity*		E	=	spawn_begin	(name);														// create SE
		strcpy					(E->s_name_replace,name);													// name
		E->s_flags				=	M_SPAWN_OBJECT_ACTIVE  | M_SPAWN_OBJECT_LOCAL;							// flags
		E->ID_Parent			=	u16(eid_who);

		// check if has same-slot-weapon(s)
		xrSE_Weapon*		W	=	dynamic_cast<xrSE_Weapon*>(E);
		if (W)
		{
			xrServer*		S		=	Level().Server;
			vector<u16>*	C		=	get_children(id_who);
			if (0==C)				
			{
				F_entity_Destroy	(E);
				return;
			}
			u8 slot					=	W->get_slot	();
			for (u32 it=0; it<C->size(); it++)
			{
				xrServerEntity*		Et	= S->ID_to_entity				((*C)[it]);
				if (0==Et)				continue;
				xrSE_Weapon*		T	= dynamic_cast<xrSE_Weapon*>	(Et);
				if (0==T)				continue;
				if (slot == T->get_slot())	
				{
					// We've found same slot occupied - don't buy anything
					F_entity_Destroy	(E);
					return;
				}
			}
		}

		// check if has money to pay
		game_PlayerState*	ps_who	=	get_id	(id_who);
		if(ps_who->money_total < cost)	{ F_entity_Destroy(E); return; }
		ps_who->money_total		= ps_who->money_total - s16(cost);

		// Spawn item
		spawn_end				(E,	id_who);
	}
}

u8 game_sv_CS::AutoTeam() 
{
	u32	cnt = get_count(), l_teams[2] = {0,0};
	for(u32 it=0; it<cnt; it++)	{
		game_PlayerState* ps = get_it(it);
		if(ps->team>=0) l_teams[ps->team]++;
	}
	return (l_teams[0]>l_teams[1])?1:0;
}
