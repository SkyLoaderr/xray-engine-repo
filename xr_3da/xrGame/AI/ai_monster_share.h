#pragma once
#include "ai_monster_defs.h"

// Singleton template definition 
template <class T> class IShared {
	static T*	_self;
	static int	_refcount;
public:

					IShared	()	{}
			virtual	~IShared()	{_self=NULL;}
public:
	static T*		Instance() {
		if(!_self) {
			_self=xr_new<T>(); 
			OUTPUT("__DEEP SHARE::Creating Instance...");
		} else OUTPUT("__DEEP SHARE::Instance already created, just return pointer");
		_refcount++;
		return _self;
	}
	void			FreeInst() {

		if(--_refcount==0) {
			IShared<T> *ptr = this;
			OUTPUT_M("__DEEP SHARE:: Sub reference && Delete instance ptr = [%u]", *"*/ ptr /*"* );
			xr_delete(ptr);
		} else OUTPUT_M("__DEEP SHARE::Sub reference, do not delete instance; ptr = [%u]",*"*/ this /*"* );
	}
};

template <class T> T*	IShared<T>::_self = NULL;
template <class T> int	IShared<T>::_refcount=0;

template<class T_shared> class CSharedObj : public IShared<CSharedObj<T_shared> >
{
	xr_map<CLASS_ID, T_shared*> _shared_tab;	
	typedef typename xr_map<CLASS_ID, T_shared*>::iterator SHARED_DATA_MAP_IT;
	
public:

				CSharedObj() {};
				~CSharedObj() {};
	
	friend class IShared<CSharedObj<T_shared> >;

public:
	// Access to data
	T_shared *get_shared(CLASS_ID id);
};

template<class T_shared> T_shared *CSharedObj<T_shared>::get_shared(CLASS_ID id) 
{
	SHARED_DATA_MAP_IT shared_it = _shared_tab.find(id);

	T_shared *_data;

	// if not found - create appropriate shared data object
	if (shared_it == _shared_tab.end()) {
		_data		= xr_new<T_shared>();
		_shared_tab.insert(mk_pair(id, _data));
		OUTPUT_M("__DEEP SHARE::Create data object with CLS_ID = %u", *"*/ id /*"*);
	} else {
		_data = shared_it->second;
		OUTPUT("__DEEP SHARE::Do not create data object just return pointer");
	}

	return _data;
}

class CSharedResource {
	bool	loaded;
public:
			CSharedResource	() {loaded = false;}

	bool	IsLoaded		() {
		if (loaded) {OUTPUT("__DEEP SHARE::Data already loaded,  do not load!!!");}
		else {OUTPUT("__DEEP SHARE::Load Data");}

		return loaded;
	}
	void	SetLoad			(bool l = true) {loaded = l;}
};


template<class shared_struc> class CSharedClass {
public:
	shared_struc				*_sd;
	CSharedObj<shared_struc>	*pSharedObj;

			void	OnCreate	()	{pSharedObj	= CSharedObj<shared_struc>::Instance();}
			void	OnDestroy	()	{pSharedObj->FreeInst();}

			bool	IsLoaded	() {return _sd->IsLoaded();}
			void	Prepare		(CLASS_ID cls_id)	{_sd = pSharedObj->get_shared(cls_id);}
			void	Finish		()	{_sd->SetLoad();}
};


