// ModelPool.h: interface for the CModelPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
#define AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_
#pragma once

// refs
class ENGINE_API IVisual;
namespace PS	{ 
	struct ENGINE_API SDef_RT;
	struct ENGINE_API SEmitter; 
};

// defs
class ENGINE_API CModelPool
{
	friend class CRender;

	struct ModelDef
	{
		string128		name;
		IVisual*		model;
	};

	vector<ModelDef>	Models;

	void				Destroy	();
public:
	IVisual*			Instance_Create		(u32 Type);
	IVisual*			Instance_Duplicate	(IVisual* V);
	IVisual*			Instance_Load		(LPCSTR N);
	IVisual*			Instance_Load		(LPCSTR N, IReader* data);
	void				Instance_Register	(LPCSTR N, IVisual* V);
	IVisual*			Instance_Find		(LPCSTR N);

	IVisual*			CreatePS			(PS::SDef_RT* source, PS::SEmitter* E);
	IVisual*			Create				(LPCSTR name);
	IVisual*			Create				(LPCSTR name, IReader* data);
	void				Delete				(IVisual* &V);

	CModelPool			();
	virtual ~CModelPool	();
};

#endif // !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
