// Tester.h: interface for the CTester class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTER_H__CEE21355_FD23_403A_BDD6_6A93FAA10A11__INCLUDED_)
#define AFX_TESTER_H__CEE21355_FD23_403A_BDD6_6A93FAA10A11__INCLUDED_
#pragma once

#include "fvisualcontainer.h"
//--------------------------------------------------------------------
#pragma pack(push,1)
struct _VIS_HEADER2 {
	WORD	wFormatVersion;	// == sizeof(_VIS_HEADER)
	WORD	wLayers;		// 1..?
	WORD	wSizeX,wSizeZ;	// map size in subdivision 
	float	fSize;			// size of one subdiv.
};
struct _VIS_SLOT2 {			// number of slots equals to wLayers
	DWORD	dwPtr;			// offset in vis data file
	DWORD	dwCnt;			// number of visuals in the list
};
#pragma pack(pop)
//--------------------------------------------------------------------
#define V_MAXSIZE		512		// maximal map size

class	FBasicVisual;
typedef vector<WORD>	v_list;

class CTester  
{
	friend	bool __fastcall v_sort(WORD a, WORD b);
	friend	bool __fastcall va_sort(WORD a, WORD b);

	// input
	FVisualContainer		VC;
	vector<FBasicVisual*>	VTest;
	vector<FBasicVisual*>	VInvalidated;
	_VIS_HEADER2			vHeader;
	_VIS_SLOT2				*pMap;
	WORD					*pVis;
	int						GetSlot(Fvector &pos, WORD **);

	// work
	int						cam;
	int						vX,vZ;
	int						fv_num;		// only for full-volume testing
	Fvector					vFrom;
	bool					visible[V_MAXCOUNT];
	WORD*					slot_indices;
	int						slot_count;

	// output
	FILE					*hVis,*hMap;
	DWORD					dwLayer;
	v_list					vis;
	void					NextCamera	();
	void					ChangeSlot	();
	void					NewSlot		();
	void					BuildCTable	();
	DWORD					CTable[V_MAXCOUNT];
public:
	bool					bDone;

	void	LoadVisibility		();
	void	LoadVisuals			();

	void	AddVTest			(FBasicVisual* pV);
	void	AnalyzeVisibility	();

	void	ExpandVisuals		();

	void	StartClipper		();

	float	GetProgress			();
	void	OnRender			();
	void	OnSurfaceReady		();

	void	SaveVisibility		();

	CTester();
	~CTester();
};

extern CTester*		pTester;

#endif // !defined(AFX_TESTER_H__CEE21355_FD23_403A_BDD6_6A93FAA10A11__INCLUDED_)
