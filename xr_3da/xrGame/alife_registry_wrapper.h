////////////////////////////////////////////////////////////////////////////
// alife_registry_wrapper.h - обертка для реестра, предусматривающая работу
//							  без alife()
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

private:
	//id - владельца реестра
	u16	holder_id;
	//реестр на случай, если нет ALife
	typename _registry_type::_data	local_registry;
};

template <typename _registry_type>
const typename _registry_type::_data* CALifeRegistryWrapper<_registry_type>::objects_ptr	() const
{
	if(NULL == ai().get_alife()) return &local_registry;
	VERIFY(0xffff != holder_id);

	typename _registry_type::_data* registy_container = ai().alife().registry((_registry_type*)NULL).object(holder_id, true);
	return registy_container;
}

template <typename _registry_type>
typename _registry_type::_data& CALifeRegistryWrapper<_registry_type>::objects	()
{
	if(NULL == ai().get_alife()) return local_registry;

	typename _registry_type::_data* registy_container = ai().alife().registry((_registry_type*)NULL).object(holder_id, true);

	if(!registy_container)	
	{
		typename _registry_type::_data new_registry;
		ai().alife().registry((_registry_type*)NULL).add(holder_id, new_registry, false);
		registy_container = ai().alife().registry((_registry_type*)NULL).object(holder_id, true);
		VERIFY(registy_container);
	}
	return *registy_container;
}
