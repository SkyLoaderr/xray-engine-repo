// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
#define AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_
#pragma once

#include "game_sv_base.h"
#include "id_generator.h"
class CSE_Abstract;

const u32	NET_Latency		= 75;		// time in (ms)

// t-defs
typedef xr_map<u16,CSE_Abstract*>	xrS_entities;

class xrClientData	: public IClient
{
public:
	CSE_Abstract*			owner;
	BOOL					net_Ready;
	u32						game_replicate_id;

	game_PlayerState		ps;

	xrClientData	():IClient(Device.GetTimerGlobal())
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
	xr_multiset<svs_respawn>	q_respawn;

	CID_Generator<u32,u8,u16,u8,u16,0,u16(-2),256,u16(-1)> m_tID_Generator;

	xrClientData*				SV_Client;

	void						Server_Client_Check (IClient* CL);
public:
	game_sv_GameState*			game;

	void					Perform_game_export		();
	BOOL					PerformRP				(CSE_Abstract* E);
	void					PerformMigration		(CSE_Abstract* E, xrClientData* from, xrClientData* to);
	
	IC u16					PerformIDgen			(u16 ID)
	{
		return				(m_tID_Generator.tfGetID(ID));
	}

	void					Perform_connect_spawn	(CSE_Abstract* E, xrClientData* to, NET_Packet& P);
	void					Perform_transfer		(CSE_Abstract* what, CSE_Abstract* from, CSE_Abstract* to);
	void					Perform_reject			(CSE_Abstract* what, CSE_Abstract* from);
	void					Perform_destroy			(CSE_Abstract* tpSE_Abstract, u32 mode, BOOL Recursive = TRUE);

	void					Process_spawn			(NET_Packet& P, DPNID sender, BOOL bSpawnWithClientsMainEntityAsParent=FALSE, CSE_Abstract* tpExistedEntity=0);
	void					Process_update			(NET_Packet& P, DPNID sender);
	void					Process_event			(NET_Packet& P, DPNID sender);
	void					Process_event_ownership	(NET_Packet& P, DPNID sender, u32 time, u16 ID);
	void					Process_event_reject	(NET_Packet& P, DPNID sender, u32 time, u16 ID);

	xrClientData*			SelectBestClientToMigrateTo		(CSE_Abstract* E, BOOL bForceAnother=FALSE);

	void					SendConnectionData		(IClient* _CL);

public:
	// constr / destr
	xrServer				();
	virtual ~xrServer		();

	// extended functionality
	virtual u32				OnMessage			(NET_Packet& P, DPNID sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual void			OnCL_Connected		(IClient* CL);
	virtual void			OnCL_Disconnected	(IClient* CL);
	virtual bool			OnCL_QueryHost		();

	virtual IClient*		client_Create		();								// create client info
	virtual void			client_Replicate	();								// replicate current state to client
	virtual void			client_Destroy		(IClient* C);					// destroy client info

	// utilities
	CSE_Abstract*			entity_Create		(LPCSTR name);
	void					entity_Destroy		(CSE_Abstract *&P);
	u32						GetEntitiesNum		()			{ return entities.size(); };

	IC void					clients_Lock		()			{	csPlayers.Enter();	}
	IC void					clients_Unlock		()			{   csPlayers.Leave();	}

	xrClientData*			ID_to_client		(DPNID ID);
	CSE_Abstract*			ID_to_entity		(u16 ID);

	// main
	virtual BOOL			Connect				(LPSTR		&session_name);
	virtual void			Disconnect			();
	void					Update				();
	void					SLS_Default			();
	void					SLS_Clear			();
	void					SLS_Save			(IWriter&	fs);
	void					SLS_Load			(IReader&	fs);

	xrClientData*			GetServer_client	()			{ return SV_Client; };
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
