// ModelPool.h: interface for the CModelPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
#define AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_
#pragma once

class ENGINE_API IVisual;

class ENGINE_API CModelPool
{
	friend class CRender;

	struct ModelDef
	{
		char			name[64];
		IVisual*		model;
	};

	xr_vector<ModelDef>	Models;

	void				OnDestroy			();
public:
                        CModelPool			();
                        ~CModelPool			();
	IVisual*			Instance_Create		(u32 Type);
	IVisual*			Instance_Duplicate	(IVisual* V);
	IVisual*			Instance_Load		(const char* N);
	IVisual*			Instance_Load		(IReader* data);

	void				OnDeviceDestroy		();

	IVisual*			Create				(const char* name);
	IVisual*			Create				(IReader* data);
	void				Delete				(IVisual* &V);

	void 				Render				(IVisual* m_pVisual, const Fmatrix& mTransform, int priority, bool strictB2F, float m_fLOD=1.f);
};

#endif // !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
