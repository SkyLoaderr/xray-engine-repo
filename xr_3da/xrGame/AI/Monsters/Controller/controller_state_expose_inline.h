#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerExposeAbstract CStateControlExpose<_Object>

TEMPLATE_SPECIALIZATION
CStateControllerExposeAbstract::CStateControlExpose(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateControllerExposeAbstract::~CStateControlExpose()
{
}

TEMPLATE_SPECIALIZATION
void CStateControllerExposeAbstract::execute()
{
	// update tasks
	for (u32 i=0; i < object->m_controlled_objects.size(); i++) {
		CControlledEntityBase *pE = dynamic_cast<CControlledEntityBase *>(object->m_controlled_objects[i]);
		VERIFY(pE);
		
		pE->get_data().m_object	= object->EnemyMan.get_enemy();
		pE->get_data().m_task		= eTaskAttack;
	}
}

TEMPLATE_SPECIALIZATION
bool CStateControllerExposeAbstract::check_start_conditions()
{
	if (object->HasUnderControl()) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateControllerExposeAbstract::check_completion()
{
	if (!object->HasUnderControl()) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateControllerExposeAbstract::finalize()
{
	// update tasks
	for (u32 i=0; i < object->m_controlled_objects.size(); i++) {
		CControlledEntityBase *pE = dynamic_cast<CControlledEntityBase *>(object->m_controlled_objects[i]);
		VERIFY(pE);

		pE->get_data().m_object	= 0;
		pE->get_data().m_task		= eTaskNone;
	}
}

TEMPLATE_SPECIALIZATION
void CStateControllerExposeAbstract::critical_finalize()
{
	// update tasks
	for (u32 i=0; i < object->m_controlled_objects.size(); i++) {
		CControlledEntityBase *pE = dynamic_cast<CControlledEntityBase *>(object->m_controlled_objects[i]);
		VERIFY(pE);

		pE->get_data().m_object	= 0;
		pE->get_data().m_task		= eTaskNone;
	}
}

