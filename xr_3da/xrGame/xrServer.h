// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
#define AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_
#pragma once

#include "xrMessages.h"

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
	xrClientData*			owner;

	// spawn data
	string64				s_name;
	string64				s_name_replace;
	u8						s_team;
	u8						s_squad;
	u8						s_group;
	u8						s_RP;
	Fvector					o_Position;
	Fvector					o_Angle;
	u16						s_flags;		// state flags
	
	virtual void			UPDATE_Read		(NET_Packet& P)				= 0;
	virtual void			UPDATE_Write	(NET_Packet& P)				= 0;
	virtual void			STATE_Read		(NET_Packet& P, u16 size)	= 0;
	virtual void			STATE_Write		(NET_Packet& P)				= 0;

	virtual BOOL			RelevantTo		(xrServerEntity* E)	= 0;

	// utils
	void					Spawn_Write		(NET_Packet& P, BOOL bLocal)
	{
		// generic
		P.w_begin			(M_SPAWN		);
		P.w_string			(s_name			);
		P.w_string			(s_name_replace	);
		P.w_u8				(s_team			);
		P.w_u8				(s_squad		);
		P.w_u8				(s_group		);
		P.w_u8				(0xFE			);	// No need for RP, use supplied (POS,ANGLEs)
		P.w_vec3			(o_Position		);
		P.w_vec3			(o_Angle		);
		P.w_u16				(ID				);
		P.w_u8				(bLocal			);

		// write specific data
		u32	position		= P.w_tell		();
		P.w_u16				(0				);
		P.w_u16				(s_flags		);
		STATE_Write			(P				);
		u16 size			= u16			(P.w_tell()-position);
		P.w_seek			(position,&size,sizeof(u16));
	}
	void					Spawn_Read		(NET_Packet& P)
	{
		u8					dummy8;
		u16					dummy16;
		// generic
		P.r_begin			(dummy16		);
		P.r_string			(s_name			);
		P.r_string			(s_name_replace	);
		P.r_u8				(s_team			);
		P.r_u8				(s_squad		);
		P.r_u8				(s_group		);
		P.r_u8				(s_RP			);
		P.r_vec3			(o_Position		);
		P.r_vec3			(o_Angle		);
		P.r_u16				(ID				);
		P.r_u8				(dummy8			);	// bLocal

		// read specific data
		u16					size;
		P.r_u16				(size			);	// size
		P.r_u16				(s_flags		);
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
	BOOL					ProcessRP			(xrServerEntity* E);
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
		xrS_entities::iterator	I	= entities.find	(ID);
		if (I!=entities.end())		return I->second;
		else						return 0;
	}

	// main
	void					Update			();
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
