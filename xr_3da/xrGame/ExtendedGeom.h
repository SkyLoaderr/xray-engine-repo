#ifndef EXTENDED_GEOM
#define EXTENDED_GEOM
#include "PHObject.h"
#pragma warning(disable:4995)
#include <ode/ode.h>
#pragma warning(default:4995)

enum Material 
{
	weels,
	cardboard,
	mesh_default,
	car_cabin,
	weapon_default,
	matrerial_default 
};

struct Triangle 
{
	//dReal* v0;
	//dReal* v1;
	//dReal* v2;
	dVector3 side0;
	dVector3 side1;
	dVector3 norm;
	dReal dist;
	dReal pos;
	dReal depth;
	CDB::TRI* T ;

};

typedef  void __stdcall ContactCallbackFun		(CDB::TRI* T,		dContactGeom* c);
typedef	 void __stdcall ObjectContactCallbackFun(bool& do_colide,	dContact& c);

struct dxGeomUserData
{
	dVector3	last_pos;
	bool		pushing_neg,pushing_b_neg;
	Triangle	neg_tri,b_neg_tri;
	CPHObject*	ph_object;
	u32			material;
	u32			tri_material;
	ContactCallbackFun* callback;
	ObjectContactCallbackFun* object_callback;
//	struct ContactsParameters
//	{
//	dReal damping;
//	dReal spring;
//	dReal bonce;
//	dReal bonce_vel;
//	dReal mu;
//	unsigned int maxc;
//	};
};

IC void dGeomCreateUserData(dxGeom* geom)
{
	if(!geom) return;
	dGeomSetData(geom,xr_new<dxGeomUserData>());
	((dxGeomUserData*)dGeomGetData(geom))->pushing_neg=false;
	((dxGeomUserData*)dGeomGetData(geom))->pushing_b_neg=false;
	((dxGeomUserData*)dGeomGetData(geom))->last_pos[0]=-dInfinity;
	((dxGeomUserData*)dGeomGetData(geom))->last_pos[1]=-dInfinity;
	((dxGeomUserData*)dGeomGetData(geom))->last_pos[2]=-dInfinity;
	((dxGeomUserData*)dGeomGetData(geom))->ph_object=NULL;
	((dxGeomUserData*)dGeomGetData(geom))->material=0;
	((dxGeomUserData*)dGeomGetData(geom))->tri_material=0;
	((dxGeomUserData*)dGeomGetData(geom))->callback=NULL;
	((dxGeomUserData*)dGeomGetData(geom))->object_callback=NULL;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::mu=1.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::damping=1.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::spring=1.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::bonce=0.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::bonce_vel=0.f;
}

IC void dGeomDestroyUserData(dxGeom* geom)
{
	if(!geom) return;
	if(dGeomGetData(geom)) xr_delete(dGeomGetData(geom));
	dGeomSetData(geom,0);
}

IC dxGeomUserData* dGeomGetUserData(dxGeom* geom)
{
	return (dxGeomUserData*) dGeomGetData(geom);
}



IC void dGeomUserDataSetPhObject(dxGeom* geom,CPHObject* phObject)
{
	((dxGeomUserData*)dGeomGetData(geom))->ph_object=phObject;
}

IC void dGeomUserDataSetContactCallback(dxGeom* geom,ContactCallbackFun* callback)
{
	((dxGeomUserData*)dGeomGetData(geom))->callback=callback;
}

IC void dGeomUserDataSetObjectContactCallback(dxGeom* geom,ObjectContactCallbackFun* obj_callback)
{
	((dxGeomUserData*)dGeomGetData(geom))->object_callback=obj_callback;
}
#endif