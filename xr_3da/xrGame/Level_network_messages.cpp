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
		case M_OWNERSHIP_TAKE:			// DUAL: Client request for ownership of an item
			{
				u16				ID_child;
				P->r_u16		(ID);
				P->r_u16		(ID_child);
				CObject*	O	= Objects.net_Find		(ID);
				CObject*	C	= Objects.net_Find		(ID_child);
				if (O && C)		O->net_OwnershipTake	(C);
				Log("! OWNERSHIP_TAKE",O->cName());
			}
			break;
		case M_OWNERSHIP_REJECT:		// DUAL: Client request ownership rejection
			{
				u16				ID_child;
				P->r_u16		(ID);
				P->r_u16		(ID_child);
				CObject*	O	= Objects.net_Find		(ID);
				CObject*	C	= Objects.net_Find		(ID_child);
				if (O && C)		O->net_OwnershipReject	(C);
				Log("! OWNERSHIP_REJECT",O->cName());
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
		case M_HIT:
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find	(ID);
				if (O)	{
					u16			ID2;
					P->r_u16	(ID2);
					CEntity*	WHO = dynamic_cast<CEntity*>	(Objects.net_Find(ID2));
					u8			perc;	P->r_u8		(perc);
					Fvector		dir;	P->r_dir	(dir);

					CEntity*	E	= dynamic_cast<CEntity*>	(O);
					if (E)		E->Hit(u32(perc),dir,WHO);
				}
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
