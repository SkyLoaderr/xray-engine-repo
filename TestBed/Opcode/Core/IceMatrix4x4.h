///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for 4x4 matrices.
 *	\file		IceMatrix4x4.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEMATRIX4X4_H__
#define __ICEMATRIX4X4_H__

	#define	MATRIX4X4_EPSILON		(1.0e-7f)
	#define	ROW	*(*this)

	class ICEMATHS_API Matrix4x4
	{
//				void	LUBackwardSubstitution( sdword *indx, float* b );
//				void	LUDecomposition( sdword* indx, float* d );

		public:
		//! Empty constructor.
		__forceinline				Matrix4x4()									{}
		//! Constructor from 16 values
		__forceinline				Matrix4x4(	float m00, float m01, float m02, float m03,
												float m10, float m11, float m12, float m13,
												float m20, float m21, float m22, float m23,
												float m30, float m31, float m32, float m33)
									{
										m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;	m[0][3] = m03;
										m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;	m[1][3] = m13;
										m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;	m[2][3] = m23;
										m[3][0] = m30;	m[3][1] = m31;	m[3][2] = m32;	m[3][3] = m33;
									}
		//! Copy constructor
		__forceinline				Matrix4x4(const Matrix4x4& mat)				{ CopyMemory(m, &mat.m, 16*sizeof(float));	}
		//! Destructor.
		__forceinline				~Matrix4x4()								{}

		//! Assign values
		__forceinline	Matrix4x4&	Set(	float m00, float m01, float m02, float m03,
											float m10, float m11, float m12, float m13,
											float m20, float m21, float m22, float m23,
											float m30, float m31, float m32, float m33)
									{
										m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;	m[0][3] = m03;
										m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;	m[1][3] = m13;
										m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;	m[2][3] = m23;
										m[3][0] = m30;	m[3][1] = m31;	m[3][2] = m32;	m[3][3] = m33;
										return *this;
									}
		// Translation
		//! Returns the translation part of the matrix.
		__forceinline	const HPoint*	GetTrans()						const	{ return (*this)[3];/*(Point*) &mTrans;*/ }
		//! Gets the translation part of the matrix
		__forceinline	const Matrix4x4&	GetTrans(Point& p)			const	{ p.x = m[3][0];	p.y = m[3][1];	p.z = m[3][2];	return *this;		}
		//! Sets the translation part of the matrix, from a Point.
		__forceinline	Matrix4x4&	SetTrans(const Point& p)					{ m[3][0] = p.x;	m[3][1] = p.y;	m[3][2] = p.z;	return *this;		}
		//! Sets the translation part of the matrix, from a HPoint.
		__forceinline	Matrix4x4&	SetTrans(const HPoint& p)					{ m[3][0] = p.x;	m[3][1] = p.y;	m[3][2] = p.z;	m[3][3] = p.w; return *this;		}
		//! Sets the translation part of the matrix, from floats.
		__forceinline	Matrix4x4&	SetTrans(float tx, float ty, float tz)		{ m[3][0] = tx;		m[3][1] = ty;	m[3][2] = tz;	return *this;		}

		// Scale
		//! Sets the scale from a Point. The point is put on the diagonal.
		__forceinline	Matrix4x4&	SetScale(const Point& p)					{ m[0][0] = p.x;	m[1][1] = p.y;	m[2][2] = p.z;	return *this;		}
		//! Sets the scale from floats. Values are put on the diagonal.
		__forceinline	Matrix4x4&	SetScale(float sx, float sy, float sz)		{ m[0][0] = sx;		m[1][1] = sy;	m[2][2] = sz;	return *this;		}
		//! Scales from a Point. Each row is multiplied by a component.
						Matrix4x4&	Scale(const Point& p)
						{
							m[0][0] *= p.x;	m[1][0] *= p.y;	m[2][0] *= p.z;
							m[0][1] *= p.x;	m[1][1] *= p.y;	m[2][1] *= p.z;
							m[0][2] *= p.x;	m[1][2] *= p.y;	m[2][2] *= p.z;
							return	*this;
						}
		//! Scales from floats. Each row is multiplied by a value.
						Matrix4x4&	Scale(float sx, float sy, float sz)
						{
							m[0][0] *= sx;	m[1][0] *= sy;	m[2][0] *= sz;
							m[0][1] *= sx;	m[1][1] *= sy;	m[2][1] *= sz;
							m[0][2] *= sx;	m[1][2] *= sy;	m[2][2] *= sz;
							return	*this;
						}

		//! Copy from a Matrix4x4
		__forceinline	Matrix4x4&	Copy(const Matrix4x4& source)
						{
							CopyMemory(m, source.m, 16*sizeof(float));
							return	*this;
						}

		// Row-column access
		//! Returns a row.
		__forceinline	void		GetRow(const udword r, HPoint& p)	const	{ p.x = m[r][0];	p.y = m[r][1];	p.z = m[r][2];	p.w = m[r][3];		}
		//! Returns a row.
		__forceinline	void		GetRow(const udword r, Point& p)	const	{ p.x = m[r][0];	p.y = m[r][1];	p.z = m[r][2];						}
		//! Sets a row.
		__forceinline	void		SetRow(const udword r, const HPoint& p)		{ m[r][0] = p.x;	m[r][1] = p.y;	m[r][2] = p.z;	m[r][3] = p.w;		}
		//! Sets a row.
		__forceinline	void		SetRow(const udword r, const Point& p)		{ m[r][0] = p.x;	m[r][1] = p.y;	m[r][2] = p.z;	m[r][3] = (r!=3) ? 0.0f : 1.0f;		}
		//! Returns a column.
		__forceinline	void		GetCol(const udword c, HPoint& p)	const	{ p.x = m[0][c];	p.y = m[1][c];	p.z = m[2][c];	p.w = m[3][c];		}
		//! Returns a column.
		__forceinline	void		GetCol(const udword c, Point& p)	const	{ p.x = m[0][c];	p.y = m[1][c];	p.z = m[2][c];							}
		//! Sets a column.
		__forceinline	void		SetCol(const udword c, const HPoint& p)		{ m[0][c] = p.x;	m[1][c] = p.y;	m[2][c] = p.z;	m[3][c] = p.w;			}
		//! Sets a column.
		__forceinline	void		SetCol(const udword c, const Point& p)		{ m[0][c] = p.x;	m[1][c] = p.y;	m[2][c] = p.z;	m[3][c] = (c!=3) ? 0.0f : 1.0f;	}

/*
		//! Returns a row.
		__forceinline	HPoint		GetRow(const udword row)	const			{ return mRow[row];														}
		//! Sets a row.
		__forceinline	Matrix4x4&	SetRow(const udword row, const HPoint& p)	{ mRow[row] = p;	return *this;										}
		//! Sets a row.
						Matrix4x4&	SetRow(const udword row, const Point& p)
						{
							m[row][0] = p.x;
							m[row][1] = p.y;
							m[row][2] = p.z;
							m[row][3] = (row != 3) ? 0.0f : 1.0f;
							return	*this;
						}
		//! Returns a column.
						HPoint		GetCol(const udword col)		const
						{
							HPoint	Res;
							Res.x = m[0][col];
							Res.y = m[1][col];
							Res.z = m[2][col];
							Res.w = m[3][col];
							return	Res;
						}
		//! Sets a column.
						Matrix4x4&	SetCol(const udword col, const HPoint& p)
						{
							m[0][col] = p.x;
							m[1][col] = p.y;
							m[2][col] = p.z;
							m[3][col] = p.w;
							return	*this;
						}
		//! Sets a column.
						Matrix4x4&	SetCol(const udword col, const Point& p)
						{
							m[0][col] = p.x;
							m[1][col] = p.y;
							m[2][col] = p.z;
							m[3][col] = (col != 3) ? 0.0f : 1.0f;
							return	*this;
						}
*/
		//! Computes the trace. The trace is the sum of the 4 diagonal components.
		__forceinline	float		Trace()			const			{ return m[0][0] + m[1][1] + m[2][2] + m[3][3];							}
		//! Clears the matrix.
		__forceinline	Matrix4x4&	Zero()							{ ZeroMemory(&m,  sizeof(m));	return *this;							}
		//! Sets the identity matrix.
		__forceinline	Matrix4x4&	Identity()						{ Zero(); m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f; return *this;	}
		//! Checks for identity
		__forceinline	bool		IsIdentity()	const
						{
							if(IR(m[0][0])!=IEEE_1_0)	return false;
							if(IR(m[0][1])!=0)			return false;
							if(IR(m[0][2])!=0)			return false;
							if(IR(m[0][3])!=0)			return false;

							if(IR(m[1][0])!=0)			return false;
							if(IR(m[1][1])!=IEEE_1_0)	return false;
							if(IR(m[1][2])!=0)			return false;
							if(IR(m[1][3])!=0)			return false;

							if(IR(m[2][0])!=0)			return false;
							if(IR(m[2][1])!=0)			return false;
							if(IR(m[2][2])!=IEEE_1_0)	return false;
							if(IR(m[2][3])!=0)			return false;

							if(IR(m[3][0])!=0)			return false;
							if(IR(m[3][1])!=0)			return false;
							if(IR(m[3][2])!=0)			return false;
							if(IR(m[3][3])!=IEEE_1_0)	return false;
							return true;
						}

		// Computes a world matrix.
						Matrix4x4&	World(const PRS& prs);
						Matrix4x4&	World(const PR& pr);
		// Computes a shadow matrix
						Matrix4x4&	Shadow(const Point& light, const Point& p0, const Point& p1, const Point& p2);
		// Computes a sphere map matrix
						Matrix4x4&	SphereMap(float scale=0.5f);
		// Computes a self-shadowing matrix
						Matrix4x4&	SelfShadow(const Point& light);
		// Computes a rotozoom matrix
						Matrix4x4&	Rotozoom(float angle, float zoom, float posx, float posy);

		//! Sets a rotation matrix around the X axis.
						Matrix4x4&	RotX(float angle)	{ float Cos = cosf(angle), Sin = sinf(angle); Identity(); m[1][1] = m[2][2] = Cos; m[2][1] = -Sin;	m[1][2] = Sin;	return *this; }
		//! Sets a rotation matrix around the Y axis.
						Matrix4x4&	RotY(float angle)	{ float Cos = cosf(angle), Sin = sinf(angle); Identity(); m[0][0] = m[2][2] = Cos; m[2][0] = Sin;	m[0][2] = -Sin;	return *this; }  
		//! Sets a rotation matrix around the Z axis.
						Matrix4x4&	RotZ(float angle)	{ float Cos = cosf(angle), Sin = sinf(angle); Identity(); m[0][0] = m[1][1] = Cos; m[1][0] = -Sin;	m[0][1] = Sin;	return *this; }

		//! Makes a rotation matrix about an arbitrary angle
						Matrix4x4&	Rot(float angle, Point& p1, Point& p2);

		//! Transposes the matrix.
						Matrix4x4&	Transpose()
						{
							IR(m[1][0]) ^= IR(m[0][1]);		IR(m[0][1]) ^= IR(m[1][0]);		IR(m[1][0]) ^= IR(m[0][1]);
							IR(m[2][0]) ^= IR(m[0][2]);		IR(m[0][2]) ^= IR(m[2][0]);		IR(m[2][0]) ^= IR(m[0][2]);
							IR(m[3][0]) ^= IR(m[0][3]);		IR(m[0][3]) ^= IR(m[3][0]);		IR(m[3][0]) ^= IR(m[0][3]);
							IR(m[1][2]) ^= IR(m[2][1]);		IR(m[2][1]) ^= IR(m[1][2]);		IR(m[1][2]) ^= IR(m[2][1]);
							IR(m[1][3]) ^= IR(m[3][1]);		IR(m[3][1]) ^= IR(m[1][3]);		IR(m[1][3]) ^= IR(m[3][1]);
							IR(m[2][3]) ^= IR(m[3][2]);		IR(m[3][2]) ^= IR(m[2][3]);		IR(m[2][3]) ^= IR(m[3][2]);
							return	*this;
						}

		//! Computes a cofactor. Used for matrix inversion.
						float		CoFactor(udword row, udword col)	const;
		//! Computes the determinant of the matrix.
						float		Determinant()	const;
		//! Inverts the matrix. Determinant must be different from zero, else matrix can't be inverted.
						Matrix4x4&	Invert();
//				Matrix&	ComputeAxisMatrix(Point& axis, float angle);

		// Cast operators
		//! Casts a Matrix4x4 to a Matrix3x3.
		__forceinline	operator Matrix3x3()	const
						{
							return Matrix3x3(
							m[0][0],	m[0][1],	m[0][2],
							m[1][0],	m[1][1],	m[1][2],
							m[2][0],	m[2][1],	m[2][2]);
						}
		//! Casts a Matrix4x4 to a Quat.
						operator Quat()	const;
//		operator	PRS() const;

		// Arithmetic operators
		//! Operator for Matrix4x4 Plus = Matrix4x4 + Matrix4x4;
		__forceinline	Matrix4x4	operator+(const Matrix4x4& mat)	const
						{
							Matrix4x4 Ret;
							Ret.SetRow(0, ROW[0] + *mat[0]);
							Ret.SetRow(1, ROW[1] + *mat[1]);
							Ret.SetRow(2, ROW[2] + *mat[2]);
							Ret.SetRow(3, ROW[3] + *mat[3]);
							return	Ret;
						}

		//! Operator for Matrix4x4 Minus = Matrix4x4 - Matrix4x4;
		__forceinline	Matrix4x4	operator-(const Matrix4x4& mat)	const
						{
							Matrix4x4 Ret;
							Ret.SetRow(0, ROW[0] - *mat[0]);
							Ret.SetRow(1, ROW[1] - *mat[1]);
							Ret.SetRow(2, ROW[2] - *mat[2]);
							Ret.SetRow(3, ROW[3] - *mat[3]);
							return	Ret;
						}

		//! Operator for Matrix4x4 Mul = Matrix4x4 * Matrix4x4;
		__forceinline	Matrix4x4	operator*(const Matrix4x4& mat)	const
						{
							return Matrix4x4(
							m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0] + m[0][3]*mat.m[3][0],
							m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1] + m[0][3]*mat.m[3][1],
							m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2] + m[0][3]*mat.m[3][2],
							m[0][0]*mat.m[0][3] + m[0][1]*mat.m[1][3] + m[0][2]*mat.m[2][3] + m[0][3]*mat.m[3][3],

							m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0] + m[1][3]*mat.m[3][0],
							m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1] + m[1][3]*mat.m[3][1],
							m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2] + m[1][3]*mat.m[3][2],
							m[1][0]*mat.m[0][3] + m[1][1]*mat.m[1][3] + m[1][2]*mat.m[2][3] + m[1][3]*mat.m[3][3],

							m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0] + m[2][3]*mat.m[3][0],
							m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1] + m[2][3]*mat.m[3][1],
							m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2] + m[2][3]*mat.m[3][2],
							m[2][0]*mat.m[0][3] + m[2][1]*mat.m[1][3] + m[2][2]*mat.m[2][3] + m[2][3]*mat.m[3][3],

							m[3][0]*mat.m[0][0] + m[3][1]*mat.m[1][0] + m[3][2]*mat.m[2][0] + m[3][3]*mat.m[3][0],
							m[3][0]*mat.m[0][1] + m[3][1]*mat.m[1][1] + m[3][2]*mat.m[2][1] + m[3][3]*mat.m[3][1],
							m[3][0]*mat.m[0][2] + m[3][1]*mat.m[1][2] + m[3][2]*mat.m[2][2] + m[3][3]*mat.m[3][2],
							m[3][0]*mat.m[0][3] + m[3][1]*mat.m[1][3] + m[3][2]*mat.m[2][3] + m[3][3]*mat.m[3][3]);
						}

		//! Operator for HPoint Mul = Matrix4x4 * HPoint;
		__forceinline	HPoint		operator*(const HPoint& v)		const	{ return HPoint(ROW[0]|v, ROW[1]|v, ROW[2]|v, ROW[3]|v); }

		//! Operator for Point Mul = Matrix4x4 * Point;
		__forceinline	Point		operator*(const Point& v)		const
						{
							return Point(	m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3],
											m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3],
											m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]	);
						}

		//! Operator for Matrix4x4 Scale = Matrix4x4 * float;
		__forceinline	Matrix4x4	operator*(float s)				const
						{
							return Matrix4x4(
							m[0][0]*s,	m[0][1]*s,	m[0][2]*s,	m[0][3]*s,
							m[1][0]*s,	m[1][1]*s,	m[1][2]*s,	m[1][3]*s,
							m[2][0]*s,	m[2][1]*s,	m[2][2]*s,	m[2][3]*s,
							m[3][0]*s,	m[3][1]*s,	m[3][2]*s,	m[3][3]*s);
						}

		//! Operator for Matrix4x4 Scale = float * Matrix4x4;
		__forceinline	friend Matrix4x4 operator*(float s, const Matrix4x4& mat)
						{
							return Matrix4x4(
							s*mat.m[0][0],	s*mat.m[0][1],	s*mat.m[0][2],	s*mat.m[0][3],
							s*mat.m[1][0],	s*mat.m[1][1],	s*mat.m[1][2],	s*mat.m[1][3],
							s*mat.m[2][0],	s*mat.m[2][1],	s*mat.m[2][2],	s*mat.m[2][3],
							s*mat.m[3][0],	s*mat.m[3][1],	s*mat.m[3][2],	s*mat.m[3][3]);
						}

		//! Operator for Matrix4x4 Div = Matrix4x4 / float;
		__forceinline	Matrix4x4	operator/(float s)				const
						{
							if(s) s = 1.0f / s;

							return Matrix4x4(
							m[0][0]*s,	m[0][1]*s,	m[0][2]*s,	m[0][3]*s,
							m[1][0]*s,	m[1][1]*s,	m[1][2]*s,	m[1][3]*s,
							m[2][0]*s,	m[2][1]*s,	m[2][2]*s,	m[2][3]*s,
							m[3][0]*s,	m[3][1]*s,	m[3][2]*s,	m[3][3]*s);
						}

		//! Operator for Matrix4x4 Div = float / Matrix4x4;
		__forceinline	friend Matrix4x4 operator/(float s, const Matrix4x4& mat)
						{
							return Matrix4x4(
							s/mat.m[0][0],	s/mat.m[0][1],	s/mat.m[0][2],	s/mat.m[0][3],
							s/mat.m[1][0],	s/mat.m[1][1],	s/mat.m[1][2],	s/mat.m[1][3],
							s/mat.m[2][0],	s/mat.m[2][1],	s/mat.m[2][2],	s/mat.m[2][3],
							s/mat.m[3][0],	s/mat.m[3][1],	s/mat.m[3][2],	s/mat.m[3][3]);
						}

		//! Operator for Matrix4x4 += Matrix4x4;
		__forceinline	Matrix4x4&	operator+=(const Matrix4x4& mat)
						{
							m[0][0]+=mat.m[0][0];	m[0][1]+=mat.m[0][1];	m[0][2]+=mat.m[0][2];	m[0][3]+=mat.m[0][3];
							m[1][0]+=mat.m[1][0];	m[1][1]+=mat.m[1][1];	m[1][2]+=mat.m[1][2];	m[1][3]+=mat.m[1][3];
							m[2][0]+=mat.m[2][0];	m[2][1]+=mat.m[2][1];	m[2][2]+=mat.m[2][2];	m[2][3]+=mat.m[2][3];
							m[3][0]+=mat.m[3][0];	m[3][1]+=mat.m[3][1];	m[3][2]+=mat.m[3][2];	m[3][3]+=mat.m[3][3];
							return	*this;
						}

		//! Operator for Matrix4x4 -= Matrix4x4;
		__forceinline	Matrix4x4&	operator-=(const Matrix4x4& mat)
						{
							m[0][0]-=mat.m[0][0];	m[0][1]-=mat.m[0][1];	m[0][2]-=mat.m[0][2];	m[0][3]-=mat.m[0][3];
							m[1][0]-=mat.m[1][0];	m[1][1]-=mat.m[1][1];	m[1][2]-=mat.m[1][2];	m[1][3]-=mat.m[1][3];
							m[2][0]-=mat.m[2][0];	m[2][1]-=mat.m[2][1];	m[2][2]-=mat.m[2][2];	m[2][3]-=mat.m[2][3];
							m[3][0]-=mat.m[3][0];	m[3][1]-=mat.m[3][1];	m[3][2]-=mat.m[3][2];	m[3][3]-=mat.m[3][3];
							return	*this;
						}

		//! Operator for Matrix4x4 *= Matrix4x4;
						Matrix4x4&	operator*=(const Matrix4x4& mat)
						{
							HPoint TempRow;

							GetRow(0, TempRow);
							m[0][0] = TempRow.x*mat.m[0][0] + TempRow.y*mat.m[1][0] + TempRow.z*mat.m[2][0] + TempRow.w*mat.m[3][0];
							m[0][1] = TempRow.x*mat.m[0][1] + TempRow.y*mat.m[1][1] + TempRow.z*mat.m[2][1] + TempRow.w*mat.m[3][1];
							m[0][2] = TempRow.x*mat.m[0][2] + TempRow.y*mat.m[1][2] + TempRow.z*mat.m[2][2] + TempRow.w*mat.m[3][2];
							m[0][3] = TempRow.x*mat.m[0][3] + TempRow.y*mat.m[1][3] + TempRow.z*mat.m[2][3] + TempRow.w*mat.m[3][3];

							GetRow(1, TempRow);
							m[1][0] = TempRow.x*mat.m[0][0] + TempRow.y*mat.m[1][0] + TempRow.z*mat.m[2][0] + TempRow.w*mat.m[3][0];
							m[1][1] = TempRow.x*mat.m[0][1] + TempRow.y*mat.m[1][1] + TempRow.z*mat.m[2][1] + TempRow.w*mat.m[3][1];
							m[1][2] = TempRow.x*mat.m[0][2] + TempRow.y*mat.m[1][2] + TempRow.z*mat.m[2][2] + TempRow.w*mat.m[3][2];
							m[1][3] = TempRow.x*mat.m[0][3] + TempRow.y*mat.m[1][3] + TempRow.z*mat.m[2][3] + TempRow.w*mat.m[3][3];

							GetRow(2, TempRow);
							m[2][0] = TempRow.x*mat.m[0][0] + TempRow.y*mat.m[1][0] + TempRow.z*mat.m[2][0] + TempRow.w*mat.m[3][0];
							m[2][1] = TempRow.x*mat.m[0][1] + TempRow.y*mat.m[1][1] + TempRow.z*mat.m[2][1] + TempRow.w*mat.m[3][1];
							m[2][2] = TempRow.x*mat.m[0][2] + TempRow.y*mat.m[1][2] + TempRow.z*mat.m[2][2] + TempRow.w*mat.m[3][2];
							m[2][3] = TempRow.x*mat.m[0][3] + TempRow.y*mat.m[1][3] + TempRow.z*mat.m[2][3] + TempRow.w*mat.m[3][3];

							GetRow(3, TempRow);
							m[3][0] = TempRow.x*mat.m[0][0] + TempRow.y*mat.m[1][0] + TempRow.z*mat.m[2][0] + TempRow.w*mat.m[3][0];
							m[3][1] = TempRow.x*mat.m[0][1] + TempRow.y*mat.m[1][1] + TempRow.z*mat.m[2][1] + TempRow.w*mat.m[3][1];
							m[3][2] = TempRow.x*mat.m[0][2] + TempRow.y*mat.m[1][2] + TempRow.z*mat.m[2][2] + TempRow.w*mat.m[3][2];
							m[3][3] = TempRow.x*mat.m[0][3] + TempRow.y*mat.m[1][3] + TempRow.z*mat.m[2][3] + TempRow.w*mat.m[3][3];

							return	*this;
						}

		//! Operator for Matrix4x4 *= float;
		__forceinline	Matrix4x4&		operator*=(float s)
						{
							m[0][0]*=s;	m[0][1]*=s;	m[0][2]*=s;	m[0][3]*=s;
							m[1][0]*=s;	m[1][1]*=s;	m[1][2]*=s;	m[1][3]*=s;
							m[2][0]*=s;	m[2][1]*=s;	m[2][2]*=s;	m[2][3]*=s;
							m[3][0]*=s;	m[3][1]*=s;	m[3][2]*=s;	m[3][3]*=s;
							return	*this;
						}

		//! Operator for Matrix4x4 /= float;
		__forceinline	Matrix4x4&		operator/=(float s)
						{
							if(s)  s = 1.0f / s;
							m[0][0]*=s;	m[0][1]*=s;	m[0][2]*=s;	m[0][3]*=s;
							m[1][0]*=s;	m[1][1]*=s;	m[1][2]*=s;	m[1][3]*=s;
							m[2][0]*=s;	m[2][1]*=s;	m[2][2]*=s;	m[2][3]*=s;
							m[3][0]*=s;	m[3][1]*=s;	m[3][2]*=s;	m[3][3]*=s;
							return	*this;
						}

		__forceinline	const HPoint*	operator[](int nRow)	const	{ return (const HPoint*)&m[nRow][0];	}
		__forceinline	HPoint*			operator[](int nRow)			{ return (HPoint*)&m[nRow][0];			}

		public:

			float	m[4][4];
