#ifndef D_TRI_SPHERE_H
#define D_TRI_SPHERE_H
#include "TriPrimitiveCollideClassDef.h"
#include "../MathUtils.h"
IC float	dSphereProj(dxGeom* sphere,const dReal* /**normal/**/)
{
	VERIFY (dGeomGetClass(sphere)== dSphereClass);
	return	dGeomSphereGetRadius(sphere);
}

int dSortedTriSphere(const dReal*	/**v1/**/,const dReal*	/**v2/**/,
					 const dReal* triAx,
					 CDB::TRI* T,
					 dReal dist,
					 dxGeom* Sphere,
					 dxGeom* Geometry, 
					 int Flags, 
					 dContactGeom* Contacts,
					 int			skip
					 );

int dTriSphere(const dReal* v0,const dReal* v1,const dReal* v2,
			   Triangle* T,
			   dxGeom* Sphere,dxGeom* Geometry, int Flags, 
			   dContactGeom* Contacts,int /**skip/**/);






TRI_PRIMITIVE_COLIDE_CLASS_DECLARE(Sphere)
#endif