////////////////////////////////////////////////////////////////////////////
// script_task.h :		структура для передачи информации о несюжетных 
//						задания в скрипты
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "script_export_space.h"

class CALifeTask;

class CScriptTask;
DEFINE_VECTOR(CScriptTask, ALIFE_TASK_VECTOR, ALIFE_TASK_VECTOR_IT);

class CScriptTask 
{
protected:
	LPCSTR				m_sName;
	int 				m_iQuantity;

public:
				CScriptTask		();
				CScriptTask		(const CALifeTask *pALifeTask);
	virtual		~CScriptTask	();

	LPCSTR		Name			() const;
	int 		Quantity		() const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptTask)
#undef script_type_list
#define script_type_list save_type_list(CScriptTask)