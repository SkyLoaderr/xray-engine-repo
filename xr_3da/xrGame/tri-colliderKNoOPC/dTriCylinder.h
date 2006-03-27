#ifndef D_TRI_CYLINDER_H
#define D_TRI_CYLINDER_H
#include "../dCylinder/dCylinder.h"
#include "TriPrimitiveCollideClassDef.h"
struct dxCylinder {	// cylinder
	dReal radius,lz;	// radius, length along z axis */
};

int dTriCyl (
						const dReal* v0,const dReal* v1,const dReal* v2,
						Triangle* T,
						dxGeom *o1, dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						);
int dSortedTriCyl (
				   const dReal* triSideAx0,const dReal* triSideAx1,
				   const dReal* triAx,
				   //const dReal* v0,
				   //const dReal* v1,
				   //const dReal* v2,
				   CDB::TRI* T,
				   dReal dist,
				   dxGeom *o1, dxGeom *o2,
				   int flags, dContactGeom *contact, int skip
				   );

IC float	dCylProj(dxGeom* cylinder,const dReal* normal)
{
	VERIFY(dGeomGetClass(cylinder)== dCylinderClassUser);
	float hlz,radius;
	dGeomCylinderGetParams(cylinder,&radius,&hlz);
	const dReal* R=dGeomGetRotation(cylinder);
	hlz*=0.5f;
	float cos1=dFabs(dDOT14(normal,R+1));
	cos1=cos1<REAL(1.) ? cos1 : REAL(1.); //cos1 may slightly exeed 1.f
	float sin1=_sqrt(REAL(1.)-cos1*cos1);
	//////////////////////////////
	return cos1*hlz+sin1*radius;
}

TRI_PRIMITIVE_COLIDE_CLASS_DECLARE(Cyl)
#endif