#include "stdafx.h"
#include "ai_monster_share.h"


template <class T> T*	IShared<T>::_self = NULL;
template <class T> int	IShared<T>::_refcount=0;

template <class T> T*  IShared<T>::Instance()
{
	if(!_self) _self=xr_new<T>(); 
	_refcount++;
	return _self;
}

template <class T> void  IShared<T>::FreeInst()
{
	if(--_refcount==0) xr_delete(this);
}


//////////////////////////////////////////////////////////////////////////
// CShareObj
template<class T_shared> CSharedObj<T_shared>::~CSharedObj() 
{
	for (SHARED_DATA_MAP_IT it = _shared_tab.begin(); it != _shared_tab.end(); it++) 
		xr_delete(it->second);
}

template<class T_shared> T_shared *CSharedObj<T_shared>::get_shared(CLASS_ID id) 
{
	// look for id
	SHARED_DATA_MAP_IT shared_it = _shared_tab.find(id);

	T_shared *data;

	// if not found - create appropriate shared data object
	if (shared_it == _shared_tab.end()) {
		data		= xr_new<T_shared>();
		_shared_tab.insert(mk_pair(id, data));
	} else data = *shared_it;

	return data;
}

