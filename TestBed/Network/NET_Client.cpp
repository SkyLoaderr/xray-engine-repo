#include "stdafx.h"
#include "net_client.h"
#include "net_messages.h"

ENGINE_API int		psNET_ClientUpdate	= 30;		// FPS
ENGINE_API int		psNET_ClientPending	= 2;
ENGINE_API char		psNET_Name[32]		= "Player";
extern int			psNET_Port;

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID NET_GUID = 
{ 0x218fa8b, 0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };

static HRESULT WINAPI Handler (PVOID pvUserContext, DWORD dwMessageType, PVOID pMessage)
{
	IPureClient* C = (IPureClient*)pvUserContext;
	return C->net_Handler(dwMessageType,pMessage);
}

IPureClient::IPureClient	()
{
}

IPureClient::~IPureClient	()
{
}

BOOL IPureClient::Connect(LPCSTR server_name)
{
	bConnected	= FALSE;

    // Create the IDirectPlay8Client object.
    CHK_DX(CoCreateInstance	(CLSID_DirectPlay8Client, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Client, (LPVOID*) &NET));
	
    // Initialize IDirectPlay8Client object.
    CHK_DX(NET->Initialize	(this, Handler, 0));
	
    // Create our IDirectPlay8Address Device Address, --- Set the SP for our Device Address
	net_Address_device	= NULL;
	DWORD c_port		= psNET_Port+1;
    CHK_DX(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_device )); 
    CHK_DX(net_Address_device->SetSP(&CLSID_DP8SP_TCPIP ));
	CHK_DX(net_Address_device->AddComponent	(DPNA_KEY_PORT, &c_port, sizeof(c_port), DPNA_DATATYPE_DWORD ));
	
    // Create our IDirectPlay8Address Server Address, --- Set the SP for our Server Address
	WCHAR	ServerNameUNICODE	[256];
	CHK_DX(MultiByteToWideChar(CP_ACP, 0, server_name, -1, ServerNameUNICODE, 256 ));

	net_Address_server = NULL;
    CHK_DX(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_server )); 
    CHK_DX(net_Address_server->SetSP		(&CLSID_DP8SP_TCPIP ));
	CHK_DX(net_Address_server->AddComponent	(DPNA_KEY_PORT, &psNET_Port, sizeof(psNET_Port), DPNA_DATATYPE_DWORD ));
    CHK_DX(net_Address_server->AddComponent	(DPNA_KEY_HOSTNAME, ServerNameUNICODE, 2*(wcslen(ServerNameUNICODE) + 1), DPNA_DATATYPE_STRING ));
	
    // Now set up the Application Description
    DPN_APPLICATION_DESC        dpAppDesc;
    ZeroMemory					(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
    dpAppDesc.guidApplication	= NET_GUID;
	
	// Setup client info
	WCHAR	ClientNameUNICODE	[256];
	CHK_DX(MultiByteToWideChar	(CP_ACP, 0, psNET_Name, -1, ClientNameUNICODE, 256 ));

	DPN_PLAYER_INFO				Pinfo;
	ZeroMemory					(&Pinfo,sizeof(Pinfo));
	Pinfo.dwSize				= sizeof(Pinfo);
	Pinfo.dwInfoFlags			= DPNINFO_NAME;
	Pinfo.pwszName				= ClientNameUNICODE;
	CHK_DX(NET->SetClientInfo	(&Pinfo,0,0,DPNSETCLIENTINFO_SYNC));

	if (stricmp(server_name,"localhost")==0)	
	{
		CHK_DX(NET->Connect(
			&dpAppDesc,				// pdnAppDesc
			net_Address_server,		// pHostAddr
			net_Address_device,		// pDeviceInfo
			NULL,					// pdnSecurity
			NULL,					// pdnCredentials
			NULL, 0,				// pvUserConnectData/Size
			NULL,					// pvAsyncContext
			NULL,					// pvAsyncHandle
			DPNCONNECT_SYNC));		// dwFlags

		// Create ONE node
		HOST_NODE	NODE;
		ZeroMemory	(&NODE, sizeof(HOST_NODE));
		
		// Copy the Host Address
		CHK_DX		(net_Address_server->Duplicate(&NODE.pHostAddress ) );
		
		// Retreive session name
		char					desc[256];
		ZeroMemory				(desc,sizeof(desc));
		DPN_APPLICATION_DESC*	dpServerDesc=(DPN_APPLICATION_DESC*)desc;
		DWORD					dpServerDescSize=sizeof(desc);
		dpServerDesc->dwSize	= sizeof(DPN_APPLICATION_DESC);
		CHK_DX					(NET->GetApplicationDesc(dpServerDesc,&dpServerDescSize,0));
		if( dpServerDesc->pwszSessionName)
			CHK_DX(WideCharToMultiByte(CP_ACP,0,dpServerDesc->pwszSessionName,-1,NODE.dpSessionName,sizeof(NODE.dpSessionName),0,0));

		net_Hosts.push_back			(NODE);
	} else {
		// We now have the host address so lets enum
		CHK_DX(NET->EnumHosts(
			&dpAppDesc,				// pApplicationDesc
			net_Address_server,		// pdpaddrHost
			net_Address_device,		// pdpaddrDeviceInfo
			NULL, 0,				// pvUserEnumData, size
			4,						// dwEnumCount
			0,						// dwRetryInterval
			1000,					// dwTimeOut
			NULL,					// pvUserContext
			NULL,					// pAsyncHandle
			DPNENUMHOSTS_SYNC)		// dwFlags
			);
		
		// ****** Connection
		IDirectPlay8Address*        pHostAddress = NULL;
		R_ASSERT					(!net_Hosts.empty());
		
		csEnumeration.Enter			();
		// real connect
		for (DWORD I=0; I<net_Hosts.size(); I++) 
			Msg("* HOST #%d: %s\n",I+1,net_Hosts[I].dpSessionName);
		
		CHK_DX(net_Hosts.front().pHostAddress->Duplicate(&pHostAddress ) );
		CHK_DX(NET->Connect(
			&dpAppDesc,				// pdnAppDesc
			pHostAddress,			// pHostAddr
			net_Address_device,		// pDeviceInfo
			NULL,					// pdnSecurity
			NULL,					// pdnCredentials
			NULL, 0,				// pvUserConnectData/Size
			NULL,					// pvAsyncContext
			NULL,					// pvAsyncHandle
			DPNCONNECT_SYNC));		// dwFlags
		csEnumeration.Leave			();
		_RELEASE					(pHostAddress);
	}

	return	TRUE;
}

