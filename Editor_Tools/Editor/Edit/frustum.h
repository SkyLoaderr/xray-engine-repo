// Frustum.h: interface for the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRUSTUM_H__E66ED755_F741_49CF_8B2A_404CCF7067F2__INCLUDED_)
#define AFX_FRUSTUM_H__E66ED755_F741_49CF_8B2A_404CCF7067F2__INCLUDED_
#pragma once

enum EFC_Visible {
	fcvNone = 0,
	fcvPartial,
	fcvFully,
	fcv_forcedword = DWORD(-1)
};

#define FRUSTUM_MAXPLANES	128
#define FRUSTUM_SAFE	(FRUSTUM_MAXPLANES*2)
typedef svector<Fvector,FRUSTUM_SAFE>	sPoly;

class ENGINE_API CFrustum
{
private:
	Fplane			planes[FRUSTUM_MAXPLANES];
	int				p_count;

	IC EFC_Visible	AABB_OverlapPlane(Fplane& P,Fvector &m, Fvector &M)
	{
		// calc extreme pts (neg,pos) along normal axis (pos in dir of norm, etc.)
		Fvector Neg, Pos;
		Fvector &N	= P.n;
		if(N.x>0) {
			if(N.y>0) {
				if(N.z>0)	{ Pos.set(M.x,M.y,M.z); Neg.set(m.x,m.y,m.z); }
				else		{ Pos.set(M.x,M.y,m.z); Neg.set(m.x,m.y,M.z); }
			} else {
				if(N.z>0)	{ Pos.set(M.x,m.y,M.z); Neg.set(m.x,M.y,m.z); }
				else		{ Pos.set(M.x,m.y,m.z); Neg.set(m.x,M.y,M.z); }
			}
		} else {
			if(N.y>0) {
				if(N.z>0)	{ Pos.set(m.x,M.y,M.z); Neg.set(M.x,m.y,m.z); }
				else		{ Pos.set(m.x,M.y,m.z); Neg.set(M.x,m.y,M.z); }
			} else {
				if(N.z>0)	{ Pos.set(m.x,m.y,M.z); Neg.set(M.x,M.y,m.z); }
				else		{ Pos.set(m.x,m.y,m.z); Neg.set(M.x,M.y,M.z); }
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
	IC void			_add	(const Fvector& P1, const Fvector& P2, const Fvector&P3)
	{
		VERIFY(p_count<FRUSTUM_MAXPLANES);
		planes[p_count++].build(P1,P2,P3);
	}

	void			CreateFromPoints	(Fvector* p, int count);
    void			CreateFromPlanes	(Fplane* p, int count);
	void			CreateFromViewMatrix();
	void			CreateFromMatrix	(Fmatrix &M);
	bool			CreateFromClipPoly	(Fvector* p, int count, CFrustum& clip);	// returns 'false' if creation failed

	sPoly*			ClipPoly			(sPoly& src, sPoly& dest)const;

	BYTE			getMask				() {
		return (1<<p_count)-1;
	}

	IC EFC_Visible	testPoly			(sPoly& src)const{
    	sPoly d;
        return (ClipPoly(src,d))?fcvFully:fcvNone;
    }

	IC EFC_Visible	testSphere			(Fvector& c, float r, BYTE& test_mask)
	{
		BYTE	bit = 1;
		for (int i=0; i<p_count; i++, bit<<=1)
		{
			if (test_mask&bit) {
				float cls = planes[i].classify(c);
				if (cls>r) return fcvNone;			// none  - return
				if (fabsf(cls)>=r) test_mask&=~bit;	// fully - no need to test this plane
			}
		}
		return test_mask ? fcvPartial:fcvFully;
	}
	IC EFC_Visible	testSphere			(Fvector& c, float r) const
	{
		bool bPartial=false;
		for (int i=0; i<p_count; i++)
		{
			float cls = planes[i].classify(c);
			if (cls>r) return fcvNone;			// none  - return
			if (fabsf(cls)<r) bPartial = true;	// part - no need to test this plane
		}
		return bPartial ? fcvPartial:fcvFully;
	}
	IC EFC_Visible	testAABB			(Fvector &m, Fvector &M, BYTE& test_mask)
	{
		// go for trivial rejection or acceptance using "faster overlap test"
		BYTE		bit = 1;

		for (int i=0; i<p_count; i++, bit<<=1)
		{
			if (test_mask&bit) {
				switch (AABB_OverlapPlane(planes[i],m,M))
				{
				case fcvNone:	return fcvNone;		// none - return
				case fcvFully:	test_mask&=~bit;	// fully - no need to test this plane
				}
			}
		}
		return test_mask ? fcvPartial:fcvFully;
	}
	IC EFC_Visible	testSAABB			(Fvector& c, float r, Fvector &m, Fvector &M, BYTE& test_mask)
	{
		BYTE	bit = 1;
		for (int i=0; i<p_count; i++, bit<<=1)
		{
			if (test_mask&bit) {
				float cls = planes[i].classify(c);
				if (cls>r) return fcvNone;			// none  - return
				if (fabsf(cls)>=r) test_mask&=~bit;	// fully - no need to test this plane
				else {
					switch (AABB_OverlapPlane(planes[i],m,M))
					{
					case fcvNone:	return fcvNone;		// none - return
					case fcvFully:	test_mask&=~bit;	// fully - no need to test this plane
					}
				}
			}
		}
		return test_mask ? fcvPartial:fcvFully;
	}
};
#endif // !defined(AFX_FRUSTUM_H__E66ED755_F741_49CF_8B2A_404CCF7067F2__INCLUDED_)
