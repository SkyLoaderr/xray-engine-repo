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
}
CScriptTask::CScriptTask (const CALifeTask *pALifeTask)
{
	VERIFY(pALifeTask);

	m_sName = pSettings->r_string(pALifeTask->m_caSection, "inv_name");
	m_iQuantity = pALifeTask->m_dwTryCount;
}

CScriptTask::~CScriptTask	()
{
}

LPCSTR	CScriptTask::Name () const
{
	return m_sName;
}

int		CScriptTask::Quantity		() const
{
	return m_iQuantity;
}