
__forceinline bool AABBSphereCollider::SphereAABBOverlap(const Point& center, const Point& extents)
{ 
	// Stats
	mNbSphereBVTests++;

	float d = 0.0f;

	//find the square of the distance
	//from the sphere to the box
	for(udword i=0;i<3;i++)
	{
		float tmp = mCenter[i] - center[i];
		float s = tmp + extents[i];

		if(s<0.0f)
		{
			d += s*s;
		}
		else
		{
			s = tmp - extents[i];

			if(s>0.0f)
			{
				d += s*s;
			}
		}
	}

#ifdef OLDIES
//	Point Min = center - extents;
//	Point Max = center + extents;

	float d = 0.0f;

	//find the square of the distance
	//from the sphere to the box
	for(udword i=0;i<3;i++)
	{
float Min = center[i] - extents[i];

//		if(mCenter[i]<Min[i])
		if(mCenter[i]<Min)
		{
//			float s = mCenter[i] - Min[i];
			float s = mCenter[i] - Min;
			d += s*s;
		}
		else
		{
float Max = center[i] + extents[i];

//			if(mCenter[i]>Max[i])
			if(mCenter[i]>Max)
			{
				float s = mCenter[i] - Max;
				d += s*s;
			}
		}
	}
#endif
	return d <= mRadius2;
}
