///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for rays.
 *	\file		IceRay.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICERAY_H__
#define __ICERAY_H__

	class ICEMATHS_API Ray
	{
		public:
		//! Constructor
		__forceinline				Ray()	{}
		//! Destructor
		__forceinline				~Ray()	{}

						Point		mOrig;		//!< Ray origin
						Point		mDir;		//!< Normed direction
	};

	class ICEMATHS_API Segment
	{
		public:
		//! Constructor
		__forceinline				Segment()	{}
		//! Destructor
		__forceinline				~Segment()	{}

						Point		mP0;		//!< Start of segment
						Point		mP1;		//!< End of segment
	};

	__forceinline void ComputeReflexionVector(Point& reflected, const Point& source, const Point& impact, const Point& normal)
	{
		Point V = impact - source;
		reflected = V - normal * 2.0f * (V|normal);
	}

	__forceinline void ComputeReflexionVector(Point& reflected, const Point& dir, const Point& normal)
	{
		reflected = dir - normal * 2.0f * (dir|normal);
	}

#endif // __ICERAY_H__
