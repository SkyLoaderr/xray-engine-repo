///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for planes.
 *	\file		IcePlane.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEPLANE_H__
#define __ICEPLANE_H__

	#define PLANE_EPSILON		(1.0e-7f)

	class ICEMATHS_API	Plane
	{
		public:
		// Constructors, destructor, copy constructor
		__forceinline				Plane()															{}
		__forceinline				Plane(float nx, float ny, float nz, float d)					{ Set(nx, ny, nz, d);								}
		__forceinline				Plane(const Point &p, const Point &n)							{ Set(p, n);										}
		__forceinline				Plane(const Point &p0, const Point &p1, const Point &p2)		{ Compute(p0, p1, p2);								}
		__forceinline				Plane(const Point &n, float d)									{ this->n = n; this->d = d;							}

		__forceinline	Plane&		Zero()															{ n.Zero(); d = 0.0f;				return *this;	}
		__forceinline	Plane&		Set(float nx, float ny, float nz, float d)						{ n.Set(nx, ny, nz); this->d = d;	return *this;	}
		__forceinline	Plane&		Set(const Point &p, const Point &n)								{ this->n = n; d = - p | n;			return *this;	}
						Plane&		Compute(const Point &p0, const Point &p1, const Point &p2);

		__forceinline	float		Distance(const Point& p) const									{ return (p | n) + d;								}
		__forceinline	bool		Belongs(const Point& p) const									{ return fabsf(Distance(p)) < PLANE_EPSILON;		}

		__forceinline	void		Normalize()
		{
			float Denom = 1.0f / n.Magnitude();
			n.x	*= Denom;
			n.y	*= Denom;
			n.z	*= Denom;
			d	*= Denom;
		}

		public:
		// Members
						Point	n;		//!< The normal to the plane
						float	d;		//!< The distance from the origin

		// Cast operators
		__forceinline				operator Point()					const	{ return n;											}
		__forceinline				operator HPoint()					const	{ return HPoint(n, d);								}

		// Arithmetic operators
		__forceinline	Plane		operator+(const Plane& p)			const	{ return Plane(n + p.n, d + p.d);					}
		__forceinline	Plane		operator*(const Matrix4x4& m)		const	{ Plane Ret(*this); return Ret *= m;				}

		__forceinline	Plane&		operator+=(const Plane& p)					{ n += p.n; n.Normalize(); d += p.d; return *this; }	// Is this really useful, Piotr?
		__forceinline	Plane&		operator*=(const Matrix4x4& m)				{ Point n2 = HPoint(n, 0.0f ) * m; d = -((Point) (HPoint( -d*n, 1.0f ) * m) | n2); n = n2; return *this; }
	};

#endif // __ICEPLANE_H__
