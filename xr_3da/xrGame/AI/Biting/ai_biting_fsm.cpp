////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\rat\\ai_rat.h"
#include "..\\..\\actor.h"

void CAI_Biting::Think()
{
	if (!g_Alive()) return;

	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	vfUpdateParameters		();

#ifndef SILENCE
	if (g_Alive())
		Msg("%s : [A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][H=%d][I=%d][J=%d][K=%d]",cName(),A,B,C,D,E,F,H,I,J,K);
#endif

}