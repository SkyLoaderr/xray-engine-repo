#ifndef __M__
#define __M__

extern ENGINE_API _matrix precalc_identity;
typedef struct _matrix {
public:
	union {
		struct {						// Direct definition
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
		};
    	struct{
    		Fvector i; float _14_;
    		Fvector j; float _24_;
    		Fvector k; float _34_;
    		Fvector c; float _44_;
        };
		float m[4][4];					// Array
	};

	IC	D3DMATRIX	*d3d(void) const { return (D3DMATRIX *)this; };
	// Class members
	IC void set(const _matrix &a) {
		CopyMemory(this,&a,16*sizeof(float));
	}
	IC void set(const Fvector& R,const Fvector& N,const Fvector& D,const Fvector& C) {
		i.set(R); _14_=0;
		j.set(N); _24_=0;
		k.set(D); _34_=0;
		c.set(C); _44_=1;
	}
	IC void identity(void) {
		_11=1; _12=0; _13=0; _14=0;
		_21=0; _22=1; _23=0; _24=0;
		_31=0; _32=0; _33=1; _34=0;
		_41=0; _42=0; _43=0; _44=1;
	}
	IC void get_rapid(_matrix33& R) const;
	IC void get_translate(Fvector& T)const{
        T.x = m[3][0];
        T.y = m[3][1];
        T.z = m[3][2];
	}
	IC void rotation(const _quaternion &Q) {
		float xx = Q.x*Q.x; float yy = Q.y*Q.y; float zz = Q.z*Q.z;
		float xy = Q.x*Q.y; float xz = Q.x*Q.z; float yz = Q.y*Q.z;
		float wx = Q.w*Q.x; float wy = Q.w*Q.y; float wz = Q.w*Q.z;

		_11 = 1 - 2 * ( yy + zz );	_12 =     2 * ( xy - wz );	_13 =     2 * ( xz + wy );	_14 = 0;
		_21 =     2 * ( xy + wz );	_22 = 1 - 2 * ( xx + zz );	_23 =     2 * ( yz - wx );	_24 = 0;
		_31 =     2 * ( xz - wy );	_32 =     2 * ( yz + wx );	_33 = 1 - 2 * ( xx + yy );	_34 = 0;
		_41 = 0;					_42 = 0;					_43 = 0;					_44 = 1;
	}
	IC void mk_xform(const _quaternion &Q, const Fvector &T) {
		float xx = Q.x*Q.x; float yy = Q.y*Q.y; float zz = Q.z*Q.z;
		float xy = Q.x*Q.y; float xz = Q.x*Q.z; float yz = Q.y*Q.z;
		float wx = Q.w*Q.x; float wy = Q.w*Q.y; float wz = Q.w*Q.z;

		_11 = 1 - 2 * ( yy + zz );	_12 =     2 * ( xy - wz );	_13 =     2 * ( xz + wy );	_14 = 0;
		_21 =     2 * ( xy + wz );	_22 = 1 - 2 * ( xx + zz );	_23 =     2 * ( yz - wx );	_24 = 0;
		_31 =     2 * ( xz - wy );	_32 =     2 * ( yz + wx );	_33 = 1 - 2 * ( xx + yy );	_34 = 0;
		_41 = T.x;					_42 = T.y;					_43 = T.z;					_44 = 1;
	}

	// Multiply RES = A[4x4]*B[4x4] (WITH projection)
	IC void mul	(const _matrix &A,const _matrix &B)
	{
		m[0][0] = A.m[0][0] * B.m[0][0] + A.m[1][0] * B.m[0][1] + A.m[2][0] * B.m[0][2] + A.m[3][0] * B.m[0][3];
		m[0][1] = A.m[0][1] * B.m[0][0] + A.m[1][1] * B.m[0][1] + A.m[2][1] * B.m[0][2] + A.m[3][1] * B.m[0][3];
		m[0][2] = A.m[0][2] * B.m[0][0] + A.m[1][2] * B.m[0][1] + A.m[2][2] * B.m[0][2] + A.m[3][2] * B.m[0][3];
		m[0][3] = A.m[0][3] * B.m[0][0] + A.m[1][3] * B.m[0][1] + A.m[2][3] * B.m[0][2] + A.m[3][3] * B.m[0][3];

		m[1][0] = A.m[0][0] * B.m[1][0] + A.m[1][0] * B.m[1][1] + A.m[2][0] * B.m[1][2] + A.m[3][0] * B.m[1][3];
		m[1][1] = A.m[0][1] * B.m[1][0] + A.m[1][1] * B.m[1][1] + A.m[2][1] * B.m[1][2] + A.m[3][1] * B.m[1][3];
		m[1][2] = A.m[0][2] * B.m[1][0] + A.m[1][2] * B.m[1][1] + A.m[2][2] * B.m[1][2] + A.m[3][2] * B.m[1][3];
		m[1][3] = A.m[0][3] * B.m[1][0] + A.m[1][3] * B.m[1][1] + A.m[2][3] * B.m[1][2] + A.m[3][3] * B.m[1][3];

		m[2][0] = A.m[0][0] * B.m[2][0] + A.m[1][0] * B.m[2][1] + A.m[2][0] * B.m[2][2] + A.m[3][0] * B.m[2][3];
		m[2][1] = A.m[0][1] * B.m[2][0] + A.m[1][1] * B.m[2][1] + A.m[2][1] * B.m[2][2] + A.m[3][1] * B.m[2][3];
		m[2][2] = A.m[0][2] * B.m[2][0] + A.m[1][2] * B.m[2][1] + A.m[2][2] * B.m[2][2] + A.m[3][2] * B.m[2][3];
		m[2][3] = A.m[0][3] * B.m[2][0] + A.m[1][3] * B.m[2][1] + A.m[2][3] * B.m[2][2] + A.m[3][3] * B.m[2][3];

		m[3][0] = A.m[0][0] * B.m[3][0] + A.m[1][0] * B.m[3][1] + A.m[2][0] * B.m[3][2] + A.m[3][0] * B.m[3][3];
		m[3][1] = A.m[0][1] * B.m[3][0] + A.m[1][1] * B.m[3][1] + A.m[2][1] * B.m[3][2] + A.m[3][1] * B.m[3][3];
		m[3][2] = A.m[0][2] * B.m[3][0] + A.m[1][2] * B.m[3][1] + A.m[2][2] * B.m[3][2] + A.m[3][2] * B.m[3][3];
		m[3][3] = A.m[0][3] * B.m[3][0] + A.m[1][3] * B.m[3][1] + A.m[2][3] * B.m[3][2] + A.m[3][3] * B.m[3][3];
	}

	// Multiply RES = A[4x3]*B[4x3] (no projection), faster than ordinary multiply
	IC void mul_43(const _matrix &A,const _matrix &B)
	{
		m[0][0] = A.m[0][0] * B.m[0][0] + A.m[1][0] * B.m[0][1] + A.m[2][0] * B.m[0][2];
		m[0][1] = A.m[0][1] * B.m[0][0] + A.m[1][1] * B.m[0][1] + A.m[2][1] * B.m[0][2];
		m[0][2] = A.m[0][2] * B.m[0][0] + A.m[1][2] * B.m[0][1] + A.m[2][2] * B.m[0][2];
		m[0][3] = 0;

		m[1][0] = A.m[0][0] * B.m[1][0] + A.m[1][0] * B.m[1][1] + A.m[2][0] * B.m[1][2];
		m[1][1] = A.m[0][1] * B.m[1][0] + A.m[1][1] * B.m[1][1] + A.m[2][1] * B.m[1][2];
		m[1][2] = A.m[0][2] * B.m[1][0] + A.m[1][2] * B.m[1][1] + A.m[2][2] * B.m[1][2];
		m[1][3] = 0;

		m[2][0] = A.m[0][0] * B.m[2][0] + A.m[1][0] * B.m[2][1] + A.m[2][0] * B.m[2][2];
		m[2][1] = A.m[0][1] * B.m[2][0] + A.m[1][1] * B.m[2][1] + A.m[2][1] * B.m[2][2];
		m[2][2] = A.m[0][2] * B.m[2][0] + A.m[1][2] * B.m[2][1] + A.m[2][2] * B.m[2][2];
		m[2][3] = 0;

		m[3][0] = A.m[0][0] * B.m[3][0] + A.m[1][0] * B.m[3][1] + A.m[2][0] * B.m[3][2] + A.m[3][0];
		m[3][1] = A.m[0][1] * B.m[3][0] + A.m[1][1] * B.m[3][1] + A.m[2][1] * B.m[3][2] + A.m[3][1];
		m[3][2] = A.m[0][2] * B.m[3][0] + A.m[1][2] * B.m[3][1] + A.m[2][2] * B.m[3][2] + A.m[3][2];
		m[3][3] = 1;
	}
	IC	void mul2	( const _matrix& A )
	{
    	_matrix B; B.set( *this );
    	mul( A, B );
    };
	IC	void mul	( const _matrix& B )
	{
		_matrix	A; A.set( *this );
    	mul( A, B );
	};
	IC	void mul2_43( const _matrix& A )
	{
    	_matrix B; B.set( *this );
    	mul_43( A, B );
    };
	IC	void mul_43	( const _matrix& B )
	{
		_matrix	A; A.set( *this );
    	mul_43( A, B );
	};
	IC	void	invert(const _matrix & a ) 
	{	
		// faster than self-invert
		float fDetInv = ( a._11 * ( a._22 * a._33 - a._23 * a._32 ) -
			a._12 * ( a._21 * a._33 - a._23 * a._31 ) +
			a._13 * ( a._21 * a._32 - a._22 * a._31 ) );

		VERIFY(fabsf(fDetInv)>flt_zero);
		fDetInv=1.0f/fDetInv;

		_11 =  fDetInv * ( a._22 * a._33 - a._23 * a._32 );
		_12 = -fDetInv * ( a._12 * a._33 - a._13 * a._32 );
		_13 =  fDetInv * ( a._12 * a._23 - a._13 * a._22 );
		_14 = 0.0f;

		_21 = -fDetInv * ( a._21 * a._33 - a._23 * a._31 );
		_22 =  fDetInv * ( a._11 * a._33 - a._13 * a._31 );
		_23 = -fDetInv * ( a._11 * a._23 - a._13 * a._21 );
		_24 = 0.0f;

		_31 =  fDetInv * ( a._21 * a._32 - a._22 * a._31 );
		_32 = -fDetInv * ( a._11 * a._32 - a._12 * a._31 );
		_33 =  fDetInv * ( a._11 * a._22 - a._12 * a._21 );
		_34 = 0.0f;

		_41 = -( a._41 * _11 + a._42 * _21 + a._43 * _31 );
		_42 = -( a._41 * _12 + a._42 * _22 + a._43 * _32 );
		_43 = -( a._41 * _13 + a._42 * _23 + a._43 * _33 );
		_44 = 1.0f;
	}

	IC	void	invert(void)				// slower than invert other matrix
	{
		_matrix a;
		a.set(*this);
		invert(a);
	}
	IC	void	transpose(const _matrix &matSource)	// faster version of transpose
	{
		_11=matSource._11;	_12=matSource._21;	_13=matSource._31;	_14=matSource._41;
		_21=matSource._12;	_22=matSource._22;	_23=matSource._32;	_24=matSource._42;
		_31=matSource._13;	_32=matSource._23;	_33=matSource._33;	_34=matSource._43;
		_41=matSource._14;	_42=matSource._24;	_43=matSource._34;	_44=matSource._44;
	}
	IC	void	transpose(void)						// self transpose - slower
	{
		_matrix a;
		a.set(*this);								// save matrix
		transpose(a);
	}
	IC	void	translate(const Fvector &Loc )		// setup translation matrix
	{
		identity();
		m[3][0] = Loc.x;
		m[3][1] = Loc.y;
		m[3][2] = Loc.z;
	}
	IC	void	translate(float _x, float _y, float _z ) // setup translation matrix
	{
		identity();
		m[3][0] = _x;
		m[3][1] = _y;
		m[3][2] = _z;
	}
	IC	void	translate_over(const Fvector &Loc )	// modify only translation
	{
		m[3][0] = Loc.x;
		m[3][1] = Loc.y;
		m[3][2] = Loc.z;
	}
	IC	void	translate_over(float _x, float _y, float _z) // modify only translation
	{
		m[3][0] = _x;
		m[3][1] = _y;
		m[3][2] = _z;
	}
	IC	void	translate_add(const Fvector &Loc )	// combine translation
	{
		m[3][0] += Loc.x;
		m[3][1] += Loc.y;
		m[3][2] += Loc.z;
	}
	IC	void	scale(float x, float y, float z )	// setup scale matrix
	{	identity(); m[0][0]=x; m[1][1]=y; m[2][2]=z; }
	IC	void	scale(const Fvector &v )			// setup scale matrix
	{	scale(v.x,v.y,v.z); }
	IC	void	scale_over(float x, float y, float z )// modify scaling
	{   m[0][0]=x;	m[1][1]=y;	m[2][2]=z;	}
	IC	void	scale_over(const Fvector &v )		// modify scaling
	{	scale_over(v.x,v.y,v.z); }
	IC	void	scale_add(const Fvector &v )		// combine scaling
	{	m[0][0]*=v.x; m[1][1]*=v.y;	m[2][2]*=v.z; }
	IC	void	rotateX (float Angle )
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [1][1] 	= Cosine;
		m [2][1] 	= -Sine;
		m [1][2] 	= Sine;
		m [2][2] 	= Cosine;
	}
	IC	void	rotateY (float Angle )				// rotation about Y axis
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m[0][0] 	= Cosine;
		m[2][0] 	= -Sine;
		m[0][2] 	= Sine;
		m[2][2] 	= Cosine;
	}
	IC	void	rotateZ (float Angle )
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [0][0] 	= Cosine;
		m [1][0] 	= -Sine;
		m [0][1] 	= Sine;
		m [1][1] 	= Cosine;
	}

	IC	void	rotation( const Fvector &vdir, const Fvector &vnorm )
	{
		Fvector vright;
		vright.crossproduct(vnorm,vdir);
		vright.normalize();
		m[0][0] = vright.x;	m[0][1] = vright.y;	m[0][2] = vright.z; m[0][3]=0;
		m[1][0] = vnorm.x;	m[1][1] = vnorm.y;	m[1][2] = vnorm.z;	m[1][3]=0;
		m[2][0] = vdir.x;	m[2][1] = vdir.y;	m[2][2] = vdir.z;	m[2][3]=0;
		m[3][0] = 0;		m[3][1] = 0;		m[3][2] = 0;		m[3][3]=1;
	}

	IC	void	getDNR(Fvector &D, Fvector &N, Fvector &R) const
	{
		R.set(m[0][0],m[0][1],m[0][2]);
		N.set(m[1][0],m[1][1],m[1][2]);
		D.set(m[2][0],m[2][1],m[2][2]);
	}

	IC	void	rotation ( const Fvector &axis, float Angle )
	{
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [0][0] 	= axis.x * axis.x + ( 1 - axis.x * axis.x) * Cosine;
		m [0][1] 	= axis.x * axis.y * ( 1 - Cosine ) + axis.z * Sine;
		m [0][2] 	= axis.x * axis.z * ( 1 - Cosine ) - axis.y * Sine;
		m [0][3] 	= 0;
		m [1][0] 	= axis.x * axis.y * ( 1 - Cosine ) - axis.z * Sine;
		m [1][1] 	= axis.y * axis.y + ( 1 - axis.y * axis.y) * Cosine;
		m [1][2] 	= axis.y * axis.z * ( 1 - Cosine ) + axis.x * Sine;
		m [1][3] 	= 0;
		m [2][0] 	= axis.x * axis.z * ( 1 - Cosine ) + axis.y * Sine;
		m [2][1] 	= axis.y * axis.z * ( 1 - Cosine ) - axis.x * Sine;
		m [2][2] 	= axis.z * axis.z + ( 1 - axis.z * axis.z) * Cosine;
		m [2][3] 	= 0; m [3][0] = 0; m [3][1] = 0;
		m [3][2] 	= 0; m [3][3] = 1;
	}

	// mirror X
	IC	void	mirrorX ()
	{	identity();	m[0][0] = -1;	}
	IC	void	mirrorX_over ()
	{	m[0][0] = -1;	}
	IC	void	mirrorX_add ()
	{	m[0][0] *= -1;	}

	// mirror Y
	IC	void	mirrorY ()
	{	identity();	m [1][1] = -1;	}
	IC	void	mirrorY_over ()
	{	m [1][1] = -1;	}
	IC	void	mirrorY_add ()
	{	m [1][1] *= -1;	}

	// mirror Z
	IC	void	mirrorZ ()
	{	identity();		m [2][2] = -1;	}
	IC	void	mirrorZ_over ()
	{	m [2][2] = -1;	}
	IC	void	mirrorZ_add ()
	{	m [2][2] *= -1;	}


	IC	void	add( const _matrix &A, const _matrix &B)
	{
		m [0][0] = A.m [0][0] + B.m [0][0];
		m [0][1] = A.m [0][1] + B.m [0][1];
		m [0][2] = A.m [0][2] + B.m [0][2];
		m [0][3] = A.m [0][3] + B.m [0][3];

		m [1][0] = A.m [1][0] + B.m [1][0];
		m [1][1] = A.m [1][1] + B.m [1][1];
		m [1][2] = A.m [1][2] + B.m [1][2];
		m [1][3] = A.m [1][3] + B.m [1][3];

		m [2][0] = A.m [2][0] + B.m [2][0];
		m [2][1] = A.m [2][1] + B.m [2][1];
		m [2][2] = A.m [2][2] + B.m [2][2];
		m [2][3] = A.m [2][3] + B.m [2][3];

		m [3][0] = A.m [3][0] + B.m [3][0];
		m [3][1] = A.m [3][1] + B.m [3][1];
		m [3][2] = A.m [3][2] + B.m [3][2];
		m [3][3] = A.m [3][3] + B.m [3][3];

	}
	IC	void	add( const _matrix &A)
	{
		m [0][0] += A.m [0][0];
		m [0][1] += A.m [0][1];
		m [0][2] += A.m [0][2];
		m [0][3] += A.m [0][3];

		m [1][0] += A.m [1][0];
		m [1][1] += A.m [1][1];
		m [1][2] += A.m [1][2];
		m [1][3] += A.m [1][3];

		m [2][0] += A.m [2][0];
		m [2][1] += A.m [2][1];
		m [2][2] += A.m [2][2];
		m [2][3] += A.m [2][3];

		m [3][0] += A.m [3][0];
		m [3][1] += A.m [3][1];
		m [3][2] += A.m [3][2];
		m [3][3] += A.m [3][3];
	}
	IC	void	sub( const _matrix &A, const _matrix &B)
	{
		m [0][0] = A.m [0][0] - B.m [0][0];
		m [0][1] = A.m [0][1] - B.m [0][1];
		m [0][2] = A.m [0][2] - B.m [0][2];
		m [0][3] = A.m [0][3] - B.m [0][3];

		m [1][0] = A.m [1][0] - B.m [1][0];
		m [1][1] = A.m [1][1] - B.m [1][1];
		m [1][2] = A.m [1][2] - B.m [1][2];
		m [1][3] = A.m [1][3] - B.m [1][3];

		m [2][0] = A.m [2][0] - B.m [2][0];
		m [2][1] = A.m [2][1] - B.m [2][1];
		m [2][2] = A.m [2][2] - B.m [2][2];
		m [2][3] = A.m [2][3] - B.m [2][3];

		m [3][0] = A.m [3][0] - B.m [3][0];
		m [3][1] = A.m [3][1] - B.m [3][1];
		m [3][2] = A.m [3][2] - B.m [3][2];
		m [3][3] = A.m [3][3] - B.m [3][3];
	}
	IC	void	sub( const _matrix &A)
	{
		m [0][0] -= A.m [0][0];
		m [0][1] -= A.m [0][1];
		m [0][2] -= A.m [0][2];
		m [0][3] -= A.m [0][3];

		m [1][0] -= A.m [1][0];
		m [1][1] -= A.m [1][1];
		m [1][2] -= A.m [1][2];
		m [1][3] -= A.m [1][3];

		m [2][0] -= A.m [2][0];
		m [2][1] -= A.m [2][1];
		m [2][2] -= A.m [2][2];
		m [2][3] -= A.m [2][3];

		m [3][0] -= A.m [3][0];
		m [3][1] -= A.m [3][1];
		m [3][2] -= A.m [3][2];
		m [3][3] -= A.m [3][3];
	}
	IC	void	mul( const _matrix &A, float v )
	{
		m[0][0] = A.m [0][0] * v;
		m[0][1] = A.m [0][1] * v;
		m[0][2] = A.m [0][2] * v;
		m[0][3] = A.m [0][3] * v;

		m[1][0] = A.m [1][0] * v;
		m[1][1] = A.m [1][1] * v;
		m[1][2] = A.m [1][2] * v;
		m[1][3] = A.m [1][3] * v;

		m[2][0] = A.m [2][0] * v;
		m[2][1] = A.m [2][1] * v;
		m[2][2] = A.m [2][2] * v;
		m[2][3] = A.m [2][3] * v;

		m[3][0] = A.m [3][0] * v;
		m[3][1] = A.m [3][1] * v;
		m[3][2] = A.m [3][2] * v;
		m[3][3] = A.m [3][3] * v;
	}
	IC	void	mul( float v )
	{
		m[0][0] *= v;		m[0][1] *= v;		m[0][2] *= v;		m[0][3] *= v;
		m[1][0] *= v;		m[1][1] *= v;		m[1][2] *= v;		m[1][3] *= v;
		m[2][0] *= v;		m[2][1] *= v;		m[2][2] *= v;		m[2][3] *= v;
		m[3][0] *= v;		m[3][1] *= v;		m[3][2] *= v;		m[3][3] *= v;
	}
	IC	void	div( const _matrix &A, float v ) {
		VERIFY(fabsf(v)>0.000001f);
		mul(A,1.0f/v);
	}
	IC	void	div( float v ) {
		VERIFY(fabsf(v)>0.000001f);
		mul(1.0f/v);
	}
	IC	void	build_projection(float fFOV, float fAspect, float fNearPlane, float fFarPlane) {
		VERIFY( fabsf(fFarPlane-fNearPlane) > 0.01f );
		VERIFY( fabsf(sinf(fFOV/2)) > 0.01f );

		fFOV/=2.0f;
		float cot	= cosf(fFOV)/sinf(fFOV);
		float w		= fAspect * cot;
		float h		= 1.0f    * cot;
		float Q		= fFarPlane / ( fFarPlane - fNearPlane );

		ZeroMemory( this, sizeof(_matrix) );
		_11 = w;
		_22 = h;
		_33 = Q;
		_34 = 1.0f;
		_43 = -Q*fNearPlane;
	}
	IC	void	build_camera(const Fvector &vFrom, const Fvector &vAt, const Fvector &vWorldUp) 
	{
//		D3DXMatrixLookAtLH((D3DXMATRIX *)this,(D3DXVECTOR3 *)&vEye,(D3DXVECTOR3 *)&vAt,(D3DXVECTOR3 *)&vUp);

		// Get the z basis vector, which points straight ahead. This is the
		// difference from the eyepoint to the lookat point.
		Fvector vView;
		vView.sub(vAt,vFrom);
		vView.normalize();

		// Get the dot product, and calculate the projection of the z basis
		// vector onto the up vector. The projection is the y basis vector.
		float fDotProduct = vWorldUp.dotproduct( vView );

		Fvector vUp;
		vUp.mul(vView, -fDotProduct);
		vUp.add(vWorldUp);
		vUp.normalize();

		// The x basis vector is found simply with the cross product of the y
		// and z basis vectors
		Fvector vRight;
		vRight.crossproduct( vUp, vView );

		// Start building the Device.mView. The first three rows contains the basis
		// vectors used to rotate the view to point at the lookat point
		_11 = vRight.x;  _12 = vUp.x;  _13 = vView.x;  _14 = 0.0f;
		_21 = vRight.y;  _22 = vUp.y;  _23 = vView.y;  _24 = 0.0f;
		_31 = vRight.z;  _32 = vUp.z;  _33 = vView.z;  _34 = 0.0f;

		// Do the translation values (rotations are still about the eyepoint)
		_41 = - vFrom.dotproduct(vRight);
		_42 = - vFrom.dotproduct( vUp  );
		_43 = - vFrom.dotproduct(vView );
		_44 = 1.0f;
	}
	IC	void	build_camera_dir(const Fvector &vFrom, const Fvector &vView, const Fvector &vWorldUp) 
	{
		// Get the dot product, and calculate the projection of the z basis
		// vector onto the up vector. The projection is the y basis vector.
		float fDotProduct = vWorldUp.dotproduct( vView );

		Fvector vUp;
		vUp.mul(vView, -fDotProduct);
		vUp.add(vWorldUp);
		vUp.normalize();

		// The x basis vector is found simply with the cross product of the y
		// and z basis vectors
		Fvector vRight;
		vRight.crossproduct( vUp, vView );

		// Start building the Device.mView. The first three rows contains the basis
		// vectors used to rotate the view to point at the lookat point
		_11 = vRight.x;  _12 = vUp.x;  _13 = vView.x;  _14 = 0.0f;
		_21 = vRight.y;  _22 = vUp.y;  _23 = vView.y;  _24 = 0.0f;
		_31 = vRight.z;  _32 = vUp.z;  _33 = vView.z;  _34 = 0.0f;

		// Do the translation values (rotations are still about the eyepoint)
		_41 = - vFrom.dotproduct(vRight);
		_42 = - vFrom.dotproduct( vUp  );
		_43 = - vFrom.dotproduct(vView );
		_44 = 1.0f;
	}

	IC	void	inertion(const _matrix &mat, float v)
	{
		float iv = 1.f-v;
		for (int i=0; i<4; i++)
		{
			m[i][0] = m[i][0]*v + mat.m[i][0]*iv;
			m[i][1] = m[i][1]*v + mat.m[i][1]*iv;
			m[i][2] = m[i][2]*v + mat.m[i][2]*iv;
			m[i][3] = m[i][3]*v + mat.m[i][3]*iv;
		}
	}
	IC	void	transform_tiny		(Fvector &dest, const Fvector &v)	const // preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + v.z*_31 + _41;
		dest.y = v.x*_12 + v.y*_22 + v.z*_32 + _42;
		dest.z = v.x*_13 + v.y*_23 + v.z*_33 + _43;
	}
	IC	void	transform_tiny32	(Fvector2 &dest, const Fvector &v)	const // preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + v.z*_31 + _41;
		dest.y = v.x*_12 + v.y*_22 + v.z*_32 + _42;
	}
	IC	void	transform_tiny23	(Fvector &dest, const Fvector2 &v)	const // preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + _41;
		dest.y = v.x*_12 + v.y*_22 + _42;
		dest.z = v.x*_13 + v.y*_23 + _43;
	}
	IC	void	transform_dir		(Fvector &dest, const Fvector &v)	const 	// preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + v.z*_31;
		dest.y = v.x*_12 + v.y*_22 + v.z*_32;
		dest.z = v.x*_13 + v.y*_23 + v.z*_33;
	}
	IC	void	transform			(Fvector4 &dest, const Fvector &v)	const 	// preferred to use
	{
		dest.w = v.x*_14 + v.y*_24 + v.z*_34 + _44;
		dest.x = (v.x*_11 + v.y*_21 + v.z*_31 + _41)/dest.w;
		dest.y = (v.x*_12 + v.y*_22 + v.z*_32 + _42)/dest.w;
		dest.z = (v.x*_13 + v.y*_23 + v.z*_33 + _43)/dest.w;
	}
	IC	void	transform			(Fvector &dest, const Fvector &v)	const 	// preferred to use
	{
		float w	= v.x*_14 + v.y*_24 + v.z*_34 + _44;
		dest.x	= (v.x*_11 + v.y*_21 + v.z*_31 + _41)/w;
		dest.y	= (v.x*_12 + v.y*_22 + v.z*_32 + _42)/w;
		dest.z	= (v.x*_13 + v.y*_23 + v.z*_33 + _43)/w;
	}

	IC	void	transform_tiny		(Fvector &v) const
	{
		Fvector			res;
		transform_tiny	(res,v);
		v.set			(res);
	}
	IC	void	transform			(Fvector &v) const
	{
		Fvector			res;
		transform		(res,v);
		v.set			(res);
	}
	IC	void	transform_dir		(Fvector &v) const
	{
		Fvector			res;
		transform_dir	(res,v);
		v.set			(res);
	}
	IC	void	setHPB	(float _h, float _p, float _b)
	{
		_h		= -_h;		_p		= -_p;
		float _sh,_ch;		_sincos(_h,_sh,_ch);
		float _sp,_cp;		_sincos(_p,_sp,_cp);
		float _sb,_cb;		_sincos(_b,_sb,_cb);
	
		i.set(_cb*_ch+_sb*_sp*_sh, 	_sb*_cp,	-_sh*_cb+_sb*_sp*_ch);	_14_=0;
		j.set(-_sb*_ch+_cb*_sp*_sh, _cb*_cp,	_sb*_sh+_cb*_sp*_ch);	_24_=0;
		k.set(_cp*_sh, 				-_sp, 		_cp*_ch);				_34_=0;
		c.set(0,					0,			0);						_44_=1;
    }
	IC	void	setYPR	(float y, float p, float r){setHPB(y,p,r);}
	IC	void	setXYZ	(float x, float y, float z){setHPB(y,x,z);}
} Fmatrix;

#endif
