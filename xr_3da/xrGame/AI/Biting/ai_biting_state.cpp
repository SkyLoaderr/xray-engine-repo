////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

using namespace AI_Biting;


//////////////////////////////////////////////////////////////////////////
void IState::Execute() 
{
	switch (stage) {
		case 0: Init(); stage++; break;
		case 1: Run(); break;
		case 2: Done(); stage++; break;
		case 3: stage = 0; return;
	}

	// проверить на условие завершение
	if (stage == 1) {
		if (IsWorkDone()) stage=0;
	}


}

//////////////////////////////////////////////////////////////////////////
void stateRunAwayInPanic::Init() 
{
	// построить путь в обратном направлении 
	pData->m_tSelectorRetreat.m_tEnemyPosition = pData->m_tSavedEnemyPosition;
	pData->m_tSelectorRetreat.m_tpEnemyNode = getAI().Node(pData->m_dwMyNodeID);
	pData->m_tSelectorRetreat.m_tMyPosition = pData->vPosition;
	pData->m_tSelectorRetreat.m_tpMyNode = pData->AI_Node;

	pData->vfSetParameters(&pData->m_tSelectorRetreat, 0, true, 0);

	pData->SeenInertion = 30000;
}
void stateRunAwayInPanic::Run() 
{
	pData->vfSetMotionActionParams(eBodyStateStand, eMovementTypeRun, 
		eMovementDirectionForward, eStateTypePanic, eActionTypeRun);

	pData->vfSetParameters(0, 0, false, 0);
}
void stateRunAwayInPanic::Done() 
{
	pData->m_tActionState = eStayInPlaceScared;
}

bool stateRunAwayInPanic::IsWorkDone() 
{
	if (pData->AI_Path.TravelPath.empty() || (pData->AI_Path.TravelStart >= (pData->AI_Path.TravelPath.size() - 1))) {
		pData->AI_Path.TravelPath.clear();
		return true;	
	}

	else return false;
}

void stateRunAwayInPanic::CheckStartCondition()
{	

	if (pData->A && !pData->E && !pData->Ee) stage = 0;
	// он видит врага или помнит его (10 сек)
	// он слышит врага 
	//if ()

}