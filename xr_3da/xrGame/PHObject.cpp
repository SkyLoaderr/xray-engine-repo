#include "stdafx.h"
#include "Physics.h"
#include "PHObject.h"
#include "PHWorld.h"
#include "level.h"

DEFINE_VECTOR(ISpatial*,qResultVec,qResultIt)

	CPHObject::CPHObject()
{
	m_flags.flags=0;
	//b_activated=false;
	//b_freezed =false;
	spatial.type|=STYPE_PHYSIC;
	m_island.Init();
}

void CPHObject::activate()
{
	if(m_flags.test(st_activated)/*b_activated*/)return;
	if(m_flags.test(st_freezed)/*b_freezed*/)	{UnFreeze();return;}
	ph_world->AddObject(this);
	vis_update_activate();
	//b_activated=true;
	m_flags.set(st_activated,TRUE);
}

void CPHObject::EnableObject()
{
	activate();
}

void CPHObject::deactivate()
{
	if(!m_flags.test(st_activated)/*!b_activated*/)return;
	VERIFY2(m_island.IsActive(),"can not do it during processing");
	ph_world->RemoveObject(PH_OBJECT_I(this));
	vis_update_deactivate();
	//b_activated=false;
	m_flags.set(st_activated,FALSE);
}


void CPHObject::spatial_move()
{
	get_spatial_params();
	ISpatial::spatial_move();
	//b_dirty=true;
	m_flags.set(st_dirty,TRUE);
}




void CPHObject::Collide()
{
	//spatial_move();

	g_SpatialSpace->q_box(ph_world->r_spatial,0,STYPE_PHYSIC,spatial.center,AABB);
	qResultVec& result=ph_world->r_spatial;
	qResultIt i=result.begin(),e=result.end();
	for(;i!=e;++i)	{
		CPHObject* obj2=static_cast<CPHObject*>(*i);
		if(obj2==this || !obj2->m_flags.test(st_dirty)/*!obj2->b_dirty*/)		continue;
		NearCallback(this,obj2,dSpacedGeom(),obj2->dSpacedGeom());
	}
///////////////////////////////
	CollideStatic(dSpacedGeom(),this);
	//b_dirty=false;
	m_flags.set(st_dirty,TRUE);
	//near_callback(this,0,(dGeomID)dSpace(),ph_world->GetMeshGeom());
}

void CPHObject::FreezeContent()
{
	R_ASSERT(!m_flags.test(st_freezed)/*!b_freezed*/);
	//b_freezed=true;
	//b_activated=false;
	m_flags.set(st_freezed,TRUE);
	m_flags.set(st_activated,FALSE);
	vis_update_deactivate();
}
void CPHObject::UnFreezeContent()
{
	R_ASSERT(m_flags.test(st_freezed)/*b_freezed*/);
	//b_freezed=false;
	//b_activated=true;
	m_flags.set(st_freezed,FALSE);
	m_flags.set(st_activated,TRUE);
	vis_update_activate();
}

void CPHObject::spatial_register()
{
	get_spatial_params();
	ISpatial::spatial_register();
	//b_dirty=true;
	m_flags.set(st_dirty,TRUE);
}


void CPHObject::Freeze()
{
	if(!m_flags.test(st_activated)/*!b_activated*/)return;
	ph_world->RemoveObject(this);
	ph_world->AddFreezedObject(this);
	FreezeContent();

}

void CPHObject::UnFreeze()
{
	if(!m_flags.test(st_freezed)/*!b_freezed*/) return;
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

