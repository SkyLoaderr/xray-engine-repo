// ModelPool.h: interface for the CModelPool class.
//////////////////////////////////////////////////////////////////////
#ifndef ModelPoolH
#define ModelPoolH
#pragma once

// refs
class ENGINE_API IRender_Visual;
namespace PS	{ 
	struct ENGINE_API SEmitter; 
	struct ENGINE_API CPEDef; 
    struct ENGINE_API CPGDef;
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
		string128			name;
		IRender_Visual*		model;
        u32					refs;
        ModelDef(){name[0]=0;refs=0;model=0;}
	};

	typedef xr_multimap<LPCSTR,IRender_Visual*,str_pred>	POOL;
	typedef POOL::iterator							POOL_IT;
	typedef xr_map<IRender_Visual*,LPCSTR>			REGISTRY;
	typedef REGISTRY::iterator						REGISTRY_IT;
private:
	xr_vector<ModelDef>		Models;				// Reference / Base
	REGISTRY				Registry;			// Just pairing of pointer / Name
	POOL					Pool;				// Unused / Inactive
	BOOL					bLogging;

	void					Destroy	();
public:
                            CModelPool			();
	virtual 				~CModelPool			();
	IRender_Visual*			Instance_Create		(u32 Type);
	IRender_Visual*			Instance_Duplicate	(IRender_Visual* V);
	IRender_Visual*			Instance_Load		(LPCSTR N);
	IRender_Visual*			Instance_Load		(LPCSTR N, IReader* data);
	void					Instance_Register	(LPCSTR N, IRender_Visual* V);
	IRender_Visual*			Instance_Find		(LPCSTR N);

	IRender_Visual*			CreatePE			(PS::CPEDef* source);
	IRender_Visual*			CreatePG			(PS::CPGDef* source);
	IRender_Visual*			Create				(LPCSTR name);
	IRender_Visual*			Create				(LPCSTR name, IReader* data);
	void					Delete				(IRender_Visual* &V, BOOL bDiscard=FALSE);
	void					Discard				(IRender_Visual* &V);

	void					Logging				(BOOL bEnable)	{ bLogging=bEnable; }

#ifdef _EDITOR    
	void					OnDeviceDestroy		();
	void 					Render				(IRender_Visual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD);
	void 					RenderSingle		(IRender_Visual* m_pVisual, const Fmatrix& mTransform, float m_fLOD);
#endif
};
#endif //ModelPoolH
