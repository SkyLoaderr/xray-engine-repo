////////////////////////////////////////////////////////////////////////////
// script_task.сpp :	структура для передачи информации о несюжетных 
//						задания в скрипты
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_task.h"

#include "alife_task.h"

CScriptTask::CScriptTask ()
{
	m_sName = NULL;
	m_iQuantity = 0;
	m_sOrganization = NULL;
	m_iPrice = 0;

}

CScriptTask::~CScriptTask	()
{
}