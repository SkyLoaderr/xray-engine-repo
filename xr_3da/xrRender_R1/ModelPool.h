// ModelPool.h: interface for the CModelPool class.
//////////////////////////////////////////////////////////////////////
#pragma once

// refs
class ENGINE_API IVisual;
namespace PS	{ 
	struct ENGINE_API SDef_RT;
	struct ENGINE_API SEmitter; 
};

// defs
class CModelPool
{
private:
	friend class CRender;

	struct str_pred : public std::binary_function<char*, char*, bool> 
	{	
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};
	struct ModelDef
	{
		string128		name;
		IVisual*		model;
	};

	typedef xr_multimap<LPCSTR,IVisual*,str_pred>	POOL;
	typedef POOL::iterator							POOL_IT;
	typedef xr_map<IVisual*,LPCSTR>					REGISTRY;
	typedef REGISTRY::iterator						REGISTRY_IT;
private:
	xr_vector<ModelDef>	Models;				// Reference / Base
	REGISTRY			Registry;			// Just pairing of pointer / Name
	POOL				Pool;				// Unused / Inactive

	void				Destroy	();
public:
	IVisual*			Instance_Create		(u32 Type);
	IVisual*			Instance_Duplicate	(IVisual* V);
	IVisual*			Instance_Load		(LPCSTR N);
	IVisual*			Instance_Load		(LPCSTR N, IReader* data);
	void				Instance_Register	(LPCSTR N, IVisual* V);
	IVisual*			Instance_Find		(LPCSTR N);

	IVisual*			CreatePS			(PS::SDef* source, PS::SEmitter* E);
	IVisual*			CreatePG			(PS::CPGDef* source);
	IVisual*			Create				(LPCSTR name);
	IVisual*			Create				(LPCSTR name, IReader* data);
	void				Delete				(IVisual* &V);

	CModelPool			();
	virtual ~CModelPool	();
};
