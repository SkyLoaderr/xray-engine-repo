#include "stdafx.h"
#include "Physics.h"
#include "PHWorld.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PhysicsCommon.h"
//////////////////////////////////////////////////////////////
//////////////CPHMesh///////////////////////////////////////////
///////////////////////////////////////////////////////////

void CPHMesh ::Create(dSpaceID space, dWorldID world){
	Geom = dCreateTriList(space, 0, 0);

}
/////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void CPHMesh ::Destroy(){
	dGeomDestroy(Geom);
	dTriListClass=-1;
}



////////////////////////////////////////////////////////////////////////////
///////////CPHWorld/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//#define PH_PLAIN
#ifdef PH_PLAIN
dGeomID plane;
#endif
void CPHWorld::Render()
{
	if (!bDebug)	return;

}
CPHWorld::CPHWorld()
{
	disable_count=0;
	m_frame_time=0.f;
	m_steps_num=0;
	m_frame_sum=0.f;
	m_delay=0; 
	m_previous_delay=0;
	m_reduce_delay=0;
	m_update_delay_count=0;
	b_world_freezed=false;
}
void CPHWorld::Create()
{
	if (psDeviceFlags.test(mtPhysics))	Device.seqFrameMT.Add	(this,REG_PRIORITY_HIGH);
	else								Device.seqFrame.Add		(this,REG_PRIORITY_LOW);

	phWorld = dWorldCreate();
	//Space = dHashSpaceCreate(0);

	//dVector3 extensions={2048,256,2048};
	Fbox	level_box		=	Level().ObjectSpace.GetBoundingVolume();
	Fvector level_size,level_center;
	level_box				.	getsize		(level_size);
	level_box				.	getcenter	(level_center);
	dVector3 extensions		=	{ level_size.x ,256.f,level_size.z};
	dVector3 center			=	{level_center.x,0.f,level_center.z};
	Space					=	dQuadTreeSpaceCreate (0, center,extensions, 6);

	dSpaceSetCleanup			(Space,0);
#ifdef ODE_SLOW_SOLVER
#else
	dWorldSetAutoEnableDepthSF1(phWorld, 100000000);
#endif
	ContactGroup			= dJointGroupCreate(0);		
	dWorldSetGravity		(phWorld, 0,-world_gravity, 0);//-2.f*9.81f
	Mesh.Create				(Space,phWorld);
	//Jeep.Create(Space,phWorld);//(Space,phWorld)
	//Gun.Create(Space);
#ifdef PH_PLAIN
	plane=dCreatePlane(Space,0,1,0,0.3f);
	//dGeomCreateUserData(plane);
#endif

	//const  dReal k_p=2400000.f;//550000.f;///1000000.f;
	//const dReal k_d=200000.f;
	dWorldSetERP(phWorld, ERP(world_spring,world_damping) );
	dWorldSetCFM(phWorld, CFM(world_spring,world_damping));
	//dWorldSetERP(phWorld,  0.2f);
	//dWorldSetCFM(phWorld,  0.000001f);
	disable_count=0;
	//Jeep.DynamicData.CalculateData();
}

/////////////////////////////////////////////////////////////////////////////

void CPHWorld::Destroy(){
	Mesh.Destroy();
#ifdef PH_PLAIN
	dGeomDestroy(plane);
#endif
	dJointGroupEmpty(ContactGroup);
	dSpaceDestroy(Space);
	dWorldDestroy(phWorld);
	dCloseODE();
	dCylinderClassUser=-1;

	Device.seqFrameMT.Remove	(this);
	Device.seqFrame.Remove		(this);
}

void CPHWorld::OnFrame()
{
	// Msg									("------------- physics: %d / %d",u32(Device.dwFrame),u32(m_steps_num));
	Device.Statistic.Physics.Begin		();
	FrameStep								(Device.fTimeDelta);
	Device.Statistic.Physics.End		();
}

