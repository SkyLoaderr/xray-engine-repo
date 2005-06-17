#include "StdAfx.h"
#include "ServerList.h"
#include "../GameSpy_defs.h" 

#include "../GameSpy/available.h"
#include "../GameSpy/QR2/qr2regkeys.h"

void __cdecl SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance);

void CServerList::GameSpy_Browser_Init()
{
	GameSpy_Browser_ClearServersList();
	//-----------------------------------------
	string16 secret_key;

	secret_key[0] = 't';
	secret_key[1] = '9';
	secret_key[2] = 'F';
	secret_key[3] = 'j';
	secret_key[4] = '3';
	secret_key[5] = 'M';
	secret_key[6] = 'x';
	secret_key[7] = '\0';
	//-----------------------------------------
	qr2_register_key(DEDICATED_KEY,   ("dedicated")  );
	qr2_register_key(FFIRE_KEY,   ("friendlyfire")  );

	GSIACResult result;
	GSIStartAvailableCheck(GAMESPY_GAMENAME);
	while((result = GSIAvailableCheckThink()) == GSIACWaiting)
		msleep(5);
	if(result != GSIACAvailable)
	{
		Msg("! The GameSpy backend is not available");
		return;
	}
	else
	{
		Msg("- The GameSpy backend is available");
	};

	m_serverBrowser = ServerBrowserNew(GAMESPY_GAMENAME, GAMESPY_GAMENAME, secret_key, 0, MAX_UPDATES, QVERSION_QR2, SBCallback, this);
	if (!m_serverBrowser)
	{
		Msg("! Unable to init GameSpy Server Browser!");
	}
	else
		Msg("- GameSpy Server Browser Inited!");
};

void CServerList::GameSpy_Browser_Destroy()
{
	GameSpy_Browser_ClearServersList();
	if (m_serverBrowser)
	{
		ServerBrowserFree(m_serverBrowser);
		m_serverBrowser = NULL;
	}
};

void __cdecl SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
	CServerList* pServerList = (CServerList*) instance;
	switch (reason)
	{
	case sbc_serveradded : //a server was added to the list, may just have an IP & port at this point
		{
			Msg("sbc_serveradded");
			pServerList->GameSpy_Browser_OnServerAdded(server);
		}break;
	case sbc_serverupdated : //server information has been updated - either basic or full information is now available about this server
		{
			Msg("sbc_serverupdated");
			pServerList->GameSpy_Browser_OnServerUpdated(server);
		}break;
	case sbc_serverupdatefailed : //an attempt to retrieve information about this server, either directly or from the master, failed
		{
			Msg("sbc_serverupdatefailed");
			pServerList->GameSpy_Browser_OnServerRemoved(server);
		}break;
	case sbc_serverdeleted : //a server was removed from the list
		{
			Msg("sbc_serverdeleted");
			pServerList->GameSpy_Browser_OnServerRemoved(server);
		}break;
	case sbc_updatecomplete : //the server query engine is now idle 
		{
			Msg("sbc_updatecomplete");			
			pServerList->GameSpy_Browser_OnUpdateCompleted();
		}break;
	case sbc_queryerror		://the master returned an error string for the provided query
		{
			Msg("sbc_queryerror");
		}break;
	};
};

void CServerList::GameSpy_Browser_RefreshList (bool Local)
{
	// if we're doing an update, cancel it
	SBState state = ServerBrowserState(m_serverBrowser);
	if((state != sb_connected) && (state != sb_disconnected))
	{
		ServerBrowserHalt(m_serverBrowser);
		Msg("GameSpy Refresh Stopped\n");		
	};

	ServerBrowserClear(m_serverBrowser);
	GameSpy_Browser_ClearServersList();

	// fields we're interested in	
	int numFields = sizeof(Fields_Of_Interest) / sizeof(Fields_Of_Interest[0]);

	// do an update
	SBError error;
	if(!Local)
		error = ServerBrowserUpdate(m_serverBrowser, SBFalse, SBFalse, Fields_Of_Interest, numFields, (char *)(const char *)"");
	else
		error = ServerBrowserLANUpdate(m_serverBrowser, SBFalse, START_PORT, END_PORT);

	if (error != sbe_noerror)
	{
		Msg("! GameSpy Update Error!");	
	}
};

