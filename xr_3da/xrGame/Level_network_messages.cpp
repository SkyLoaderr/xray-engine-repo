#include "stdafx.h"
#include "entity.h"

void CLevel::ClientReceive()
{
	for (NET_Packet* P = net_Retreive(); P; P=net_Retreive())
	{
		u16			m_type;
		u16			ID;
		P->r_begin	(m_type);
		switch (m_type)
		{
		case M_SPAWN:
			g_sv_Spawn(P);
			break;
		case M_DESTROY:
			{
				u16 count;
				P->r_u16				(count);
				for (; count; count--)	
				{
					P->r_u16			(ID);
					CObject* O			= Objects.net_Find(u32(ID));
					if (O)	
					{
						O->net_Destroy			();
						Objects.DestroyObject	(O);
					}
				}
			}
			break;
		case M_UPDATE:
			Objects.net_Import	(P);
			break;
		case M_EVENT:
			{
				u32	time; u16 type;
				P->r_u32		(time);
				P->r_u16		(type);
				P->r_u16		(ID);
				CObject*	 O	= Objects.net_Find	(ID);
				if (0==O)		break;
				CGameObject* GO = dynamic_cast<CGameObject*>(O);
				if (0==GO)		break;
				GO->net_Event	(*P);
			}
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
	}
}
