#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "HUDmanager.h"

void	game_sv_Deathmatch::Create					(LPSTR &options)
{
	__super::Create					(options);
	fraglimit	= get_option_i		(options,"fraglimit",0);
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
	switch_Phase(GAME_PHASE_PENDING);
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

		// spawn
		LPCSTR					options	=	get_name_it	(it);
		CSE_Abstract			*E		=	spawn_begin	("actor");													// create SE
		CSE_ALifeCreatureActor				*A		=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
		strcpy					(A->s_name_replace,get_option_s(options,"name","Player"));					// name
		A->s_team				=	u8(0);																	// no-team
		A->s_flags.set			(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);	// flags
		assign_RP				(A);
		spawn_end				(A,get_it_2_id(it));
	}
}

void	game_sv_Deathmatch::OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
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
			if ((Device.TimerAsync()-start_time)>u32(10*1000))
				OnRoundStart();
		}
		break;
	}
}
	
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
	return FALSE;
}

BOOL	game_sv_Deathmatch::OnDetach		(u16 eid_who, u16 eid_what)
{
	return TRUE;
}

void	game_sv_Deathmatch::OnPlayerReady			(u32 id)
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
			for		(u32 it=0; it<cnt; ++it)	
			{
				ps		=	get_it	(it);
				if (ps->flags & GAME_PLAYER_FLAG_READY)	++ready;
			}

			if (ready == cnt)
			{
				OnRoundStart	();
			}
		}
	}
}

void game_sv_Deathmatch::OnPlayerConnect	(u32 id_who)
{
	__super::OnPlayerConnect	(id_who);

	LPCSTR	options			=	get_name_id	(id_who);

	// Spawn "actor"
	CSE_Abstract			*E	=	spawn_begin	("actor");													// create SE
	CSE_ALifeCreatureActor				*A	=	dynamic_cast<CSE_ALifeCreatureActor*>(E);
	strcpy					(A->s_name_replace,get_option_s(options,"name","Player"));					// name
	A->s_team				=	u8(0);																	// no-team
	A->s_flags.set			(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);	// flags
	assign_RP				(A);
	spawn_end				(A,id_who);
}


void game_sv_Deathmatch::OnPlayerDisconnect		(u32 id_who)
{
	__super::OnPlayerDisconnect	(id_who);

	LPCSTR	Name = NULL;

	Name = get_name_id(id_who);
	Name = get_option_s(Name,"name",Name);


	xrServer*	S					=	Level().Server;
	
	// Remove everything
	xr_vector<u16>*	C				=	get_children(id_who);
	xr_vector<u16>::iterator i,e;
	i=C->begin();
	e=C->end();
	for(;i!=e;++i)
//	while(C->size())
	{
		u16		eid						= (*i);
		
		CSE_Abstract*		what		= S->ID_to_entity(eid);
		if (!what) continue;
		S->Perform_destroy				(what,net_flags(TRUE, TRUE), TRUE);
	}
	CSE_Abstract*		from		= S->ID_to_entity(get_id_2_eid(id_who));
	S->Perform_destroy				(from,net_flags(TRUE, TRUE), TRUE);

//	HUD().outMessage			(0xffffffff,"DM","Player '%s' disconnected",Name);
};