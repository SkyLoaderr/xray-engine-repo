////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_movement_action.h"
#include "script_space.h"
#include "script_game_object.h"

using namespace luabind;

void CScriptMovementAction::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptMovementAction>("move")
			.enum_("body")
			[
				value("crouch",					int(MonsterSpace::eBodyStateCrouch)),
				value("standing",				int(MonsterSpace::eBodyStateStand))
			]
			.enum_("move")
			[
				value("walk",					int(MonsterSpace::eMovementTypeWalk)),
				value("run",					int(MonsterSpace::eMovementTypeRun)),
				value("stand",					int(MonsterSpace::eMovementTypeStand))
			]
			.enum_("path")
			[
				value("line",					int(CDetailPathManager::eDetailPathTypeSmooth)),
				value("dodge",					int(CDetailPathManager::eDetailPathTypeSmoothDodge)),
				value("criteria",				int(CDetailPathManager::eDetailPathTypeSmoothCriteria)),
				value("curve",					int(CDetailPathManager::eDetailPathTypeSmooth)),
				value("curve_criteria",			int(CDetailPathManager::eDetailPathTypeSmoothCriteria))
			]
			.enum_("input")
			[
				value("none",					int(CScriptMovementAction::eInputKeyNone)),
				value("fwd",					int(CScriptMovementAction::eInputKeyForward)),
				value("back",					int(CScriptMovementAction::eInputKeyBack)),
				value("left",					int(CScriptMovementAction::eInputKeyLeft)),
				value("right",					int(CScriptMovementAction::eInputKeyRight)),
				value("up",						int(CScriptMovementAction::eInputKeyShiftUp)),
				value("down",					int(CScriptMovementAction::eInputKeyShiftDown)),
				value("break",					int(CScriptMovementAction::eInputKeyBreaks)),
				value("on",						int(CScriptMovementAction::eInputKeyEngineOn)),
				value("off",					int(CScriptMovementAction::eInputKeyEngineOff))
			]
			.enum_("monster")
			[

				value("walk_fwd",					int(MonsterSpace::eMA_WalkFwd)),
				value("walk_bkwd",					int(MonsterSpace::eMA_WalkBkwd)),
				value("run_fwd",					int(MonsterSpace::eMA_Run)),
				value("drag",						int(MonsterSpace::eMA_Drag)),
				value("jump",						int(MonsterSpace::eMA_Jump)),
				value("steal",						int(MonsterSpace::eMA_Steal))
			]
			.enum_("monster_speed_param")
			[
				value("default",					int(MonsterSpace::eSP_Default)),
				value("force",						int(MonsterSpace::eSP_ForceSpeed))
			]

			.def(								constructor<>())
			.def(								constructor<const CScriptMovementAction::EInputKeys>())
			.def(								constructor<const CScriptMovementAction::EInputKeys, float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,CScriptGameObject*>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,CScriptGameObject*,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const CPatrolPathParams &,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const Fvector &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const Fvector &,float>())
			.def(								constructor<const Fvector &,float>())
			
			// Monsters 
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,Fvector &,				MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CPatrolPathParams &,	MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CScriptGameObject*,				MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,Fvector &>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CScriptGameObject*>())


			.def("body",						&CScriptMovementAction::SetBodyState)
			.def("move",						&CScriptMovementAction::SetMovementType)
			.def("path",						&CScriptMovementAction::SetPathType)
			.def("object",						&CScriptMovementAction::SetObjectToGo)
			.def("patrol",						&CScriptMovementAction::SetPatrolPath)
			.def("position",					&CScriptMovementAction::SetPosition)
			.def("input",						&CScriptMovementAction::SetInputKeys)
			.def("completed",					(bool (CScriptMovementAction::*)())(CScriptMovementAction::completed))
	];
}