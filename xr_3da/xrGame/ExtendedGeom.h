#ifndef EXTENDED_GEOM
#define EXTENDED_GEOM
#include <ode\src\objects.h>
#include <ode\src\geom_internal.h>

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

struct dxExtendedGeom :public dxGeom{
dVector3 last_pos;
bool pushing_neg,pushing_b_neg;
Triangle neg_tri,b_neg_tri;
dxExtendedGeom(){
pushing_neg=false;
pushing_b_neg=false;
last_pos[0]=dInfinity;
}
};

struct dxGeomUserData {
dVector3 last_pos;
bool pushing_neg,pushing_b_neg;
Triangle neg_tri,b_neg_tri;
dReal friction;
dReal bk_friction;
};

inline void dGeomCreateUserData(dxGeom* geom){
if(!geom) return;
geom->data=new dxGeomUserData();
((dxGeomUserData*)geom->data)->pushing_neg=false;
((dxGeomUserData*)geom->data)->pushing_b_neg=false;
((dxGeomUserData*)geom->data)->last_pos[0]=dInfinity;
((dxGeomUserData*)geom->data)->last_pos[1]=dInfinity;
((dxGeomUserData*)geom->data)->last_pos[2]=dInfinity;

}

inline void dGeomDestroyUserData(dxGeom* geom){
if(!geom) return;
if(geom->data) delete geom->data;
geom->data=0;
}

inline dxGeomUserData* dGeomGetUserData(const dxGeom* geom){
return (dxGeomUserData*) geom->data;
}

inline void dGeomUserDataSetFriction(dxGeom* geom,dReal friction){
((dxGeomUserData*)geom->data)->friction=friction;
((dxGeomUserData*)geom->data)->bk_friction=friction;
}
#endif