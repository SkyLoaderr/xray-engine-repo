#ifndef D_TRI_CYLINDER_H
#define D_TRI_CYLINDER_H

struct dxCylinder {	// cylinder
	dReal radius,lz;	// radius, length along z axis */
};

int dTriCyl (
						const dReal* v0,const dReal* v1,const dReal* v2,
						CDB::TRI* T,
						dxGeom *o1, dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						);

#endif