#pragma once

#include "net_shared.h"

const DWORD NET_QueueSize	= 128;
class ENGINE_API INetQueue
{
	CCriticalSection	cs;
	NET_Packet			table[NET_QueueSize];
	DWORD				id_read;
	DWORD				id_write;
public:
	INetQueue()		
	{
		id_read		= 0;
		id_write	= 0;
	}

	IC NET_Packet*	Create()
	{
		cs.Enter	();
		NET_Packet*	P = &(table[id_write++]);
		if (id_write == NET_QueueSize)	id_write = 0;
		R_ASSERT(id_write != id_read);
		cs.Leave	();
		return P;
	}
	IC NET_Packet*	Retreive()
	{
		if (id_read==id_write)	return NULL;
		cs.Enter	();
		NET_Packet*	P = &(table[id_read++]);
		if (id_read == NET_QueueSize)	id_read	= 0;
		cs.Leave	();
		return P;
	}
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
	HRESULT					net_Handler		(DWORD dwMessageType, PVOID pMessage);
	
	BOOL	Connect			(LPCSTR server_name);
	void	Syncronize		();
	void	Disconnect		();

	BOOL	isCompleted_Connect	()	{ return net_Connected;		}
	BOOL	isCompleted_Sync	()	{ return net_Syncronised;	}
	
	LPCSTR	SessionName		()	{ return net_Hosts.front().dpSessionName; }

	// receive
	IC NET_Packet*	Retreive() { return net_Queue.Retreive(); }

	// send
	void	Send			(NET_Packet& P, DWORD dwFlags=DPNSEND_GUARANTEED, DWORD dwTimeout=0);
	BOOL	HasBandwidth	();
	
	// time management
	IC DWORD	timeServer			() { return Device.dwTimeGlobal + net_TimeDelta; }
	IC DWORD	timeServer_Async	() { return Device.TimerAsync() + net_TimeDelta; }
	IC DWORD	timeServer_Delta	() { return net_TimeDelta; }
	IC void		timeServer_Correct	(DWORD sv_time, DWORD cl_time);
};
