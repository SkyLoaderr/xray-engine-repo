#include "stdafx.h"
#include "xrGameSpyServer.h"
#include "xrGameSpyServer_callbacks.h"

//--------------------------- QR2 callbacks ---------------------------------------
void callback_serverkey(int keyid, qr2_buffer_t outbuf, void *userdata)
{
	if (!userdata) return;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;

	switch (keyid)
	{
	case HOSTNAME_KEY:
		qr2_buffer_add(outbuf, pServer->HostName.c_str());;
		break;
	case MAPNAME_KEY:
		qr2_buffer_add(outbuf, pServer->MapName.c_str());
		break;
	case GAMETYPE_KEY:
		qr2_buffer_add(outbuf, pServer->game->type_name());
		break;
	case NUMPLAYERS_KEY:
		qr2_buffer_add_int(outbuf, pServer->GetPlayersCount());
		break;
	case MAXPLAYERS_KEY:
		qr2_buffer_add_int(outbuf, pServer->m_iMaxPlayers);
		break;
	case PASSWORD_KEY:
		{
			if ( 0 == *(pServer->Password))
				qr2_buffer_add_int(outbuf, 0);
			else
				qr2_buffer_add_int(outbuf, 1);
		}break;
	case GAMEVER_KEY:
		qr2_buffer_add(outbuf, "0.1");
		break;
	case DEDICATED_KEY:
		{
			qr2_buffer_add_int(outbuf, g_pGamePersistent->bDedicatedServer);
		}break;
	case HOSTPORT_KEY:
		{
			qr2_buffer_add_int(outbuf, pServer->GetPort());
		}break;
	case FFIRE_KEY:
		{
			if (pServer->game && pServer->game->CanHaveFriendlyFire())
			{			
				qr2_buffer_add_int(outbuf, pServer->game->isFriendlyFireEnabled());
			}
			qr2_buffer_add_int(outbuf, 0);
		}break;
	}
	GSI_UNUSED(userdata);
};

void callback_playerkey(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
};

void callback_teamkey(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
};

void callback_keylist(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata)
{
	switch (keytype)
	{
	case key_server:
		qr2_keybuffer_add(keybuffer, HOSTNAME_KEY);
		qr2_keybuffer_add(keybuffer, MAPNAME_KEY);
		qr2_keybuffer_add(keybuffer, GAMEVER_KEY);
		qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);		
		qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);

		qr2_keybuffer_add(keybuffer, GAMETYPE_KEY);
		qr2_keybuffer_add(keybuffer, PASSWORD_KEY);

		qr2_keybuffer_add(keybuffer, HOSTPORT_KEY);
		qr2_keybuffer_add(keybuffer, DEDICATED_KEY);
		qr2_keybuffer_add(keybuffer, FFIRE_KEY);
		break;
	case key_player:
//		qr2_keybuffer_add(keybuffer, PLAYER__KEY);
//		qr2_keybuffer_add(keybuffer, SCORE__KEY);
//		qr2_keybuffer_add(keybuffer, DEATHS__KEY);
//		qr2_keybuffer_add(keybuffer, PING__KEY);
//		qr2_keybuffer_add(keybuffer, TEAM__KEY);
		break;
	case key_team:
//		qr2_keybuffer_add(keybuffer, TEAM_T_KEY);
//		qr2_keybuffer_add(keybuffer, SCORE_T_KEY);
		break;
	};

	GSI_UNUSED(userdata);
};

int callback_count(qr2_key_type keytype, void *userdata)
{
	if (!userdata) return 0;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	switch (keytype)
	{
	case key_player:
		{
			return pServer->client_Count();
		}break;
	case key_team:
		{
			if (!pServer->game) return 0;
			switch (pServer->game->Type())
			{
			case GAME_DEATHMATCH:
				return 1;
			case GAME_TEAMDEATHMATCH:
				return 2;
			case GAME_ARTEFACTHUNT:
				return 2;
			}
		}break;
	default:
		return 0;
	}

	GSI_UNUSED(userdata);

	return 0;
};

void callback_adderror(qr2_error_t error, gsi_char *errmsg, void *userdata)
{
	int x=0;
	x=x;
};

void callback_nn(int cookie, void *userdata)
{
};

void callback_cm(gsi_char *data, int len, void *userdata)
{
};

//--------------------------- CD Key callbacks -----------------------------------
void ClientAuthorizeCallback(int productid, int localid, int authenticated, char *errmsg, void *instance)
{
	xrGameSpyServer* pServer = (xrGameSpyServer*) instance;
	ClientID ID; ID.set(u32(localid));
	if (pServer) pServer->OnCDKey_Validation(ID, authenticated, errmsg);
	/*
	if (authenticated)
	{
		Msg("GameSpy::CDKey::Server : Client accepted - <%s>", errmsg);
	}
	else
	{
		Msg("GameSpy::CDKey::Server : Client rejected - <%s>", errmsg);
	}
	*/
};