#include "stdafx.h"
#include "Physics.h"
#include "PHObject.h"
#include "PHWorld.h"

DEFINE_VECTOR(ISpatial*,qResultVec,qResultIt)

	CPHObject::CPHObject()
{
	b_activated=false;
	b_freezed =false;
	spatial.type|=STYPE_PHYSIC;
}

void CPHObject::Activate()
{
	if(b_activated)return;
	if(b_freezed)	{UnFreeze();return;}
	ph_world->AddObject(this);
	b_activated=true;

}

void CPHObject::EnableObject()
{
	Activate();
}

void CPHObject::Deactivate()
{
	if(!b_activated)return;
	ph_world->RemoveObject(PH_OBJECT_I(this));
	b_activated=false;
}


void CPHObject::spatial_move()
{
	get_spatial_params();
	ISpatial::spatial_move();
	b_dirty=true;
}

void CPHObject::Collide()
{
	//spatial_move();

	g_SpatialSpace->q_box(ph_world->r_spatial,0,STYPE_PHYSIC,spatial.center,AABB);
	qResultVec& result=ph_world->r_spatial;
	qResultIt i=result.begin(),e=result.end();
	for(;i!=e;++i)
	{
		
		CPHObject* obj2=static_cast<CPHObject*>(*i);
		if(obj2==this || !obj2->b_dirty)		continue;
		NearCallback(this,obj2,dSpacedGeom(),obj2->dSpacedGeom());
	}
///////////////////////////////
	CollideStatic(dSpacedGeom());
	b_dirty=false;
	//near_callback(this,0,(dGeomID)dSpace(),ph_world->GetMeshGeom());
}

void CPHObject::FreezeContent()
{
	b_freezed=true;
	b_activated=false;
}
void CPHObject::UnFreezeContent()
{
	b_freezed=false;
	b_activated=true;
}

void CPHObject::spatial_register()
{
	get_spatial_params();
	ISpatial::spatial_register();
	b_dirty=true;
}


void CPHObject::Freeze()
{
	if(!b_activated)return;
	ph_world->RemoveObject(this);
	ph_world->AddFreezedObject(this);
	FreezeContent();
}

void CPHObject::UnFreeze()
{
	if(!b_freezed) return;
	UnFreezeContent();
	ph_world->RemoveFreezedObject(this);
	ph_world->AddObject(this);
}

CPHUpdateObject::CPHUpdateObject()
{
	b_activated=false;
}

void CPHUpdateObject::Activate()
{
	if(b_activated)return;

	ph_world->AddUpdateObject(this);
	b_activated=true;
}

void CPHUpdateObject::Deactivate()
{
	if(!b_activated)return;
	ph_world->RemoveUpdateObject(this);
	b_activated=false;
}

