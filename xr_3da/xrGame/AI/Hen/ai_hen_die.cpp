////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_die.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Die"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"

BOOL _HenDie::Parse(CCustomMonster* Me)
{
	Me->q_look.setup		(0,AI::t_None,0,0	);
	Me->q_action.setup		(AI::AIC_Action::FireEnd);
	Me->AI_Path.TravelPath.clear();
	return FALSE;
}

