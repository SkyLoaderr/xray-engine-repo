////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_schedule_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife schedule registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CALifeScheduleRegistry::update		()
{
#ifdef _DEBUG
	_REGISTRY::const_iterator	I = objects().begin();
	_REGISTRY::const_iterator	E = objects().end();
	for ( ; I != E; ++I) {
		CSE_ALifeObject			*alife_object = dynamic_cast<CSE_ALifeObject*>((*I).second);
		VERIFY					(alife_object);
		VERIFY3					(!alife_object->m_bOnline,alife_object->s_name,alife_object->s_name_replace);
	}
#endif
	u32							count = objects().empty() ? 0 : inherited::update(CUpdatePredicate());
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS][SU][%d : %d]",count, objects().size());
	}
#endif
}

IC	void CALifeScheduleRegistry::add		(CSE_ALifeDynamicObject *object)
{
	CSE_ALifeSchedulable		*schedulable = dynamic_cast<CSE_ALifeSchedulable*>(object);
	if (!schedulable || !schedulable->need_update(object))
		return;

	inherited::add				(object->ID,schedulable);
}

IC	void CALifeScheduleRegistry::remove	(CSE_ALifeDynamicObject *object, bool no_assert)
{
	CSE_ALifeSchedulable		*schedulable = dynamic_cast<CSE_ALifeSchedulable*>(object);
	if (!schedulable)
		return;

	inherited::remove			(object->ID,no_assert || !schedulable->need_update(object));
}
