// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
#define AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_
#pragma once

#include "xrServer_Objects_ALife_All.h"
#include "game_sv_base.h"

const u32	NET_Latency		= 50;		// time in (ms)

// t-defs
typedef xr_map<u16,CAbstractServerObject*>	xrS_entities;

class xrClientData	: public IClient
{
public:
	CAbstractServerObject*			owner;
	BOOL					net_Ready;
	u32						game_replicate_id;

	game_PlayerState		ps;

	xrClientData	()
	{
		owner		= NULL;
		net_Ready	= FALSE;
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
	xrS_entities				entities;
	xr_deque<u16>				id_free;
	xr_multiset<svs_respawn>	q_respawn;

public:
	game_sv_GameState*			game;

	void					Perform_game_export		();
	BOOL					PerformRP				(CAbstractServerObject* E);
	void					PerformMigration		(CAbstractServerObject* E, xrClientData* from, xrClientData* to);
	u16						PerformIDgen			(u16 desired);
	void					Perform_connect_spawn	(CAbstractServerObject* E, xrClientData* to, NET_Packet& P);
	void					Perform_transfer		(CAbstractServerObject* what, CAbstractServerObject* from, CAbstractServerObject* to);
	void					Perform_reject			(CAbstractServerObject* what, CAbstractServerObject* from);
	void					Perform_destroy			(CAbstractServerObject* tpServerEntity, u32 mode);

	void					Process_spawn			(NET_Packet& P, DPNID sender, BOOL bSpawnWithClientsMainEntityAsParent=FALSE, CAbstractServerObject* tpExistedEntity=0);
	void					Process_update			(NET_Packet& P, DPNID sender);
	void					Process_event			(NET_Packet& P, DPNID sender);
	void					Process_event_ownership	(NET_Packet& P, DPNID sender, u32 time, u16 ID);
	void					Process_event_reject	(NET_Packet& P, DPNID sender, u32 time, u16 ID);

	xrClientData*			SelectBestClientToMigrateTo		(CAbstractServerObject* E, BOOL bForceAnother=FALSE);

public:
	// constr / destr
	xrServer				();
	virtual ~xrServer		();

	// extended functionality
	virtual u32				OnMessage			(NET_Packet& P, DPNID sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual void			OnCL_Connected		(IClient* CL);
	virtual void			OnCL_Disconnected	(IClient* CL);

	virtual IClient*		client_Create		();								// create client info
	virtual void			client_Replicate	();								// replicate current state to client
	virtual void			client_Destroy		(IClient* C);					// destroy client info

	// utilities
	CAbstractServerObject*			entity_Create		(LPCSTR name);
	void					entity_Destroy		(CAbstractServerObject* P);

	IC void					clients_Lock		()			{	csPlayers.Enter();	}
	IC void					clients_Unlock		()			{   csPlayers.Leave();	}

	xrClientData*			ID_to_client		(DPNID ID);
	CAbstractServerObject*			ID_to_entity		(u16 ID);

	// main
	virtual BOOL			Connect				(LPCSTR		session_name);
	virtual void			Disconnect			();
	void					Update				();
	void					SLS_Default			();
	void					SLS_Clear			();
	void					SLS_Save			(IWriter&	fs);
	void					SLS_Load			(IReader&	fs);
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
