////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fire.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Fire and enemy parameters for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

void CAI_Biting::vfSaveEnemy()
{
	m_tSavedEnemy			= m_tEnemy.Enemy;
	m_tSavedEnemyPosition	= m_tEnemy.Enemy->Position();
	m_tpSavedEnemyNode		= m_tEnemy.Enemy->AI_Node;
	m_dwSavedEnemyNodeID	= m_tEnemy.Enemy->AI_NodeID;
	m_dwSeenEnemyLastTime	= Level().timeServer();
	m_tMySavedPosition		= vPosition;
	m_dwMyNodeID			= AI_NodeID;
	vfValidatePosition		(m_tSavedEnemyPosition,m_dwSavedEnemyNodeID);
};

