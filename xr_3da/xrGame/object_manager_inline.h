////////////////////////////////////////////////////////////////////////////
//	Module 		: object_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Object manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename T\
>

#define CAbstractObjectManager CObjectManager<T>

TEMPLATE_SPECIALIZATION
CAbstractObjectManager::CObjectManager				()
{
	Init					();
}

TEMPLATE_SPECIALIZATION
CAbstractObjectManager::~CObjectManager				()
{
}

TEMPLATE_SPECIALIZATION
void CAbstractObjectManager::Init					()
{
}

TEMPLATE_SPECIALIZATION
void CAbstractObjectManager::Load					(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CAbstractObjectManager::reinit					()
{
	m_objects.clear			();
	m_selected				= 0;
}

TEMPLATE_SPECIALIZATION
void CAbstractObjectManager::reload					(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CAbstractObjectManager::update					()
{
	float						result = flt_max/2*(-1);
	m_selected					= 0;
	xr_set<const T*>::const_iterator	I = m_objects.begin();
	xr_set<const T*>::const_iterator	E = m_objects.end();
	for ( ; I != E; ++I) {
		float					value = evaluate(*I);
		if (result < value) {
			result				= value;
			m_selected			= *I;
		}
	}
}

TEMPLATE_SPECIALIZATION
float CAbstractObjectManager::evaluate				(const T *object) const
{
	return					(0.f);
}

TEMPLATE_SPECIALIZATION
bool CAbstractObjectManager::useful					(const T *object) const
{
	const ISpatial			*self = dynamic_cast<const ISpatial*>(object);
	if (!self)
		return				(false);

	if ((object->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI)
		return				(false);

	return					(true);
}

TEMPLATE_SPECIALIZATION
bool CAbstractObjectManager::add					(const T *object)
{
	if (!useful(object))
		return				(false);

	T						*type = dynamic_cast<const T*>(object);
	if (!type)
		return				(false);

	xr_set<const T*>::const_iterator	I = m_objects.find(type);
	if (m_objects.end() == I) {
		m_objects.insert	(type);
		return				(true);
	}
	return					(true);
}

TEMPLATE_SPECIALIZATION
IC	const T *CAbstractObjectManager::selected		() const
{
	return					(m_selected);
}

TEMPLATE_SPECIALIZATION
void CAbstractObjectManager::reset					()
{
	m_objects.clear			();
}

TEMPLATE_SPECIALIZATION
IC	const xr_set<const T*> &CAbstractObjectManager::objects() const
{
	return					(m_objects);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager