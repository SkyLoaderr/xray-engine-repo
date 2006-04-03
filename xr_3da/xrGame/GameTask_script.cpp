#include "stdafx.h"
#include "GameTask.h"
#include "script_space.h"
#include <luabind/adopt_policy.hpp>

using namespace luabind;
void CGameTask::script_register(lua_State *L)
{
	module(L)
		[
			class_<enum_exporter<ETaskState> >("task")
				.enum_("task_state")
				[
					value("fail",					int(eTaskStateFail)),
					value("in_progress",			int(eTaskStateInProgress)),
					value("completed",				int(eTaskStateCompleted)),
					value("task_dummy",				int(eTaskStateDummy))
				],

			class_<SGameTaskObjective>("SGameTaskObjective")
				.def(				constructor<CGameTask*, int>()											)
				.def("set_description",						&SGameTaskObjective::SetDescription_script		)
				.def("set_article_id",						&SGameTaskObjective::SetArticleID_script		)
				.def("set_map_hint",						&SGameTaskObjective::SetMapHint_script			)
				.def("set_map_location",					&SGameTaskObjective::SetMapLocation_script		)
				.def("set_object_id",						&SGameTaskObjective::SetObjectID_script			)
				.def("set_article_key",						&SGameTaskObjective::SetArticleKey_script		)
				.def("add_complete_info",					&SGameTaskObjective::AddCompleteInfo_script		)
				.def("add_fail_info",						&SGameTaskObjective::AddFailInfo_script			)
				.def("add_on_complete_info",				&SGameTaskObjective::AddOnCompleteInfo_script	)
				.def("add_on_fail_info",					&SGameTaskObjective::AddOnFailInfo_script		)
				
				.def("add_complete_func",					&SGameTaskObjective::AddCompleteFunc_script			)
				.def("add_fail_func",						&SGameTaskObjective::AddFailFunc_script				)
				.def("add_on_complete_func",				&SGameTaskObjective::AddOnCompleteFunc_script	)
				.def("add_on_fail_func",					&SGameTaskObjective::AddOnFailFunc_script			)

				.def("get_state",							&SGameTaskObjective::TaskState						)
				.def("get_idx",								&SGameTaskObjective::GetIDX_script						)
				.def("get_state",							&SGameTaskObjective::TaskState						),


			class_<CGameTask>("CGameTask")
				.def(										constructor<>()									)
				.def("load",								&CGameTask::Load_script							)
				.def("set_title",							&CGameTask::GetTitle_script						)
				.def("get_title",							&CGameTask::SetTitle_script						)
				.def("add_objective",						&CGameTask::AddObjective_script,		adopt(_2))
				.def("get_objective",						&CGameTask::GetObjective_script					)
				.def("get_id",								&CGameTask::GetID_script						)
				.def("get_objectives_cnt",					&CGameTask::GetObjectiveSize_script				)
		];
}