///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code to compute the minimal bounding sphere.
 *	\file		IceBoundingSphere.h
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEBOUNDINGSPHERE_H__
#define __ICEBOUNDINGSPHERE_H__

	class MESHMERIZER_API Sphere
	{
		public:
		// Constructor/Destructor
											Sphere()																		{}
											Sphere(const Point& center, float radius) : mCenter(center), mRadius(radius)	{}
											Sphere(const udword n, Point* p);

						bool				Compute(udword nbverts, Point* verts);
						bool				FastCompute(udword nbverts, Point* verts);

		// Access methods
		__forceinline	const Point&		GetCenter()		const						{ return mCenter; }
		__forceinline	float				GetRadius()		const						{ return mRadius; }

		__forceinline	const Point&		Center()		const						{ return mCenter; }
		__forceinline	float				Radius()		const						{ return mRadius; }

		__forceinline	Sphere&				Set(const Point& center, float radius)		{ mCenter = center; mRadius = radius; return *this; }
		__forceinline	Sphere&				SetCenter(const Point& center)				{ mCenter = center; return *this; }
		__forceinline	Sphere&				SetRadius(float radius)						{ mRadius = radius; return *this; }

		public:
						Point				mCenter;		//!< Sphere center
						float				mRadius;		//!< Sphere radius
	};

#endif // __ICEBOUNDINGSPHERE_H__
