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
	u32							count = objects().empty() ? 0 : inherited::update(CUpdatePredicate());
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS][SU][%d : %d]",count, objects().size());
	}
#endif
}

IC	void CALifeScheduleRegistry::add		(CSE_ALifeDynamicObject *object)
{
	CSE_ALifeSchedulable		*schedulable = smart_cast<CSE_ALifeSchedulable*>(object);
	if (!schedulable || !schedulable->need_update(object))
		return;

	inherited::add				(object->ID,schedulable);
}

IC	void CALifeScheduleRegistry::remove	(CSE_ALifeDynamicObject *object, bool no_assert)
{
	CSE_ALifeSchedulable		*schedulable = smart_cast<CSE_ALifeSchedulable*>(object);
	if (!schedulable)
		return;

	inherited::remove			(object->ID,no_assert || !schedulable->need_update(object));
}

IC	CSE_ALifeSchedulable *CALifeScheduleRegistry::object	(const ALife::_OBJECT_ID &id, bool no_assert) const
{
	_const_iterator				I = objects().find(id);
	if (I == objects().end()) {
		R_ASSERT2				(no_assert,"The spesified object hasn't been found in the schedule registry!");
		return					(0);
	}
	return						((*I).second);
}
