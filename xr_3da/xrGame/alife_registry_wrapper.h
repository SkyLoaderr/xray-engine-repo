////////////////////////////////////////////////////////////////////////////
// alife_registry_wrapper.h - ������� ��� �������, ����������������� ������
//							  ��� alife()
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"
#include "alife_simulator.h"


template <typename _registry_type>
class CALifeRegistryWrapper
{
public:
	IC				CALifeRegistryWrapper	() {holder_id = 0xffff;}
	virtual			~CALifeRegistryWrapper	() {}

	IC	void		init					(u16 id) {holder_id = id;}
	
	typename _registry_type::_data&			objects					();
	const typename _registry_type::_data*	objects_ptr				() const;

	typename _registry_type::_data&			objects					(u16 id);
	const typename _registry_type::_data*	objects_ptr				(u16 id) const;


private:
	//id - ��������� �������
	u16	holder_id;

#ifdef DEBUG
	//������ �� ������, ���� ��� ALife (��� �������)
	typename _registry_type::_data	local_registry;
#endif
};

template <typename _registry_type>
const typename _registry_type::_data* CALifeRegistryWrapper<_registry_type>::objects_ptr	(u16 id) const
{
#ifdef DEBUG
	if(NULL == ai().get_alife()) return &local_registry;
#endif

	VERIFY(0xffff != id);

	typename _registry_type::_data* registy_container = ai().alife().registry((_registry_type*)NULL).object(id, true);
	return registy_container;
}

template <typename _registry_type>
typename _registry_type::_data& CALifeRegistryWrapper<_registry_type>::objects	(u16 id)
{
#ifdef DEBUG
	if(NULL == ai().get_alife()) return local_registry;
#endif

	typename _registry_type::_data* registy_container = ai().alife().registry((_registry_type*)NULL).object(id, true);

	if(!registy_container)	
	{
		typename _registry_type::_data new_registry;
		ai().alife().registry((_registry_type*)NULL).add(id, new_registry, false);
		registy_container = ai().alife().registry((_registry_type*)NULL).object(id, true);
		VERIFY(registy_container);
	}
	return *registy_container;
}


template <typename _registry_type>
const typename _registry_type::_data* CALifeRegistryWrapper<_registry_type>::objects_ptr	() const
{
	return objects_ptr(holder_id);
}

template <typename _registry_type>
typename _registry_type::_data& CALifeRegistryWrapper<_registry_type>::objects	()
{
	return objects(holder_id);
}
