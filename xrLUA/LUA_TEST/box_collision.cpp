////////////////////////////////////////////////////////////////////////////
//	Module 		: box_collision.h
//	Created 	: 29.07.2004
//  Modified 	: 29.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Box collision test
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#pragma warning(pop)

class MagicBox3 {
public:
					MagicBox3		();
					MagicBox3		(const Fmatrix &m, const Fvector &half_size);

	Fvector&		Center			();
	const Fvector&	Center			() const;

	Fvector&		Axis			(int i);
	const Fvector&	Axis			(int i) const;
	Fvector*		Axes			();
	const Fvector*	Axes			() const;

	float&			Extent			(int i);
	const float&	Extent			(int i) const;
	float*			Extents			();
	const float*	Extents			() const;

	void			ComputeVertices	(Fvector akVertex[8]) const;

	bool			intersects		(const MagicBox3 &box) const;

protected:
	Fvector m_kCenter;
	Fvector m_akAxis[3];
	float	m_afExtent[3];
};

//----------------------------------------------------------------------------
inline MagicBox3::MagicBox3 ()
{
    // no initialization for efficiency
}

inline MagicBox3::MagicBox3 (const Fmatrix &m, const Fvector &half_size)
{
	m_kCenter		= m.c;
	m_akAxis[0]		= m.i;
	m_akAxis[1]		= m.j;
	m_akAxis[2]		= m.k;
	m_afExtent[0]	= half_size.x;
	m_afExtent[1]	= half_size.y;
	m_afExtent[2]	= half_size.z;
}

//----------------------------------------------------------------------------
inline Fvector& MagicBox3::Center ()
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline const Fvector& MagicBox3::Center () const
{
    return m_kCenter;
}
//----------------------------------------------------------------------------
inline Fvector& MagicBox3::Axis (int i)
{
    VERIFY( 0 <= i && i < 3 );
    return m_akAxis[i];
}
//----------------------------------------------------------------------------
inline const Fvector& MagicBox3::Axis (int i) const
{
    VERIFY( 0 <= i && i < 3 );
    return m_akAxis[i];
}
//----------------------------------------------------------------------------
inline Fvector* MagicBox3::Axes ()
{
    return m_akAxis;
}
//----------------------------------------------------------------------------
inline const Fvector* MagicBox3::Axes () const
{
    return m_akAxis;
}
//----------------------------------------------------------------------------
inline float& MagicBox3::Extent (int i)
{
    VERIFY( 0 <= i && i < 3 );
    return m_afExtent[i];
}
//----------------------------------------------------------------------------
inline const float& MagicBox3::Extent (int i) const
{
    VERIFY( 0 <= i && i < 3 );
    return m_afExtent[i];
}
//----------------------------------------------------------------------------
inline float* MagicBox3::Extents ()
{
    return m_afExtent;
}
//----------------------------------------------------------------------------
inline const float* MagicBox3::Extents () const
{
    return m_afExtent;
}
//----------------------------------------------------------------------------

