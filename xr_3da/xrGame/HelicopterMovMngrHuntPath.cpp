#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"


void		
CHelicopterMovementManager::buildHuntPath(Fvector& enemyPos)
{
	Fvector curPoint	= helicopter()->XFORM().c;
	
	Fvector radius;
	radius.set( enemyPos.z-curPoint.z, 0.0f, -(enemyPos.x-curPoint.x) ).normalize_safe();
//	radius.mul(HELI_HUNT_RADIUS);
	radius.mul(2.0f);

	Fvector destPoint;
	destPoint.add(enemyPos, radius);

	Fvector destDir;
	destDir.sub(enemyPos, destPoint);

	m_keyTrajectory.clear();
	addCurrentPosToTrajectory();

	m_keyTrajectory.push_back( SWayPoint(destPoint, destDir) );

	m_currKeyIdx = 0;
	build_smooth_path(0, true, false);

}