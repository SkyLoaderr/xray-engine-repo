#include "stdafx.h"
#include "net_client.h"
#include "net_messages.h"

const DWORD syncQueueSize	= 256;
const int syncSamples		= 8;
class ENGINE_API syncQueue
{
	DWORD				table[syncQueueSize];
	DWORD				write;
	DWORD				count;
public:
	syncQueue()			{ clear(); }

	IC void			push	(DWORD value)
	{
		table[write++]	= value;
		if (write == syncQueueSize)	write = 0;

		if (count <= syncQueueSize)	count++;
	}
	IC DWORD*		begin	()	{ return table;			}
	IC DWORD*		end		()	{ return table+count;	}
	IC DWORD		size	()	{ return count;			}
	IC void         clear	()	{ write=0; count=0;		}
} net_DeltaArray;

//-------
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
	R_ASSERT(server_name);

	net_Connected	= FALSE;
	net_Syncronised	= FALSE;

    // Create the IDirectPlay8Client object.
    R_CHK(CoCreateInstance	(CLSID_DirectPlay8Client, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Client, (LPVOID*) &NET));
	
    // Initialize IDirectPlay8Client object.
    R_CHK(NET->Initialize	(this, Handler, 0));
	
    // Create our IDirectPlay8Address Device Address, --- Set the SP for our Device Address
	net_Address_device	= NULL;
	DWORD c_port		= psNET_Port+1;
    R_CHK(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_device )); 
    R_CHK(net_Address_device->SetSP(&CLSID_DP8SP_TCPIP ));
	R_CHK(net_Address_device->AddComponent	(DPNA_KEY_PORT, &c_port, sizeof(c_port), DPNA_DATATYPE_DWORD ));
	
    // Create our IDirectPlay8Address Server Address, --- Set the SP for our Server Address
	WCHAR	ServerNameUNICODE	[256];
	R_CHK(MultiByteToWideChar(CP_ACP, 0, server_name, -1, ServerNameUNICODE, 256 ));

	net_Address_server = NULL;
    R_CHK(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_server )); 
    R_CHK(net_Address_server->SetSP		(&CLSID_DP8SP_TCPIP ));
	R_CHK(net_Address_server->AddComponent	(DPNA_KEY_PORT, &psNET_Port, sizeof(psNET_Port), DPNA_DATATYPE_DWORD ));
    R_CHK(net_Address_server->AddComponent	(DPNA_KEY_HOSTNAME, ServerNameUNICODE, 2*(wcslen(ServerNameUNICODE) + 1), DPNA_DATATYPE_STRING ));
	
    // Now set up the Application Description
    DPN_APPLICATION_DESC        dpAppDesc;
    ZeroMemory					(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
    dpAppDesc.guidApplication	= NET_GUID;
	
	// Setup client info
	WCHAR	ClientNameUNICODE	[256];
	R_CHK(MultiByteToWideChar	(CP_ACP, 0, psNET_Name, -1, ClientNameUNICODE, 256 ));

	DPN_PLAYER_INFO				Pinfo;
	ZeroMemory					(&Pinfo,sizeof(Pinfo));
	Pinfo.dwSize				= sizeof(Pinfo);
	Pinfo.dwInfoFlags			= DPNINFO_NAME;
	Pinfo.pwszName				= ClientNameUNICODE;
	R_CHK(NET->SetClientInfo	(&Pinfo,0,0,DPNSETCLIENTINFO_SYNC));

	if (stricmp(server_name,"localhost")==0)	
	{
		R_CHK(NET->Connect(
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
		R_CHK		(net_Address_server->Duplicate(&NODE.pHostAddress ) );
		
		// Retreive session name
		char					desc[256];
		ZeroMemory				(desc,sizeof(desc));
		DPN_APPLICATION_DESC*	dpServerDesc=(DPN_APPLICATION_DESC*)desc;
		DWORD					dpServerDescSize=sizeof(desc);
		dpServerDesc->dwSize	= sizeof(DPN_APPLICATION_DESC);
		R_CHK					(NET->GetApplicationDesc(dpServerDesc,&dpServerDescSize,0));
		if( dpServerDesc->pwszSessionName)
			R_CHK(WideCharToMultiByte(CP_ACP,0,dpServerDesc->pwszSessionName,-1,NODE.dpSessionName,sizeof(NODE.dpSessionName),0,0));

		net_Hosts.push_back			(NODE);
	} else {
		// We now have the host address so lets enum
		R_CHK(NET->EnumHosts(
			&dpAppDesc,				// pApplicationDesc
			net_Address_server,		// pdpaddrHost
			net_Address_device,		// pdpaddrDeviceInfo
			NULL, 0,				// pvUserEnumData, size
			2,						// dwEnumCount
			0,						// dwRetryInterval
			1000,					// dwTimeOut
			NULL,					// pvUserContext
			NULL,					// pAsyncHandle
			DPNENUMHOSTS_SYNC)		// dwFlags
			);
		
		// ****** Connection
		IDirectPlay8Address*        pHostAddress = NULL;
		R_ASSERT					(!net_Hosts.empty());
		
		net_csEnumeration.Enter		();
		// real connect
		for (DWORD I=0; I<net_Hosts.size(); I++) 
			Msg("* HOST #%d: %s\n",I+1,net_Hosts[I].dpSessionName);
		
		R_CHK(net_Hosts.front().pHostAddress->Duplicate(&pHostAddress ) );
		R_CHK(NET->Connect(
			&dpAppDesc,				// pdnAppDesc
			pHostAddress,			// pHostAddr
			net_Address_device,		// pDeviceInfo
			NULL,					// pdnSecurity
			NULL,					// pdnCredentials
			NULL, 0,				// pvUserConnectData/Size
			NULL,					// pvAsyncContext
			NULL,					// pvAsyncHandle
			DPNCONNECT_SYNC));		// dwFlags
		net_csEnumeration.Leave		();
		_RELEASE					(pHostAddress);
	}

	// Caps
	GUID			sp_guid;
	DPN_SP_CAPS		sp_caps;

	net_Address_device->GetSP(&sp_guid);
	ZeroMemory		(&sp_caps,sizeof(sp_caps));
	sp_caps.dwSize	= sizeof(sp_caps);
	R_CHK			(NET->GetSPCaps(&sp_guid,&sp_caps,0));
	sp_caps.dwSystemBufferSize	= 0;
	R_CHK			(NET->SetSPCaps(&sp_guid,&sp_caps,0));
	R_CHK			(NET->GetSPCaps(&sp_guid,&sp_caps,0));

	// Sync
	Syncronize	();
	return	TRUE;
}

void IPureClient::Disconnect()
{
    if( NET )	NET->Close(0);

    // Clean up Host list
	net_csEnumeration.Enter			();
	for (DWORD i=0; i<net_Hosts.size(); i++) {
		HOST_NODE&	N = net_Hosts[i];
		_RELEASE	(N.pHostAddress);
	}
	net_Hosts.clear					();
	net_csEnumeration.Leave			();

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
			net_csEnumeration.Enter			();
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
				R_CHK		(pEnumHostsResponseMsg->pAddressSender->Duplicate(&NODE.pHostAddress ) );
				CopyMemory	(&NODE.dpAppDesc,pDesc,sizeof(DPN_APPLICATION_DESC));
				
				// Null out all the pointers we aren't copying
				NODE.dpAppDesc.pwszSessionName					= NULL;
				NODE.dpAppDesc.pwszPassword						= NULL;
				NODE.dpAppDesc.pvReservedData					= NULL;
				NODE.dpAppDesc.dwReservedDataSize				= 0;
				NODE.dpAppDesc.pvApplicationReservedData		= NULL;
				NODE.dpAppDesc.dwApplicationReservedDataSize	= 0;
				
				if( pDesc->pwszSessionName)
					R_CHK(WideCharToMultiByte(CP_ACP,0,pDesc->pwszSessionName,-1,NODE.dpSessionName,sizeof(NODE.dpSessionName),0,0));

				net_Hosts.push_back			(NODE);
			}
			net_csEnumeration.Leave			();
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
			DWORD			time	= Device.TimerAsync();
            PDPNMSG_RECEIVE	pMsg	= (PDPNMSG_RECEIVE) pMessage;
			void*			m_data	= pMsg->pReceiveData;
			DWORD			m_size	= pMsg->dwReceiveDataSize;
			MSYS_CONFIG*	cfg		= (MSYS_CONFIG*)m_data;
			
			if ((m_size>2*sizeof(DWORD)) && (cfg->sign1==0x12071980) && (cfg->sign2==0x26111975))
			{
				// Internal system message
				if ((m_size == sizeof(MSYS_PING)))
				{
					// It is reverted(server) ping
					DWORD		time	= Device.TimerAsync();
					MSYS_PING*	msg		= (MSYS_PING*)m_data;
					DWORD		ping	= time - (msg->dwTime_ClientSend);
					DWORD		delta	= msg->dwTime_Server + ping/2 - time;
					net_DeltaArray.push	(delta);
					Sync_Average		();
//					Msg					("* ping: %d",ping);
				} else if ((m_size == sizeof(MSYS_CONFIG)))
				{
					// It is configuration message
					NET_Compressor_FREQ			send,receive;
					send.setFromWORDS			(cfg->send);
					receive.setFromWORDS		(cfg->receive);
					net_Compressor.Initialize	(send,receive);
					net_Connected				= TRUE;
					Msg("* New reparse point received");
				} else {
					Msg("! Unknown system message");
				}
			} else if (net_Connected)	{
				// One of the messages - decompress it
				NET_Packet* P	= net_Queue.Create();
				P->B.count		= net_Compressor.Decompress(P->B.data,LPBYTE(m_data),m_size);
				P->timeReceive	= time;
			}
            break;
        }
    }

    return S_OK;
}

