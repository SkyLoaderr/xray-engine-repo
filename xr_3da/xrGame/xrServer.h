// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
#define AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_
#pragma once

#include "xrServer_Entities.h"

const u32	NET_Latency		= 100;		// time in (ms)

// t-defs
typedef std::map<u16,xrServerEntity*>	xrS_entities;

class xrClientData	: public IClient
{
public:
	xrServerEntity*			owner;
	BOOL					net_Ready;

	s16						g_kills;
	s16						g_deaths;
	s32						g_money;
	
	xrClientData	()
	{
		owner		= NULL;
		net_Ready	= FALSE;
		g_kills		= 0;
		g_deaths	= 0;
		g_money		= 0;
	}
};


// main
struct	svs_respawn
{
	u32		timestamp;
	u16		phantom;
};
IC bool operator < (const svs_respawn& A, const svs_respawn& B)	{ return A.timestamp<B.timestamp; }

class xrServer	: public IPureServer  
{
private:
	xrS_entities			entities;
	deque<u16>				id_free;
	multiset<svs_respawn>	q_respawn;

	BOOL					PerformRP				(xrServerEntity* E);
	void					PerformMigration		(xrServerEntity* E, xrClientData* from, xrClientData* to);
	u16						PerformIDgen			(u16 desired);
	void					Perform_connect_spawn	(xrServerEntity* E, xrClientData* to, NET_Packet& P);

	void					Process_spawn			(NET_Packet& P, DPNID sender);
	void					Process_update			(NET_Packet& P, DPNID sender);
	void					Process_event			(NET_Packet& P, DPNID sender);
	void					Process_event_ownership	(NET_Packet& P, DPNID sender, u32 time, u16 ID);

	xrClientData*			SelectBestClientToMigrateTo		(xrServerEntity* E);

public:
	// constr / destr
	xrServer				();
	virtual ~xrServer		();

	// extended functionality
	virtual DWORD			OnMessage			(NET_Packet& P, DPNID sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual void			OnCL_Connected		(IClient* CL);
	virtual void			OnCL_Disconnected	(IClient* CL);

	virtual IClient*		client_Create		();								// create client info
	virtual void			client_Replicate	();								// replicate current state to client
	virtual void			client_Destroy		(IClient* C);					// destroy client info

	// utilities
	xrServerEntity*			entity_Create		(LPCSTR name);
	void					entity_Destroy		(xrServerEntity* P);

	IC xrClientData*		ID_to_client		(DPNID ID)
	{
		if (0==ID)			return 0;
		csPlayers.Enter		();
		for (DWORD client=0; client<net_Players.size(); client++)
		{
			if (net_Players[client]->ID==ID)	{
				csPlayers.Leave		();
				return (xrClientData*)net_Players[client];
			}
		}
		csPlayers.Leave		();
		return 0;
	}

	IC xrServerEntity*		ID_to_entity		(u16 ID)
	{
		if (0xffff==ID)				return 0;
		xrS_entities::iterator	I	= entities.find	(ID);
		if (I!=entities.end())		return I->second;
		else						return 0;
	}

	// main
	void					Update				();
	void					SLS_Default			();
	void					SLS_Save			(CFS_Base& FS);
	void					SLS_Load			(CStream&  FS);
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
