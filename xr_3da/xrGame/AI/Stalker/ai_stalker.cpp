////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

CAI_Stalker::CAI_Stalker()
{
	m_tStateList.clear();
}

CAI_Stalker::~CAI_Stalker()
{
	Msg("FSM report for %s :",cName());
	for (int i=0; i<(int)m_tStateList.size(); i++)
		Msg("%3d %6d",m_tStateList[i].eState,m_tStateList[i].dwTime);
	Msg("Total updates : %d",m_dwUpdateCount);
}

// when soldier is dead
void CAI_Stalker::Death()
{
	inherited::Death( );
//	eCurrentState = aiStalkerDie;
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::OnDeviceCreate()
{ 
	inherited::OnDeviceCreate();
	CStalkerAnimations::Load(PKinematics(pVisual));
}

void CAI_Stalker::Load	(LPCSTR section)
{ 
	inherited::Load		(section);
	
//	Fvector				P = vPosition;
//	P.x					+= ::Random.randF();
//	P.z					+= ::Random.randF();
	
//	vfLoadSounds		();
}

BOOL CAI_Stalker::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	return				(TRUE);
}