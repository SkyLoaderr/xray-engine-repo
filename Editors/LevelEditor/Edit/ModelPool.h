// ModelPool.h: interface for the CModelPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
#define AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_
#pragma once

class ENGINE_API CVisual;

class ENGINE_API CModelPool
{
	friend class CRender;

	struct ModelDef
	{
		char			name[64];
		CVisual*		model;
	};

	vector<ModelDef>	Models;

	void				OnDestroy			();
public:
	CVisual*			Instance_Create		(DWORD Type);
	CVisual*			Instance_Duplicate	(CVisual* V);
	CVisual*			Instance_Load		(const char* N);
	CVisual*			Instance_Load		(IReader* data);

	void				OnDeviceDestroy		();

	CVisual*			Create				(const char* name);
	CVisual*			Create				(IReader* data);
	void				Delete				(CVisual* &V);

	CModelPool			();
	~CModelPool			();
};

#endif // !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
