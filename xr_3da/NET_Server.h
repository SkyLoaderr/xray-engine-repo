#pragma once

#include "net_shared.h"

class ENGINE_API IClient
{
public:
	IClientStatistic	stats;

	DPNID				ID;
	DWORD				dwTime_LastUpdate;
	char				Name	[64];
	DWORD				Flags;	// local/host/normal

	IClient()	{
		dwTime_LastUpdate	= 0;
	}
};

class ENGINE_API IServerStatistic
{
public:
	DWORD					bytes_out,bytes_out_real;
	DWORD					bytes_in, bytes_in_real;

	void					clear()
	{
		bytes_out	= bytes_out_real=0;
		bytes_in	= bytes_in_real	=0;
	}
};

class ENGINE_API IPureServer
{
protected:
	IDirectPlay8Server*		NET;
	IDirectPlay8Address*	net_Address_device;
	
	NET_Compressor			net_Compressor;
	NET_Compressor_FREQ		traffic_out;
	NET_Compressor_FREQ		traffic_in;

	CCriticalSection		csPlayers;
	svector<IClient*,64>	net_Players;

	// Compressor configuration
	MSYS_CONFIG				msgConfig;
	void					config_Load			();
	void					config_Save			();

	// Utilities
	void					pCompress			(NET_Packet& D, NET_Packet& S);
	void					pDecompress			(NET_Packet& D, void* data, DWORD size);
	
	// Statistic
	IServerStatistic		stats;
public:
	IPureServer				();
	virtual ~IPureServer	();
	HRESULT					net_Handler			(DWORD dwMessageType, PVOID pMessage);
	
	BOOL					Connect				(LPCSTR session_name);
	void					Disconnect			();
	void					Reparse				();									// Reparse configuration and freq data

	// send
	void					SendTo_LL			(DPNID ID, void* data, DWORD size, DWORD dwFlags=DPNSEND_GUARANTEED, DWORD dwTimeout=0);
	void					SendTo				(DPNID ID, NET_Packet& P, DWORD dwFlags=DPNSEND_GUARANTEED, DWORD dwTimeout=0);
	void					SendBroadcast_LL	(DPNID exclude, void* data, DWORD size, DWORD dwFlags=DPNSEND_GUARANTEED);
	void					SendBroadcast		(DPNID exclude, NET_Packet& P, DWORD dwFlags=DPNSEND_GUARANTEED);

	// statistic
	const IServerStatistic*	GetStatistic		() { return &stats; }

	// extended functionality
	virtual DWORD			OnMessage			(NET_Packet& P, DPNID sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual void			OnCL_Connected		(IClient* CL);
	virtual void			OnCL_Disconnected	(IClient* CL);

	virtual IClient*		client_Create		()	= 0;						// create client info
	virtual void			client_Replicate	()	= 0;						// replicate current state to client
	virtual void			client_Destroy		(IClient* C) = 0;				// destroy client info

	IC DWORD				client_Count		()	{ return net_Players.size(); }
	IC IClient*				client_Get			(DWORD num)	{ return net_Players[num]; }
	
	BOOL					HasBandwidth		(IClient* C);
};
