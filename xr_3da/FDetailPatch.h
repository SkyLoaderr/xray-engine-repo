// FDetailPatch.h: interface for the FDetailPatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FDETAILPATCH_H__08A1E8B6_2C7D_407F_B6A5_60BF935C5860__INCLUDED_)
#define AFX_FDETAILPATCH_H__08A1E8B6_2C7D_407F_B6A5_60BF935C5860__INCLUDED_
#pragma once

#include "FBasicVisual.h"
#include "VirtualVector.h"

#pragma pack(push,4)
struct DPatch {
	Fvector P;
	Fvector	N;
	float	S;
	DWORD	C;
};
#pragma pack(pop)

class FDetailPatch : public FBasicVisual  
{
	CVertexStream*	Stream;
	vector<DPatch>	patches;
public:
	virtual void	Render		(float LOD);
	virtual void	Load		(const char* N, CStream *data, DWORD dwFlags);

	FDetailPatch();
	virtual ~FDetailPatch();
};

#endif // !defined(AFX_FDETAILPATCH_H__08A1E8B6_2C7D_407F_B6A5_60BF935C5860__INCLUDED_)
