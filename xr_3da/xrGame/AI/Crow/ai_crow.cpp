////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "ai_crow.h"
#include "..\\ai_monsters.h"
#include "..\\..\\hudmanager.h"

//#define WRITE_LOG

CAI_Crow::CAI_Crow()
{
	st_current = aiCrowFreeFly;
}

CAI_Crow::~CAI_Crow()
{
	// removing all data no more being neded 
}

// crow update
void CAI_Crow::Update(DWORD DT)
{
	inherited::Update(DT);
}

// Core events
void CAI_Crow::Load					( LPCSTR section )
{
	inherited::Load					(section);

	CKinematics*	M				= PKinematics(pVisual);
	R_ASSERT						(M);
	M->PlayCycle					("init");
}

BOOL CAI_Crow::Spawn(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	BOOL R = inherited::Spawn	(bLocal,server_id,o_pos,o_angle,P,flags);
	bVisible					= TRUE;
	return R;
}

void CAI_Crow::net_Export(NET_Packet* P)					// export to server
{
}

void CAI_Crow::net_Import(NET_Packet* P)
{
}
