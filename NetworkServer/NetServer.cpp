// NetworkServer.cpp: implementation of the CNetworkServer class.
//
//////////////////////////////////////////////////////////////////////

#include	"stdafx.h"
#include	"NetServer.h"
#include	"net_messages.h"
#include	"log.h"

void			Timestamp(DWORD t);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNetworkServer::CNetworkServer(EProtocol P, char *session, DWORD dwFlags) : CNetwork()
{
	Msg						("Starting NETWORK SERVER '%s'...",session);
	R_ASSERT				(SelectProtocol	(P));
	R_ASSERT				(ApplyProtocol	( ));
	R_ASSERT				(strlen(session)<14);

	DPSESSIONDESC2			dpsd;
	DPNAME					dpname;
	ZeroMemory				( &dpsd, sizeof(dpsd) );
	dpsd.dwSize				= sizeof(dpsd);
	dpsd.guidApplication	= g_AppGUID;
	dpsd.lpszSessionNameA	= session;
	dpsd.dwMaxPlayers		= 8;
	dpsd.dwFlags			= dwFlags; 
	R_CHK					( pDPlay->Open( &dpsd, DPOPEN_CREATE) );

	ZeroMemory				( &dpname, sizeof(DPNAME) );
	dpname.dwSize			= sizeof(DPNAME);
	dpname.lpszShortNameA	= session;

	R_CHK					( pDPlay->CreatePlayer( &dpLocalPlayerID, &dpname, 
							NULL, NULL, 0, DPPLAYER_SERVERPLAYER ) );

	tDiff					= timeGetTime();	// difference between system & game time
	fTime					= 0;
	Log						("NETWORK SERVER started.");
}

CNetworkServer::~CNetworkServer()
{
	R_ASSERT				(dpLocalPlayerID);
	R_CHK					(pDPlay->DestroyPlayer(dpLocalPlayerID));
	R_CHK					(pDPlay->Close());

	// the base class destructor do the other stuff here :)
}

extern float fTimeDelta;
void CNetworkServer::OnFrame()
{
	if (OnMove(fTimeDelta)) {
		char buf[1024],buf2[256];
		sprintf(buf,"Players: %d\n",players.size());
		for (int i=0; i<players.size(); i++) {
			sprintf(buf2,"%14s %d\n",players[i].name,players[i].dwPing);
			strcat(buf,buf2);
		}
		Status(buf);
		Timestamp(GetTimestamp());
	}

	// See what's out there
	while (true) {
		DPID    idFrom	= 0;
		DPID    idTo	= 0;
		DWORD   dwSize	= 1024;
		HRESULT hrr		= DP_OK;
		
		hrr = pDPlay->Receive( &idFrom, &idTo, DPRECEIVE_ALL, MessageData, &dwSize );
		if( hrr == DPERR_NOMESSAGES )	break;
		else							R_CHK(hrr);	
		
		// Handle the net_messages. If its from DPID_SYSMSG, its a system message, 
		// otherwise its an application message. 
		if( idFrom == DPID_SYSMSG ) {
			DPMSG_GENERIC* pMsg = (DPMSG_GENERIC*) MessageData;
			HandleSystemMessages(pMsg);
		} else {
			// application messages
			GAMEMSG *M = (GAMEMSG *)MessageData;
			net_LogMessage("GET",M->dwType);
		}
	}
}

void CNetworkServer::OnPlayerCreate(DPID id)
{
	Msg("'%s' connected",GetNameByID(id));
}
void CNetworkServer::OnPlayerDestroy(DPID id)
{
	Msg("'%s' disconnected",GetNameByID(id));
}
