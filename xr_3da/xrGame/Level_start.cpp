#include "stdafx.h"

BOOL CLevel::net_Start	( LPCSTR options, BOOL server )
{
	BOOL bResult				= FALSE;

	pApp->LoadBegin				();

	// Start client and server if need it
	if (server)
	{
		pApp->LoadTitle				("SERVER: Starting...");

		// Activate level
		string64					l_name;
		strcpy						(l_name,options);
		if (strchr(l_name,'|'))		*strchr(l_name,'|') = 0;
		int id						= pApp->Level_ID(l_name);
		if (id<0)	
		{
			pApp->LoadEnd			();
			return FALSE;
		}
		pApp->Level_Set				(id);

		// Connect
		Server						= new xrServer;
		Server->Connect				(options);

		// Analyze game and perform apropriate SERVER spawning
		Server->SLS_Default			();

		// Start client
		bResult						= net_Start_client("localhost");
	} else {
		bResult						= net_Start_client(options);
	}

	// Analyze game and perform apropriate CLIENT spawning
	if (bResult) 
	{
		u16	spawn_flags					= M_SPAWN_OBJECT_ACTIVE  | M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER;
		switch (GameID())
		{
		default:
			Device.Fatal	("Unknown game type");
			break;
		case GAME_SINGLE:
			// Already spawned actor on server
			break;
		case GAME_DEATHMATCH:
			// Spawn at ANY r-point
			{
				g_cl_Spawn("actor",0xFF,spawn_flags);
			}
			break;
		case GAME_CTF:
			// Spawn at TEAM r-point
			{
				g_cl_Spawn("actor",0xFD,spawn_flags);
			}
			break;
		case GAME_ASSAULT:
			// Spawn at TEAM r-point
			{
				// Create
				xrServerEntity*	E	= F_entity_Create("actor");
				R_ASSERT			(E);
				xrSE_Actor* A		= (xrSE_Actor*)E;

				// Fill
				strcpy				(A->s_name,"actor");
				strcpy				(A->s_name_replace,"");
				A->s_gameid			=	u8(GameID());
				A->s_team			=	u8(0);
				E->s_RP				=	0xff;

				A->ID				=	0xffff;
				A->ID_Parent		=	0xffff;
				A->ID_Phantom		=	0xffff;
				A->s_flags			=	spawn_flags;
				A->RespawnTime		=	0;

				// Send
				NET_Packet			P;
				A->Spawn_Write		(P,TRUE);
				Send				(P,net_flags(TRUE));

				// Destroy
				F_entity_Destroy	(E);
			}
			break;
		}
	}

	pApp->LoadEnd					();

	return bResult;
}
