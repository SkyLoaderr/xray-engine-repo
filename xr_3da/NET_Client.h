#pragma once

#include "net_shared.h"

class ENGINE_API INetQueue
{
	CCriticalSection	cs;
	deque<NET_Packet*>	ready;
	vector<NET_Packet*>	free;
public:
	INetQueue();
	~INetQueue();

	NET_Packet*			Create();
	NET_Packet*			Retreive();
	void				Release();
};

class ENGINE_API IPureClient
{
	friend void __cdecl		sync_thread(void*);
protected:
	struct HOST_NODE
	{
		DPN_APPLICATION_DESC	dpAppDesc;
		IDirectPlay8Address*	pHostAddress;
		char					dpSessionName[128];
	};
protected:
	IDirectPlay8Client*		NET;
	IDirectPlay8Address*	net_Address_device;
	IDirectPlay8Address*	net_Address_server;
	
	CCriticalSection		net_csEnumeration;
	vector<HOST_NODE>		net_Hosts;

	NET_Compressor			net_Compressor;

	BOOL					net_Connected;
	BOOL					net_Syncronised;
	BOOL					net_Disconnected;

	INetQueue				net_Queue;
	IClientStatistic		net_Statistic;
	
	DWORD					net_Time_LastUpdate;
	s32						net_TimeDelta;
	s32						net_TimeDelta_Calculated;
	
	void					Sync_Thread		();
	void					Sync_Average	();
public:
	IPureClient				();
	virtual ~IPureClient	();
	HRESULT					net_Handler				(DWORD dwMessageType, PVOID pMessage);
	
	BOOL					Connect					(LPCSTR server_name);
	void					Disconnect				();

	void					net_Syncronize			();
	BOOL					net_isCompleted_Connect	()	{ return net_Connected;		}
	BOOL					net_isCompleted_Sync	()	{ return net_Syncronised;	}
	BOOL					net_isDisconnected		()	{ return net_Disconnected;	}
	LPCSTR					net_SessionName			()	{ return net_Hosts.front().dpSessionName; }

	// receive
	IC NET_Packet*			net_Retreive			()	{ return net_Queue.Retreive(); }

	// send
	void					Send					(NET_Packet& P, DWORD dwFlags=DPNSEND_GUARANTEED, DWORD dwTimeout=0);
	BOOL					net_HasBandwidth		();
	
	// time management
	IC DWORD	timeServer			()	{ return Device.dwTimeGlobal + net_TimeDelta; }
	IC DWORD	timeServer_Async	()	{ return Device.TimerAsync() + net_TimeDelta; }
	IC DWORD	timeServer_Delta	()	{ return net_TimeDelta; }
	IC void		timeServer_Correct	(DWORD sv_time, DWORD cl_time);
};
