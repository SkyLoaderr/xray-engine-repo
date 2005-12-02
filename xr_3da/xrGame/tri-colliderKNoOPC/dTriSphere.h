#ifndef D_TRI_SPHERE_H
#define D_TRI_SPHERE_H
#include "TriPrimitiveCollideClassDef.h"
#include "../MathUtils.h"
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
			   Triangle* T,
			   dxGeom* Sphere,dxGeom* Geometry, int Flags, 
			   dContactGeom* Contacts,int /**skip/**/);

inline dReal FragmentonSphereTest(const dReal* center, const dReal radius,
								  const dReal* pt1, const dReal* pt2,dReal* norm)
{
	dVector3 direction={pt2[0]-pt1[0],pt2[1]-pt1[1],pt2[2]-pt1[2]};
	cast_fv(direction).normalize();
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

	float mag=dSqrt(dDOT(line_to_center,line_to_center));
	//dNormalize3(norm);
	

	dReal depth=radius-mag;
	if(depth<0.f) return -1.f;
	if(mag>0.f)
	{
		norm[0]=line_to_center[0]/mag;
		norm[1]=line_to_center[1]/mag;
		norm[2]=line_to_center[2]/mag;
	}
	else
	{
		norm[0]=0;
		norm[1]=1;
		norm[2]=0;
	}
	return depth;
}


inline dReal PointSphereTest(const dReal* center, const dReal radius,
							 const dReal* pt,dReal* norm)
{

	norm[0]=center[0]-pt[0];
	norm[1]=center[1]-pt[1];
	norm[2]=center[2]-pt[2];
	dReal mag=dSqrt(dDOT(norm,norm));
	dReal depth=radius-dSqrt(dDOT(norm,norm));
	if(depth<0.f)	return -1.f;
	if(mag>0.f)
	{
		norm[0]/=mag;norm[1]/=mag;norm[2]/=mag;
	}
	else 
	{
		norm[0]=0;norm[1]=1;norm[2]=0;
	}
	return depth;
	
}

TRI_PRIMITIVE_COLIDE_CLASS_DECLARE(Sphere)
#endif