#include "stdafx.h"
#include "HelicopterMovementManager.h"

bool dice()
{
	return (::Random.randF(-1.0f, 1.0f) > 0.0f);
}

#define KEY_MAX_DIST  50.0f

void		
CHelicopterMovementManager::createLevelPatrolTrajectory(u32 keyCount, xr_vector<Fvector>& keyPoints)
{
	Fvector		keyPoint;
	Fvector		down_dir;
	bool		useXBound;
	bool		min_max;
	float		desireH = 10.0f;

	Collide::rq_result R;

	down_dir.set(0.0f, -1.0f, 0.0f);

	Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
	
	for(u32 i = 0; i<keyCount; ++i)	{
		useXBound	= dice();
		min_max		= dice();

		if(useXBound){
			(min_max)?keyPoint.x = level_box.min.x:keyPoint.x = level_box.max.x;
			keyPoint.z = ::Random.randF(level_box.min.z, level_box.max.z);
		}else{
			(min_max)?keyPoint.z = level_box.min.z:keyPoint.z = level_box.max.z;
			keyPoint.x = ::Random.randF(level_box.min.x, level_box.max.x);
		}

		keyPoint.y = 25.0f;
/*		keyPoint.y = ::Random.randF(30.0f, 40.0f);
		keyPoint.y = level_box.max.y;
		Level().ObjectSpace.RayPick(keyPoint, down_dir, level_box.max.y-level_box.min.y+1.0f, Collide::rqtStatic, R);
		
		keyPoint.y = keyPoint.y-R.range+desireH;
*/
		//промежуточные точки
		if( keyPoints.size() )
		{
			Fvector& prevPoint = keyPoints.back();
			float dist = prevPoint.distance_to(keyPoint);
			float k = (dist / KEY_MAX_DIST) - 1.0f;
			for( float i=1; i<k; ++i )
			{
				Fvector p = makeIntermediateKey(prevPoint, keyPoint, (i/(k+1.0f)) );
				keyPoints.push_back(p);
			}

		}

		keyPoints.push_back(keyPoint);
	}
	
}

Fvector		
CHelicopterMovementManager::makeIntermediateKey(Fvector& start, Fvector& dest, float k)
{
	Fvector point;
	point.lerp(start, dest, k);
	point.add( Fvector().random_dir().mul(10.0f) );
	return point;
}
