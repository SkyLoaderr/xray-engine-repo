#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
///////////////////////////////////////////////////////////////
///#pragma warning(disable:4995)
//#include "../ode/src/collision_kernel.h"
//#include <../ode/src/joint.h>
//#include <../ode/src/objects.h>

//#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHShell.h"

void CPHShell::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	if(bActive)
		return;
	if(!disable)EnableObject();

	ELEMENT_I i;

	mXFORM.set(m0);
	m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	dSpaceSetCleanup(m_space,0);
	for(i=elements.begin();elements.end() != i;++i){

		(*i)->Activate(m0,dt01, m2, disable);
	}
	//if(m_spliter_holder)m_spliter_holder->Activate();
	bActive=true;
	bActivating=true;
	spatial_register();
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
}



void CPHShell::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	if(bActive)
		return;
	if(!disable)EnableObject();

	ELEMENT_I i;

	mXFORM.set(transform);
	m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	dSpaceSetCleanup(m_space,0);
	//dSpaceSetCleanup (m_space, 0);
	for(i=elements.begin();elements.end() != i;++i){
		//(*i)->Start();
		//(*i)->SetTransform(m0);
		(*i)->Activate(transform,lin_vel, ang_vel);
	}
	//SetPhObjectInElements();
	//if(m_spliter_holder)m_spliter_holder->Activate();
	bActive=true;
	bActivating=true;
	spatial_register();
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
}



void CPHShell::Activate(bool place_current_forms,bool disable)
{ 
	if(bActive)
		return;
	if(!disable)EnableObject();
	if(!m_space)
	{
		m_space=dSimpleSpaceCreate(ph_world->GetSpace());
		dSpaceSetCleanup(m_space,0);
	}

	{		
		ELEMENT_I i=elements.begin(),e=elements.end();
		if(place_current_forms) for(;i!=e;++i)(*i)->Activate(mXFORM,disable);
	}

	{
		JOINT_I i=joints.begin(),e=joints.end();
		for(;i!=e;++i) (*i)->Activate();
	}	
	//if(m_spliter_holder)m_spliter_holder->Activate();
	bActive=true;
	bActivating=true;
	spatial_register();
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
}


void CPHShell::Build(bool place_current_forms/*true*/,bool disable/*false*/)
{
	if(bActive)
		return;
	bActive=true;
	bActivating=true;
	if(!m_space)
	{
		m_space=dSimpleSpaceCreate(0);
		dSpaceSetCleanup(m_space,0);
	}

	{		
		ELEMENT_I i=elements.begin(),e=elements.end();
		if(place_current_forms) for(;i!=e;++i)
		{
			(*i)->build(disable);
		}
	}

	{
		JOINT_I i=joints.begin(),e=joints.end();
		for(;i!=e;++i) (*i)->Create();
	}	
	
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
}

void CPHShell::RunSimulation(bool place_current_forms/*true*/)
{

	EnableObject();
	dSpaceAdd(ph_world->GetSpace(),(dGeomID)m_space);
	dSpaceSetCleanup(m_space,0);
	{		
		ELEMENT_I i=elements.begin(),e=elements.end();
		if(place_current_forms) for(;i!=e;++i)(*i)->RunSimulation(mXFORM);
	}
	{
		JOINT_I i=joints.begin(),e=joints.end();
		for(;i!=e;++i) (*i)->RunSimulation();
	}	
	spatial_register();
//	if(m_spliter_holder)m_spliter_holder->Activate();
	//spatial_register();

}

void CPHShell::AfterSetActive()
{
	if(bActive)	return;
	PureActivate();
	bActive=true;
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(;i!=e;++i)(*i)->PresetActive();

}

void CPHShell::PureActivate()
{
	if(bActive)	return;
	bActive=true;
	dSpaceAdd(ph_world->GetSpace(),(dGeomID)m_space);
	EnableObject();
	spatial_register();
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
	//if(m_spliter_holder)m_spliter_holder->Activate();
	m_object_in_root.identity();
}

void CPHShell::PresetActive()
{
	VERIFY(!bActive);
	if(!m_space) 
	{
		m_space=dSimpleSpaceCreate(0);
		dSpaceSetCleanup(m_space,0);
	}
}



void CPHShell::Deactivate(){
	if(!bActive)
		return;
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
		(*i)->Deactivate();

	JOINT_I j;
	for(j=joints.begin();joints.end() != j;++j)
		(*j)->Deactivate();

	//if(!bActivating)
		DisableObject();

	if(m_space) {
		dSpaceDestroy(m_space);
		m_space=NULL;
	}
	bActive=false;
	bActivating=false;
	spatial_unregister();
	//dJointGroupDestroy(m_saved_contacts);
	//b_contacts_saved=false;
	ZeroCallbacks();
}