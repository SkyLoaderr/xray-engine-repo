#include "stdafx.h"
#include "entity.h"

void CLevel::ClientReceive()
{
	for (NET_Packet* P = net_msg_Retreive(); P; P=net_msg_Retreive())
	{
		u16			m_type;
		u16			ID;
		P->r_begin	(m_type);
		switch (m_type)
		{
		case M_SPAWN:
			{
				// Begin analysis
				string64			s_name;
				P->r_string			(s_name);

				// Create DC (xrSE)
				CAbstractServerObject*		E	= F_entity_Create	(s_name);
				R_ASSERT			(E);
				E->Spawn_Read		(*P);
				if (E->s_flags.is(M_SPAWN_UPDATE))
					E->UPDATE_Read	(*P);
				game_spawn_queue.push_back(E);
			}
			break;
		case M_UPDATE:
			game.net_import_update	(*P);
			Objects.net_Import		(P);
			break;
		case M_SV_CONFIG_GAME:
			game.net_import_state	(*P);
			break;
		case M_SV_CONFIG_FINISHED:
			game_configured			= TRUE;
			break;
		case M_EVENT:
			game_events.insert		(*P);
			break;
		case M_MIGRATE_DEACTIVATE:	// TO:   Changing server, just deactivate
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (O)			O->net_MigrateInactive	(*P);
				Log("! MIGRATE_DEACTIVATE",O->cName());
			}
			break;
		case M_MIGRATE_ACTIVATE:	// TO:   Changing server, full state
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (O)			O->net_MigrateActive	(*P);
				Log("! MIGRATE_ACTIVATE",O->cName());
			}
			break;
		case M_CHAT:
			{
				char	buffer[256];
				P->r_string(buffer);
				Msg		("- %s",buffer);
			}
			break;
		}

		net_msg_Release();
	}

	if (!game_spawn_queue.empty())
	{
//		u32			m0	= Memory.stat_calls;
		CTimer		T;	T.Start		();

		g_sv_Spawn					(game_spawn_queue.front());
		F_entity_Destroy			(game_spawn_queue.front());
		game_spawn_queue.pop_front	();

		T.Stop		();
//		u32			m1	= Memory.stat_calls;
		//Msg			("--spawn--TOTAL: %f ms, %d mo",1000.f*T.Get(),m1-m0);
	}
}
