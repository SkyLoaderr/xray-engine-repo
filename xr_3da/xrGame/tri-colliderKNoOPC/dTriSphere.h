#ifndef D_TRI_SPHERE_H
#define D_TRI_SPHERE_H
#include "TriPrimitiveCollideClassDef.h"

IC float	dSphereProj(dxGeom* sphere,const dReal* /**normal/**/)
{
	dIASSERT (dGeomGetClass(sphere)== dSphereClass);
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
			   CDB::TRI* T,
			   dxGeom* Sphere,dxGeom* Geometry, int Flags, 
			   dContactGeom* Contacts,int /**skip/**/);

inline dReal FragmentonSphereTest(const dReal* center, const dReal radius,
								  const dReal* pt1, const dReal* pt2,dReal* norm)
{
	dVector3 direction={pt2[0]-pt1[0],pt2[1]-pt1[1],pt2[2]-pt1[2]};
	dNormalize3(direction);
	dReal center_prg=dDOT(center,direction);
	dReal pt1_prg=dDOT(pt1,direction);
	dReal pt2_prg=dDOT(pt2,direction);
	dReal from1_dist=center_prg-pt1_prg;
	if(center_prg<pt1_prg||center_prg>pt2_prg) return -1;
	dVector3 line_to_center={
		-pt1[0]-direction[0]*from1_dist+center[0],
			-pt1[1]-direction[1]*from1_dist+center[1],
			-pt1[2]-direction[2]*from1_dist+center[2]
	};
	norm[0]=line_to_center[0];
	norm[1]=line_to_center[1];
	norm[2]=line_to_center[2];

	dNormalize3(norm);


	dReal depth=radius-dDOT(line_to_center,norm);
	if(depth<0.f) return -1.f;

	return depth;



}


inline dReal PointSphereTest(const dReal* center, const dReal radius,
							 const dReal* pt,dReal* norm)
{

	norm[0]=center[0]-pt[0];
	norm[1]=center[1]-pt[1];
	norm[2]=center[2]-pt[2];
	dVector3 to_center={norm[0],norm[1],norm[2]};
	dNormalize3(norm);
	dReal depth=radius-dDOT(to_center,norm);

	if(depth>0.f)	return depth;
	else			return -1.f;

}

TRI_PRIMITIVE_COLIDE_CLASS_DECLARE(Sphere)
#endif