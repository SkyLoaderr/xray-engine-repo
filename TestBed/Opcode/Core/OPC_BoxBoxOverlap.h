///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	OBB-OBB overlap test using the separating axis theorem.
 *	- original code by Gomez / Gamasutra (similar to Gottschalk's one in RAPID)
 *	- optimized for AABB trees by computing the rotation matrix once (SOLID-fashion)
 *	- the fabs matrix is precomputed as well and epsilon-tweaked (RAPID-style, we found this almost mandatory)
 *	- Class III axes can be disabled... (SOLID & Intel fashion)
 *	- ...or enabled to perform some profiling
 *	- CPU comparisons used when appropriate
 *	- lazy evaluation sometimes saves some work in case of early exits (unlike SOLID)
 *
 *	\param		a			[in] extents from box A
 *	\param		Pa			[in] center from box A
 *	\param		b			[in] extents from box B
 *	\param		Pb			[in] center from box B
 *	\return		true if boxes overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__forceinline bool AABBTreeCollider::BoxBoxOverlap(const Point& a, const Point& Pa, const Point& b, const Point& Pb)
{
	// Stats
	mNbBVBVTests++;

	float t,t2;

	// Class I : A's basis vectors
#ifdef OPC_CPU_COMPARE
	float Tx = (mR1to0.m[0][0]*Pb.x + mR1to0.m[1][0]*Pb.y + mR1to0.m[2][0]*Pb.z) + mT1to0.x - Pa.x;
	t = a.x + b.x*mAR.m[0][0] + b.y*mAR.m[1][0] + b.z*mAR.m[2][0];
	if(AIR(Tx) > IR(t))	return false;

	float Ty = (mR1to0.m[0][1]*Pb.x + mR1to0.m[1][1]*Pb.y + mR1to0.m[2][1]*Pb.z) + mT1to0.y - Pa.y;
	t = a.y + b.x*mAR.m[0][1] + b.y*mAR.m[1][1] + b.z*mAR.m[2][1];
	if(AIR(Ty) > IR(t))	return false;

	float Tz = (mR1to0.m[0][2]*Pb.x + mR1to0.m[1][2]*Pb.y + mR1to0.m[2][2]*Pb.z) + mT1to0.z - Pa.z;
	t = a.z + b.x*mAR.m[0][2] + b.y*mAR.m[1][2] + b.z*mAR.m[2][2];
	if(AIR(Tz) > IR(t))	return false;
#else
	float Tx = (mR1to0.m[0][0]*Pb.x + mR1to0.m[1][0]*Pb.y + mR1to0.m[2][0]*Pb.z) + mT1to0.x - Pa.x;
	t = a.x + b.x*mAR.m[0][0] + b.y*mAR.m[1][0] + b.z*mAR.m[2][0];
	if(fabsf(Tx) > t)	return false;

	float Ty = (mR1to0.m[0][1]*Pb.x + mR1to0.m[1][1]*Pb.y + mR1to0.m[2][1]*Pb.z) + mT1to0.y - Pa.y;
	t = a.y + b.x*mAR.m[0][1] + b.y*mAR.m[1][1] + b.z*mAR.m[2][1];
	if(fabsf(Ty) > t)	return false;

	float Tz = (mR1to0.m[0][2]*Pb.x + mR1to0.m[1][2]*Pb.y + mR1to0.m[2][2]*Pb.z) + mT1to0.z - Pa.z;
	t = a.z + b.x*mAR.m[0][2] + b.y*mAR.m[1][2] + b.z*mAR.m[2][2];
	if(fabsf(Tz) > t)	return false;
#endif

	// Class II : B's basis vectors
#ifdef OPC_CPU_COMPARE
	t = Tx*mR1to0.m[0][0] + Ty*mR1to0.m[0][1] + Tz*mR1to0.m[0][2];	t2 = a.x*mAR.m[0][0] + a.y*mAR.m[0][1] + a.z*mAR.m[0][2] + b.x;
	if(AIR(t)>IR(t2))	return false;

	t = Tx*mR1to0.m[1][0] + Ty*mR1to0.m[1][1] + Tz*mR1to0.m[1][2];	t2 = a.x*mAR.m[1][0] + a.y*mAR.m[1][1] + a.z*mAR.m[1][2] + b.y;
	if(AIR(t)>IR(t2))	return false;

	t = Tx*mR1to0.m[2][0] + Ty*mR1to0.m[2][1] + Tz*mR1to0.m[2][2];	t2 = a.x*mAR.m[2][0] + a.y*mAR.m[2][1] + a.z*mAR.m[2][2] + b.z;
	if(AIR(t)>IR(t2))	return false;
#else
	t = Tx*mR1to0.m[0][0] + Ty*mR1to0.m[0][1] + Tz*mR1to0.m[0][2];	t2 = a.x*mAR.m[0][0] + a.y*mAR.m[0][1] + a.z*mAR.m[0][2] + b.x;
	if(fabsf(t) > t2)	return false;

	t = Tx*mR1to0.m[1][0] + Ty*mR1to0.m[1][1] + Tz*mR1to0.m[1][2];	t2 = a.x*mAR.m[1][0] + a.y*mAR.m[1][1] + a.z*mAR.m[1][2] + b.y;
	if(fabsf(t) > t2)	return false;

	t = Tx*mR1to0.m[2][0] + Ty*mR1to0.m[2][1] + Tz*mR1to0.m[2][2];	t2 = a.x*mAR.m[2][0] + a.y*mAR.m[2][1] + a.z*mAR.m[2][2] + b.z;
	if(fabsf(t) > t2)	return false;
#endif

	// Class III : 9 cross products
	// Cool trick: always perform the full test for first level, regardless of settings.
	// That way pathological cases (such as the pencils scene) are quickly rejected anyway !
	if(mFullBoxBoxTest || mNbBVBVTests==1)
	{
#ifdef OPC_CPU_COMPARE
		t = Tz*mR1to0.m[0][1] - Ty*mR1to0.m[0][2];	t2 = a.y*mAR.m[0][2] + a.z*mAR.m[0][1] + b.y*mAR.m[2][0] + b.z*mAR.m[1][0];	if(AIR(t) > IR(t2))	return false;	// L = A0 x B0
		t = Tz*mR1to0.m[1][1] - Ty*mR1to0.m[1][2];	t2 = a.y*mAR.m[1][2] + a.z*mAR.m[1][1] + b.x*mAR.m[2][0] + b.z*mAR.m[0][0];	if(AIR(t) > IR(t2))	return false;	// L = A0 x B1
		t = Tz*mR1to0.m[2][1] - Ty*mR1to0.m[2][2];	t2 = a.y*mAR.m[2][2] + a.z*mAR.m[2][1] + b.x*mAR.m[1][0] + b.y*mAR.m[0][0];	if(AIR(t) > IR(t2))	return false;	// L = A0 x B2
		t = Tx*mR1to0.m[0][2] - Tz*mR1to0.m[0][0];	t2 = a.x*mAR.m[0][2] + a.z*mAR.m[0][0] + b.y*mAR.m[2][1] + b.z*mAR.m[1][1];	if(AIR(t) > IR(t2))	return false;	// L = A1 x B0
		t = Tx*mR1to0.m[1][2] - Tz*mR1to0.m[1][0];	t2 = a.x*mAR.m[1][2] + a.z*mAR.m[1][0] + b.x*mAR.m[2][1] + b.z*mAR.m[0][1];	if(AIR(t) > IR(t2))	return false;	// L = A1 x B1
		t = Tx*mR1to0.m[2][2] - Tz*mR1to0.m[2][0];	t2 = a.x*mAR.m[2][2] + a.z*mAR.m[2][0] + b.x*mAR.m[1][1] + b.y*mAR.m[0][1];	if(AIR(t) > IR(t2))	return false;	// L = A1 x B2
		t = Ty*mR1to0.m[0][0] - Tx*mR1to0.m[0][1];	t2 = a.x*mAR.m[0][1] + a.y*mAR.m[0][0] + b.y*mAR.m[2][2] + b.z*mAR.m[1][2];	if(AIR(t) > IR(t2))	return false;	// L = A2 x B0
		t = Ty*mR1to0.m[1][0] - Tx*mR1to0.m[1][1];	t2 = a.x*mAR.m[1][1] + a.y*mAR.m[1][0] + b.x*mAR.m[2][2] + b.z*mAR.m[0][2];	if(AIR(t) > IR(t2))	return false;	// L = A2 x B1
		t = Ty*mR1to0.m[2][0] - Tx*mR1to0.m[2][1];	t2 = a.x*mAR.m[2][1] + a.y*mAR.m[2][0] + b.x*mAR.m[1][2] + b.y*mAR.m[0][2];	if(AIR(t) > IR(t2))	return false;	// L = A2 x B2
#else
		t = Tz*mR1to0.m[0][1] - Ty*mR1to0.m[0][2];	t2 = a.y*mAR.m[0][2] + a.z*mAR.m[0][1] + b.y*mAR.m[2][0] + b.z*mAR.m[1][0];	if(fabsf(t) > t2)	return false;
		t = Tz*mR1to0.m[1][1] - Ty*mR1to0.m[1][2];	t2 = a.y*mAR.m[1][2] + a.z*mAR.m[1][1] + b.x*mAR.m[2][0] + b.z*mAR.m[0][0];	if(fabsf(t) > t2)	return false;
		t = Tz*mR1to0.m[2][1] - Ty*mR1to0.m[2][2];	t2 = a.y*mAR.m[2][2] + a.z*mAR.m[2][1] + b.x*mAR.m[1][0] + b.y*mAR.m[0][0];	if(fabsf(t) > t2)	return false;
		t = Tx*mR1to0.m[0][2] - Tz*mR1to0.m[0][0];	t2 = a.x*mAR.m[0][2] + a.z*mAR.m[0][0] + b.y*mAR.m[2][1] + b.z*mAR.m[1][1];	if(fabsf(t) > t2)	return false;
		t = Tx*mR1to0.m[1][2] - Tz*mR1to0.m[1][0];	t2 = a.x*mAR.m[1][2] + a.z*mAR.m[1][0] + b.x*mAR.m[2][1] + b.z*mAR.m[0][1];	if(fabsf(t) > t2)	return false;
		t = Tx*mR1to0.m[2][2] - Tz*mR1to0.m[2][0];	t2 = a.x*mAR.m[2][2] + a.z*mAR.m[2][0] + b.x*mAR.m[1][1] + b.y*mAR.m[0][1];	if(fabsf(t) > t2)	return false;
		t = Ty*mR1to0.m[0][0] - Tx*mR1to0.m[0][1];	t2 = a.x*mAR.m[0][1] + a.y*mAR.m[0][0] + b.y*mAR.m[2][2] + b.z*mAR.m[1][2];	if(fabsf(t) > t2)	return false;
		t = Ty*mR1to0.m[1][0] - Tx*mR1to0.m[1][1];	t2 = a.x*mAR.m[1][1] + a.y*mAR.m[1][0] + b.x*mAR.m[2][2] + b.z*mAR.m[0][2];	if(fabsf(t) > t2)	return false;
		t = Ty*mR1to0.m[2][0] - Tx*mR1to0.m[2][1];	t2 = a.x*mAR.m[2][1] + a.y*mAR.m[2][0] + b.x*mAR.m[1][2] + b.y*mAR.m[0][2];	if(fabsf(t) > t2)	return false;
#endif
	}
	return true;
}