//////////////////////////////////////////////////////////////////////////////
//static dReal frame_time=0.f;
static u32 start_time=0;
void CPHWorld::Step()
{

	PH_OBJECT_I			i_object;
	PH_UPDATE_OBJECT_I	i_update_object;
	++disable_count;		
	if(disable_count==dis_frames+1) disable_count=0;

	++m_steps_num;



	Device.Statistic.ph_collision.Begin	();
	//dSpaceCollide		(Space, 0, &NearCallback); 
	for(i_object=m_objects.begin();m_objects.end() != i_object;)
	{
		CPHObject* obj=(*i_object);
		obj->Collide();
		++i_object;
	}
	Device.Statistic.ph_collision.End	();


	for(i_object=m_objects.begin();m_objects.end() != i_object;)
	{	
		CPHObject* obj=(*i_object);
		++i_object;
		obj->PhTune(fixed_step);
	}

	for(i_update_object=m_update_objects.begin();m_update_objects.end() != i_update_object;)
	{	CPHUpdateObject* obj=(*i_update_object);
		++i_update_object;
		obj->PhTune(fixed_step);
	}

	Device.Statistic.ph_core.Begin		();
#ifdef ODE_SLOW_SOLVER
	dWorldStep		(phWorld,	fixed_step);
#else
	//IterationCycleI=(++IterationCycleI)%phIterationCycle;

	dWorldStepFast1	(phWorld,	fixed_step,	phIterations/*+Random.randI(0,phIterationCycle)*/);

#endif
	Device.Statistic.ph_core.End		();



	for(i_object=m_objects.begin();m_objects.end() != i_object;)
	{
		CPHObject* obj=(*i_object);
		++i_object;
		obj->PhDataUpdate(fixed_step);
		obj->spatial_move();
	}

	for(i_update_object=m_update_objects.begin();m_update_objects.end() != i_update_object;)
	{	CPHUpdateObject* obj=*i_update_object;
	++i_update_object;
	obj->PhDataUpdate(fixed_step);
	}

	dJointGroupEmpty(ContactGroup);//this is to be called after PhDataUpdate!!!-the order is critical!!!
	ContactFeedBacks.empty();
	ContactEffectors.empty();

	if(physics_step_time_callback) 
	{
		physics_step_time_callback(start_time,start_time+u32(fixed_step*1000));	
		start_time += u32(fixed_step*1000);
	};


	//	for(iter=m_objects.begin();m_objects.end()!=iter;++iter)
	//			(*iter)->StepFrameUpdate(step);
}

u32 CPHWorld::CalcNumSteps (u32 dTime)
{
	u32 res = iFloor((float(dTime) / 1000 / fixed_step)+0.5f);
	return res;
};

void CPHWorld::FrameStep(dReal step)
{
	VERIFY	(_valid(step));
	step*=phTimefactor;
	// compute contact joints and forces

	//step+=astep;

	//const  dReal k_p=24000000.f;//550000.f;///1000000.f;
	//const dReal k_d=400000.f;
	u32 it_number;
	float frame_time=m_frame_time;
	frame_time+=step;
	//m_frame_sum+=step;

	if(!(frame_time<fixed_step))
	{
		it_number	=	iFloor	(frame_time/fixed_step);
		frame_time	-=	it_number*fixed_step;
		m_frame_time=frame_time;
	}
	else
	{
		m_frame_time=frame_time;
		return;
	}
	/*
	++m_update_delay_count;

	if(m_update_delay_count==update_delay){
	if(m_delay){
	if(m_delay<m_previous_delay) --m_reduce_delay;
	else 	++m_reduce_delay;
	}
	m_previous_delay=m_delay;
	m_update_delay_count=0;
	}

	m_delay+=(it_number-m_reduce_delay-1);
	*/
	//for(UINT i=0;i<(m_reduce_delay+1);++i)
	start_time = Device.dwTimeGlobal;// - u32(m_frame_time*1000);
	for(UINT i=0; i<it_number;++i)	Step();
}

void CPHWorld::AddObject(CPHObject* object){
	m_objects.push_back(object);
	//xr_list <CPHObject*> ::iterator i= m_objects.end();
	//return (--m_objects.end());
};

void CPHWorld::AddUpdateObject(CPHUpdateObject* object)
{
	m_update_objects.push_back(object);
}

void CPHWorld::RemoveUpdateObject(PH_UPDATE_OBJECT_I i)
{
	m_update_objects.erase(i);
}
//void EnableObject();
void CPHWorld::RemoveObject(PH_OBJECT_I i){
	m_objects.erase((i));
};

void CPHWorld::AddFreezedObject(CPHObject* obj)
{
	m_freezed_objects.push_back(obj);
}

void CPHWorld::RemoveFreezedObject(PH_OBJECT_I i)
{
	m_freezed_objects.erase(i);
}
void CPHWorld::Freeze()
{
	R_ASSERT2(!b_world_freezed,"already freezed!!!");
	m_freezed_objects.move_items(m_objects);
	PH_OBJECT_I iter=m_freezed_objects.begin(),
		e=	m_freezed_objects.end()	;

	for(; e != iter;++iter)
		(*iter)->FreezeContent();
	b_world_freezed=true;
}
void CPHWorld::UnFreeze()
{
	R_ASSERT2(b_world_freezed,"is not freezed!!!");
	PH_OBJECT_I iter=m_freezed_objects.begin(),
		e=	m_freezed_objects.end()	;
	for(; e != iter;++iter)
		(*iter)->UnFreezeContent();
	m_objects.move_items(m_freezed_objects);
	b_world_freezed=false;
}
bool CPHWorld::IsFreezed()
{
	return b_world_freezed;
}