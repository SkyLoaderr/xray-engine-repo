// Frustum.h: interface for the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRUSTUM_H__E66ED755_F741_49CF_8B2A_404CCF7067F2__INCLUDED_)
#define AFX_FRUSTUM_H__E66ED755_F741_49CF_8B2A_404CCF7067F2__INCLUDED_
#pragma once
#pragma pack(push,4)

enum EFC_Visible {
	fcvNone = 0,
	fcvPartial,
	fcvFully,
	fcv_forcedword = u32(-1)
};

#include "fixedvector.h"

#define FRUSTUM_MAXPLANES	8
#define FRUSTUM_P_LEFT		(1<<0)
#define FRUSTUM_P_RIGHT		(1<<1)
#define FRUSTUM_P_TOP		(1<<2)
#define FRUSTUM_P_BOTTOM	(1<<3)
#define FRUSTUM_P_NEAR		(1<<4)
#define FRUSTUM_P_FAR		(1<<5)

#define FRUSTUM_P_LRTB		(FRUSTUM_P_LEFT|FRUSTUM_P_RIGHT|FRUSTUM_P_TOP|FRUSTUM_P_BOTTOM)
#define FRUSTUM_P_ALL		(FRUSTUM_P_LRTB|FRUSTUM_P_NEAR|FRUSTUM_P_FAR)

#define FRUSTUM_SAFE		(FRUSTUM_MAXPLANES*4)
typedef svector<Fvector,FRUSTUM_SAFE>	sPoly;

class ENGINE_API	CFrustum
{
public:
	Fplane			planes	[FRUSTUM_MAXPLANES];
	int				p_count;

	EFC_Visible		AABB_OverlapPlane	(const Fplane& P, const Fvector &m, const Fvector &M) const;
public:
	IC void			_clear				()			{ p_count=0; }
	IC void			_add				(Fplane &P) { VERIFY(p_count<FRUSTUM_MAXPLANES); planes[p_count++].set(P);	}
	IC void			_add				(Fvector& P1, Fvector& P2, Fvector&P3)
	{
		VERIFY(p_count<FRUSTUM_MAXPLANES);
		planes[p_count++].build(P1,P2,P3);
	}

	void			SimplifyPoly_AABB	(sPoly* P, Fplane& plane);

	void			CreateOccluder		(Fvector* p, int count, Fvector& vBase, CFrustum& clip);
	BOOL			CreateFromClipPoly	(Fvector* p, int count, Fvector& vBase, CFrustum& clip);	// returns 'false' if creation failed
	void			CreateFromPoints	(Fvector* p, int count, Fvector& vBase );
	void			CreateFromMatrix	(Fmatrix &M, u32 mask);
	void			CreateFromPortal	(sPoly* P, Fvector& vBase, Fmatrix& mFullXFORM);
	void			CreateFromPlanes	(Fplane* p, int count);

	sPoly*			ClipPoly			(sPoly& src, sPoly& dest) const;

	u32				getMask				() const { return (1<<p_count)-1; }

	EFC_Visible		testSphere			(Fvector& c, float r, u32& test_mask) const;
	BOOL			testSphere_dirty	(Fvector& c, float r) const;
	EFC_Visible		testAABB			(Fvector &m, Fvector &M, u32& test_mask) const;
	EFC_Visible		testSAABB			(Fvector& c, float r, Fvector &m, Fvector &M, u32& test_mask) const;
	BOOL			testPolyInside_dirty(Fvector* p, int count) const;

	IC BOOL			testPolyInside		(sPoly& src)const
    {
    	sPoly d;
        return !!ClipPoly(src,d);
    }
   	IC BOOL			testPolyInside		(Fvector* p, int count)const
    {
    	sPoly src(p,count);
        return testPolyInside(src);
    }
};
#pragma pack(pop)

#endif // !defined(AFX_FRUSTUM_H__E66ED755_F741_49CF_8B2A_404CCF7067F2__INCLUDED_)
