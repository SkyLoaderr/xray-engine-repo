////////////////////////////////////////////////////////////////////////////
//	Module 		: LevelGameDef.cpp
//	Created 	: 25.12.2002
//  Modified 	: 25.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Interface with Level Editor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LevelGameDef.h"

#ifdef _EDITOR
#include "PropertiesListTypes.h"

void CNPC_Point::FillProp(LPCSTR pref, PropValueVec& values)
{
   	FILL_PROP_EX(values, pref, "Model",						&caModel,					PHelper.CreateGameObject(sizeof(caModel)));
   	FILL_PROP_EX(values, pref, "Team",						&ucTeam,						PHelper.CreateU8	(0,255,1));
   	FILL_PROP_EX(values, pref, "Squad",						&ucSquad,					PHelper.CreateU8	(0,255,1));
   	FILL_PROP_EX(values, pref, "Group",						&ucGroup,					PHelper.CreateU8	(0,255,1));
   	FILL_PROP_EX(values, pref, "Group ID",					&wGroupID,					PHelper.CreateU16	(0,65535,1));
   	FILL_PROP_EX(values, pref, "Count",						&wCount,					PHelper.CreateU16	(0,65535,1));
   	FILL_PROP_EX(values, pref, "Birth radius",				&fBirthRadius,				PHelper.CreateFloat	(0,1000.f,1.f));
   	FILL_PROP_EX(values, pref, "Birth probability",			&fBirthProbability,			PHelper.CreateFloat	(0,1.f,.1f));
   	FILL_PROP_EX(values, pref, "Increase coefficient",		&fIncreaseCoefficient,		PHelper.CreateFloat	(0,1.f,.05f));
   	FILL_PROP_EX(values, pref, "Anomaly death probability",	&fAnomalyDeathProbability,	PHelper.CreateFloat	(0,1.f,.1f));
	FILL_PROP_EX(values, pref, "Route points",				&caRouteGraphPoints,		PHelper.CreateGameObject(sizeof(caRouteGraphPoints)));
}
#endif
