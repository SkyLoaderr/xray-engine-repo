#include "stdafx.h"
#include "net_server.h"

LPCSTR nameTraffic	= "traffic.net";

ENGINE_API int		psNET_ServerUpdate	= 30;		// FPS
ENGINE_API int		psNET_ServerPending	= 2;
ENGINE_API int		psNET_Port			= 5445;

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID NET_GUID = 
{ 0x218fa8b, 0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };

static HRESULT WINAPI Handler (PVOID pvUserContext, DWORD dwMessageType, PVOID pMessage)
{
	IPureServer* C = (IPureServer*)pvUserContext;
	return C->net_Handler	(dwMessageType,pMessage);
}

IPureServer::IPureServer	()
{
	stats.clear	();
}

IPureServer::~IPureServer	()
{
}

void IPureServer::pCompress	(NET_Packet& D, NET_Packet& S)
{
	// Update freq
	BYTE*	it		= S.B.data;
	BYTE*	end		= it+S.B.count;
	for (; it!=end; )	traffic_out[*it++]++;

	// Compress data
	D.B.count		= net_Compressor.Compress(D.B.data,S.B.data,S.B.count);

	// Update statistic
	stats.bytes_out			+= S.B.count;
	stats.bytes_out_real	+= D.B.count;
}
void IPureServer::pDecompress(NET_Packet& D, void* m_data, DWORD m_size)
{
	// Decompress data
	D.B.count		=  net_Compressor.Decompress(D.B.data,LPBYTE(m_data),m_size);

	// Update statistic
	stats.bytes_in		+= D.B.count;
	stats.bytes_in_real	+= m_size;

	// Update freq
	BYTE*	it		= D.B.data;
	BYTE*	end		= it+D.B.count;
	for (; it!=end; )	traffic_in[*it++]++;
}

void IPureServer::config_Load()
{
	//************ Build sys_config
	
	// traffic in
	int hin = _open(nameTraffic,O_BINARY|O_RDONLY);
	if (hin>0) {
		_read	(hin,&traffic_in,sizeof(traffic_in));
		_read	(hin,&traffic_out,sizeof(traffic_out));
		_close	(hin);
		traffic_in.Normalize	();
		traffic_out.Normalize	();
	} else {
		traffic_in.setIdentity	();
		traffic_out.setIdentity	();
	}

	// build message
	msgConfig.sign1				= 0x12071980;
	msgConfig.sign2				= 0x26111975;
	DWORD	I;
	for (I=0; I<256; I++)		msgConfig.send[I]		= WORD(traffic_in[I]);
	for (I=0; I<256; I++)		msgConfig.receive[I]	= WORD(traffic_out[I]);

	// initialize compressors
	net_Compressor.Initialize	(traffic_out,traffic_in);
}

