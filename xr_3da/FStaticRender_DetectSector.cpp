#include "stdafx.h"
#include "fstaticrender.h"
#include "xr_creator.h"

CSector* CRender::detectSector(Fvector& P)
{
	XRC.RayMode	(RAY_ONLYNEAREST);
	Fvector dir; dir.set(0,-1,0);

	// Portals model
	int id1 = -1;
	float	range1 = 100.f;
	XRC.RayPick(0,rmPortals,P,dir,range1);
	const RAPID::raypick_info *RP1 = XRC.GetMinRayPickInfo();
	if (RP1) { id1 = RP1->id; range1 = RP1->range; }

	// Geometry model
	int id2 = -1;
	float	range2 = range1;

	XRC.RayPick(0,pCreator->ObjectSpace.GetStaticModel(),P,dir,range2);
	const RAPID::raypick_info *RP2 = XRC.GetMinRayPickInfo();
	if (RP2) { id2 = RP2->id; range2 = RP2->range; }

	// Select ID
	int ID;
	if (id1>=0) {
		if (id2>=0) ID = (range1<=range2+EPS)?id1:id2;	// both was found
		else ID = id1;									// only id1 found
	} else if (id2>=0) ID = id2;						// only id2 found
	else return 0;

	if (ID==id1) {
		// Take sector, facing to our point from portal
		RAPID::tri*	pTri	= rmPortals->tris + ID;
		CPortal*	pPortal	= (CPortal*) pTri->dummy;
		return pPortal->getSectorFacing(P);
	} else {
		// Take triangle at ID and use it's Sector
		RAPID::tri* pTri	= pCreator->ObjectSpace.GetStaticTris()+ID;
		return getSector(pTri->sector);
	}
}

