// FVisualContainer.h: interface for the FVisualContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FVISUALCONTAINER_H__6DB33481_9D03_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FVISUALCONTAINER_H__6DB33481_9D03_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

class ENGINE_API FBasicVisual;

class ENGINE_API FVisualContainer
{
public:
	vector < FBasicVisual*>	Items;

	FBasicVisual*			Get(char *name);

	__forceinline FBasicVisual*	Get(int idx) { VERIFY(idx<Items.size()); VERIFY(idx>=0); return Items[idx]; };

	void					PerformLighting();
	void					PerformOptimize();

	// Constructor/destructor/loader
	FVisualContainer();
	~FVisualContainer();
	void Load(void);
};

extern FBasicVisual*	CreateVisualByType(DWORD type);


#endif // !defined(AFX_FVISUALCONTAINER_H__6DB33481_9D03_11D3_B4E3_4854E82A090D__INCLUDED_)
