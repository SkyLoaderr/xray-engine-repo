#pragma once

// refs
class ENGINE_API				CObject;

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API 				IGame_ObjectPool
{
private:
	struct str_pred : public std::binary_function<char*, char*, bool> 
	{	
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};
	typedef xr_multimap<LPCSTR,CObject*,str_pred>	POOL;
	typedef POOL::iterator							POOL_IT;

private:
	POOL						map_POOL;

public:
	void						load				( );
	void						unload				( );

	CObject*					create				( LPCSTR	name	);
	void						destroy				( CObject*	O		);

	IGame_ObjectPool			();
	virtual ~IGame_ObjectPool	();
};
//-----------------------------------------------------------------------------------------------------------
