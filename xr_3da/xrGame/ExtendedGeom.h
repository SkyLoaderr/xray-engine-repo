#ifndef EXTENDED_GEOM
#define EXTENDED_GEOM
#include "PHObject.h"
#include <ode/ode.h>
enum Material {
weels,
cardboard,
mesh_default,
car_cabin,
weapon_default,
matrerial_default
};

struct ContactsParameters{
	dReal damping;
	dReal spring;
	dReal bonce;
	dReal bonce_vel;
	dReal mu;
	unsigned int maxc;

};

struct Triangle {
	dReal* v0;
	dReal* v1;
	dReal* v2;
	dVector3 side0;
	dVector3 side1;
	dVector3 norm;
	dReal dist;
	dReal pos;
	dReal depth;
	CDB::TRI* T ;
	bool compare (Triangle t1){

		if(t1.v0[0]!=v0[0] ||
			t1.v0[1]!=v0[1] ||
			t1.v0[2]!=v0[2] ||
			t1.v1[0]!=v1[0] ||
			t1.v1[1]!=v1[1] ||
			t1.v1[2]!=v1[2] ||
			t1.v2[0]!=v2[0] ||
			t1.v2[1]!=v2[1] ||
			t1.v2[2]!=v2[2]) return false;
		else return true;


	}
};


struct dxGeomUserData {
dVector3 last_pos;
bool pushing_neg,pushing_b_neg;
Triangle neg_tri,b_neg_tri;
dReal friction;
CPHObject* ph_object;
};

inline void dGeomCreateUserData(dxGeom* geom){
if(!geom) return;
dGeomSetData(geom,new dxGeomUserData());
((dxGeomUserData*)dGeomGetData(geom))->pushing_neg=false;
((dxGeomUserData*)dGeomGetData(geom))->pushing_b_neg=false;
((dxGeomUserData*)dGeomGetData(geom))->last_pos[0]=dInfinity;
((dxGeomUserData*)dGeomGetData(geom))->last_pos[1]=dInfinity;
((dxGeomUserData*)dGeomGetData(geom))->last_pos[2]=dInfinity;
((dxGeomUserData*)dGeomGetData(geom))->ph_object=NULL;
}

inline void dGeomDestroyUserData(dxGeom* geom){
if(!geom) return;
if(dGeomGetData(geom)) delete dGeomGetData(geom);
dGeomSetData(geom,0);
}

inline dxGeomUserData* dGeomGetUserData(dxGeom* geom){
return (dxGeomUserData*) dGeomGetData(geom);
}

inline void dGeomUserDataSetFriction(dxGeom* geom,dReal friction){
((dxGeomUserData*)dGeomGetData(geom))->friction=friction;

}

inline void dGeomUserDataSetPhObject(dxGeom* geom,CPHObject* phObject){
((dxGeomUserData*)dGeomGetData(geom))->ph_object=phObject;

}
#endif