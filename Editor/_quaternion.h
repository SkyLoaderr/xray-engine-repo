#ifndef __Q__
#define __Q__

/***************************************************************************
	The quatern module contains basic support for a quaternion object.

	quaternions are an extension of complex numbers that allows an
	expression for rotation that can be easily interpolated.  Quaternions are also
	more numericaly stable for repeated rotations than matrices.


	A quaternion is a 4 element 'vector'  [w,x,y,z] where:

	q = w + xi + yj + zk
	i*i = -1
	j*j = -1
	k*k = -1
	i*j = -j*i = k
	j*k = -k*j = i
	k*i = -i*k = j
	q' (conjugate) = w - xi - yj - zk
	||q|| (magnitude) = sqrt(q*q') = sqrt(w*w + x*x + y*y + z*z)
	unit quaternion ||q|| == 1; this implies  q' == qinverse
	quaternions are associative (q1*q2)*q3 == q1*(q2*q3)
	quaternions are not commutative  q1*q2 != q2*q1
	qinverse (inverse (1/q) ) = q'/(q*q')

	q can be expressed by w + xi + yj + zk or [w,x,y,z]
	or as in this implementation (s,v) where s=w, and v=[x,y,z]

	quaternions can represent a rotation.  The rotation is an angle t, around a
	unit vector u.   q=(s,v);  s= cos(t/2);   v= u*sin(t/2).

	quaternions can apply the rotation to a point.  let the point be p [px,py,pz],
	and let P be a quaternion(0,p).  Protated = q*P*qinverse
	( Protated = q*P*q' if q is a unit quaternion)

	concatenation rotations is similar to matrix concatenation.  given two rotations
	q1 and q2,  to rotate by q1, then q2:  let qc = (q2*q1), then the combined
	rotation is given by qc*P*qcinverse (= qc*P*qc' if q is a unit quaternion)

	multiplication:
	q1 = w1 + x1i + y1j + z1k
	q2 = w2 + x2i + y2j + z2k
	q1*q2 = q3 =
			(w1*w2 - x1*x2 - y1*y2 - z1*z2)     {w3}
	        (w1*x2 + x1*w2 + y1*z2 - z1*y2)i	{x3}
			(w1*y2 - x1*z2 + y1*w2 + z1*x2)j    {y3}
			(w1*z2 + x1*y2 + y1*x2 + z1*w2)k	{z3}

	also,
	q1 = (s1,v1) = [s1,(x1,y1,z1)]
	q2 = (s2,v2) = [s2,(x2,y2,z2)]
	q1*q2 = q3	=	(s1*s2 - dot_product(v1,v2),			{s3}
					(s1*v2 + s2*v1 + cross_product(v1,v2))	{v3}


	interpolation - it is possible (and sometimes reasonable) to interpolate between
	two quaternions by interpolating each component.  This does not quarantee a
	resulting unit quaternion, and will result in an animation with non-linear
	rotational velocity.

	spherical interpolation: (slerp) treat the quaternions as vectors
	find the angle between them (w = arccos(q1 dot q2) ).
	given 0<=t<=1,  q(t) = q1*(sin((1-t)*w)/sin(w) + q2 * sin(t*w)/sin(w).
	since q == -q, care must be taken to rotate the proper way.

	this implementation uses the notation quaternion q = (quatS,quatV)
	  where quatS is a scalar, and quatV is a 3 element vector.

***************************************************************************/

#define UNIT_TOLERANCE			0.001f
	// Quaternion magnitude must be closer than this tolerance to 1.0 to be
	// considered a unit quaternion

#define QZERO_TOLERANCE			0.00001f
	// quaternion magnitude must be farther from this tolerance to 0.0 to be
	// normalized

#define TRACE_QZERO_TOLERANCE	0.1f
	// trace of matrix must be greater than this to be used for converting a matrix
	// to a quaternion.

#define AA_QZERO_TOLERANCE		0.0001f

#define QEPSILON				0.00001f

