#include "stdafx.h"
#include "../dCylinder/dCylinder.h"
//#pragma warning(disable:4995)
//#pragma warning(disable:4267)
//#include "../ode/src/collision_kernel.h"
//#pragma warning(default:4995)
//#pragma warning(default:4267)
//#pragma warning(disable:4995)
//#pragma warning(disable:4267)
#include "../ode/src/collision_std.h"
//#pragma warning(default:4995)
//#pragma warning(default:4267)
struct dxRayMotions
{
	dGeomID ray;
};




int dRayMotionsClassUser = -1;

#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))






int dCollideRMB (dxGeom *o1, dxGeom *o2, int flags,
				  dContactGeom *contact, int skip)
{
	dxRayMotions	*c = (dxRayMotions*) dGeomGetClassData(o1);
	return dCollideRayBox (c->ray,o2, flags,contact,skip);

}

int dCollideRMS(dxGeom *o1, dxGeom *o2, int flags,
				 dContactGeom *contact, int skip)
{
	dxRayMotions	*c = (dxRayMotions*) dGeomGetClassData(o1);
	return dCollideRaySphere (c->ray, o2,flags, contact, skip);
}

int dCollideRMCyl (dxGeom *o1, dxGeom *o2, int flags,
				 dContactGeom *contact, int skip)
{
	dxRayMotions	*c = (dxRayMotions*) dGeomGetClassData(o1);
	return	dCollideRayCCylinder (c->ray, o2,flags,contact,skip);
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
	dxRayMotions	*c = (dxRayMotions*) dGeomGetClassData(geom);
	dGeomGetAABB(c->ray,aabb);
}
void dGeomRayMotionDestroy (dGeomID ray)
{
	dGeomDestroy(((dxRayMotions*)dGeomGetClassData(ray))->ray);
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
		c.dtor = &dGeomRayMotionDestroy;
		dRayMotionsClassUser =dCreateGeomClass (&c);
		
	}
	dGeomID g = dCreateGeom (dRayMotionsClassUser);
	if (space) dSpaceAdd (space,g);
	dxRayMotions	*c = (dxRayMotions*) dGeomGetClassData(g);
	c->ray=dCreateRay(space,REAL(1.));
	return g;
}

void dGeomRayMotionsSet (dGeomID g,const dReal* p,const dReal* d, dReal l)
{
	dxRayMotions	*c = (dxRayMotions*) dGeomGetClassData(g);
	dGeomRaySetLength (c->ray, l);
	dGeomRaySet (c->ray, p[0], p[1], p[2],
		d[0], d[1], d[2]);
	dGeomMoved(g);

}
