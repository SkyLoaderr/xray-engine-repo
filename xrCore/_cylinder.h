#ifndef _CYLINDER_H
#define _CYLINDER_H

template <class T>
class _cylinder
{
public:
	_vector<T>	m_center;
	_vector<T>	m_direction;
	T			m_height;
	T			m_radius;
public:
	IC void		invalidate	()			{ m_center.set(0,0,0); m_direction.set(0,0,0); m_height=0; m_radius=0;}
    IC int		intersect	(const _vector<T>& start, const _vector<T>& dir, T afT[2])
    {
        T fEpsilon = 1e-12f;

        // set up quadratic Q(t) = a*t^2 + 2*b*t + c
        _vector<T> kU, kV, kW = m_direction;
        _vector<T>::generate_orthonormal_basis(kW,kU,kV);
        _vector<T> kD; kD.set(kU.dotproduct(dir),kV.dotproduct(dir),kW.dotproduct(dir));
        T fDLength = kD.normalize();
        T fInvDLength = 1.0f/fDLength;
        _vector<T> kDiff; kDiff.sub(start,m_center);
        _vector<T> kP; kP.set(kU.dotproduct(kDiff),kV.dotproduct(kDiff),kW.dotproduct(kDiff));
        T fHalfHeight = 0.5f*m_height;
        T fRadiusSqr = m_radius*m_radius;

        T fInv, fA, fB, fC, fDiscr, fRoot, fT, fT0, fT1, fTmp0, fTmp1;

        if ( _abs(kD.z) >= 1.0f - fEpsilon ){
            // line is parallel to cylinder axis
            if ( kP.x*kP.x+kP.y*kP.y <= fRadiusSqr ){
                fTmp0 = fInvDLength/kD.z;
                afT[0] = (+fHalfHeight - kP.z)*fTmp0;
                afT[1] = (-fHalfHeight - kP.z)*fTmp0;
                return 2;
            }
            else{
                return 0;
            }
        }

        if ( _abs(kD.z) <= fEpsilon ){
            // line is perpendicular to axis of cylinder
            if ( _abs(kP.z) > fHalfHeight ){
                // line is outside the planar caps of cylinder
                return 0;
            }

            fA = kD.x*kD.x + kD.y*kD.y;
            fB = kP.x*kD.x + kP.y*kD.y;
            fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
            fDiscr = fB*fB - fA*fC;
            if ( fDiscr < 0.0f ){
                // line does not intersect cylinder wall
                return 0;
            }
            else if ( fDiscr > 0.0f ){
                fRoot = _sqrt(fDiscr);
                fTmp0 = fInvDLength/fA;
                afT[0] = (-fB - fRoot)*fTmp0;
                afT[1] = (-fB + fRoot)*fTmp0;
                return 2;
            }else{
                afT[0] = -fB*fInvDLength/fA;
                return 1;
            }
        }

        // test plane intersections first
        int iQuantity = 0;
        fInv = 1.0f/kD.z;
        fT0 = (+fHalfHeight - kP.z)*fInv;
        fTmp0 = kP.x + fT0*kD.x;
        fTmp1 = kP.y + fT0*kD.y;
        if ( fTmp0*fTmp0 + fTmp1*fTmp1 <= fRadiusSqr )
            afT[iQuantity++] = fT0*fInvDLength;

        fT1 = (-fHalfHeight - kP.z)*fInv;
        fTmp0 = kP.x + fT1*kD.x;
        fTmp1 = kP.y + fT1*kD.y;
        if ( fTmp0*fTmp0 + fTmp1*fTmp1 <= fRadiusSqr )
            afT[iQuantity++] = fT1*fInvDLength;

        if ( iQuantity == 2 ){
            // line intersects both top and bottom
            return 2;
        }

        // If iQuantity == 1, then line must intersect cylinder wall
        // somewhere between caps in a single point.  This case is detected
        // in the following code that tests for intersection between line and
        // cylinder wall.

        fA = kD.x*kD.x + kD.y*kD.y;
        fB = kP.x*kD.x + kP.y*kD.y;
        fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
        fDiscr = fB*fB - fA*fC;
        if ( fDiscr < 0.0f ){
            // line does not intersect cylinder wall
            assert( iQuantity == 0 );
            return 0;
        }else if ( fDiscr > 0.0f ){
            fRoot = _sqrt(fDiscr);
            fInv = 1.0f/fA;
            fT = (-fB - fRoot)*fInv;
            if ( fT0 <= fT1 ){
                if ( fT0 <= fT && fT <= fT1 )
                    afT[iQuantity++] = fT*fInvDLength;
            }else{
                if ( fT1 <= fT && fT <= fT0 )
                    afT[iQuantity++] = fT*fInvDLength;
            }

            if ( iQuantity == 2 ){
                // Line intersects one of top/bottom of cylinder and once on
                // cylinder wall.
                return 2;
            }

            fT = (-fB + fRoot)*fInv;
            if ( fT0 <= fT1 ){
                if ( fT0 <= fT && fT <= fT1 )
                    afT[iQuantity++] = fT*fInvDLength;
            }else{
                if ( fT1 <= fT && fT <= fT0 )
                    afT[iQuantity++] = fT*fInvDLength;
            }
        }else{
            fT = -fB/fA;
            if ( fT0 <= fT1 ){
                if ( fT0 <= fT && fT <= fT1 )
                    afT[iQuantity++] = fT*fInvDLength;
            }else{
                if ( fT1 <= fT && fT <= fT0 )
                    afT[iQuantity++] = fT*fInvDLength;
            }
        }

        return iQuantity;
    }
    IC bool		intersect	(const _vector<T>& start, const _vector<T>& dir, T& dist)
    {
    	T afT[2];
        int cnt = intersect(start,dir,afT);
        bool bResult=false;
        for (int k=0; k<cnt; k++) if (afT[k]<dist){ dist = afT[k]; bResult=true; }
        return bResult;
    }
//----------------------------------------------------------------------------
};

typedef _cylinder<float>	Fcylinder;
typedef _cylinder<double>	Dcylinder;

#endif // _DEBUG
