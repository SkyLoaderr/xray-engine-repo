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
			g_sv_Spawn(P);
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
}
