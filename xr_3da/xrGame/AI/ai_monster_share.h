	#pragma once

// Singleton template definition 
template <class T> class IShared {
	static T*	_self;
	static int	_refcount;
public:
					IShared	()	{}
	virtual			~IShared()	{_self=NULL;}
public:
	static	T*		Instance();
			void	FreeInst();
};


template<class T_shared> class CSharedObj : public IShared<CSharedObj<T_shared> >
{
	xr_map<CLASS_ID, T_shared*> _shared_tab;	
	typedef typename xr_map<CLASS_ID, T_shared*>::iterator SHARED_DATA_MAP_IT;
	
public:
				CSharedObj() {};
	virtual		~CSharedObj();
	
	friend class IShared<CSharedObj<T_shared> >;

public:
	// Access to data
	T_shared *get_shared(CLASS_ID id);
};


class CSharedResource {
	bool	loaded;
public:
			CSharedResource	() {loaded = false;}

	bool	IsLoaded		() {return loaded;}
	void	SetLoad			(bool l = true) {loaded = l;}
};



