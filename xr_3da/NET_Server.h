#pragma once

#include "net_shared.h"


class XRNETSERVER_API IClient
{
	struct Flags
	{
		u32		bLocal		: 1;
		u32		bConnected	: 1;
	};
public:
	IClientStatistic	stats;

	ClientID			ID;
	shared_str				Name;
	Flags				flags;	// local/host/normal
	u32					dwTime_LastUpdate;

	IClient(CTimer* timer):stats(timer)	{
		dwTime_LastUpdate	= 0;
	}
};

class XRNETSERVER_API IServerStatistic
{
public:
	u32					bytes_out,bytes_out_real;
	u32					bytes_in, bytes_in_real;

	void					clear()
	{
		bytes_out	= bytes_out_real=0;
		bytes_in	= bytes_in_real	=0;

		dwBytesSended = 0;
		dwSendTime = 0;
		dwBytesPerSec = 0;
	}

	u32					dwBytesSended;
	u32					dwSendTime;
	u32					dwBytesPerSec;
};

class XRNETSERVER_API IPureServer
{
protected:
	IDirectPlay8Server*		NET;
	IDirectPlay8Address*	net_Address_device;
	
	NET_Compressor			net_Compressor;
	NET_Compressor_FREQ		traffic_out;
	NET_Compressor_FREQ		traffic_in;

	xrCriticalSection		csPlayers;
	xr_vector<IClient*>		net_Players;
	IClient*				SV_Client;

	int						psNET_Port;	

	// Compressor configuration
	MSYS_CONFIG				msgConfig;
	void					config_Load			();
	void					config_Save			();

	// 
	xrCriticalSection		csMessage;

	// Utilities
	void					pCompress			(NET_Packet& D, NET_Packet& S);
	void					pDecompress			(NET_Packet& D, void* data, u32 size);
	void					client_link_aborted	(ClientID/*DPNID*/ ID	);
	void					client_link_aborted	(IClient* C	);
	
	// Statistic
	IServerStatistic		stats;
	CTimer*					device_timer;

	virtual void			new_client			(ClientID clientID, LPCSTR name, bool bLocal)   =0;

public:
	IPureServer				(CTimer* timer);
	virtual ~IPureServer	();
	HRESULT					net_Handler			(u32 dwMessageType, PVOID pMessage);
	
	virtual BOOL			Connect				(LPCSTR session_name);
	virtual void			Disconnect			();
	virtual void			Reparse				();									// Reparse configuration and freq data

	// send
	virtual void			SendTo_LL			(ClientID/*DPNID*/ ID, void* data, u32 size, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	void					SendTo				(ClientID/*DPNID*/ ID, NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	void					SendBroadcast_LL	(ClientID/*DPNID*/ exclude, void* data, u32 size, u32 dwFlags=DPNSEND_GUARANTEED);
	void					SendBroadcast		(ClientID/*DPNID*/ exclude, NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED);

	// statistic
	const IServerStatistic*	GetStatistic		() { return &stats; }
	void					ClearStatistic		();

	// extended functionality
	virtual u32				OnMessage			(NET_Packet& P, ClientID/*DPNID*/ sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual void			OnCL_Connected		(IClient* C);
	virtual void			OnCL_Disconnected	(IClient* C);
	virtual bool			OnCL_QueryHost		()		{ return true; };

	virtual IClient*		client_Create		()				= 0;			// create client info
	virtual void			client_Replicate	()				= 0;			// replicate current state to client
	virtual void			client_Destroy		(IClient* C)	= 0;			// destroy client info

	IC u32					client_Count		()			{ return net_Players.size(); }
	IC IClient*				client_Get			(u32 num)	{ return net_Players[num]; }
	
	BOOL					HasBandwidth		(IClient* C);

	IC int					GetPort				()			{ return psNET_Port; };
	virtual bool			DisconnectClient	(IClient* C);
};
