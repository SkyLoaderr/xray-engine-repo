#ifndef FOBB_H
#define FOBB_H

struct	Fobb
{
protected:
    static bool		clip		(float fDenom, float fNumer, float& rfT0, float& rfT1)
    {
        // Return value is 'true' if line segment intersects the current test
        // plane.  Otherwise 'false' is returned in which case the line segment
        // is entirely clipped.

        if ( fDenom > 0.0f ){
            if ( fNumer > fDenom*rfT1 ) return false;
            if ( fNumer > fDenom*rfT0 ) rfT0 = fNumer/fDenom;
            return true;
        }else if ( fDenom < 0.0f ){
            if ( fNumer > fDenom*rfT0 ) return false;
            if ( fNumer > fDenom*rfT1 ) rfT1 = fNumer/fDenom;
            return true;
        }else{
            return fNumer <= 0.0f;
        }
    }
	static bool 	intersect	(const Fvector& start, const Fvector& dir, const Fvector& extent, float& rfT0, float& rfT1)
	{
		float fSaveT0 = rfT0, fSaveT1 = rfT1;

		bool bNotEntirelyClipped =
			clip(+dir.x,-start.x-extent[0],rfT0,rfT1) &&
			clip(-dir.x,+start.x-extent[0],rfT0,rfT1) &&
			clip(+dir.y,-start.y-extent[1],rfT0,rfT1) &&
			clip(-dir.y,+start.y-extent[1],rfT0,rfT1) &&
			clip(+dir.z,-start.z-extent[2],rfT0,rfT1) &&
			clip(-dir.z,+start.z-extent[2],rfT0,rfT1);

		return bNotEntirelyClipped && ( rfT0 != fSaveT0 || rfT1 != fSaveT1 );
	}
public:
	Fmatrix33	m_rotate;
	Fvector		m_translate;
	Fvector		m_halfsize;

	IC void		invalidate() {
		m_rotate.identity	();
		m_translate.set		(0,0,0);
		m_halfsize.set		(0,0,0);
	}
	IC void		xform_get(Fmatrix& D) const
	{
		D.i.set(m_rotate.i); D._14_ = 0;
		D.j.set(m_rotate.j); D._24_ = 0;
		D.k.set(m_rotate.k); D._34_ = 0;
		D.c.set(m_translate);D._44_ = 1;
	}
	IC void		xform_set(const Fmatrix& S)
	{
		m_rotate.i.set	(S.i);
		m_rotate.j.set	(S.j);
		m_rotate.k.set	(S.k);
		m_translate.set	(S.c);
	}
	IC void		xform_full(Fmatrix& D) const
	{
		Fmatrix		R,S;
		xform_get	(R);
		S.scale		(m_halfsize);
		D.mul_43	(R,S);
	}

	// NOTE: Unoptimized
	IC void		transform(Fobb& src, Fmatrix& M)	
	{
		Fmatrix	srcR,destR;

		src.xform_get(srcR);
		destR.mul_43(M,srcR);
		xform_set(destR);
		m_halfsize.set	(src.m_halfsize);
	}

    IC bool 	intersect(const Fvector& start, const Fvector& dir, float& dist) const
    {
        // convert ray to box coordinates
        Fvector kDiff; 
        kDiff.sub(start,m_translate);
        Fvector kOrigin;
        kOrigin.set(kDiff.dotproduct(m_rotate.i), kDiff.dotproduct(m_rotate.j), kDiff.dotproduct(m_rotate.k));
        Fvector kDirection;
        kDirection.set(dir.dotproduct(m_rotate.i),dir.dotproduct(m_rotate.j),dir.dotproduct(m_rotate.k));

        float fT0 = 0.0f, fT1 = flt_max;
        if (intersect(kOrigin,kDirection,m_halfsize, fT0,fT1)){
            bool bPick=false;
            if ( fT0 > 0.0f ){
                if (fT0<dist){ dist = fT0; bPick=true;}
                if (fT1<dist){ dist = fT1; bPick=true;}
            }else{
                if (fT1<dist){ dist = fT1; bPick=true;}
            }
            return bPick;
        }
        return false;
    }
};

#endif