// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
#define AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_
#pragma once

#include "xrMessages.h"

const u32	NET_Latency		= 150;		// time in (ms)

// refs
class xrServerEntity;

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

class xrServerEntity
{
public:
	BOOL					net_Ready;

	u16						ID;				// internal ID
	u16						ID_Parent;		// internal ParentID, 0xffff means no parent
	xrClientData*			owner;

	// spawn data
	string64				s_name;
	string64				s_name_replace;
	u8						s_RP;
	u16						s_flags;		// state flags

	// update data
	Fvector					o_Position;
	Fvector					o_Angle;

	virtual void			UPDATE_Read		(NET_Packet& P)				= 0;
	virtual void			UPDATE_Write	(NET_Packet& P)				= 0;
	virtual void			STATE_Read		(NET_Packet& P, u16 size)	= 0;
	virtual void			STATE_Write		(NET_Packet& P)				= 0;
	
	virtual u8				g_team()		{ return 0;	}
	virtual u8				g_squad()		{ return 0;	}
	virtual u8				g_group()		{ return 0;	}
	
	// utils
	void					Spawn_Write		(NET_Packet& P, BOOL bLocal)
	{
		// generic
		P.w_begin			(M_SPAWN		);
		P.w_string			(s_name			);
		P.w_string			(s_name_replace	);
		P.w_u8				(0xFE			);	// No need for RP, use supplied (POS,ANGLEs)
		P.w_vec3			(o_Position		);
		P.w_vec3			(o_Angle		);
		P.w_u16				(ID				);
		P.w_u16				(ID_Parent		);
		if (bLocal)			P.w_u16(s_flags	| M_SPAWN_OBJECT_LOCAL );
		else				P.w_u16(s_flags );

		// write specific data
		u32	position		= P.w_tell		();
		P.w_u16				(0				);
		STATE_Write			(P				);
		u16 size			= u16			(P.w_tell()-position);
		P.w_seek			(position,&size,sizeof(u16));
	}
	void					Spawn_Read		(NET_Packet& P)
	{
		u16					dummy16;
		// generic
		P.r_begin			(dummy16		);
		P.r_string			(s_name			);
		P.r_string			(s_name_replace	);
		P.r_u8				(s_RP			);
		P.r_vec3			(o_Position		);
		P.r_vec3			(o_Angle		);
		P.r_u16				(ID				);
		P.r_u16				(ID_Parent		);
		P.r_u16				(s_flags		); s_flags&=~M_SPAWN_OBJECT_LOCAL;

		// read specific data
		u16					size;
		P.r_u16				(size			);	// size
		STATE_Read			(P,size			);
	}

	xrServerEntity			()
	{
		net_Ready			= FALSE;
		ID					= 0xffff;
		owner				= 0;
		ZeroMemory			(s_name,		sizeof(string64));
		ZeroMemory			(s_name_replace,sizeof(string64));
	}
	~xrServerEntity			()
	{
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
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
