//#include "stdafx.h"
#ifndef __DCTRILISTCOLLIDER_INCLUDED__

#define __DCTRILISTCOLLIDER_INCLUDED__

class dcTriListCollider
#ifdef DEBUG
	: public pureRender
#endif
{

	dxGeom* Geometry;	// The geom object

	dxTriList* GeomData;	// Buffered pointer to classdata
#ifdef DEBUG
	Fvector AABB;
	Fvector* BoxCenter;
#endif
public:
	dcTriListCollider(dxGeom* Geometry);
	~dcTriListCollider();

	int CollideBox(dxGeom* Box, int Flags, dContactGeom* Contact, int Stride);
	int CollideSphere(dxGeom* Sphere, int Flags, dContactGeom* Contact, int Stride);
	int CollideCylinder(dxGeom* Cylinder, int Flags, dContactGeom* Contacts, int Stride);
#ifdef DEBUG
	virtual void			OnRender			();
#endif
};

#endif	//__DCTRILISTCOLLIDER_INCLUDED__