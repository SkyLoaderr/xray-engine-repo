///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes a ray-AABB intersection.
 *	Original code by Andrew Woo, from "Graphics Gems", Academic Press, 1990
 *	Optimized code by Pierre Terdiman, 2000 (~20-30% faster on my Celeron 500)
 *	Epsilon value added by Klaus Hartmann. (discarding it saves a few cycles only)
 *
 *	Hence this version is faster as well as more robust than the original one.
 *
 *	Should work provided:
 *	1) the integer representation of 0.0f is 0x00000000
 *	2) the sign bit of the float is the most significant one
 *
 *	Report bugs: p.terdiman@codercorner.com
 *
 *	\param		center		[in] box center
 *	\param		extents		[in] box extents
 *	\param		coord		[out] impact coordinates
 *	\return		true if ray intersects AABB
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__forceinline bool AABBRayCollider::RayAABBOverlap(const Point& center, const Point& extents, Point& coord)
{
	// Stats
	mNbRayBVTests++;

	// We need the box in its (Min, Max) form here. Pity, the (Center, Extents) one is
	// used for tree-tree collision... Nonetheless by lazy-evaluating Min & Max we may
	// save some work. (not done here)
	Point MinB = center - extents;
	Point MaxB = center + extents;

	Point MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;
	BOOL Inside = TRUE;

	// Find candidate planes.
	for(udword i=0;i<3;i++)
	{
		if(mOrigin[i] < MinB[i])
		{
			coord[i]	= MinB[i];
			Inside		= FALSE;

			// Calculate T distances to candidate planes
			MaxT[i] = (MinB[i] - mOrigin[i]) * mOneOverDir[i];
		}
		else if(mOrigin[i] > MaxB[i])
		{
			coord[i]	= MaxB[i];
			Inside		= FALSE;

			// Calculate T distances to candidate planes
			MaxT[i] = (MaxB[i] - mOrigin[i]) * mOneOverDir[i];
		}
	}

	// Ray origin inside bounding box
	if(Inside)
	{
		coord = mOrigin;
		return true;
	}

	// Get largest of the maxT's for final choice of intersection
	udword WhichPlane = 0;
	if(MaxT[1] > MaxT[WhichPlane])	WhichPlane = 1;
	if(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

	// Check final candidate actually inside box
	if(IR(MaxT[WhichPlane])&0x80000000) return false;

	for(i=0;i<3;i++)
	{
		if(i!=WhichPlane)
		{
			coord[i] = mOrigin[i] + MaxT[WhichPlane] * mDir[i];
#ifdef OPC_RAYAABB_EPSILON
			if(coord[i] < MinB[i] - OPC_RAYAABB_EPSILON || coord[i] > MaxB[i] + OPC_RAYAABB_EPSILON)	return false;
#else
			if(coord[i] < MinB[i] || coord[i] > MaxB[i])	return false;
#endif
		}
	}
	return true;	// ray hits box
}
