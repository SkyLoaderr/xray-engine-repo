// ModelPool.h: interface for the CModelPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
#define AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_
#pragma once

class ENGINE_API FBasicVisual;

class ENGINE_API CModelPool
{
	friend class CRender;

	struct ModelDef
	{
		char			name[64];
		FBasicVisual*	model;
	};

	vector<ModelDef>	Models;

	void				OnDestroy			();
public:
	FBasicVisual*		Instance_Create		(DWORD Type);
	FBasicVisual*		Instance_Duplicate	(FBasicVisual* V);
	FBasicVisual*		Instance_Load		(const char* N);
	FBasicVisual*		Instance_Load		(CStream* data);

	void				OnDeviceDestroy		();

	FBasicVisual*		Create				(const char* name);
	FBasicVisual*		Create				(CStream* data);
	void				Delete				(FBasicVisual* &V);

	CModelPool			();
	~CModelPool			();
};

#endif // !defined(AFX_MODELPOOL_H__F4AB6697_66F2_45A2_A501_4E791B8C0A3E__INCLUDED_)