void IPureClient::Disconnect()
{
    if( NET )	NET->Close(0);

    // Clean up Host list
	csEnumeration.Enter			();
	for (DWORD i=0; i<net_Hosts.size(); i++) {
		HOST_NODE&	N = net_Hosts[i];
		_RELEASE	(N.pHostAddress);
	}
	net_Hosts.clear				();
	csEnumeration.Leave			();

	// Release interfaces
    _RELEASE	(net_Address_server);
    _RELEASE	(net_Address_device);
    _RELEASE	(NET);
}

HRESULT	IPureClient::net_Handler(DWORD dwMessageType, PVOID pMessage)
{
    HRESULT     hr = S_OK;

    switch (dwMessageType)
    {
        case DPN_MSGID_ENUM_HOSTS_RESPONSE:
        {
            PDPNMSG_ENUM_HOSTS_RESPONSE     pEnumHostsResponseMsg;
            const DPN_APPLICATION_DESC*     pDesc;
            HOST_NODE*                      pHostNode = NULL;
            WCHAR*                          pwszSession = NULL;

            pEnumHostsResponseMsg			= (PDPNMSG_ENUM_HOSTS_RESPONSE) pMessage;
            pDesc							= pEnumHostsResponseMsg->pApplicationDescription;

            // Insert each host response if it isn't already present
			csEnumeration.Enter				();
			BOOL	bHostRegistered			= FALSE;
            for (DWORD I=0; I<net_Hosts.size(); I++)
            {
				HOST_NODE&	N = net_Hosts	[I];
                if	( pDesc->guidInstance == N.dpAppDesc.guidInstance)
                {
                    // This host is already in the list
                    bHostRegistered = TRUE;
                    break;
                }
            }

			if (!bHostRegistered) 
			{
				// This host session is not in the list then so insert it.
				HOST_NODE	NODE;
				ZeroMemory	(&NODE, sizeof(HOST_NODE));
				
				// Copy the Host Address
				CHK_DX		(pEnumHostsResponseMsg->pAddressSender->Duplicate(&NODE.pHostAddress ) );
				CopyMemory	(&NODE.dpAppDesc,pDesc,sizeof(DPN_APPLICATION_DESC));
				
				// Null out all the pointers we aren't copying
				NODE.dpAppDesc.pwszSessionName					= NULL;
				NODE.dpAppDesc.pwszPassword						= NULL;
				NODE.dpAppDesc.pvReservedData					= NULL;
				NODE.dpAppDesc.dwReservedDataSize				= 0;
				NODE.dpAppDesc.pvApplicationReservedData		= NULL;
				NODE.dpAppDesc.dwApplicationReservedDataSize	= 0;
				
				if( pDesc->pwszSessionName)
					CHK_DX(WideCharToMultiByte(CP_ACP,0,pDesc->pwszSessionName,-1,NODE.dpSessionName,sizeof(NODE.dpSessionName),0,0));

				net_Hosts.push_back			(NODE);
			}
			csEnumeration.Leave				();
            break;
        }
		
        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION   pTermSessionMsg;

            pTermSessionMsg = (PDPNMSG_TERMINATE_SESSION) pMessage;

            printf("\nThe Session has been terminated!\n");
            break;
        }
    
        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE	pMsg	= (PDPNMSG_RECEIVE) pMessage;
			void*			m_data	= pMsg->pReceiveData;
			DWORD			m_size	= pMsg->dwReceiveDataSize;
			MSYS_CONFIG*	cfg		= (MSYS_CONFIG*)m_data;
			
			if ((m_size == sizeof(MSYS_CONFIG))&&(cfg->sign1==0x12071980)&&(cfg->sign2==0x26111975))
			{
				// It is really configuration message
				NET_Compressor_FREQ			send,receive;
				send.setFromWORDS			(cfg->send);
				receive.setFromWORDS		(cfg->receive);
				net_Compressor.Initialize	(send,receive);
				bConnected					= TRUE;
				Msg("* New reparse point received\n");
			} else if (bConnected)	{
				// One of the messages - decompress it
				NET_Packet* P	= Queue.Create();
				P->B.count		= net_Compressor.Decompress(P->B.data,LPBYTE(m_data),m_size);
			}
            break;
        }
    }

    return S_OK;
}

void	IPureClient::Send(NET_Packet& P, DWORD dwFlags, DWORD dwTimeout)
{
	// first - compress message and setup buffer
	NET_Buffer	Compressed;
	Compressed.count = net_Compressor.Compress	(Compressed.data,P.B.data,P.B.count);

	// send it
	DPN_BUFFER_DESC	desc;
	desc.dwBufferSize	= Compressed.count;
	desc.pBufferData	= Compressed.data;
	
    DPNHANDLE hAsync;
	
	CHK_DX(NET->Send(
		&desc,1,
		dwTimeout,
		0,&hAsync,
		dwFlags
		));
}

BOOL	IPureClient::HasBandwidth()
{
	DWORD	dwTime			= Device.dwTimeGlobal;
	DWORD	dwInterval		= 1000/psNET_ClientUpdate;

	if ((dwTime-dwTime_LastUpdate)>dwInterval)	
	{
		// check queue for "empty" state
		DWORD	dwPending;
		CHK_DX	(NET->GetSendQueueInfo(&dwPending,0,0));

		if (dwPending > DWORD(psNET_ClientPending))	return FALSE;

		// ok
		dwTime_LastUpdate	= dwTime;
		return TRUE;
	}
	return FALSE;
}
