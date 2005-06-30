#include "StdAfx.h"
#include "../xrGameSpyServer.h"
#include "GameSpy_Keys.h"
//--------------------------- QR2 callbacks ---------------------------------------
void __cdecl callback_serverkey(int keyid, void* outbuf, void *userdata)
{
	if (!userdata) return;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;

	switch (keyid)
	{
	case HOSTNAME_KEY:
		pQR2->BufferAdd(outbuf, pServer->HostName.c_str());;
		break;
	case MAPNAME_KEY:
		pQR2->BufferAdd(outbuf, pServer->MapName.c_str());
		break;
	case GAMETYPE_KEY:
		pQR2->BufferAdd(outbuf, pServer->game->type_name());
		break;
	case NUMPLAYERS_KEY:
		pQR2->BufferAdd_Int(outbuf, pServer->GetPlayersCount());
		break;
	case MAXPLAYERS_KEY:
		pQR2->BufferAdd_Int(outbuf, pServer->m_iMaxPlayers);
		break;
	case PASSWORD_KEY:
		{
			if ( 0 == *(pServer->Password))
				pQR2->BufferAdd_Int(outbuf, 0);
			else
				pQR2->BufferAdd_Int(outbuf, 1);
		}break;
	case GAMEVER_KEY:
		pQR2->BufferAdd(outbuf, "0.1");
		break;
	case DEDICATED_KEY:
		{
			pQR2->BufferAdd_Int(outbuf, pServer->m_bDedicated);
		}break;
	case HOSTPORT_KEY:
		{
			pQR2->BufferAdd_Int(outbuf, pServer->GetPort());
		}break;
	case FFIRE_KEY:
		{
			if (pServer->game && pServer->game->CanHaveFriendlyFire())
			{			
				pQR2->BufferAdd_Int(outbuf, pServer->game->isFriendlyFireEnabled());
				break;
			}
			pQR2->BufferAdd_Int(outbuf, 0);
		}break;
	}
	//GSI_UNUSED(userdata);
};

void __cdecl callback_playerkey(int keyid, int index, void* outbuf, void *userdata)
{
};

void __cdecl callback_teamkey(int keyid, int index, void* outbuf, void *userdata)
{
};

void __cdecl callback_keylist(qr2_key_type keytype, void* keybuffer, void *userdata)
{
	if (!userdata) return;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;
	
	switch (keytype)
	{
	case key_server:
		pQR2->KeyBufferAdd(keybuffer, HOSTNAME_KEY);
		pQR2->KeyBufferAdd(keybuffer, MAPNAME_KEY);
		pQR2->KeyBufferAdd(keybuffer, GAMEVER_KEY);
		pQR2->KeyBufferAdd(keybuffer, NUMPLAYERS_KEY);		
		pQR2->KeyBufferAdd(keybuffer, MAXPLAYERS_KEY);

		pQR2->KeyBufferAdd(keybuffer, GAMETYPE_KEY);
		pQR2->KeyBufferAdd(keybuffer, PASSWORD_KEY);

		pQR2->KeyBufferAdd(keybuffer, HOSTPORT_KEY);
		pQR2->KeyBufferAdd(keybuffer, DEDICATED_KEY);
		pQR2->KeyBufferAdd(keybuffer, FFIRE_KEY);
		break;
	case key_player:
		//		pQR2->KeyBufferAdd(keybuffer, PLAYER__KEY);
		//		pQR2->KeyBufferAdd(keybuffer, SCORE__KEY);
		//		pQR2->KeyBufferAdd(keybuffer, DEATHS__KEY);
		//		pQR2->KeyBufferAdd(keybuffer, PING__KEY);
		//		pQR2->KeyBufferAdd(keybuffer, TEAM__KEY);
		break;
	case key_team:
		//		pQR2->KeyBufferAdd(keybuffer, TEAM_T_KEY);
		//		pQR2->KeyBufferAdd(keybuffer, SCORE_T_KEY);
		break;
	};

	//GSI_UNUSED(userdata);
};

int __cdecl callback_count(qr2_key_type keytype, void *userdata)
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

	//GSI_UNUSED(userdata);

	return 0;
};

void __cdecl callback_adderror(qr2_error_t error, char *errmsg, void *userdata)
{
	int x=0;
	x=x;
};

void __cdecl callback_nn(int cookie, void *userdata)
{
};

void __cdecl callback_cm(char *data, int len, void *userdata)
{
};
