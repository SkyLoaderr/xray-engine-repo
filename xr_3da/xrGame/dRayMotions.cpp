#include "stdafx.h"
#include "../dCylinder/dCylinder.h"
struct dxRayMotions
{
	static dGeomID ray;
};

dGeomID dxRayMotions::ray=0;


int dRayMotionsClassUser = -1;

#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))






int dCollideRMB (dxGeom *o1, dxGeom *o2, int flags,
				  dContactGeom *contact, int skip)
{
return 0;
}

int dCollideRMS(dxGeom *o1, dxGeom *o2, int flags,
				 dContactGeom *contact, int skip)
{
return 0;
}

int dCollideRMCyl (dxGeom *o1, dxGeom *o2, int flags,
				 dContactGeom *contact, int skip)
{
return 0;
}

static  dColliderFn * dRayMotionsColliderFn (int num)
{
	if (num == dBoxClass) return (dColliderFn *) &dCollideRMB;
	if (num == dSphereClass) return (dColliderFn *) &dCollideRMS;
	if (num == dCylinderClassUser) return (dColliderFn *) &dCollideRMCyl;
	return 0;
}


static  void dRayMotionsAABB (dxGeom *geom, dReal aabb[6])
{
	aabb[0] = -dInfinity;
	aabb[1] = dInfinity;
	aabb[2] = -dInfinity;
	aabb[3] = dInfinity;
	aabb[4] = -dInfinity;
	aabb[5] = dInfinity;
}
dxGeom *dCreateRayMotions (dSpaceID space)
{
	if (dRayMotionsClassUser == -1)
	{
		dGeomClass c;
		c.bytes = sizeof (dxRayMotions);
		c.collider = &dRayMotionsColliderFn;
		c.aabb = &dRayMotionsAABB;
		c.aabb_test = 0;
		c.dtor = 0;
		dRayMotionsClassUser =dCreateGeomClass (&c);

	}
	dGeomID g = dCreateGeom (dRayMotionsClassUser);
	if (space) dSpaceAdd (space,g);
	//dxRayMotions	*c = (dxRayMotions*) dGeomGetClassData(g);
	return g;
}