void IPureServer::config_Save	()
{
	// traffic in
	int hin = _open(nameTraffic, O_BINARY|O_WRONLY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
	R_ASSERT(hin>0);
	_write(hin,&traffic_in,sizeof(traffic_in));
	_write(hin,&traffic_out,sizeof(traffic_out));
	_close(hin);
}

void IPureServer::Reparse	()
{
	config_Save		();
	config_Load		();
	SendBroadcast_LL(0,&msgConfig,sizeof(msgConfig));
}

BOOL IPureServer::Connect(LPCSTR session_name)
{
    // Create the IDirectPlay8Client object.
    CHK_DX(CoCreateInstance	(CLSID_DirectPlay8Server, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Server, (LPVOID*) &NET));
	
    // Initialize IDirectPlay8Client object.
    CHK_DX(NET->Initialize	(this, Handler, 0));
	
    // Create our IDirectPlay8Address Device Address, --- Set the SP for our Device Address
	net_Address_device = NULL;
    CHK_DX(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_device )); 
    CHK_DX(net_Address_device->SetSP		(&CLSID_DP8SP_TCPIP ));
	CHK_DX(net_Address_device->AddComponent	(DPNA_KEY_PORT, &psNET_Port, sizeof(psNET_Port), DPNA_DATATYPE_DWORD ));
	
	// Set server-player info
    DPN_APPLICATION_DESC		dpAppDesc;
    DPN_PLAYER_INFO				dpPlayerInfo;
    WCHAR						wszName		[] = L"XRay Server";
	
    ZeroMemory					(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize			= sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags	= DPNINFO_NAME;
    dpPlayerInfo.pwszName		= wszName;
    dpPlayerInfo.pvData			= NULL;
    dpPlayerInfo.dwDataSize		= NULL;
    dpPlayerInfo.dwPlayerFlags	= 0;
	
	CHK_DX(NET->SetServerInfo( &dpPlayerInfo, NULL, NULL, DPNSETSERVERINFO_SYNC ) );
	
    // Set server/session description
	WCHAR	SessionNameUNICODE[1024];
	CHK_DX(MultiByteToWideChar(CP_ACP, 0, session_name, -1, SessionNameUNICODE, 1024 ));
	
    // Now set up the Application Description
    ZeroMemory					(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
    dpAppDesc.dwFlags			= DPNSESSION_CLIENT_SERVER | DPNSESSION_NODPNSVR;
    dpAppDesc.guidApplication	= NET_GUID;
    dpAppDesc.pwszSessionName	= SessionNameUNICODE;
	
    // We are now ready to host the app
	CHK_DX(NET->Host			(&dpAppDesc,			// AppDesc
		&net_Address_device, 1, // Device Address
		NULL, NULL,             // Reserved
		NULL,                   // Player Context
		0 ) );					// dwFlags
	
	config_Load		();
	return	TRUE;
}

void IPureServer::Disconnect()
{
	config_Save		();

    if( NET )	NET->Close(0);
	
	// Release interfaces
    _RELEASE	(net_Address_device);
    _RELEASE	(NET);
}

HRESULT	IPureServer::net_Handler(DWORD dwMessageType, PVOID pMessage)
{
    HRESULT     hr = S_OK;
	
    switch (dwMessageType)
    {
	case DPN_MSGID_CREATE_PLAYER :
        {
			PDPNMSG_CREATE_PLAYER	msg = PDPNMSG_CREATE_PLAYER(pMessage);
			const	DWORD			max_size = 1024;
			char	bufferData		[max_size];
            DWORD	bufferSize		= max_size;
			ZeroMemory				(bufferData,bufferSize);

			// retreive info
			DPN_PLAYER_INFO*		Pinfo = (DPN_PLAYER_INFO*) bufferData;
			Pinfo->dwSize			= sizeof(DPN_PLAYER_INFO);
			HRESULT _hr				= NET->GetClientInfo(msg->dpnidPlayer,Pinfo,&bufferSize,0);
			if (_hr==DPNERR_INVALIDPLAYER)	break;	// server player
			CHK_DX					(_hr);
			
			// setup structure
			IClient*				P = client_Create();
			P->ID					= msg->dpnidPlayer;
			P->Flags				= Pinfo->dwPlayerFlags;
			CHK_DX(WideCharToMultiByte(CP_ACP,0,Pinfo->pwszName,-1,P->Name,sizeof(P->Name),0,0));

			// register player
			csPlayers.Enter			();
			net_Players.push_back	(P);
			csPlayers.Leave			();
			
			// config client
			SendTo_LL				(P->ID,&msgConfig,sizeof(msgConfig));

			// gen message
			OnCL_Connected			(P);
        }
		break;
	case DPN_MSGID_DESTROY_PLAYER:
		{
			PDPNMSG_DESTROY_PLAYER	msg = PDPNMSG_DESTROY_PLAYER(pMessage);

			csPlayers.Enter			();
			for (DWORD I=0; I<net_Players.size(); I++)
				if (net_Players[I]->ID==msg->dpnidPlayer)
				{
					// gen message
					OnCL_Disconnected	(net_Players[I]);

					// real destroy
					client_Destroy		(net_Players[I]);
					net_Players.erase	(net_Players.begin()+I);
					break;
				}
			csPlayers.Leave			();
		}
		break;
	case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE	msg = PDPNMSG_RECEIVE(pMessage);
			void*	m_data		= msg->pReceiveData;
			DWORD	m_size		= msg->dwReceiveDataSize;
			DPNID   m_sender	= msg->dpnidSender;

			// Decompress message
			NET_Packet P;
			pDecompress		(P,m_data,m_size);
			DWORD	result	= OnMessage(P,m_sender);
			if (result)		SendBroadcast(m_sender,P,result);
        }
        break;
    }
	
    return S_OK;
}

void	IPureServer::SendTo_LL(DPNID ID, void* data, DWORD size, DWORD dwFlags, DWORD dwTimeout)
{
	// send it
	DPN_BUFFER_DESC	desc;
	desc.dwBufferSize	= size;
	desc.pBufferData	= LPBYTE(data);
	
    DPNHANDLE hAsync;

	CHK_DX(NET->SendTo(
		ID,
		&desc,1,
		dwTimeout,
		0,&hAsync,
		dwFlags
		));
}

void	IPureServer::SendTo		(DPNID ID, NET_Packet& P, DWORD dwFlags, DWORD dwTimeout)
{
	// first - compress message and setup buffer
	NET_Packet	Compressed;
	pCompress	(Compressed,P);
	
	SendTo_LL(ID,Compressed.B.data,Compressed.B.count,dwFlags,dwTimeout);
}

void	IPureServer::SendBroadcast_LL(DPNID exclude, void* data, DWORD size, DWORD dwFlags)
{
	csPlayers.Enter	();
	for (DWORD I=0; I<net_Players.size(); I++)
	{
		IClient* PLAYER		= net_Players[I];
		if (PLAYER->ID==exclude)		continue;
		SendTo_LL	(PLAYER->ID, data, size, dwFlags);
	}
	csPlayers.Leave	();
}

void	IPureServer::SendBroadcast(DPNID exclude, NET_Packet& P, DWORD dwFlags)
{
	// Perform broadcasting
	NET_Packet			BCast;
	pCompress			(BCast,P);
	SendBroadcast_LL	(exclude, BCast.B.data, BCast.B.count, dwFlags);
}

DWORD	IPureServer::OnMessage	(NET_Packet& P, DPNID sender)	// Non-Zero means broadcasting with "flags" as returned
{
	/*
	u16 m_type;
	P.r_begin	(m_type);
	switch (m_type)
	{
	case M_CHAT:
		{
			char	buffer[256];
			P.r_string(buffer);
			printf	("RECEIVE: %s\n",buffer);
		}
		break;
	}
	*/
	return DPNSEND_GUARANTEED;
}

void IPureServer::OnCL_Connected		(IClient* CL)
{
	Msg("* Player '%s' connected.\n",	CL->Name);
}
void IPureServer::OnCL_Disconnected		(IClient* CL)
{
	Msg("* Player '%s' disconnected.\n",CL->Name);
}
BOOL IPureServer::HasBandwidth			(IClient* C)
{
	DWORD	dwTime			= Device.dwTimeGlobal;
	DWORD	dwInterval		= 1000/psNET_ServerUpdate;

	if ((dwTime-C->dwTime_LastUpdate)>dwInterval)	
	{
		// check queue for "empty" state
		DWORD	dwPending;
		CHK_DX	(NET->GetSendQueueInfo(C->ID,&dwPending,0,0));

		if (dwPending > DWORD(psNET_ServerPending))	return FALSE;

		// ok
		C->dwTime_LastUpdate	= dwTime;
		return TRUE;
	}
	return FALSE;
}
