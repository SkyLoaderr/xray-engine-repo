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
typedef svector<xrServerEntity*,256>	xrS_entities;

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

	BYTE					ID;				// internal ID
	xrClientData*			owner;

	// spawn data
	LPSTR					s_name;
	LPSTR					s_name_replace;
	BYTE					s_team;
	BYTE					s_squad;
	BYTE					s_group;

	virtual void			Read			(NET_Packet& P)		= 0;
	virtual void			Write			(NET_Packet& P)		= 0;
	virtual BOOL			RelevantTo		(xrServerEntity* E)	= 0;
	virtual BOOL			Spawn			(BYTE rp, xrS_entities& ent)	= 0;
	virtual void			GetPlacement	(Fvector4& dest)	= 0;

	// utils
	void					msgSpawn		(NET_Packet& P, BOOL bLocal)
	{
		P.w_begin			(M_SV_SPAWN);
		P.w_string			(s_name);
		P.w_string			(s_name_replace);
		P.w_u8				(s_team);
		P.w_u8				(s_squad);
		P.w_u8				(s_group);
		P.w_u8				(ID);
		P.w_u8				(bLocal);

		Fvector4			Placement;
		GetPlacement		(Placement);
		P.w					(&Placement,3*sizeof(float));
		P.w_angle8			(Placement.w);
	}

	xrServerEntity			()
	{
		net_Ready			= FALSE;
		ID					= 0xff;
		owner				= 0;
	}
	~xrServerEntity			()
	{
		_FREE	(s_name_replace);
		_FREE	(s_name);
	}
};

// main
class xrServer	: public IPureServer  
{
private:
	xrS_entities			entities;
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
	xrServerEntity*			entity_Create	(LPCSTR name);
	void					entity_Destroy	(xrServerEntity* P);

	IC xrClientData*		ID_to_client	(DPNID ID)
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
	IC xrServerEntity*		ID_to_entity	(BYTE ID)
	{
		xrS_entities::iterator	I=entities.begin(),E=entities.end();
		for (; I!=E; I++)
		{
			xrServerEntity*	Test = *I;
			if (Test->ID==ID)	{
				return Test;
			}
		}
		return 0;
	}

	// main
	void					Update			();
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
