///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for homogeneous vectors.
 *	\file		IceHPoint.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEHPOINT_H__
#define __ICEHPOINT_H__

	enum PointComponent
	{
		_X				= 0,
		_Y				= 1,
		_Z				= 2,
		_W				= 3,
		_FORCE_DWORD	= 0x7fffffff
	};

	class ICEMATHS_API HPoint : public Point
	{
		public:

		//! Empty constructor
		__forceinline				HPoint()																		{}
		//! Constructor from floats
		__forceinline				HPoint(float _x, float _y, float _z, float _w=0.0f) : Point(_x, _y, _z), w(_w)	{}
		//! Constructor from array
		__forceinline				HPoint(float f[4]) : Point(f), w(f[3])											{}
		//! Constructor from a Point
		__forceinline				HPoint(const Point& p, float _w=0.0f) : Point(p), w(_w)							{}
		//! Destructor
		__forceinline				~HPoint()																		{}

		//! Clear the point
		__forceinline	HPoint&		Zero()											{ x =			y =			z =			w = 0.0f;		return *this;	}

		//! Assignment from values
		__forceinline	HPoint&		Set(float _x, float _y, float _z, float _w )	{ x  = _x;		y  = _y;	z  = _z;	w  = _w;		return *this;	}
		//! Assignment from array
		__forceinline	HPoint&		Set(float f[4])									{ x  = f[0];	y  = f[1];	z  = f[2];	w  = f[3];		return *this;	}
		//! Assignment from another h-point
		__forceinline	HPoint&		Set(const HPoint& src)							{ x  = src.x;	y  = src.y;	z  = src.z;	w = src.w;		return *this;	}

		//! Add a vector
		__forceinline	HPoint&		Add(float _x, float _y, float _z, float _w )	{ x += _x;		y += _y;	z += _z;	w += _w;		return *this;	}
		//! Add a vector
		__forceinline	HPoint&		Add(float f[4])									{ x += f[0];	y += f[1];	z += f[2];	w += f[3];		return *this;	}

		//! Subtract a vector
		__forceinline	HPoint&		Sub(float _x, float _y, float _z, float _w )	{ x -= _x;		y -= _y;	z -= _z;	w -= _w;		return *this;	}
		//! Subtract a vector
		__forceinline	HPoint&		Sub(float f[4])									{ x -= f[0];	y -= f[1];	z -= f[2];	w -= f[3];		return *this;	}
		
		//! Multiplies by a scalar
		__forceinline	HPoint&		Mul(float s)									{ x *= s;		y *= s;		z *= s;		w *= s;			return *this;	}

		//! Returns MIN(x, y, z, w);
						float		Min()								const		{ return MIN(x, MIN(y, MIN(z, w)));										}
		//! Returns MAX(x, y, z, w);
						float		Max()								const		{ return MAX(x, MAX(y, MAX(z, w)));										}
		//! TO BE DOCUMENTED
						HPoint&		Min(const HPoint& p)							{ x = MIN(x, p.x); y = MIN(y, p.y); z = MIN(z, p.z); w = MIN(w, p.w);	return *this;	}
		//! TO BE DOCUMENTED
						HPoint&		Max(const HPoint& p)							{ x = MAX(x, p.x); y = MAX(y, p.y); z = MAX(z, p.z); w = MAX(w, p.w);	return *this;	}

		//! Computes square magnitude
		__forceinline	float		SquareMagnitude()					const		{ return x*x + y*y + z*z + w*w;											}
		//! Computes magnitude
		__forceinline	float		Magnitude()							const		{ return sqrtf(x*x + y*y + z*z + w*w);									}

		//! Normalize the vector
		__forceinline	HPoint&		Normalize()
						{
							float M = Magnitude();
							if(M)
							{
								M = 1.0f / M;
								x *= M;
								y *= M;
								z *= M;
								w *= M;
							}
							return *this;
						}

		// Arithmetic operators
		//! Operator for HPoint Negate = - HPoint;
		__forceinline	HPoint		operator-()							const		{ return HPoint(-x, -y, -z, -w);							}

		//! Operator for HPoint Plus  = HPoint + HPoint;
		__forceinline	HPoint		operator+(const HPoint& p)			const		{ return HPoint(x + p.x, y + p.y, z + p.z, w + p.w);		}
		//! Operator for HPoint Minus = HPoint - HPoint;
		__forceinline	HPoint		operator-(const HPoint& p)			const		{ return HPoint(x - p.x, y - p.y, z - p.z, w - p.w);		}

		//! Operator for HPoint Mul   = HPoint * HPoint;
		__forceinline	HPoint		operator*(const HPoint& p)			const		{ return HPoint(x * p.x, y * p.y, z * p.z, w * p.w);		}
		//! Operator for HPoint Scale = HPoint * float;
		__forceinline	HPoint		operator*(float s)					const		{ return HPoint(x * s, y * s, z * s, w * s);				}
		//! Operator for HPoint Scale = float * HPoint;
		__forceinline	friend	HPoint	operator*(float s, const HPoint& p)			{ return HPoint(s * p.x, s * p.y, s * p.z, s * p.w);		}

		//! Operator for HPoint Div   = HPoint / HPoint;
		__forceinline	HPoint		operator/(const HPoint& p)			const		{ return HPoint(x / p.x, y / p.y, z / p.z, w / p.w);		}
		//! Operator for HPoint Scale = HPoint / float;
		__forceinline	HPoint		operator/(float s)					const		{ s = 1.0f / s; return HPoint(x * s, y * s, z * s, w * s);	}
		//! Operator for HPoint Scale = float / HPoint;
		__forceinline	friend	HPoint	operator/(float s, const HPoint& p)			{ return HPoint(s / p.x, s / p.y, s / p.z, s / p.w);		}

		//! Operator for float DotProd = HPoint | HPoint;
		__forceinline	float		operator|(const HPoint& p)			const		{ return x*p.x + y*p.y + z*p.z + w*p.w;						}
		// No cross-product in 4D

		//! Operator for HPoint += HPoint;
		__forceinline	HPoint&		operator+=(const HPoint& p)						{ x += p.x; y += p.y; z += p.z; w += p.w;	return *this;	}
		//! Operator for HPoint += float;
		__forceinline	HPoint&		operator+=(float s)								{ x += s;   y += s;   z += s;   w += s;		return *this;	}

		//! Operator for HPoint -= HPoint;
		__forceinline	HPoint&		operator-=(const HPoint& p)						{ x -= p.x; y -= p.y; z -= p.z; w -= p.w;	return *this;	}
		//! Operator for HPoint -= float;
		__forceinline	HPoint&		operator-=(float s)								{ x -= s;   y -= s;   z -= s;   w -= s;		return *this;	}

		//! Operator for HPoint *= HPoint;
		__forceinline	HPoint&		operator*=(const HPoint& p)						{ x *= p.x; y *= p.y; z *= p.z; w *= p.w;	return *this;	}
		//! Operator for HPoint *= float;
		__forceinline	HPoint&		operator*=(float s)								{ x*=s; y*=s; z*=s; w*=s;					return *this;	}

		//! Operator for HPoint /= HPoint;
		__forceinline	HPoint&		operator/=(const HPoint& p)						{ x /= p.x; y /= p.y; z /= p.z; w /= p.w;	return *this;	}
		//! Operator for HPoint /= float;
		__forceinline	HPoint&		operator/=(float s)								{ s = 1.0f / s; x*=s; y*=s; z*=s; w*=s;		return *this;	}

		// Arithmetic operators
		//! Operator for Point Mul = HPoint * Matrix3x3;
//						Point		operator*(const Matrix3x3& mat)		const;
		//! Operator for HPoint Mul = HPoint * Matrix4x4;
//						HPoint		operator*(const Matrix4x4& mat)		const;

		// HPoint *= Matrix3x3 doesn't exist, the matrix is first casted to a 4x4
		//! Operator for HPoint *= Matrix4x4
//						HPoint&		operator*=(const Matrix4x4& mat);

		// Cast operators
		//! Cast a HPoint to a Point. w is discarded.
		__forceinline				operator	Point()					const		{ return Point(x, y, z);									}

		public:
			float	w;
	};

#endif // __ICEHPOINT_H__

