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

#define FRUSTUM_SAFE	(FRUSTUM_MAXPLANES*4)
typedef svector<Fvector,FRUSTUM_SAFE>	sPoly;

class ENGINE_API CFrustum  
{
public:
	Fplane			planes	[FRUSTUM_MAXPLANES];
	int				p_count;

	IC EFC_Visible	AABB_OverlapPlane(const Fplane& P, const Fvector &m, const Fvector &M) const
	{
		// calc extreme pts (neg,pos) along normal axis (pos in dir of norm, etc.)
		const Fvector &N	= P.n;
		Fvector Neg, Pos;
		if(positive(N.x)) {
			if(positive(N.y)) { 
				if(positive(N.z))	{ Pos.set(M.x,M.y,M.z); Neg.set(m.x,m.y,m.z); }
				else				{ Pos.set(M.x,M.y,m.z); Neg.set(m.x,m.y,M.z); } 
			} else { 
				if(positive(N.z))	{ Pos.set(M.x,m.y,M.z); Neg.set(m.x,M.y,m.z); }
				else				{ Pos.set(M.x,m.y,m.z); Neg.set(m.x,M.y,M.z); } 
			}
		} else {
			if(positive(N.y)) { 
				if(positive(N.z))	{ Pos.set(m.x,M.y,M.z); Neg.set(M.x,m.y,m.z); }
				else				{ Pos.set(m.x,M.y,m.z); Neg.set(M.x,m.y,M.z); } 
			} else { 
				if(positive(N.z))	{ Pos.set(m.x,m.y,M.z); Neg.set(M.x,M.y,m.z); }
				else				{ Pos.set(m.x,m.y,m.z); Neg.set(M.x,M.y,M.z); } 
			}
		}

		// check distance to plane from extremal points to determine overlap
		if (P.classify(Neg) > 0) return	fcvNone;
		else if (P.classify(Pos) <= 0) return(fcvFully);
		else return fcvPartial;
	}

public:
	IC void			_clear	()			{ p_count=0; }
	IC void			_add	(Fplane &P) { VERIFY(p_count<FRUSTUM_MAXPLANES); planes[p_count++].set(P);	}
	IC void			_add	(Fvector& P1, Fvector& P2, Fvector&P3) 
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

	IC EFC_Visible	testSphere			(Fvector& c, float r, u32& test_mask) const
	{
		u32	bit = 1;
		for (int i=0; i<p_count; i++, bit<<=1)
		{
			if (test_mask&bit) {
				float cls = planes[i].classify(c);
				if (cls>r) { test_mask=0; return fcvNone;}	// none  - return
				if (_abs(cls)>=r) test_mask&=~bit;			// fully - no need to test this plane
			}
		}
		return test_mask ? fcvPartial:fcvFully;
	}
	IC BOOL			testSphere_dirty		(Fvector& c, float r) const
	{
		switch (p_count) {
		case 8:	if (planes[7].classify(c)>r)	return FALSE;
		case 7:	if (planes[6].classify(c)>r)	return FALSE;
		case 6:	if (planes[5].classify(c)>r)	return FALSE;
		case 5:	if (planes[4].classify(c)>r)	return FALSE;
		case 4:	if (planes[3].classify(c)>r)	return FALSE;
		case 3:	if (planes[2].classify(c)>r)	return FALSE;
		case 2:	if (planes[1].classify(c)>r)	return FALSE;
		case 1:	if (planes[0].classify(c)>r)	return FALSE;
		case 0:	break;
		default:	NODEFAULT;
		}
		return TRUE;
	}
	IC EFC_Visible	testAABB			(Fvector &m, Fvector &M, u32& test_mask) const
	{
		// go for trivial rejection or acceptance using "faster overlap test"
		u32		bit = 1;

		for (int i=0; i<p_count; i++, bit<<=1)
		{
			if (test_mask&bit) {
				switch (AABB_OverlapPlane(planes[i],m,M))
				{
				case fcvNone:	{ test_mask=0; return fcvNone;	}	// none - return
				case fcvFully:	test_mask&=~bit;					// fully - no need to test this plane
				}
			}
		}
		return test_mask ? fcvPartial:fcvFully;
	}
	IC EFC_Visible	testSAABB			(Fvector& c, float r, Fvector &m, Fvector &M, u32& test_mask) const
	{
		u32	bit = 1;
		for (int i=0; i<p_count; i++, bit<<=1)
		{
			if (test_mask&bit) {
				float cls = planes[i].classify(c);
				if (cls>r) { test_mask=0; return fcvNone;}	// none  - return
				if (_abs(cls)>=r) test_mask&=~bit;			// fully - no need to test this plane
				else {
					switch (AABB_OverlapPlane(planes[i],m,M))
					{
					case fcvNone:	{ test_mask=0; return fcvNone;};	// none - return
					case fcvFully:	test_mask&=~bit;					// fully - no need to test this plane
					}
				}
			}
		}
		return test_mask ? fcvPartial:fcvFully;
	}
	IC BOOL			testPolyInside_dirty(Fvector* p, int count) const
	{
		Fvector* e = p+count;
		for (int i=0; i<p_count; i++)
		{
			const Fplane &P = planes[i];
			for (Fvector* I=p; I!=e; I++)
				if (P.classify(*I)>0) return false;
		}
		return true;
	}
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
