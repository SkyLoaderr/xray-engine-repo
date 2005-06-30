#include "StdAfx.h"
#include "GameSpy_Browser.h"
#include "GameSpy_Base_Defs.h"

#include "GameSpy_Available.h"

void __cdecl SBCallback(void* sb, SBCallbackReason reason, void* server, void *instance);

CGameSpy_Browser::CGameSpy_Browser()
{
	m_SecretKey[0] = 't';
	m_SecretKey[1] = '9';
	m_SecretKey[2] = 'F';
	m_SecretKey[3] = 'j';
	m_SecretKey[4] = '3';
	m_SecretKey[5] = 'M';
	m_SecretKey[6] = 'x';
	m_SecretKey[7] = '\0';
	//-------------------------
	hGameSpyDLL = NULL;

	LoadGameSpy();
	//-------------------------
	m_pGSBrowser = NULL;
	//-------------------------
	
};

CGameSpy_Browser::~CGameSpy_Browser()
{
	if (m_pGSBrowser)
	{		
		xrGS_ServerBrowserFree(m_pGSBrowser);
		m_pGSBrowser = NULL;
	}
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};

void	CGameSpy_Browser::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");
	
	GAMESPY_LOAD_FN(xrGS_ServerBrowserNew);	
	GAMESPY_LOAD_FN(xrGS_ServerBrowserFree);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserClear);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserState);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserHalt);	
	GAMESPY_LOAD_FN(xrGS_ServerBrowserUpdate);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserLANUpdate);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserCount);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserGetServer);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserGetServerByIP);

	GAMESPY_LOAD_FN(xrGS_SBServerGetPublicAddress);
	GAMESPY_LOAD_FN(xrGS_SBServerGetPublicQueryPort);
	GAMESPY_LOAD_FN(xrGS_SBServerGetStringValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetIntValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetFloatValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetBoolValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetPing);
}

bool	CGameSpy_Browser::Init()
{
	//-------------------------------------
	CGameSpy_Available GSA;
	if (!GSA.CheckAvailableServices()) return false;
	//-------------------------------------
	m_QR2.RegisterAdditionalKeys();

	m_pGSBrowser = xrGS_ServerBrowserNew(GAMESPY_GAMENAME, GAMESPY_GAMENAME, m_SecretKey, 0, GAMESPY_MAX_UPDATES, GSA.xrGS_GetQueryVersion(), SBCallback, this);
	if (!m_pGSBrowser)
	{
		Msg("! Unable to init GameSpy Server Browser!");
	}
	else
		Msg("- GameSpy Server Browser Inited!");

	return true;
};

void			CGameSpy_Browser::RefreshList_Full(bool Local)
{
	SBState state = xrGS_ServerBrowserState(m_pGSBrowser);               
	if((state != sb_connected) && (state != sb_disconnected))
	{
		xrGS_ServerBrowserHalt(m_pGSBrowser);
		Msg("GameSpy Refresh Stopped\n");		
	};

	xrGS_ServerBrowserClear(m_pGSBrowser);
//	GameSpy_Browser_ClearServersList();

	// fields we're interested in	
	int numFields = sizeof(Fields_Of_Interest) / sizeof(Fields_Of_Interest[0]);

	// do an update
	SBError error;
	if(!Local)
		error = xrGS_ServerBrowserUpdate(m_pGSBrowser, SBFalse, SBFalse, Fields_Of_Interest, numFields, (char *)(const char *)"");
	else
		error = xrGS_ServerBrowserLANUpdate(m_pGSBrowser, SBFalse, START_PORT, END_PORT);

	if (error != sbe_noerror)
	{
		Msg("! GameSpy Update Error!");	
	}
};

void __cdecl SBCallback(void* sb, SBCallbackReason reason, void* server, void *instance)
{
//	CGameSpy_Browser* pGSBrowser = (CGameSpy_Browser*) instance;
	switch (reason)
	{
	case sbc_serveradded : //a server was added to the list, may just have an IP & port at this point
		{
			Msg("sbc_serveradded");
		}break;
	case sbc_serverupdated : //server information has been updated - either basic or full information is now available about this server
		{
			Msg("sbc_serverupdated");
		}break;
	case sbc_serverupdatefailed : //an attempt to retrieve information about this server, either directly or from the master, failed
		{
			Msg("sbc_serverupdatefailed");
		}break;
	case sbc_serverdeleted : //a server was removed from the list
		{
			Msg("sbc_serverdeleted");
		}break;
	case sbc_updatecomplete : //the server query engine is now idle 
		{
			Msg("sbc_updatecomplete");
		}break;
	case sbc_queryerror		://the master returned an error string for the provided query
		{
			Msg("sbc_queryerror");
		}break;
	default:
		{
			R_ASSERT2(0, "Unknown Callback Reason");
		}break;
	};
};

void CGameSpy_Browser::CallBack_OnUpdateCompleted()
{
	int NumServers = xrGS_ServerBrowserCount(m_pGSBrowser);

	ServerInfo NewServerInfo;
	for (int i=0; i<NumServers; i++)
	{
		void* pServer = xrGS_ServerBrowserGetServer(m_pGSBrowser, i);
		ReadServerInfo(&NewServerInfo, pServer);
	}
};

int	CGameSpy_Browser::GetServersCount()
{
	return xrGS_ServerBrowserCount(m_pGSBrowser);
};

void CGameSpy_Browser::GetServerInfoByIndex(ServerInfo* pServerInfo, int idx)
{
	void* pServer = xrGS_ServerBrowserGetServer(m_pGSBrowser, idx);
	ReadServerInfo(pServerInfo, pServer);
	pServerInfo->Index = idx;
}

void	CGameSpy_Browser::ReadServerInfo	(ServerInfo* pServerInfo, void* pServer)
{
	if (!pServer || !pServerInfo) return;
	sprintf(pServerInfo->m_Address, "%s:%d", xrGS_SBServerGetPublicAddress(pServer), xrGS_SBServerGetPublicQueryPort(pServer));
	sprintf(pServerInfo->m_HostName, "%s", xrGS_SBServerGetPublicAddress(pServer));
	sprintf(pServerInfo->m_ServerName, "%s", xrGS_SBServerGetStringValue(pServer, m_QR2.xrGS_RegisteredKey(HOSTNAME_KEY), pServerInfo->m_HostName));

	sprintf(pServerInfo->m_SessionName, "%s", xrGS_SBServerGetStringValue(pServer, m_QR2.xrGS_RegisteredKey(MAPNAME_KEY), "Unknown"));
	sprintf(pServerInfo->m_ServerGameType, "%s", xrGS_SBServerGetStringValue(pServer, m_QR2.xrGS_RegisteredKey(GAMETYPE_KEY), "UNKNOWN"));
	pServerInfo->m_bPassword	= xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(PASSWORD_KEY), SBFalse) == SBTrue;
	pServerInfo->m_Ping = (s16)xrGS_SBServerGetPing(pServer);
	pServerInfo->m_ServerNumPlayers = (s16)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(NUMPLAYERS_KEY), 0);
	pServerInfo->m_ServerMaxPlayers = (s16)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(MAXPLAYERS_KEY), 32);
	pServerInfo->m_bDedicated	= (xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(DEDICATED_KEY), SBFalse)) == SBTrue;
	pServerInfo->m_bFFire		= xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(FFIRE_KEY), SBFalse) == SBTrue;
	pServerInfo->m_Port		= (s16)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(HOSTPORT_KEY), 0);
};

