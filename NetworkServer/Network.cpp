// Network.cpp: implementation of the CNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Network.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL _EnumConnectionsCallback( LPCGUID   pguidSP,
                                          VOID*     pConnection,
                                          DWORD     dwConnectionSize,
                                          LPCDPNAME pName,
                                          DWORD     dwFlags,
                                          VOID*     pvContext )
{
    HRESULT       hr;

    // Create a IDirectPlay object
    LPDIRECTPLAY4 pDP = NULL;
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_ALL,
                                       IID_IDirectPlay4A, (VOID**)&pDP ) ) )
        return FALSE; // Error, stop enumerating

    // Test the if the connection is available by attempting to initialize the connection
    if( FAILED( hr = pDP->InitializeConnection( pConnection, 0 ) ) )
    {
        _RELEASE( pDP );
        return TRUE; // Unavailable connection, keep enumerating
    }
    _RELEASE( pDP ); // Don't need the IDirectPlay interface anymore, so release it

	CNetwork *p = (CNetwork *)pvContext;
	if (memcmp(p->pGUID,pguidSP,16)==0) {
		// Found a good connection, so put it in the list
		p->pConnection=malloc(dwConnectionSize);
		CopyMemory(p->pConnection, pConnection, dwConnectionSize );
		return FALSE;
	}
    return TRUE; // Keep enumerating
}

//******************************* Create direct play
void CNetwork::CreateDPlay()
{
	HRESULT hr;
    if( FAILED(hr = CoCreateInstance(	CLSID_DirectPlay, NULL, CLSCTX_ALL,
 										IID_IDirectPlay4A, (VOID**)&pDPlay ) ) )
    {
		Device.Error(hr,__FILE__,__LINE__);
    }
}

//******************************* Constructor
CNetwork::CNetwork()
{
	CreateDPlay			();

	pConnection			= NULL;

	dpLocalPlayerID		= 0;
	curProtocol			= dpTCP;

	fTime				= PING_UPDATE_TIME;
}

CNetwork::~CNetwork()
{
	_FREE				( pConnection			);
	_RELEASE			( pDPlay	);
}

BOOL CNetwork::SelectProtocol(EProtocol P)
{
	pGUID = NULL;

	Log("* NET: SelectProtocol: ",DWORD(P));

	//******************************* Free previous data
	_FREE(pConnection);

	//******************************* Select GUID
	switch (P)
	{
	case dpTCP:			pGUID = (void *)&DPSPGUID_TCPIP;	break;
	case dpIPX:			pGUID = (void *)&DPSPGUID_IPX;		break;
	case dpSERIAL:		pGUID = (void *)&DPSPGUID_SERIAL;	break;
	case dpMODEM2MODEM:	pGUID = (void *)&DPSPGUID_MODEM;	break;
	default:			return false;
	}

	//******************************* Enumerate protocols
	if( FAILED( pDPlay->EnumConnections(
		&g_AppGUID, _EnumConnectionsCallback, this, DPCONNECTION_DIRECTPLAY
		) ) )
	{
		return false;
	}
	curProtocol = P;
	if (pConnection)	return true;
	else				return false;
}

BOOL CNetwork::ApplyProtocol()
{
	Log("* NET: ApplyProtocol");

	if (pConnection==NULL) return false;
	if (FAILED(pDPlay->InitializeConnection(pConnection,0))) return false;
	return true;
}

void CNetwork::HandleSystemMessages(DPMSG_GENERIC *pMsg)
{
	switch( pMsg->dwType )
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			DPMSG_CREATEPLAYERORGROUP* pM = (DPMSG_CREATEPLAYERORGROUP*)pMsg;
			VERIFY		(pM->dwPlayerType==DPPLAYERTYPE_PLAYER);
			SPlayer		P;
			strcpy		(P.name,pM->dpnName.lpszShortNameA);
			P.dwPing	= 15;		// defaults - 15ms
			P.id		= pM->dpId;
			players.push_back(P);
			std::sort(players.begin(),players.end());
			OnPlayerCreate(P.id);
		}
		break;
	case DPSYS_DESTROYPLAYERORGROUP:	// remote player disconnected
		{
			DPMSG_DESTROYPLAYERORGROUP* pM = (DPMSG_DESTROYPLAYERORGROUP*) pMsg;
			VERIFY		(pM->dwPlayerType==DPPLAYERTYPE_PLAYER);
			for (DWORD i=0; i<players.size(); i++) {
				if (players[i].id == pM->dpId) {
					OnPlayerDestroy(pM->dpId);
					players.erase(players.begin()+i);
					break;
				}
			}
		}
		break;
	}
}

const char *CNetwork::GetNameByID(DPID id)
{
	// need to be optimized
	for (vector<SPlayer>::iterator i= players.begin();	i<players.end(); i++)
	{
		if (i->id == id) return i->name;
	}
	return "unknown";
}

BOOL CNetwork::OnMove(float fTimeDelta) {
	fTime-=fTimeDelta;
	if (fTime<0.f) {
		for (DWORD i=0; i<players.size(); i++) {
			DPCAPS C; C.dwSize = sizeof(C);
			pDPlay->GetPlayerCaps(players[i].id,&C,0);
			players[i].dwPing = C.dwLatency;
		}
		fTime+=PING_UPDATE_TIME;
		return true;
	}
	return false;
}
