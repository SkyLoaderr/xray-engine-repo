#include "stdafx.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "hudmanager.h"
#include "xrserver_objects.h"

void xrServer::Perform_connect_spawn(CSE_Abstract* E, xrClientData* CL, NET_Packet& P)
{
	if (E->net_Processed)						return;
	if (E->s_flags.is(M_SPAWN_OBJECT_PHANTOM))	return;

	// Connectivity order
	CSE_Abstract* Parent = ID_to_entity	(E->ID_Parent);
	if (Parent)		Perform_connect_spawn	(Parent,CL,P);

	// Process
	if (0==E->owner)	
	{
		// PROCESS NAME; Name this entity
		if (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER))
		{
			CL->owner		= E;
			strcpy			(E->s_name_replace,*CL->Name);
		}

		// Associate
		E->owner		= CL;
		E->Spawn_Write	(P,TRUE	);
	}
	else				
	{
		// Just inform
		E->Spawn_Write	(P,FALSE);
	}
	SendTo				(CL->ID,P,net_flags(TRUE,TRUE));
	E->net_Processed	= TRUE;
}

void xrServer::SendConnectionData	(IClient* _CL)
{
	xrClientData*	CL				= (xrClientData*)_CL;
	NET_Packet		P;
	u32			mode				= net_flags(TRUE,TRUE);
	// Replicate current entities on to this client
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; ++I)						I->second->net_Processed	= FALSE;
	for (I=entities.begin(); I!=E; ++I)		Perform_connect_spawn		(I->second,CL,P);

	// Send "finished" signal
	P.w_begin						(M_SV_CONFIG_FINISHED);
	SendTo							(CL->ID,P,mode);
};

void xrServer::OnCL_Connected		(IClient* _CL)
{
	xrClientData*	CL				= (xrClientData*)_CL;
	Server_Client_Check(CL); 

	csPlayers.Enter					();

	//. HUD().outMessage			(0xffffffff,"SERVER","Player '%s' connected",CL->Name);
//	NET_Packet		P;

	// Game config (all, info includes _new_ player)
	Export_game_type(CL);
	Perform_game_export();
	SendConnectionData(CL);
/*
	u32			mode				= net_flags(TRUE,TRUE);
	// Replicate current entities on to this client
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; ++I)						I->second->net_Processed	= FALSE;
	for (I=entities.begin(); I!=E; ++I)		Perform_connect_spawn		(I->second,CL,P);

	// Send "finished" signal
	P.w_begin						(M_SV_CONFIG_FINISHED);
	SendTo							(CL->ID,P,mode);
*/
	//
	
	game->OnPlayerConnect			(CL->ID);
	csPlayers.Leave					();

	

///	if (!CL->flags.bLocal)
///	{			
///		HUD().outMessage		(0xffffffff,"SERVER","Player '%s' connected",game->get_option_s(CL->Name,"name",CL->Name));
///	};

}
