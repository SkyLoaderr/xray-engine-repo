// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
#define AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_
#pragma once

#include "xrServer_Entities.h"

const u32	NET_Latency		= 150;		// time in (ms)

// t-defs
typedef std::map<u16,xrServerEntity*>	xrS_entities;

class xrClientData	: public IClient
{
public:
	xrServerEntity*			owner;
	BOOL					net_Ready;
	
	xrClientData	()
	{
		owner		= NULL;
		net_Ready	= FALSE;
	}
};


// main
class xrServer	: public IPureServer  
{
private:
	xrS_entities			entities;
	vector<bool>			ids_used;
	BOOL					PerformRP			(xrServerEntity* E);
	void					PerformMigration	(xrServerEntity* E, xrClientData* from, xrClientData* to);
	void					Process_spawn		(NET_Packet& P, DPNID sender);
	void					Process_update		(NET_Packet& P, DPNID sender);
	void					Process_ownership	(NET_Packet& P, DPNID sender);
	void					Process_rejecting	(NET_Packet& P, DPNID sender);

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
	void					SLS_Save			(CFS_Base& FS);
	void					SLS_Load			(CStream&  FS);
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
