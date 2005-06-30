#include "StdAfx.h"
#include "ServerList.h"
/*
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
*/