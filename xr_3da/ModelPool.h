// ModelPool.h: interface for the CModelPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
#define AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_
#pragma once

// refs
class ENGINE_API CVisual;
namespace PS	{ 
	struct ENGINE_API SDef_RT;
	struct ENGINE_API SEmitter; 
};

// defs
class ENGINE_API CModelPool : public pureDeviceDestroy
{
	friend class CRender;

	struct ModelDef
	{
		string128		name;
		CVisual*		model;
	};

	vector<ModelDef>	Models;

	void				Destroy	();
public:
	CVisual*			Instance_Create		(u32 Type);
	CVisual*			Instance_Duplicate	(CVisual* V);
	CVisual*			Instance_Load		(LPCSTR N);
	CVisual*			Instance_Load		(LPCSTR N, CStream* data);
	void				Instance_Register	(LPCSTR N, CVisual* V);
	CVisual*			Instance_Find		(LPCSTR N);

	virtual void		OnDeviceDestroy		();

	CVisual*			CreatePS			(PS::SDef_RT* source, PS::SEmitter* E);
	CVisual*			Create				(LPCSTR name);
	CVisual*			Create				(LPCSTR name, CStream* data);
	void				Delete				(CVisual* &V);

	CModelPool			();
	virtual ~CModelPool	();
};

#endif // !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
