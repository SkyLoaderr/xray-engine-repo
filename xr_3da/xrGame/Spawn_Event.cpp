// Spawn_Event.cpp: implementation of the CSpawn_Event class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Spawn_Event.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpawn_Event::CSpawn_Event()
{
	Commands.push_back	(Pair());
}

CSpawn_Event::~CSpawn_Event()
{

}

void CSpawn_Event::Save	(CFS_Base& FS)
{
	for (int cmd=0; cmd<Commands.size(); cmd++)
	{

	}
}