void CServerList::GameSpy_Browser_OnServerAdded		(SBServer server)
{
	if (!server) return;
	SERVERINFO_it pSI_it = GameSpy_Browser_GetServerInfo(server, true);
	if (pSI_it == NULL) return;
	GameSpy_Browser_ReadServerData(server, &(*pSI_it));
};

void CServerList::GameSpy_Browser_OnServerUpdated	(SBServer server)
{
	if (!server) return;
	SERVERINFO_it pSI_it = GameSpy_Browser_GetServerInfo(server, false);
	if (pSI_it == NULL) return;
	GameSpy_Browser_ReadServerData(server, &(*pSI_it));
};

void CServerList::GameSpy_Browser_OnServerRemoved	(SBServer server)
{
	if (!server) return;
	SERVERINFO_it pSI_it = GameSpy_Browser_GetServerInfo(server, false);
	if (pSI_it == NULL) return;
	m_aServers.erase(pSI_it);
};

void	CServerList::GameSpy_Browser_ReadServerData	(SBServer server, ServerInfo* pServerInfo)
{
	if (!server) return;
	sprintf(pServerInfo->m_Address, "%s:%d", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));
	sprintf(pServerInfo->m_HostName, "%s", SBServerGetPublicAddress(server));
	sprintf(pServerInfo->m_ServerName, "%s", SBServerGetStringValue(server, qr2_registered_key_list[HOSTNAME_KEY], pServerInfo->m_HostName));

	sprintf(pServerInfo->m_SessionName, "%s", SBServerGetStringValue(server, qr2_registered_key_list[MAPNAME_KEY], "Unknown"));
	sprintf(pServerInfo->m_ServerGameType, "%s", SBServerGetStringValue(server, qr2_registered_key_list[GAMETYPE_KEY], "UNKNOWN"));
	pServerInfo->m_bPassword	= SBServerGetBoolValue(server, qr2_registered_key_list[PASSWORD_KEY], SBFalse) == SBTrue;
	pServerInfo->m_Ping = (s16)SBServerGetPing(server);
	pServerInfo->m_ServerNumPlayers = (s16)SBServerGetIntValue(server, qr2_registered_key_list[NUMPLAYERS_KEY], 0);
	pServerInfo->m_ServerMaxPlayers = (s16)SBServerGetIntValue(server, qr2_registered_key_list[MAXPLAYERS_KEY], 32);
	pServerInfo->m_bDedicated	= SBServerGetBoolValue(server, qr2_registered_key_list[DEDICATED_KEY], SBFalse) == SBTrue;
	pServerInfo->m_bFFire		= SBServerGetBoolValue(server, qr2_registered_key_list[FFIRE_KEY], SBFalse) == SBTrue;
	pServerInfo->m_Port		= (s16)SBServerGetIntValue(server, qr2_registered_key_list[HOSTPORT_KEY], 0);
//	pServerInfo->m_Server = server;

};

SERVERINFO_it	CServerList::GameSpy_Browser_GetServerInfo	(SBServer server, bool Create)
{
	string128 Address;
	sprintf(Address, "%s:%d", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));

	SERVERINFO_it pServerInfoI = NULL;
	pServerInfoI	= std::find(m_aServers.begin(), m_aServers.end(), Address);
	if (pServerInfoI == m_aServers.end() || !((*pServerInfoI) == Address))
	{
		pServerInfoI = NULL;
		if (Create)
		{
			m_aServers.push_back(ServerInfo(Address));
			pServerInfoI = m_aServers.end()-1;
		}
	};
	return pServerInfoI;
};

void CServerList::GameSpy_Browser_OnUpdateCompleted()
{
	m_list.RemoveAll();
	if (m_aServers.empty()) return;
	for (u32 i=0; i<m_aServers.size(); i++)
	{
		ServerInfo* pSI = &(m_aServers[i]);
		AddServerToList(pSI);
	}
};
