////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"
#include "..\\ai_monsters_misc.h"


CAI_Biting::CAI_Biting()
{
	// init local parameters

}

CAI_Biting::~CAI_Biting()
{
	// delete sounds

	xr_delete				(m_pPhysicsShell);

}

/*void CAI_Biting::Die()
{
	inherited::Death( );

	// ...

}

void CAI_Biting::Load(LPCSTR section)
{
	// load parameters from ".ini" file
	inherited::Load(section);
	
	// Load params from section

}

BOOL CAI_Biting::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	return true;
}

void CAI_Biting::net_Export(NET_Packet& P) 
{
}

void CAI_Biting::net_Import(NET_Packet& P)
{
}

void CAI_Biting::Update(u32 dt)
{
}

void CAI_Biting::UpdateCL()
{
}



*/

