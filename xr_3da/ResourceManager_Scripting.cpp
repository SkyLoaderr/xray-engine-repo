#include "stdafx.h"
#pragma hdrstop

#include	"ResourceManager.h"
#include	"tss.h"
#include	"blenders\blender.h"
#include	"blenders\blender_recorder.h"

// lua
#pragma warning(disable:4244)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4267)

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// lua-bind
#include <luabind\\luabind.hpp>
#include <luabind\\object.hpp>

#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4530)
#pragma warning(default:4267)

// wrapper
class	adopt_compiler
{
	CBlender_Compile*	C;	
public:
	adopt_compiler		(CBlender_Compile*	_C)	: C(_C)		{ }
};

// export
void	export	(lua_State* S)
{
	class_<CBlender_Compile>("move")
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
					value("line",					int(MonsterSpace::ePathTypeStraight)),
					value("dodge",					int(MonsterSpace::ePathTypeDodge)),
					value("criteria",				int(MonsterSpace::ePathTypeCriteria)),
					value("curve",					int(MonsterSpace::ePathTypeStraightDodge)),
					value("curve_criteria",			int(MonsterSpace::ePathTypeDodgeCriteria))
				]
				.enum_("input")
					[
						value("none",					int(CMovementAction::eInputKeyNone)),
						value("fwd",					int(CMovementAction::eInputKeyForward)),
						value("back",					int(CMovementAction::eInputKeyBack)),
						value("left",					int(CMovementAction::eInputKeyLeft)),
						value("right",					int(CMovementAction::eInputKeyRight)),
						value("up",						int(CMovementAction::eInputKeyShiftUp)),
						value("down",					int(CMovementAction::eInputKeyShiftDown)),
						value("break",					int(CMovementAction::eInputKeyBreaks)),
						value("on",						int(CMovementAction::eInputKeyEngineOn)),
						value("off",					int(CMovementAction::eInputKeyEngineOff))
					]
					.enum_("act")
						[
							value("act_stand_idle",					int(MonsterSpace::eActStandIdle)),
							value("act_sit_idle",					int(MonsterSpace::eActSitIdle)),
							value("act_lie_idle",					int(MonsterSpace::eActLieIdle)),
							value("act_walk_fwd",					int(MonsterSpace::eActWalkFwd)),
							value("act_walk_bkwd",					int(MonsterSpace::eActWalkBkwd)),
							value("act_run",						int(MonsterSpace::eActRun)),
							value("act_eat",						int(MonsterSpace::eActEat)),
							value("act_sleep",						int(MonsterSpace::eActSleep)),
							value("act_rest",						int(MonsterSpace::eActRest)),
							value("act_drag",						int(MonsterSpace::eActDrag)),
							value("act_attack",						int(MonsterSpace::eActAttack)),
							value("act_steal",						int(MonsterSpace::eActSteal)),
							value("act_look_around",				int(MonsterSpace::eActLookAround)),
							value("act_jump",						int(MonsterSpace::eActJump))
						]

						.def(								constructor<>())
							.def(								constructor<const CMovementAction::EInputKeys>())
							.def(								constructor<const CMovementAction::EInputKeys, float>())
							.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,CLuaGameObject*>())
							.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,CLuaGameObject*,float>())
							.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const CPatrolPathParams &>())
							.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const CPatrolPathParams &,float>())
							.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const Fvector &>())
							.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const Fvector &,float>())
							.def(								constructor<MonsterSpace::EActState,const Fvector &>())
							.def(								constructor<MonsterSpace::EActState,const CPatrolPathParams &>())
							.def(								constructor<MonsterSpace::EActState,CLuaGameObject*>())
							.def(								constructor<const Fvector &,float>())
							.def("body",						&CMovementAction::SetBodyState)
							.def("move",						&CMovementAction::SetMovementType)
							.def("path",						&CMovementAction::SetPathType)
							.def("object",						&CMovementAction::SetObjectToGo)
							.def("patrol",						&CMovementAction::SetPatrolPath)
							.def("position",					&CMovementAction::SetPosition)
							.def("input",						&CMovementAction::SetInputKeys)
							.def("act",							&CMovementAction::SetAct),
}