void	IPureClient::timeServer_Correct(DWORD sv_time, DWORD cl_time)
{
	DWORD		ping	= net_Statistic.getPing();
	DWORD		delta	= sv_time + ping/2 - cl_time;
	net_DeltaArray.push	(delta);
	Sync_Average		();
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
	
	// verify
	VERIFY		(desc.dwBufferSize);
	VERIFY		(desc.pBufferData);

    DPNHANDLE	hAsync=0;
	VERIFY		(NET);
	R_CHK(NET->Send(
		&desc,1,
		dwTimeout,
		0,&hAsync,
		dwFlags
		));
}

BOOL	IPureClient::HasBandwidth	()
{
	DWORD	dwTime			= Device.dwTimeGlobal;
	DWORD	dwInterval		= 1000/psNET_ClientUpdate;

	if ((dwTime-net_Time_LastUpdate)>dwInterval)	
	{
		R_ASSERT(NET);
		
		// check queue for "empty" state
		DWORD				dwPending=0;
		R_CHK				(NET->GetSendQueueInfo(&dwPending,0,0));
		if (dwPending > DWORD(psNET_ClientPending))	return FALSE;

		// Query network statistic for this client
		DPN_CONNECTION_INFO	CI;
		ZeroMemory			(&CI,sizeof(CI));
		CI.dwSize			= sizeof(CI);
		CHK_DX				(NET->GetConnectionInfo(&CI,0));
		net_Statistic.Update(CI);

		// ok
		net_Time_LastUpdate	= dwTime;
		return TRUE;
	}
	return FALSE;
}