/*
			union{
				struct{
					//! Direct access to rows.
					HPoint		mRow[3];
					//! The translation part.
					HPoint		mTrans;
				};
				//! Can also be defined by an array of 16 floats.
				float			m[4][4];
			};
*/
	};

	//! Quickly rotates & translates a vector, using the 4x3 part of a 4x4 matrix
	__forceinline void TransformPoint4x3(Point& dest, const Point& source, const Matrix4x4& rot)
	{
		dest.x = rot.m[3][0] + source.x * rot.m[0][0] + source.y * rot.m[1][0] + source.z * rot.m[2][0];
		dest.y = rot.m[3][1] + source.x * rot.m[0][1] + source.y * rot.m[1][1] + source.z * rot.m[2][1];
		dest.z = rot.m[3][2] + source.x * rot.m[0][2] + source.y * rot.m[1][2] + source.z * rot.m[2][2];
	}

	ICEMATHS_API void InvertPRMatrix(Matrix4x4& dest, const Matrix4x4& src);

	class ICEMATHS_API MatrixPalette : public Container
	{
		public:
		// Constructor / Destructor
		__forceinline				MatrixPalette()						{}
		__forceinline				~MatrixPalette()					{}

		__forceinline	udword		GetNbMatrices()		const			{ return GetNbEntries() / (sizeof(Matrix4x4)/sizeof(float));		}
		__forceinline	Matrix4x4*	GetMatrices()		const			{ return (Matrix4x4*)GetEntries();									}

						void		AddMatrix(const Matrix4x4& mat)		{ float* f = (float*)mat.m;	for(udword i=0;i<16;i++)	Add(f[i]);	}
	};

#endif // __ICEMATRIX4X4_H__

