#pragma once

// refs
class ENGINE_API				CObject;

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API 				IGame_ObjectPool
{
private:
	struct str_pred : public std::binary_function<ref_str&, ref_str&, bool> 
	{	
		IC bool operator()(const ref_str& x, const ref_str& y) const
		{	return xr_strcmp(x,y)<0;	}
	};
	typedef xr_multimap<ref_str,CObject*,str_pred>	POOL;
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
