#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"
#include "level.h"


Fvector CHelicopterMovManager::makeIntermediateKey(Fvector& start, 
												   Fvector& dest, 
												   float k)
{
	Fvector point;
	point.lerp(start, dest, k);
//	point.add( Fvector().random_dir().mul(m_intermediateKeyRandFactor) );
	getPathAltitude(point);
	return point;
}



void CHelicopterMovManager::createLevelPatrolTrajectory(u32 keyCount, 
														const Fvector& fromPos, 
														xr_vector<Fvector>& keys )
{
	Fvector					keyPoint;	//working
	Fvector					prevPoint;	//pre-last point
	Fvector					lastPoint;
	Fvector					dir;

	Fbox levelBox = m_boundingVolume;
	keyPoint = fromPos;
	VERIFY(Fbox(levelBox).scale(0.05f).contains(keyPoint));
	keys.push_back(keyPoint);
	lastPoint = keyPoint;
	
	if(CHelicopterMotion::KeyCount()>1) {
		CHelicopterMotion::GetKeyT(CHelicopterMotion::KeyCount()-2, prevPoint);
		makeNewPoint(prevPoint, lastPoint, levelBox, keyPoint);
	}else {
		selectSafeDir(lastPoint, levelBox, dir);
		makeNewPoint(levelBox, lastPoint, dir, keyPoint);
	};

	VERIFY(Fbox(levelBox).scale(0.05f).contains(keyPoint));
	keys.push_back(keyPoint);
	prevPoint = lastPoint;

	for(u32 i = 0; i<keyCount; ++i)	{
		lastPoint = keyPoint;

		makeNewPoint(prevPoint, lastPoint, levelBox, keyPoint);
		VERIFY(Fbox(levelBox).scale(0.05f).contains(keyPoint));
		
		
		prevPoint = lastPoint;
		keys.push_back(keyPoint);
	};

}
void CHelicopterMovManager::makeNewPoint (const Fbox& fbox, 
										  const Fvector& point, 
										  const Fvector& direction, 
										  Fvector& newPoint)
{
	Fvector dir = direction;
	//find new point
	float kp1,kp2,kp3,kp4;
	//plane1
	kp1 = (fbox.max.x-point.x)/dir.x;
	//plane2
	kp2 = (fbox.max.z-point.z)/dir.z;
	//plane3
	kp3 = (fbox.min.x-point.x)/dir.x;
	//plane4
	kp4 = (fbox.min.z-point.z)/dir.z;

	float resK = flt_max;

	if((kp1>EPS_L)&&(kp1<resK))
		resK = kp1;

	if((kp2>EPS_L)&&(kp2<resK))
		resK = kp2;

	if((kp3>EPS_L)&&(kp3<resK))
		resK = kp3;

	if((kp4>EPS_L)&&(kp4<resK))
		resK = kp4;


	dir.y = 0.0f;
	newPoint.add(point, dir.mul(resK) );

	getPathAltitude(newPoint);


}

void CHelicopterMovManager::makeNewPoint(	const Fvector& prevPoint, 
											const Fvector& point,
											const Fbox& fbox,
											Fvector& newPoint)
{
	u16 planeID		= getPlaneID(fbox, point); 

	Fvector prevDir;
	prevDir.sub(point, prevPoint);
	prevDir.y = 0.0f;
	prevDir.normalize_safe();

	VERIFY(fsimilar(prevDir.y, 0.0f));

	Fvector newDir;

	getReflectDir(prevDir, planeID, newDir);
	newDir.normalize_safe();

	VERIFY(fsimilar(newDir.y, 0.0f));
	makeNewPoint(fbox, point, newDir, newPoint);
}

u16 CHelicopterMovManager::getPlaneID(const Fbox& box, const Fvector& point)
{
//					(2)
//		----------------------------maxZ
//		|						|
//	 (3)|		level Box		|(1)
//		|						|
//		----------------------------minZ
//		|minX		(4)			|maxX

	if( fsimilar(box.min.x, point.x, EPS_L) )
		return 3;

	if( fsimilar(box.max.x, point.x, EPS_L) )
		return 1;

	if( fsimilar(box.min.z, point.z, EPS_L) )
		return 4;

	if( fsimilar(box.max.z, point.z, EPS_L) )
		return 2;

	return 5;
}


void CHelicopterMovManager::getReflectDir(const Fvector& dir,
										  const u16 planeID,
										  Fvector& newDir)
{
	switch (planeID) {
		case 1:
		case 3:
			{
				newDir.set(-dir.x, dir.y, dir.z);
				if(_abs(newDir.z)<0.3f) {
					float sign;
					(newDir.z>0.0f)?sign=1.0f:sign=-1.0f;
					newDir.z += sign*0.3f;
				}
				break;
			}
		case 2:
		case 4:
			{
				newDir.set(dir.x, dir.y, -dir.z);
				if(_abs(newDir.x)<0.3f) {
					float sign;
					(newDir.x>0.0f)?sign=1.0f:sign=-1.0f;
					newDir.x += sign*0.3f;
				}
				break;
			}
		case 5:
			{
				newDir.set(dir.x, 0.0f, dir.z);
			}
	};

}

void CHelicopterMovManager::selectSafeDir(const Fvector& prevPoint,
										  const Fbox& fbox, 
										  Fvector& newDir)
{
	Fvector pt = prevPoint;
	newDir = m_startDir;
	newDir.normalize_safe();
	VERIFY(fsimilar(newDir.y, 0.0f));

	while( !fbox.contains(pt.add(newDir)) ) {
		pt = prevPoint;
		
		float h ;
		float p ;
		newDir.getHP(h,p);
		h += PI_DIV_2;
		newDir.setHP(h,p);
	}

	newDir.y = 0.0f;
}

void CHelicopterMovManager::getPathAltitude (Fvector& point)
{
	Collide::rq_result		cR;
	Fvector down_dir;
	down_dir.set(0.0f, -1.0f, 0.0f);

//	Fbox levelBox = Level().ObjectSpace.GetBoundingVolume();
	Fbox levelBox = m_boundingVolume;
	point.y = levelBox.max.y+EPS_L;

	Level().ObjectSpace.RayPick(point, down_dir, levelBox.max.y-levelBox.min.y+1.0f, Collide::rqtStatic, cR);
	
	point.y = point.y-cR.range;

	if( point.y+m_baseAltitude < levelBox.max.y )
		point.y += m_baseAltitude;
	else
		point.y = levelBox.max.y-EPS_L;

	VERIFY( levelBox.max.y > point.y );

	float minY = levelBox.min.y+(levelBox.max.y-levelBox.min.y)*m_alt_korridor;
	float maxY = levelBox.max.y+m_baseAltitude;
//	minY = maxY-EPS_L;
	clamp (point.y,minY,maxY);
	VERIFY( levelBox.max.y > point.y );

}