void	IPureClient::Sync_Thread	()
{
	MSYS_PING			clPing;

	//***** Ping server
	net_DeltaArray.clear();
	R_ASSERT			(NET);
	for (; NET; )
	{
		// Waiting for queue empty state
		if (net_Syncronised)	Sleep(3000);
		else {
			DWORD				dwPending=0;
			do {
				Sleep			(1);
				R_CHK			(NET->GetSendQueueInfo(&dwPending,0,0));
			} while (dwPending);
		}

		// Construct message
		clPing.sign1				= 0x12071980;
		clPing.sign2				= 0x26111975;
		clPing.dwTime_ClientSend	= Device.TimerAsync();

		// Send it
		DPN_BUFFER_DESC		desc;
		DPNHANDLE			hAsync=0;
		desc.dwBufferSize	= sizeof(clPing);
		desc.pBufferData	= LPBYTE(&clPing);
		if (0==NET)			break;
		if (FAILED(NET->Send(&desc,1,0,0,&hAsync,net_flags(FALSE,FALSE,TRUE))))	{
			Msg("* CLIENT: Thread: EXIT. (failed to send - disconnected?)");
			break;
		}
		
		// Waiting for reply-packet to arrive
		if (!net_Syncronised)	{
			DWORD	old_size	= net_DeltaArray.size();
			DWORD	timeBegin	= Device.TimerAsync();
			while ((net_DeltaArray.size()==old_size)&&(Device.TimerAsync()-timeBegin<5000))		Sleep(1);
			
			if (net_DeltaArray.size()>=syncSamples)	{
				net_Syncronised	= TRUE;
				net_TimeDelta	= net_TimeDelta_Calculated;
			}
		}
	}
}

void	IPureClient::Sync_Average	()
{
	//***** Analyze results
	s64		summary_delta	= 0;
	s32		size			= net_DeltaArray.size();
	DWORD*	I				= net_DeltaArray.begin();
	DWORD*  E				= I+size;
	for (; I!=E; I++)		summary_delta	+= *((int*)I);

	s64 frac				=	s64(summary_delta) % s64(size);
	if (frac<0)				frac=-frac;
	summary_delta			/=	s64(size);
	if (frac>s64(size/2))	summary_delta += (summary_delta<0)?-1:1;
	net_TimeDelta_Calculated=	s32(summary_delta);
	net_TimeDelta			=	(net_TimeDelta*5+net_TimeDelta_Calculated)/6;
//	Msg("* CLIENT: d(%d), dc(%d), s(%d)",net_TimeDelta,net_TimeDelta_Calculated,size);
}

void __cdecl		sync_thread(void* P)
{
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	IPureClient*	C = (IPureClient*)P;
	C->Sync_Thread	();
}
void	IPureClient::Syncronize	()
{
	net_Syncronised	= FALSE;
	net_TimeDelta	= 0;
	_beginthread	(sync_thread,0,this);
}
