// FStaticVisibility.h: interface for the FStaticVisibility class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSTATICVISIBILITY_H__CC9820C2_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FSTATICVISIBILITY_H__CC9820C2_A341_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

struct V_Header {
	union {
		struct { DWORD nX,nY,nZ;};
		struct { DWORD N[3];	};
	};
	float	relevance;
	Fvector start;
};

class CStream;

class FStaticVisibility
{
	V_Header			vHeader;
	DWORD				*pMap;
	DWORD				*pNodes;
	DWORD				*pLights;
	DWORD				*pGlows;
	DWORD				*pOccluders;
public:
	DWORD				RelevantGeometry	(Fvector &pos, WORD **G);
	DWORD				RelevantLights		(Fvector &pos, WORD **L);
	DWORD				RelevantGlows		(Fvector &pos, WORD **G);
	DWORD				RelevantOccluders	(Fvector &pos, WORD **O);

	// Constructor/destructor/loader
	void				Load(CStream *fs);
	FStaticVisibility	();
	~FStaticVisibility	();
};

#endif // !defined(AFX_FSTATICVISIBILITY_H__CC9820C2_A341_11D3_B4E3_4854E82A090D__INCLUDED_)