typedef struct _quaternion {
	float x,y,z,w;

	__forceinline	void	set(float W, float X, float Y, float Z)	// don't normalize
	{	x=X; y=Y; z=Z; w=W;			}
	__forceinline	void	set(_quaternion &Q)						// don't normalize
	{	set(Q.w, Q.x, Q.y, Q.z);	}

	// multiplies q1 * q2, and places the result in *this.
	// no failure. 	renormalization not automatic
	__forceinline	void	mul(_quaternion &q1l, _quaternion &q2l)
	{
		VERIFY( q1l.isValid() );
		VERIFY( q2l.isValid() );

		w  =	(  (q1l.w*q2l.w) - (q1l.x*q2l.x)
			- (q1l.y*q2l.y) - (q1l.z*q2l.z) );

		x  =	(  (q1l.w*q2l.x) + (q1l.x*q2l.w)
			+ (q1l.y*q2l.z) - (q1l.z*q2l.y) );

		y  =	(  (q1l.w*q2l.y) - (q1l.x*q2l.z)
			+ (q1l.y*q2l.w) + (q1l.z*q2l.x) );

		z  = (  (q1l.w*q2l.z) + (q1l.x*q2l.y)
			- (q1l.y*q2l.x) + (q1l.z*q2l.w) );
	}

	// validates numerical stability
	__forceinline	const bool	isValid(void) {
		if ((w * w) < 0.0f)	return false;
		if ((x * x) < 0.0f)	return false;
		if ((y * y) < 0.0f)	return false;
		if ((z * z) < 0.0f)	return false;
		return true;
	}

	// checks for Unit-length quanternion
	__forceinline	const bool	isUnit(void) {
		float m  =  magnitude();

		if (( m < 1.0+UNIT_TOLERANCE ) && ( m > 1.0-UNIT_TOLERANCE ))
			return true;
		return false;
	}

	// normalizes Q to be a unit geQuaternion
	__forceinline	void	normalize(void) {
		float	m,one_over_magnitude;

		m =  sqrtf(magnitude());

		if (( m < QZERO_TOLERANCE ) && ( m > -QZERO_TOLERANCE ))
			return;

		one_over_magnitude = 1.0f / m;

		w *= one_over_magnitude;
		x *= one_over_magnitude;
		y *= one_over_magnitude;
		z *= one_over_magnitude;
	}

	// inversion
	__forceinline	void	inverse(_quaternion &Q)
	{	set(-Q.x,-Q.y,-Q.z,Q.w);	}

	// identity - no rotation
	__forceinline	void	identity(void)
	{	set(0.f,0.f,0.f,1.f);		}

	// length
	__forceinline	float	magnitude(void) {
		return w*w + x*x + y*y + z*z;
	}

	// makes unit rotation
	__forceinline	void	rotationYawPitchRoll(float _x, float _y, float _z) {
		float fSinYaw   = sinf(_x*.5f);
		float fCosYaw   = cosf(_x*.5f);
		float fSinPitch = sinf(_y*.5f);
		float fCosPitch = cosf(_y*.5f);
		float fSinRoll  = sinf(_z*.5f);
		float fCosRoll  = cosf(_z*.5f);

		x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
	}

	// makes unit rotation
	__forceinline	void	rotationYawPitchRoll(const _vector &ypr)
	{	rotationYawPitchRoll(ypr.x,ypr.y,ypr.z);	}

	// set a quaternion from an axis and a rotation around the axis
	__forceinline	void	rotation(_vector &axis, float angle)
	{
		float	sinTheta;

		w		= cosf(angle*0.5f);
		sinTheta= sinf(angle*0.5f);
		x = sinTheta * axis.x;
		y = sinTheta * axis.y;
		z = sinTheta * axis.z;

	}

	// gets an axis and angle of rotation around the axis from a quaternion
	// returns TRUE if there is an axis.
	// returns FALSE if there is no axis (and Axis is set to 0,0,0, and Theta is 0)
	__forceinline	bool	get_axis_angle(_vector &axis, float &angle)
	{
		float OneOverSinTheta;

		float HalfTheta  = acosf( w );
		if (HalfTheta>QZERO_TOLERANCE) 	{
			OneOverSinTheta = 1.0f / sinf( HalfTheta );
			axis.x	= OneOverSinTheta * x;
			axis.y	= OneOverSinTheta * y;
			axis.z	= OneOverSinTheta * z;
			angle	= 2.0f * HalfTheta;
			return	true;
		} else 	{
			axis.x	= axis.y = axis.z = 0.0f;
			angle	= 0.0f;
			return	false;
		}
	}

	// spherical interpolation between q0 and q1.   0<=t<=1
	// resulting quaternion is 'between' q0 and q1
	// with t==0 being all q0, and t==1 being all q1.
	// returns a quaternion with a positive W - always takes shortest route
	// through the positive W domain.
	__forceinline	void	slerp(_quaternion &Q0, _quaternion &Q1, float T)
	{
		float omega,cosom,sinom,Scale0,Scale1;

		_quaternion QL;
		VERIFY( ( 0 <= T ) && ( T <= 1.0f ) );
		VERIFY( Q0.isUnit() );
		VERIFY( Q1.isUnit() );

		cosom =	(Q0.w * Q1.w) + (Q0.x * Q1.x) + (Q0.y * Q1.y) + (Q0.z * Q1.z);

		if (cosom < 0) 	{
			cosom = -cosom;
			QL.x = -Q1.x;
			QL.y = -Q1.y;
			QL.z = -Q1.z;
			QL.w = -Q1.w;
		} else {
			QL.set(Q1);
		}

		if ( (1.0f - cosom) > QEPSILON ) {
			omega  = acosf( cosom );
			sinom  = sinf( omega );
			Scale0 = sinf( (1.0f-T) * omega) / sinom;
			Scale1 = sinf( T*omega) / sinom;
		} else  {
			// has numerical difficulties around cosom == 0
			// in this case degenerate to linear interpolation
			Scale0 = 1.0f - T;
			Scale1 = T;
		}
		x = Scale0 * Q0.x + Scale1 * QL.x;
		y = Scale0 * Q0.y + Scale1 * QL.y;
		z = Scale0 * Q0.z + Scale1 * QL.z;
		w = Scale0 * Q0.w + Scale1 * QL.w;
	}

	// return TRUE if quaternions differ elementwise by less than Tolerance.
	__forceinline	bool	cmp(_quaternion &Q, float Tolerance=0.0001f)
	{
		if (	// they are the same but with opposite signs
			(	(fabsf(x + Q.x) <= Tolerance )
			&&  (fabsf(y + Q.y) <= Tolerance )
			&&  (fabsf(z + Q.z) <= Tolerance )
			&&  (fabsf(w + Q.w) <= Tolerance )
			)
			||  // they are the same with same signs
			(	(fabsf(x - Q.x) <= Tolerance )
			&&  (fabsf(y - Q.y) <= Tolerance )
			&&  (fabsf(z - Q.z) <= Tolerance )
			&&  (fabsf(w - Q.w) <= Tolerance )
			)
			)
			return true;
		else
			return false;
	}
} Fquaternion;

#undef UNIT_TOLERANCE
#undef QZERO_TOLERANCE
#undef TRACE_QZERO_TOLERANCE
#undef AA_QZERO_TOLERANCE
#undef QEPSILON

#endif
