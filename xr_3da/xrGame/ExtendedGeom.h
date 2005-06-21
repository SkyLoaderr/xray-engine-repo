#ifndef EXTENDED_GEOM
#define EXTENDED_GEOM
#include "PHObject.h"
#include "ode_include.h"
#include "physicscommon.h"
#ifdef DEBUG
extern	u32				dbg_total_saved_tries					;
#endif
class CPhysicsShellHolder;

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


class CGameObject;
struct dxGeomUserData
{
	dVector3					last_pos										;
	bool						pushing_neg,pushing_b_neg,b_static_colide		;
	Triangle					neg_tri,b_neg_tri								;

	CPHObject					*ph_object										;
	CPhysicsShellHolder			*ph_ref_object									;
	u16							material										;
	u16							tri_material									;
	ContactCallbackFun			*callback										;
	void						*callback_data									;
	ObjectContactCallbackFun	*object_callback								;
	u16							element_position								;
	u16							bone_id											;
	xr_vector<int>				cashed_tries									;
	Fvector						last_aabb_size									;
	Fvector						last_aabb_pos									;

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

IC dGeomID retrieveGeom(dGeomID geom)
{
	if(dGeomGetClass(geom)==dGeomTransformClass)
		return dGeomTransformGetGeom(geom);
	else
		return geom;
}

IC dxGeomUserData* retrieveGeomUserData(dGeomID geom)
{
			return dGeomGetUserData(retrieveGeom(geom));
			//if(dGeomGetClass(geom)==dGeomTransformClass)
			//	return dGeomGetUserData(dGeomTransformGetGeom(geom));
			//else
			//	return dGeomGetUserData(geom);
}

IC CPhysicsShellHolder* retrieveRefObject(dGeomID geom)
{
	dxGeomUserData* ud=dGeomGetUserData(retrieveGeom(geom));
	if(ud)return ud->ph_ref_object;
	else return NULL;
}
IC void dGeomCreateUserData(dxGeom* geom)
{
	if(!geom) return;
	dGeomSetData(geom,xr_new<dxGeomUserData>());
	(dGeomGetUserData(geom))->pushing_neg=false;
	(dGeomGetUserData(geom))->pushing_b_neg=false;
	(dGeomGetUserData(geom))->b_static_colide=true;
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
	(dGeomGetUserData(geom))->bone_id=u16(-1);
	(dGeomGetUserData(geom))->callback_data=NULL;
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
	if(P)
	{
#ifdef DEBUG
	dbg_total_saved_tries-=P->cashed_tries.size();
#endif
	P->cashed_tries		.clear();
	}
	xr_delete			(P);
	dGeomSetData		(geom,0);
}

IC void dGeomUserDataSetCallbackData(dxGeom* geom,void *cd)
{
	(dGeomGetUserData(geom))->callback_data=cd;
}
IC void dGeomUserDataSetPhObject(dxGeom* geom,CPHObject* phObject)
{
	(dGeomGetUserData(geom))->ph_object=phObject;
}

IC void dGeomUserDataSetPhysicsRefObject(dxGeom* geom,CPhysicsShellHolder* phRefObject)
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
IC void dGeomUserDataSetBoneId(dxGeom* geom,u16 bone_id)
{
	(dGeomGetUserData(geom))->bone_id=bone_id;
}
IC void dGeomUserDataResetLastPos(dxGeom* geom)
{
	(dGeomGetUserData(geom))->last_pos[0]=-dInfinity;
	(dGeomGetUserData(geom))->last_pos[1]=-dInfinity;
	(dGeomGetUserData(geom))->last_pos[2]=-dInfinity;
}
IC void dGeomUserDataClearCashedTries(dxGeom* geom)
{
	dxGeomUserData*	P	= dGeomGetUserData(geom);

#ifdef DEBUG
	dbg_total_saved_tries-=P->cashed_tries.size();
#endif
	P->cashed_tries.clear();
	P->last_aabb_size.set(0.f,0.f,0.f);
}
#endif