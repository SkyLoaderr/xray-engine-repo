#ifndef EXTENDED_GEOM
#define EXTENDED_GEOM
#include "PHObject.h"
#include "ode_include.h"


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
class CGameObject;
struct dxGeomUserData
{
	dVector3	last_pos;
	bool		pushing_neg,pushing_b_neg;
	Triangle	neg_tri,b_neg_tri;
	CPHObject*	ph_object;
	CGameObject* ph_ref_object;
	u16			material;
	u16			tri_material;
	ContactCallbackFun* callback;
	ObjectContactCallbackFun* object_callback;
	u16			element_position;
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
IC dxGeomUserData* dGeomGetUserData(dxGeom* geom)
{
	return (dxGeomUserData*) dGeomGetData(geom);
}

IC dxGeomUserData* retrieveGeomUserData(dGeomID geom)
{
			if(dGeomGetClass(geom)==dGeomTransformClass)
				return dGeomGetUserData(dGeomTransformGetGeom(geom));
			else
				return dGeomGetUserData(geom);
}

IC void dGeomCreateUserData(dxGeom* geom)
{
	if(!geom) return;
	dGeomSetData(geom,xr_new<dxGeomUserData>());
	(dGeomGetUserData(geom))->pushing_neg=false;
	(dGeomGetUserData(geom))->pushing_b_neg=false;
	(dGeomGetUserData(geom))->last_pos[0]=-dInfinity;
	(dGeomGetUserData(geom))->last_pos[1]=-dInfinity;
	(dGeomGetUserData(geom))->last_pos[2]=-dInfinity;
	(dGeomGetUserData(geom))->ph_object=NULL;
	(dGeomGetUserData(geom))->material=0;
	(dGeomGetUserData(geom))->tri_material=0;
	(dGeomGetUserData(geom))->callback=NULL;
	(dGeomGetUserData(geom))->object_callback=NULL;
	(dGeomGetUserData(geom))->ph_ref_object=NULL;
	(dGeomGetUserData(geom))->element_position=u16(-1);
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::mu=1.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::damping=1.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::spring=1.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::bonce=0.f;
	//((dxGeomUserData*)dGeomGetData(geom))->ContactsParameters::bonce_vel=0.f;
}



IC void dGeomDestroyUserData(dxGeom* geom)
{
	if(!geom)			return;
	dxGeomUserData*	P	= dGeomGetUserData(geom);
	xr_delete			(P);
	dGeomSetData		(geom,0);
}

IC void dGeomUserDataSetPhObject(dxGeom* geom,CPHObject* phObject)
{
	(dGeomGetUserData(geom))->ph_object=phObject;
}

IC void dGeomUserDataSetPhysicsRefObject(dxGeom* geom,CGameObject* phRefObject)
{
	(dGeomGetUserData(geom))->ph_ref_object=phRefObject;
}

IC void dGeomUserDataSetContactCallback(dxGeom* geom,ContactCallbackFun* callback)
{
	(dGeomGetUserData(geom))->callback=callback;
}

IC void dGeomUserDataSetObjectContactCallback(dxGeom* geom,ObjectContactCallbackFun* obj_callback)
{
	(dGeomGetUserData(geom))->object_callback=obj_callback;
}

IC void dGeomUserDataSetElementPosition(dxGeom* geom,u16 e_pos)
{
	(dGeomGetUserData(geom))->element_position=e_pos;
}

IC void dGeomUserDataResetLastPos(dxGeom* geom)
{
	(dGeomGetUserData(geom))->last_pos[0]=-dInfinity;
	(dGeomGetUserData(geom))->last_pos[1]=-dInfinity;
	(dGeomGetUserData(geom))->last_pos[2]=-dInfinity;
}
#endif