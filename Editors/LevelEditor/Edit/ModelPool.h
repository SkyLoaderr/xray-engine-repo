// ModelPool.h: interface for the CModelPool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ModelPoolH
#define ModelPoolH
#pragma once

class ENGINE_API IRender_Visual;

namespace PS{
	class CPEDef;
	class CPGDef;
};

class ENGINE_API CModelPool
{
	friend class CRender;

	struct ModelDef
	{
		char			name[64];
		IRender_Visual*	model;
	};

	xr_vector<ModelDef>	Models;

	void				OnDestroy			();
public:
                        CModelPool			();
                        ~CModelPool			();
	IRender_Visual*	 	Instance_Create		(u32 Type);
	IRender_Visual*	 	Instance_Duplicate	(IRender_Visual* V);
	IRender_Visual*	 	Instance_Load		(const char* N);
	IRender_Visual*	 	Instance_Load		(IReader* data);

	void				OnDeviceDestroy		();

	IRender_Visual*	 	Create				(const char* name);
	IRender_Visual*	 	Create				(IReader* data);
	void				Delete				(IRender_Visual* &V);

	void 				Render				(IRender_Visual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD=1.f);
	void 				RenderSingle		(IRender_Visual* m_pVisual, const Fmatrix& mTransform, float m_fLOD=1.f);

	IRender_Visual* 	CreatePE			(PS::CPEDef* source);
	IRender_Visual* 	CreatePG			(PS::CPGDef* source);
};

#endif
