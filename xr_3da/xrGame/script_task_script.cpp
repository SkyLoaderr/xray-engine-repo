////////////////////////////////////////////////////////////////////////////
// script_task_script.cpp :	структура для передачи информации о несюжетных 
//							задания в скрипты
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_task.h"
#include "script_space.h"

#include "GameTask.h"

using namespace luabind;

void CScriptTask::script_register(lua_State *L)
{
	module(L)
		[
			class_<CScriptTask>("task")
            	.enum_("task_state")
				[
					value("fail",					int(eTaskStateFail)),
					value("in_progress",			int(eTaskStateInProgress)),
					value("completed",				int(eTaskStateCompleted)),
					value("task_dummy",				int(eTaskStateDummy))
				]

				.def		 (						constructor<>())
				.def_readonly("name",				&CScriptTask::m_sName)
				.def_readonly("organization",		&CScriptTask::m_sOrganization)
				.def_readonly("quantity",			&CScriptTask::m_iQuantity)
				.def_readonly("price",				&CScriptTask::m_iPrice)
		];
}

void CScriptTaskList::script_register(lua_State *L)
{
	module(L)
		[
			class_<CScriptTaskList>("task_list")
			.def(		constructor<u16>()		)
			.def("size",&CScriptTaskList::size	)
			.def("get",	&CScriptTaskList::get	)
		];
}