bool MagicBox3::intersects	(const MagicBox3 &rkBox1) const
{
	const MagicBox3 &rkBox0 = *this;
    // convenience variables
    const Fvector* akA = rkBox0.Axes();
    const Fvector* akB = rkBox1.Axes();
    const float* afEA = rkBox0.Extents();
    const float* afEB = rkBox1.Extents();

    // compute difference of box centers, D = C1-C0
    Fvector kD = Fvector().sub(rkBox1.Center(),rkBox0.Center());

    float aafC[3][3];     // matrix C = A^T B, c_{ij} = dotproduct(A_i,B_j)
    float aafAbsC[3][3];  // |c_{ij}|
    float afAD[3];        // dotproduct(A_i,D)
    float fR0, fR1, fR;   // interval radii and distance between centers
    float fR01;           // = R0 + R1
    
    // axis C0+t*A0
    aafC[0][0] = akA[0].dotproduct(akB[0]);
    aafC[0][1] = akA[0].dotproduct(akB[1]);
    aafC[0][2] = akA[0].dotproduct(akB[2]);
    afAD[0] = akA[0].dotproduct(kD);
    aafAbsC[0][0] = _abs(aafC[0][0]);
    aafAbsC[0][1] = _abs(aafC[0][1]);
    aafAbsC[0][2] = _abs(aafC[0][2]);
    fR = _abs(afAD[0]);
    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
    fR01 = afEA[0] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1
    aafC[1][0] = akA[1].dotproduct(akB[0]);
    aafC[1][1] = akA[1].dotproduct(akB[1]);
    aafC[1][2] = akA[1].dotproduct(akB[2]);
    afAD[1] = akA[1].dotproduct(kD);
    aafAbsC[1][0] = _abs(aafC[1][0]);
    aafAbsC[1][1] = _abs(aafC[1][1]);
    aafAbsC[1][2] = _abs(aafC[1][2]);
    fR = _abs(afAD[1]);
    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
    fR01 = afEA[1] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2
    aafC[2][0] = akA[2].dotproduct(akB[0]);
    aafC[2][1] = akA[2].dotproduct(akB[1]);
    aafC[2][2] = akA[2].dotproduct(akB[2]);
    afAD[2] = akA[2].dotproduct(kD);
    aafAbsC[2][0] = _abs(aafC[2][0]);
    aafAbsC[2][1] = _abs(aafC[2][1]);
    aafAbsC[2][2] = _abs(aafC[2][2]);
    fR = _abs(afAD[2]);
    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
    fR01 = afEA[2] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*B0
    fR = _abs(akB[0].dotproduct(kD));
    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
    fR01 = fR0 + afEB[0];
    if ( fR > fR01 )
        return false;

    // axis C0+t*B1
    fR = _abs(akB[1].dotproduct(kD));
    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
    fR01 = fR0 + afEB[1];
    if ( fR > fR01 )
        return false;

    // axis C0+t*B2
    fR = _abs(akB[2].dotproduct(kD));
    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
    fR01 = fR0 + afEB[2];
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB0
    fR = _abs(afAD[2]*aafC[1][0]-afAD[1]*aafC[2][0]);
    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB1
    fR = _abs(afAD[2]*aafC[1][1]-afAD[1]*aafC[2][1]);
    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB2
    fR = _abs(afAD[2]*aafC[1][2]-afAD[1]*aafC[2][2]);
    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB0
    fR = _abs(afAD[0]*aafC[2][0]-afAD[2]*aafC[0][0]);
    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB1
    fR = _abs(afAD[0]*aafC[2][1]-afAD[2]*aafC[0][1]);
    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB2
    fR = _abs(afAD[0]*aafC[2][2]-afAD[2]*aafC[0][2]);
    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB0
    fR = _abs(afAD[1]*aafC[0][0]-afAD[0]*aafC[1][0]);
    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB1
    fR = _abs(afAD[1]*aafC[0][1]-afAD[0]*aafC[1][1]);
    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB2
    fR = _abs(afAD[1]*aafC[0][2]-afAD[0]*aafC[1][2]);
    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    return true;
}

void box_collision_test()
{
	printf				("Box collision test!\n");
	
	Fvector				h0= Fvector().set(1.f,1.f,1.f);
	Fmatrix				m0;
	m0.identity			();
	m0.c.set			(0*2.f,2.420f,0*2.f);
	
	Fvector				h1= Fvector().set(1.f,1.f,1.f);
	Fmatrix				m1;
	m1.identity			();
	m1.setHPB			(0*PI_DIV_2,PI_DIV_4,0*PI_DIV_2);
	m1.c.set			(.0f,.0f,.0f);

	MagicBox3			box0(m0,h0);
	MagicBox3			box1(m1,h1);
	
	bool				result = box0.intersects(box1);
	printf				("%s!\n",result ? "intersects" : "nope");
}
