#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "xrServer.h"
#include "xrMessages.h"

void	game_sv_Deathmatch::OnEvent (NET_Packet &P, u16 type, u32 time, u32 sender )
{

	switch	(type)
	{
	case GAME_EVENT_PLAYER_READY:// cs & dm 
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			OnPlayerReady		(l_pC->ID);

/*	//!!!
			CSE_Abstract*		E			= game->get_entity_from_eid	(destination);
			if (E) {
				xrClientData*	C			= E->owner;
				if (C && (C->owner == E))
				{
					OnPlayerReady		(C->ID);
				}

			}
*/
		}break;

	case GAME_EVENT_PLAYER_CHANGE_SKIN: //dm only
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			u8 l_skin;
			P.r_u8(l_skin);
			OnPlayerChangeSkin(l_pC->ID, l_skin);
		}break;

	case GAME_EVENT_PLAYER_KILL: //dm only
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			OnPlayerWantsDie(l_pC->ID);
		}break;

	case GAME_EVENT_PLAYER_BUY_FINISHED: // dm only
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			OnPlayerBuyFinished(l_pC->ID, P);
		}break;

	default:
		inherited::OnEvent(P, type, time, sender);
	};//switch
}


