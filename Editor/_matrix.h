#ifndef __M__
#define __M__

extern _matrix precalc_identity;
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

	__forceinline	D3DMATRIX	*d3d(void) { return (D3DMATRIX *)this; };
	// Class members
	__forceinline void set(_matrix &M) {
		CopyMemory(this,&M,16*sizeof(float));
	}
	__forceinline void identity(void) {
    	_11=1; _12=0; _13=0; _14=0;
    	_21=0; _22=1; _23=0; _24=0;
    	_31=0; _32=0; _33=1; _34=0;
    	_41=0; _42=0; _43=0; _44=1;
//		CopyMemory(this,&precalc_identity,16*sizeof(float));
	}
	__forceinline void get_rapid(_matrix33& R);
	__forceinline void get_translate(_vector& T){
        T.x = m[3][0];
        T.y = m[3][1];
        T.z = m[3][2];
	}
	__forceinline void rotation(_quaternion &Q) {
		float xx = Q.x*Q.x; float yy = Q.y*Q.y; float zz = Q.z*Q.z;
		float xy = Q.x*Q.y; float xz = Q.x*Q.z; float yz = Q.y*Q.z;
		float wx = Q.w*Q.x; float wy = Q.w*Q.y; float wz = Q.w*Q.z;

		_11 = 1 - 2 * ( yy + zz );
		_12 =     2 * ( xy - wz );
		_13 =     2 * ( xz + wy );

		_21 =     2 * ( xy + wz );
		_22 = 1 - 2 * ( xx + zz );
		_23 =     2 * ( yz - wx );

		_31 =     2 * ( xz - wy );
		_32 =     2 * ( yz + wx );
		_33 = 1 - 2 * ( xx + yy );

		_14 = _24 = _34 = 0.0f;
		_41 = _42 = _43 = 0.0f;
		_44 = 1.0f;
	}

	__forceinline void mul(_matrix &b) {								// recomended to use second variation
		float* pA = (float*)this;
		float* pB = (float*)&b;
		float  pM[16];

		// i=0
		pM[4*0+0] = pA[4*0+0] * pB[4*0+0] + pA[4*1+0] * pB[4*0+1] + pA[4*2+0] * pB[4*0+2] + pA[4*3+0] * pB[4*0+3];
		pM[4*0+1] = pA[4*0+1] * pB[4*0+0] + pA[4*1+1] * pB[4*0+1] + pA[4*2+1] * pB[4*0+2] + pA[4*3+1] * pB[4*0+3];
		pM[4*0+2] = pA[4*0+2] * pB[4*0+0] + pA[4*1+2] * pB[4*0+1] + pA[4*2+2] * pB[4*0+2] + pA[4*3+2] * pB[4*0+3];
		pM[4*0+3] = pA[4*0+3] * pB[4*0+0] + pA[4*1+3] * pB[4*0+1] + pA[4*2+3] * pB[4*0+2] + pA[4*3+3] * pB[4*0+3];

		// i=1
		pM[4*1+0] = pA[4*0+0] * pB[4*1+0] + pA[4*1+0] * pB[4*1+1] + pA[4*2+0] * pB[4*1+2] + pA[4*3+0] * pB[4*1+3];
		pM[4*1+1] = pA[4*0+1] * pB[4*1+0] + pA[4*1+1] * pB[4*1+1] + pA[4*2+1] * pB[4*1+2] + pA[4*3+1] * pB[4*1+3];
		pM[4*1+2] = pA[4*0+2] * pB[4*1+0] + pA[4*1+2] * pB[4*1+1] + pA[4*2+2] * pB[4*1+2] + pA[4*3+2] * pB[4*1+3];
		pM[4*1+3] = pA[4*0+3] * pB[4*1+0] + pA[4*1+3] * pB[4*1+1] + pA[4*2+3] * pB[4*1+2] + pA[4*3+3] * pB[4*1+3];

		// i=2
		pM[4*2+0] = pA[4*0+0] * pB[4*2+0] + pA[4*1+0] * pB[4*2+1] + pA[4*2+0] * pB[4*2+2] + pA[4*3+0] * pB[4*2+3];
		pM[4*2+1] = pA[4*0+1] * pB[4*2+0] + pA[4*1+1] * pB[4*2+1] + pA[4*2+1] * pB[4*2+2] + pA[4*3+1] * pB[4*2+3];
		pM[4*2+2] = pA[4*0+2] * pB[4*2+0] + pA[4*1+2] * pB[4*2+1] + pA[4*2+2] * pB[4*2+2] + pA[4*3+2] * pB[4*2+3];
		pM[4*2+3] = pA[4*0+3] * pB[4*2+0] + pA[4*1+3] * pB[4*2+1] + pA[4*2+3] * pB[4*2+2] + pA[4*3+3] * pB[4*2+3];

		// i=3
		pM[4*3+0] = pA[4*0+0] * pB[4*3+0] + pA[4*1+0] * pB[4*3+1] + pA[4*2+0] * pB[4*3+2] + pA[4*3+0] * pB[4*3+3];
		pM[4*3+1] = pA[4*0+1] * pB[4*3+0] + pA[4*1+1] * pB[4*3+1] + pA[4*2+1] * pB[4*3+2] + pA[4*3+1] * pB[4*3+3];
		pM[4*3+2] = pA[4*0+2] * pB[4*3+0] + pA[4*1+2] * pB[4*3+1] + pA[4*2+2] * pB[4*3+2] + pA[4*3+2] * pB[4*3+3];
		pM[4*3+3] = pA[4*0+3] * pB[4*3+0] + pA[4*1+3] * pB[4*3+1] + pA[4*2+3] * pB[4*3+2] + pA[4*3+3] * pB[4*3+3];

		CopyMemory(this,pM,16*sizeof(float));
	}
	__forceinline void mul(_matrix &a,_matrix &b)					// faster than mul to self
	{
		float* pA = (float*)&a;
		float* pB = (float*)&b;
		float* pM = (float*)this;

		// i=0
		pM[4*0+0] = pA[4*0+0] * pB[4*0+0] + pA[4*1+0] * pB[4*0+1] + pA[4*2+0] * pB[4*0+2] + pA[4*3+0] * pB[4*0+3];
		pM[4*0+1] = pA[4*0+1] * pB[4*0+0] + pA[4*1+1] * pB[4*0+1] + pA[4*2+1] * pB[4*0+2] + pA[4*3+1] * pB[4*0+3];
		pM[4*0+2] = pA[4*0+2] * pB[4*0+0] + pA[4*1+2] * pB[4*0+1] + pA[4*2+2] * pB[4*0+2] + pA[4*3+2] * pB[4*0+3];
		pM[4*0+3] = pA[4*0+3] * pB[4*0+0] + pA[4*1+3] * pB[4*0+1] + pA[4*2+3] * pB[4*0+2] + pA[4*3+3] * pB[4*0+3];

		// i=1
		pM[4*1+0] = pA[4*0+0] * pB[4*1+0] + pA[4*1+0] * pB[4*1+1] + pA[4*2+0] * pB[4*1+2] + pA[4*3+0] * pB[4*1+3];
		pM[4*1+1] = pA[4*0+1] * pB[4*1+0] + pA[4*1+1] * pB[4*1+1] + pA[4*2+1] * pB[4*1+2] + pA[4*3+1] * pB[4*1+3];
		pM[4*1+2] = pA[4*0+2] * pB[4*1+0] + pA[4*1+2] * pB[4*1+1] + pA[4*2+2] * pB[4*1+2] + pA[4*3+2] * pB[4*1+3];
		pM[4*1+3] = pA[4*0+3] * pB[4*1+0] + pA[4*1+3] * pB[4*1+1] + pA[4*2+3] * pB[4*1+2] + pA[4*3+3] * pB[4*1+3];

		// i=2
		pM[4*2+0] = pA[4*0+0] * pB[4*2+0] + pA[4*1+0] * pB[4*2+1] + pA[4*2+0] * pB[4*2+2] + pA[4*3+0] * pB[4*2+3];
		pM[4*2+1] = pA[4*0+1] * pB[4*2+0] + pA[4*1+1] * pB[4*2+1] + pA[4*2+1] * pB[4*2+2] + pA[4*3+1] * pB[4*2+3];
		pM[4*2+2] = pA[4*0+2] * pB[4*2+0] + pA[4*1+2] * pB[4*2+1] + pA[4*2+2] * pB[4*2+2] + pA[4*3+2] * pB[4*2+3];
		pM[4*2+3] = pA[4*0+3] * pB[4*2+0] + pA[4*1+3] * pB[4*2+1] + pA[4*2+3] * pB[4*2+2] + pA[4*3+3] * pB[4*2+3];

		// i=3
		pM[4*3+0] = pA[4*0+0] * pB[4*3+0] + pA[4*1+0] * pB[4*3+1] + pA[4*2+0] * pB[4*3+2] + pA[4*3+0] * pB[4*3+3];
		pM[4*3+1] = pA[4*0+1] * pB[4*3+0] + pA[4*1+1] * pB[4*3+1] + pA[4*2+1] * pB[4*3+2] + pA[4*3+1] * pB[4*3+3];
		pM[4*3+2] = pA[4*0+2] * pB[4*3+0] + pA[4*1+2] * pB[4*3+1] + pA[4*2+2] * pB[4*3+2] + pA[4*3+2] * pB[4*3+3];
		pM[4*3+3] = pA[4*0+3] * pB[4*3+0] + pA[4*1+3] * pB[4*3+1] + pA[4*2+3] * pB[4*3+2] + pA[4*3+3] * pB[4*3+3];
	}

	__forceinline	void	invert(const _matrix & a ) {			// faster than self-invert
		identity();

		float fDetInv = ( a._11 * ( a._22 * a._33 - a._23 * a._32 ) -
			a._12 * ( a._21 * a._33 - a._23 * a._31 ) +
			a._13 * ( a._21 * a._32 - a._22 * a._31 ) );

		VERIFY(fabsf(fDetInv)>.0001f);
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

	__forceinline	void	invert(void)				// slower than invert other matrix
	{
		_matrix a;
		CopyMemory(&a,this,16*sizeof(float));					// save matrix
		invert(a);
	}
	__forceinline	void	transpose(const _matrix &matSource)	// faster version of transpose
	{
		_11=matSource._11;	_12=matSource._21;	_13=matSource._31;	_14=matSource._41;
		_21=matSource._12;	_22=matSource._22;	_23=matSource._32;	_24=matSource._42;
		_31=matSource._13;	_32=matSource._23;	_33=matSource._33;	_34=matSource._43;
		_41=matSource._14;	_42=matSource._24;	_43=matSource._34;	_44=matSource._44;
	}
	__forceinline	void	transpose(void)						// self transpose - slower
	{
		_matrix a;
		CopyMemory(&a,this,16*sizeof(float));					// save matrix
		transpose(a);
	}
	__forceinline	void	translate(const _vector &Loc )		// setup translation matrix
	{
		identity();
		m[3][0] = Loc.x;
		m[3][1] = Loc.y;
		m[3][2] = Loc.z;
	}
	__forceinline	void	translate_over(const _vector &Loc )	// modify only translation
	{
		m[3][0] = Loc.x;
		m[3][1] = Loc.y;
		m[3][2] = Loc.z;
	}
	__forceinline	void	translate_add(const _vector &Loc )	// combine translation
	{
		m[3][0] += Loc.x;
		m[3][1] += Loc.y;
		m[3][2] += Loc.z;
	}
	__forceinline	void	scale(const _vector &v )			// setup scale matrix
	{
		identity();
		m [0][0] = v.x;
		m [1][1] = v.y;
		m [2][2] = v.z;
	}
	__forceinline	void	scale_over(const _vector &v )		// modify scaling
	{
		m [0][0] = v.x;
		m [1][1] = v.y;
		m [2][2] = v.z;
	}
	__forceinline	void	scale_add(const _vector &v )		// combine scaling
	{
		m [0][0] *= v.x;
		m [1][1] *= v.y;
		m [2][2] *= v.z;
	}
	__forceinline	void	rotateX (float Angle )
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [1][1] 	= Cosine;
		m [2][1] 	= -Sine;
		m [1][2] 	= Sine;
		m [2][2] 	= Cosine;
	}
	__forceinline	void	rotateY (float Angle )				// rotation about Y axis
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m[0][0] 	= Cosine;
		m[2][0] 	= -Sine;
		m[0][2] 	= Sine;
		m[2][2] 	= Cosine;
	}
	__forceinline	void	rotateZ (float Angle )
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [0][0] 	= Cosine;
		m [1][0] 	= -Sine;
		m [0][1] 	= Sine;
		m [1][1] 	= Cosine;
	}

	__forceinline	void	rotation( const _vector &vdir, const _vector &vnorm )
	{
		_vector vright;
		vright.crossproduct(vnorm,vdir);
		vright.normalize();
		m[0][0] = vright.x;	m[0][1] = vright.y;	m[0][2] = vright.z; m[0][3]=0;
		m[1][0] = vnorm.x;	m[1][1] = vnorm.y;	m[1][2] = vnorm.z;	m[1][3]=0;
		m[2][0] = vdir.x;	m[2][1] = vdir.y;	m[2][2] = vdir.z;	m[2][3]=0;
		m[3][0] = 0;		m[3][1] = 0;		m[3][2] = 0;		m[3][3]=1;
	}

	__forceinline	void	rotation ( const _vector &axis, float Angle )
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
	__forceinline	void	mirrorX ()
	{	identity();	m[0][0] = -1;	}
	__forceinline	void	mirrorX_over ()
	{	m[0][0] = -1;	}
	__forceinline	void	mirrorX_add ()
	{	m[0][0] *= -1;	}

	// mirror Y
	__forceinline	void	mirrorY ()
	{	identity();	m [1][1] = -1;	}
	__forceinline	void	mirrorY_over ()
	{	m [1][1] = -1;	}
	__forceinline	void	mirrorY_add ()
	{	m [1][1] *= -1;	}

	// mirror Z
	__forceinline	void	mirrorZ ()
	{	identity();		m [2][2] = -1;	}
	__forceinline	void	mirrorZ_over ()
	{	m [2][2] = -1;	}
	__forceinline	void	mirrorZ_add ()
	{	m [2][2] *= -1;	}


	__forceinline	void	add( const _matrix &A, const _matrix &B)
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
	__forceinline	void	add( const _matrix &A)
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
	__forceinline	void	sub( const _matrix &A, const _matrix &B)
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
	__forceinline	void	sub( const _matrix &A)
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
	__forceinline	void	mul( const _matrix &A, float v )
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
	__forceinline	void	mul( float v )
	{
		m[0][0] *= v;		m[0][1] *= v;		m[0][2] *= v;		m[0][3] *= v;
		m[1][0] *= v;		m[1][1] *= v;		m[1][2] *= v;		m[1][3] *= v;
		m[2][0] *= v;		m[2][1] *= v;		m[2][2] *= v;		m[2][3] *= v;
		m[3][0] *= v;		m[3][1] *= v;		m[3][2] *= v;		m[3][3] *= v;
	}
	__forceinline	void	div( const _matrix &A, float v ) {
		VERIFY(fabsf(v)>0.000001f);
		mul(A,1.0f/v);
	}
	__forceinline	void	div( float v ) {
		VERIFY(fabsf(v)>0.000001f);
		mul(1.0f/v);
	}
	__forceinline	void	build_projection(float fFOV, float fAspect, float fNearPlane, float fFarPlane) {
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
	__forceinline	void	build_camera(Fvector &vFrom, Fvector &vAt, Fvector &vWorldUp) {
//		D3DXMatrixLookAtLH((D3DXMATRIX *)this,(D3DXVECTOR3 *)&vEye,(D3DXVECTOR3 *)&vAt,(D3DXVECTOR3 *)&vUp);

		// Get the z basis vector, which points straight ahead. This is the
		// difference from the eyepoint to the lookat point.
		Fvector vView;
		vView.sub(vAt,vFrom);
		vView.normalize();

		// Get the dot product, and calculate the projection of the z basis
		// vector onto the up vector. The projection is the y basis vector.
		FLOAT fDotProduct = vWorldUp.dotproduct( vView );

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

	__forceinline	void	inertion(_matrix &mat, float v)
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

    // AlexRR
    __forceinline void transform( _vector& dst, _vector& src ){
    	dst.x = src.x * _11 + src.y * _21 + src.z * _31 + _41;
    	dst.y = src.x * _12 + src.y * _22 + src.z * _32 + _42;
    	dst.z = src.x * _13 + src.y * _23 + src.z * _33 + _43;
    	float w = src.x * _14 + src.y * _24 + src.z * _34 + _44;
    	_ASSERTE( !is_zero(w) );
    	dst.x /= w;
    	dst.y /= w;
    	dst.z /= w; };

    __forceinline void transform( _vector& dst ){
    	_vector v; v.set( dst );
    	transform( dst, v ); };

    __forceinline void shorttransform( _vector& dst, _vector& src ){
    	dst.x = src.x * _11 + src.y * _21 + src.z * _31;
    	dst.y = src.x * _12 + src.y * _22 + src.z * _32;
    	dst.z = src.x * _13 + src.y * _23 + src.z * _33; };
    __forceinline void shorttransform( _vector& dst ){
    	_vector v; v.set( dst );
    	shorttransform( dst, v ); };
    __forceinline void mul2( _matrix& m ){
    	_matrix src2; src2.set( (*this) );
    	mul( m, src2 ); };

} Fmatrix;

#endif
