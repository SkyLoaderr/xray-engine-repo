#define INTERNAL_BUILD
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "dRay/include/dRay.h"
#include <ode\src\objects.h>
#include <ode\src\geom_internal.h>
#include "ExtendedGeom.h"
#include "dBoxGeomA.h"
// #include "contacts.h"
float friction_table[2]={5000.f,100.f};



//void _stdcall dGeomTransformSetInfo (dGeomID g, int mode);
/////////////////////////////////////////
static dContact bulletContact;
static bool isShooting;
static dVector3 RayD;
static dVector3 RayO;

dWorldID phWorld;
/////////////////////////////////////
static void FUNCCALL NearCallback(void* /*data*/, dGeomID o1, dGeomID o2);
/*
Log("fgfgfgf","ffff");

Log("fgfgfgf",12);
Fvector f;
Log("dfdf ",f);
Msg("%s","rrr");
*/
//Level().HUD().pHUDFont->Out(x,y,"%s","sdfsdsd");

void CPHWorld::Render()
{
	if (!bDebug)	return;
	/*
	Device.Shader.OnFrameEnd		();
	Device.set_xform_world			(Fidentity);
	Fmatrix M;
	Fvector scale;


	Jeep.DynamicData.GetWorldMX(M);
	scale.set(Jeep.jeepBox[0]/2.f,Jeep.jeepBox[1]/2.f,Jeep.jeepBox[2]/2.f);
	Device.Primitive.dbg_DrawOBB	(M,scale,0xffffffff);

	Jeep.DynamicData.GetTGeomWorldMX(M);
	scale.set(Jeep.cabinBox[0]/2.f,Jeep.cabinBox[1]/2.f,Jeep.cabinBox[2]/2.f);
	Device.Primitive.dbg_DrawOBB	(M,scale,0xffffffff);



	scale.set(1.6f/0.8f*0.28f,1.6f/0.8f*0.28f,1.6f/0.8f*0.28f);
	Jeep.DynamicData[0].GetWorldMX(M);

	Fvector t = M.c;
	M.scale(scale);
	M.c = t;

	Device.Primitive.dbg_DrawEllipse(M, 0xffffffff);
	Jeep.DynamicData[1].GetWorldMX(M);
	t = M.c;
	M.scale(scale);
	M.c = t;
	Device.Primitive.dbg_DrawEllipse(M, 0xffffffff);
	Jeep.DynamicData[2].GetWorldMX(M);
	t = M.c;
	M.scale(scale);
	M.c = t;
	Device.Primitive.dbg_DrawEllipse(M, 0xffffffff);
	Jeep.DynamicData[3].GetWorldMX(M);
	t = M.c;
	M.scale(scale);
	M.c = t;
	Device.Primitive.dbg_DrawEllipse(M, 0xffffffff);
	*/
}

//////////////////////////////////////////////////////////////
//////////////CPHMesh///////////////////////////////////////////
///////////////////////////////////////////////////////////

void CPHMesh ::Create(dSpaceID space, dWorldID world){
Geom = dCreateTriList(space, 0, 0);

}

////////////////////////////////////////////////////////////////////////////

void CPHMesh ::Destroy(){
						dGeomDestroy(Geom);
						}


/////////////////////////////////////////////////////////////////////////////
/////////////CPHJeep////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CPHJeep::Create1(dSpaceID space, dWorldID world){
	
	static const dReal scaleParam=1.f;
	static const dVector3 scaleBox={scaleParam, scaleParam, scaleParam};
	//jeepBox={scaleBox[0],scaleBox[0],scaleBox[0]};
	jeepBox[0]=REAL(4.2)*scaleBox[0];jeepBox[1]=REAL(1.)*scaleBox[1];jeepBox[2]=REAL(2.08)*scaleBox[2];
	//jeepBox[0]=REAL(1.0)*scaleBox[0];jeepBox[1]=REAL(1.)*scaleBox[1];jeepBox[2]=REAL(1.3)*scaleBox[2];
	cabinBox[0]=scaleBox[0]*1.9f;cabinBox[1]=scaleBox[1]*0.6f;cabinBox[2]=scaleBox[2]*2.08f;
	//cabinBox[0]=scaleBox[0]*1.9f;cabinBox[1]=scaleBox[1]*0.6f;cabinBox[2]=scaleBox[2]*1.3f;
	static const dReal wheelRadius = 0.79f/2.f* scaleParam;
	

	static const dVector3 startPosition={6.0f,12.f,0.f};
	static const dReal weelSepX=scaleBox[0]*2.74f/2.f,weelSepZ=scaleBox[2]*1.7f/2.f,weelSepY=scaleBox[1]*0.6f;

	dMass m;

	// car body
	//dMass m;
	dMassSetBox(&m, 1, jeepBox[0], jeepBox[1], jeepBox[2]); // density,lx,ly,lz
	dMassAdjust(&m, 50); // mass

	Bodies[0] = dBodyCreate(world);
	dBodySetMass(Bodies[0], &m);
	dBodySetPosition(Bodies[0], startPosition[0], startPosition[1], startPosition[2]); // x,y,z

	Geoms[0] = dCreateBox(0, jeepBox[0], jeepBox[1], jeepBox[2]); // lx,ly,lz
	Geoms[6] = dCreateBox(0, cabinBox[0], cabinBox[1], cabinBox[2]); // lx,ly,lz

	//dGeomSetBody(Geoms[5], Bodies[5]);
	Geoms[5]=dCreateGeomTransform(space);
	//Geoms[7]=dCreateGeomTransform(0);
	dGeomSetPosition(Geoms[6], -jeepBox[0]/2.f+cabinBox[0]/2+0.55f, cabinBox[1]/2.f+jeepBox[1]/2.f, 0.f); // x,y,z
	//dGeomSetPosition(Geoms[0], 0,0/*-jeepBox[1]-wheelRadius*/, 0); // x,y,z
	dGeomTransformSetGeom(Geoms[5],Geoms[6]);
	//dGeomTransformSetGeom(Geoms[7],Geoms[0]);


	dGeomSetBody(Geoms[5], Bodies[0]);
	dGeomSetBody(Geoms[0], Bodies[0]);



	

	// wheel bodies
	dMassSetSphere(&m, 1, wheelRadius); // density, radius
	dMassAdjust(&m, 10); // mass
	dQuaternion q;
	dQFromAxisAndAngle(q, 1, 0, 0, M_PI * 0.5);
	UINT i;
	for(i = 1; i <= 4; ++i)
	{
		Bodies[i] = dBodyCreate(world);
		dBodySetMass(Bodies[i], &m);
		dBodySetQuaternion(Bodies[i], q);
		Geoms[i] = dCreateSphere(0, wheelRadius);
		dGeomSetBody(Geoms[i], Bodies[i]);
	}




	dBodySetPosition(Bodies[1], startPosition[0]-weelSepX, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z
	dBodySetPosition(Bodies[2], startPosition[0]-weelSepX, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z-0.9, 2.6,   9.3); // x,y,z
	dBodySetPosition(Bodies[3], startPosition[0]+weelSepX, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z 0.9, 2.6,  10.7); // x,y,z
	dBodySetPosition(Bodies[4], startPosition[0]+weelSepX, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z 0.9, 2.6,   9.3); // x,y,z



	// wheel joints
	for(i = 0; i < 4; ++i)
	{
		Joints[i] = dJointCreateHinge2(world, 0);
		dJointAttach(Joints[i], Bodies[0], Bodies[i+1]);
		const dReal* const wPos = dBodyGetPosition(Bodies[i+1]);
		dJointSetHinge2Anchor(Joints[i], wPos[0], wPos[1], wPos[2]);
		dJointSetHinge2Axis1(Joints[i], 0.f, 1.f, 0.f);
		dJointSetHinge2Axis2(Joints[i], 0.f, 0.f, ((i % 2) == 0) ? -1.f : 1.f);

		dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
		dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
		dJointSetHinge2Param(Joints[i], dParamFMax, 50);

		dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		dJointSetHinge2Param(Joints[i], dParamFMax2, 80);

		dJointSetHinge2Param(Joints[i], dParamSuspensionERP, 0.3f);
		dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 0.0003f);
	}

	GeomsGroup = dCreateGeomGroup(space);  
	for(i = 0; i < NofGeoms-1; ++i)
		dGeomGroupAdd(GeomsGroup, Geoms[i]);



	//dynamic data
CreateDynamicData();

}

/////////////////////////////////////////////////////////////////////////////
void CPHJeep::Create(dSpaceID space, dWorldID world){
	
	static const dReal scaleParam=1.f;
	static const dVector3 scaleBox={scaleParam, scaleParam, scaleParam};
	//jeepBox={scaleBox[0],scaleBox[0],scaleBox[0]};
	//jeepBox[0]=REAL(4.2)*scaleBox[0];jeepBox[1]=REAL(1.)*scaleBox[1];jeepBox[2]=REAL(2.08)*scaleBox[2];
	jeepBox[0]=REAL(3.680)*scaleBox[0];jeepBox[1]=REAL(0.612)*scaleBox[1];jeepBox[2]=0.88f*2.f*scaleBox[2];
	//cabinBox[0]=scaleBox[0]*1.9f;cabinBox[1]=scaleBox[1]*0.6f;cabinBox[2]=scaleBox[2]*2.08f;
	cabinBox[0]=scaleBox[0]*1.7f;cabinBox[1]=scaleBox[1]*0.66f;cabinBox[2]=scaleBox[2]*2.f*0.76f;

	static const dReal wheelRadius = 0.345f* scaleParam;

	VelocityRate=3.f;
	DriveForce=0;
	DriveVelocity=12.f * M_PI;
	DriveDirection=0;
	Breaks=false;
	startPosition[0]=10.0f;startPosition[1]=1.f;startPosition[2]=0.f;
	//static const dReal weelSepX=scaleBox[0]*2.74f/2.f,weelSepZ=scaleBox[2]*1.7f/2.f,weelSepY=scaleBox[1]*0.6f;
	static const dReal weelSepXF=scaleBox[0]*1.32f,weelSepXB=scaleBox[0]*1.155f,weelSepZ=scaleBox[2]*1.53f/2.f,weelSepY=scaleBox[1]*0.463f;
	static const dReal cabinSepX=scaleBox[0]*0.61f,cabinSepY=scaleBox[1]*0.55f;
	MassShift=0.25f;
	dMass m;

	// car body
	//dMass m;
	dMassSetBox(&m, 1.f, jeepBox[0], jeepBox[1]/4.f, jeepBox[2]); // density,lx,ly,lz
	dMassAdjust(&m, 800.f); // mass
	//dMassTranslate(&m,0.f,-1.f,0.f);
	Bodies[0] = dBodyCreate(world);
	dBodySetMass(Bodies[0], &m);
	dBodySetPosition(Bodies[0], startPosition[0], startPosition[1]-MassShift, startPosition[2]); // x,y,z
	
	Geoms[0] = dCreateBox(0, jeepBox[0], jeepBox[1], jeepBox[2]); // lx,ly,lz
	Geoms[6] = dCreateBox(0, cabinBox[0], cabinBox[1], cabinBox[2]); // lx,ly,lz

	//dGeomSetBody(Geoms[5], Bodies[5]);
	Geoms[5]=dCreateGeomTransform(0);
	Geoms[7]=dCreateGeomTransform(0);
	dGeomCreateUserData(Geoms[5]);
	dGeomCreateUserData(Geoms[7]);
	dGeomCreateUserData(Geoms[0]);
	dGeomCreateUserData(Geoms[6]);
	dGeomGetUserData(Geoms[5])->friction=500.f;
	dGeomGetUserData(Geoms[7])->friction=500.f;
	dGeomSetPosition(Geoms[0], 0.f, MassShift, 0.f); // x,y,z
	//dGeomSetPosition(Geoms[6], -jeepBox[0]/2.f+cabinBox[0]/2.f+0.55f, cabinBox[1]/2.f+jeepBox[1]/2.f+MassShift, 0.f); // x,y,z
	dGeomSetPosition(Geoms[6], -cabinSepX, cabinSepY+MassShift, 0.f); // x,y,z
	//dGeomSetPosition(Geoms[0], 0,0/*-jeepBox[1]-wheelRadius*/, 0); // x,y,z
	dGeomTransformSetGeom(Geoms[5],Geoms[6]);
	dGeomTransformSetGeom(Geoms[7],Geoms[0]);
	dGeomTransformSetInfo(Geoms[5],1);
	dGeomTransformSetInfo(Geoms[7],1);
	dGeomSetBody(Geoms[5], Bodies[0]);
	dGeomSetBody(Geoms[7], Bodies[0]);

	// wheel bodies
	dMassSetSphere(&m, 1, wheelRadius); // density, radius
	dMassAdjust(&m, 20); // mass
	dQuaternion q;
	dQFromAxisAndAngle(q, 1, 0, 0, M_PI * 0.5);
	UINT i;
	for(i = 1; i <= 4; ++i)
	{
		Bodies[i] = dBodyCreate(world);
		dBodySetMass(Bodies[i], &m);
		dBodySetQuaternion(Bodies[i], q);
		//Geoms[i] = dCreateSphere(0, wheelRadius);
		Geoms[i] = dCreateCylinder(0, wheelRadius,0.19f);
		dGeomSetBody(Geoms[i], Bodies[i]);
	}




	dBodySetPosition(Bodies[1], startPosition[0]-weelSepXB, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z
	dBodySetPosition(Bodies[2], startPosition[0]-weelSepXB, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z-0.9, 2.6,   9.3); // x,y,z
	dBodySetPosition(Bodies[3], startPosition[0]+weelSepXF, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z 0.9, 2.6,  10.7); // x,y,z
	dBodySetPosition(Bodies[4], startPosition[0]+weelSepXF, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z 0.9, 2.6,   9.3); // x,y,z



	// wheel joints
	for(i = 0; i < 4; ++i)
	{
		Joints[i] = dJointCreateHinge2(world, 0);
		dJointAttach(Joints[i], Bodies[0], Bodies[i+1]);
		const dReal* const wPos = dBodyGetPosition(Bodies[i+1]);
		dJointSetHinge2Anchor(Joints[i], wPos[0], wPos[1], wPos[2]);
		dJointSetHinge2Axis1(Joints[i], 0.f, 1.f, 0.f);
		dJointSetHinge2Axis2(Joints[i], 0.f, 0.f, ((i % 2) == 0) ? -1.f : 1.f);

		dJointSetHinge2Param(Joints[i], dParamLoStop, 0.f);
		dJointSetHinge2Param(Joints[i], dParamHiStop, 0.f);
		dJointSetHinge2Param(Joints[i], dParamFMax, 10000.f);
		dJointSetHinge2Param(Joints[i], dParamFudgeFactor, 0.001f);

		dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
		dJointSetHinge2Param(Joints[i], dParamFMax2, 500.f);
		dReal k_p=20000.f;
		dReal k_d=1000.f;
		dReal h=0.02222f;
			

		dJointSetHinge2Param(Joints[i], dParamSuspensionERP, h*k_p / (h*k_p + k_d));
		dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 1.f / (h*k_p + k_d));

	}

	GeomsGroup = dCreateGeomGroup(space);  
	for(i = 1; i < NofGeoms-1; ++i)
		dGeomGroupAdd(GeomsGroup, Geoms[i]);
	dGeomGroupAdd(GeomsGroup, Geoms[7]);


	//dynamic data
CreateDynamicData();

}
////////////////////////////////////////////////////////////////
void CPHJeep::JointTune(dReal step){
const	dReal k_p=30000.f;
const	dReal k_d=1000.f;
	for(UINT i = 0; i < 4; ++i)
	{

		
		dJointSetHinge2Param(Joints[i], dParamSuspensionERP, step*k_p / (step*k_p + k_d));
		dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 1.f / (step*k_p + k_d));

	}
}
/////////
void CPHJeep::Destroy(){
	for(UINT i=0;i<NofGeoms;i++) dGeomDestroyUserData(Geoms[i]);
	DynamicData.Destroy();
	/*
	for(UINT i=0;i<NofJoints;i++) dJointDestroy(Joints[i]);
	for(UINT i=0;i<NofBodies;i++) dBodyDestroy(Bodies[i]);
	for(UINT i=0;i<NofGeoms;i++) dGeomDestroy(Geoms[i]);
	dGeomDestroy(GeomsGroup);
*/

}

void CPHJeep::Steer1(const char& velocity, const char& steering)
{
	static const dReal steeringRate = M_PI * 4 / 3;
	static const dReal steeringLimit = M_PI / 6;
	static const dReal wheelVelocity = 1.f * M_PI;
	ULONG i;
	switch(steering)
	{
	case 1:
	case -1:
		for(i = 2; i < 4; ++i)
		{
			dJointSetHinge2Param(Joints[i], dParamHiStop, steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamLoStop, -steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamVel, ((i < 2) ? steering : -steering) * steeringRate);
		}
		break;

	default: // case 0
		for(i = 0; i < 4; ++i)
		{
			dReal angle = dJointGetHinge2Angle1(Joints[i]);
			if(angle < 0)
			{
				dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
				dJointSetHinge2Param(Joints[i], dParamVel, steeringRate);
			}
			else
			{
				dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
				dJointSetHinge2Param(Joints[i], dParamVel, -steeringRate);
			}
		}
		break;
	}

	switch(velocity)
	{
	case 1:
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -wheelVelocity : wheelVelocity);
		break;

	case -1:
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? wheelVelocity : -wheelVelocity);
		break;

	default: // case 0
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		break;
	}
}

///////////////////////////////////////////////////////////////
void CPHJeep::CreateDynamicData(){
DynamicData.Create(4,Bodies[0]);
DynamicData.SetChild(0,0,Bodies[1]);
DynamicData.SetChild(1,0,Bodies[2]);
DynamicData.SetChild(2,0,Bodies[3]);
DynamicData.SetChild(3,0,Bodies[4]);
DynamicData.SetTransform(Geoms[5]);
DynamicData.SetGeom(Geoms[0]);
Fmatrix Translate;

DynamicData.CalculateData();
DynamicData.SetAsZeroRecursive();

Translate.translate(0,MassShift-0.810f,0);
DynamicData.SetZeroTransform(Translate);

}

void CPHJeep::Steer(const char& steering)
{
	static const dReal steeringRate = M_PI * 2 / 5;
	static const dReal steeringLimit = M_PI / 4;
	
	ULONG i;
	switch(steering)
	{
	case 1:
	case -1:
		weels_limited=true;
		for(i = 2; i < 4; ++i)
		{
			dJointSetHinge2Param(Joints[i], dParamHiStop, steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamLoStop, -steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamVel, ((i < 2) ? steering : -steering) * steeringRate);
		}
		break;

	default: // case 0
		weels_limited=false;
		for(i = 2; i < 4; ++i)
		{
			dReal angle = dJointGetHinge2Angle1(Joints[i]);
	
			
			if(angle < 0)
			{
				dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
			
				dJointSetHinge2Param(Joints[i], dParamVel, steeringRate);
			}
			else
			{	

				dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
				dJointSetHinge2Param(Joints[i], dParamVel, -steeringRate);
			}
		
		}
		break;
	}

}

void CPHJeep::LimitWeels()
{
if(weels_limited) return;

	for(int i = 2; i < 4; ++i)
		{
			dReal angle = dJointGetHinge2Angle1(Joints[i]);
			if(dFabs(angle)<M_PI/180.f)
			{
				
				dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
				dJointSetHinge2Param(Joints[i], dParamLoStop, 0);///
				dJointSetHinge2Param(Joints[i], dParamVel, 0);
				weels_limited=true;
			}
		}
	
}
////////////////////////////////////////////////////////////////
void CPHJeep::Drive(const char& velocity,dReal force)
{

	static const dReal wheelVelocity = 12.f * M_PI;//3*18.f * M_PI;
	ULONG i;


	switch(velocity)
	{
	case 1:
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -wheelVelocity*VelocityRate : wheelVelocity*VelocityRate);
		break;

	case -1:
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? wheelVelocity*VelocityRate : -wheelVelocity*VelocityRate);
		break;

	default: // case 0
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		break;
	}
	for(i=0;i<4;i++)
			dJointSetHinge2Param(Joints[i], dParamFMax2, force);
}
//////////////////////////////////////////////////////////
void CPHJeep::Drive()
{

	static const dReal wheelVelocity = 12.f * M_PI;//3*18.f * M_PI;
	ULONG i;

if(!Breaks)
	switch(DriveDirection)
	{
	case 1:
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -DriveVelocity : DriveVelocity);
		break;

	case -1:
        for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? DriveVelocity : -DriveVelocity);
		break;
	case 0:
		for(i = 0; i < 4; ++i){
			dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
			dJointSetHinge2Param(Joints[i], dParamFMax2, 100);
		}
		return;
	}
	else {
		for(i = 0; i < 2; ++i){
			dJointSetHinge2Param(Joints[i], dParamFMax2, 5000);
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
			}
		return;
		}
	
	for(i=0;i<4;i++)
			dJointSetHinge2Param(Joints[i], dParamFMax2, DriveForce);
}
/////////////////////////////////////////
void CPHJeep::NeutralDrive(){
	
	for(UINT i = 0; i < 4; ++i){
			dJointSetHinge2Param(Joints[i], dParamFMax2, 10);
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
			}
}
//////////////////////////////////////////////////////////
void CPHJeep::Revert(){
dBodyAddForce(Bodies[0], 0, 2*9000, 0);
dBodyAddRelTorque(Bodies[0], 300, 0, 0);
}
////////////////////////////////////////////////////////////////////////////
///////////CPHWorld/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CPHWorld::Create(){

	phWorld = dWorldCreate();
	Space = dHashSpaceCreate();
	ContactGroup = dJointGroupCreate(0);		
	dWorldSetGravity(phWorld, 0,-2.f*9.81f, 0);//-2.f*9.81f
	Mesh.Create(Space,phWorld);
	//Jeep.Create(Space,phWorld);//(Space,phWorld)
	Gun.Create(Space);
	///dCreatePlane(Space,0,1,0,5);
	dReal k_p=1000000.f;
	dReal k_d=2000.f;
	dReal h=0.02222f;
	dWorldSetERP(phWorld,  h*k_p / (h*k_p + k_d));
	dWorldSetCFM(phWorld,  1.f / (h*k_p + k_d));
	disable_count=0;
	//Jeep.DynamicData.CalculateData();
}

/////////////////////////////////////////////////////////////////////////////

void CPHWorld::Destroy(){
	Mesh.Destroy();
	//Jeep.Destroy();
	Gun.Destroy();
	//vector<CPHElement*>::iterator i;
	//for(i=elements.begin();i!=elements.end();i++){
	//delete(*i);
	//}
	dJointGroupEmpty(ContactGroup);
	dSpaceDestroy(Space);
	dWorldDestroy(phWorld);
	dCloseODE();

}

//////////////////////////////////////////////////////////////////////////////
//static dReal frame_time=0.f;
const int dis_frames=11;
void CPHWorld::Step(dReal step)
{
			// compute contact joints and forces

	list<CPHObject*>::iterator iter;
	//step+=astep;
	//const dReal max_step=0.02f;//0.0034f;
	//const dReal min_step=0.005f;
	const  dReal k_p=24000000.f;//550000.f;///1000000.f;
	const dReal k_d=400000.f;
	UINT it_number;
	const dReal fixed_step=0.02f;
	frame_time+=step;

	if(!(frame_time<fixed_step)){
	it_number=(UINT)(frame_time/fixed_step);
	frame_time-=it_number*fixed_step;
	}
	else return;


			for(UINT i=0; i<it_number;i++)
		{
			disable_count++;	
			if(disable_count==dis_frames+1) disable_count=0;

					
			dWorldSetERP(phWorld,  fixed_step*k_p / (fixed_step*k_p + k_d));
			dWorldSetCFM(phWorld,  1.f / (fixed_step*k_p + k_d));

//			dWorldSetERP(phWorld,  0.2);
//			dWorldSetCFM(phWorld,  0.0001);



			dSpaceCollide(Space, 0, &NearCallback); 
		
		for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhTune(fixed_step);	

			dWorldStep(phWorld, fixed_step);
			
		for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhDataUpdate(fixed_step);
			dJointGroupEmpty(ContactGroup);
	
		

		}
	
/*
	if(step<=max_step){
			
	disable_count++;	
	if(disable_count==dis_frames+1) disable_count=0;

		
			dWorldSetERP(phWorld,  step*k_p / (step*k_p + k_d));
			dWorldSetCFM(phWorld,  1.f / (step*k_p + k_d));
	
			
			for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhTune(step);	
			
			
			dSpaceCollide(Space, 0, &NearCallback);
		
			
			//if(isShooting&&bulletContact.geom.pos[0]!=dInfinity){
			//	dJointID c = dJointCreateContact(phWorld, ContactGroup, &bulletContact);
			//	dJointAttach(c, dGeomGetBody(bulletContact.geom.g1), dGeomGetBody(bulletContact.geom.g2));
				
			//	}
			
		
			
	

			dWorldStep(phWorld, step);

			for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhDataUpdate(step);
			//Jeep.DynamicData.CalculateData();
			
			dJointGroupEmpty(ContactGroup);
	
			//isShooting=false;
			//bulletContact.geom.pos[0]=dInfinity;
				
	}
	else{
		UINT n=(UINT)(step/max_step)+1;
		for(UINT i=0; i<n;i++)
		{
			disable_count++;	
			if(disable_count==dis_frames+1) disable_count=0;

			dWorldSetERP(phWorld,  step*k_p/n / (step*k_p/n + k_d));
			dWorldSetCFM(phWorld,  1.f / (step*k_p/n + k_d));

//			dWorldSetERP(phWorld,  0.2);
//			dWorldSetCFM(phWorld,  0.0001);

			for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhTune(step/n);	
//			Jeep.JointTune(step/n);
			dSpaceCollide(Space, 0, &NearCallback); 
		

			dWorldStep(phWorld, step/n);
			//Jeep.DynamicData.CalculateData();
		for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhDataUpdate(step/n);
			dJointGroupEmpty(ContactGroup);
	
		

		}
	}
//Jeep.DynamicData.CalculateData();
//	}
//step=0.f;
*/
}

static void FUNCCALL NearCallback(void* /*data*/, dGeomID o1, dGeomID o2){
		const ULONG N = 100;
		dContact contacts[N];
		// get the contacts up to a maximum of N contacts
		ULONG n = dCollide(o1, o2, N, &contacts[0].geom, sizeof(dContact));	
		

		if(
			dGeomGetClass(o1)==dRayClass||dGeomGetClass(o2)==dRayClass
			){
	
			if(isShooting){
				dVector3 norm={RayD[0],RayD[1],RayD[2]};
				dNormalize3(norm);
			//	ulong n = dCollide(o1, o2, N, &bulletContact.geom, sizeof(dContact));
				for(UINT i = 0; i < n; ++i)
				{
			
					if(dDOT(contacts[i].geom.pos,norm)<dDOT(bulletContact.geom.pos,norm))

					{
					memcpy(&bulletContact,&contacts[i],sizeof(dContact));
					bulletContact.geom.normal[0]=norm[0];
					bulletContact.geom.normal[1]=norm[1];
					bulletContact.geom.normal[2]=norm[2];
				
					bulletContact.geom.depth=0.5;
				
					bulletContact.surface.mode = dContactBounce;
					bulletContact.surface.mu = 0.0f;
					bulletContact.surface.bounce = 0.3f;
					bulletContact.surface.bounce_vel = 0.01f;
					}
				}
	
			}
		return;
	}
		


	if(n>N)
		n=N;
	ULONG i;

//if(dGeomGetClass(o1)==2/*dGeomTransformClass*/){
/*	for(i = 0; i < n; ++i)
	{

        contacts[i].surface.mode = dContactBounce;
		contacts[i].surface.mu = 2500;
		contacts[i].surface.bounce = 0.3f;
		contacts[i].surface.bounce_vel = 0.005f;
		dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(o1), dGeomGetBody(contacts[i].geom.g2));
		}

	}
	*/
//else if(dGeomGetClass(o2)==2/*dGeomTransformClass*/){
/*		for(i = 0; i < n; ++i)
		{

        contacts[i].surface.mode = dContactBounce;
		contacts[i].surface.mu = 2500.f;
		contacts[i].surface.bounce = 0.1f;
		contacts[i].surface.bounce_vel = 0.005f;
		dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(o2));
		}
	}

else
*/
	for(i = 0; i < n; ++i)
	{

        contacts[i].surface.mode =dContactApprox1; //dContactBounce|dContactApprox1;
		contacts[i].surface.mu =1.f;// 5000.f;
		bool pushing_neg=false;
		if(contacts[i].geom.g2->data){ 
			//contacts[i].surface.mu=dGeomGetUserData(contacts[i].geom.g2)->friction;
			if(dGeomGetClass(contacts[i].geom.g2)==dGeomTransformClass){
				const dGeomID geom=dGeomTransformGetGeom(contacts[i].geom.g2);
				pushing_neg=dGeomGetUserData(geom)->pushing_b_neg||dGeomGetUserData(geom)->pushing_neg;
			}
			else
				pushing_neg=dGeomGetUserData(contacts[i].geom.g2)->pushing_b_neg||
				dGeomGetUserData(contacts[i].geom.g2)->pushing_neg;
	
			if(dGeomGetUserData(contacts[i].geom.g2)->ph_object){
					dGeomGetUserData(contacts[i].geom.g2)->ph_object->InitContact(&contacts[i]);
					dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
					dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
					continue;
			}
		}
		if(contacts[i].geom.g1->data){ 
			//contacts[i].surface.mu=dGeomGetUserData(contacts[i].geom.g1)->friction;
			if(dGeomGetClass(contacts[i].geom.g1)==dGeomTransformClass){
				const dGeomID geom=dGeomTransformGetGeom(contacts[i].geom.g1);
				pushing_neg=dGeomGetUserData(geom)->pushing_b_neg||dGeomGetUserData(geom)->pushing_neg;
			}
			else
				pushing_neg=dGeomGetUserData(contacts[i].geom.g1)->pushing_b_neg||
				dGeomGetUserData(contacts[i].geom.g1)->pushing_neg;

			if(dGeomGetUserData(contacts[i].geom.g1)->ph_object){
					dGeomGetUserData(contacts[i].geom.g1)->ph_object->InitContact(&contacts[i]);
					dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
					dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
					continue;
				}

		}
		if(pushing_neg) contacts[i].surface.mu=dInfinity;

		contacts[i].surface.bounce = 0.0f;//0.1f;
		contacts[i].surface.bounce_vel =0.001f;//0.005f;
		dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
		}
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////CPHGun//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void CPHGun::Create(dSpaceID space){
	
	dVector3 rayO={0.f,0.f,0.f};
	dVector3 rayD={0.f,1.f,0.f};
	Ray=dGeomCreateRay(space,1.f);
	dGeomRaySet(Ray,rayO,rayD);
	isShooting=false;
}

///////////////////////////////////////////////////////////////////////////////////////////
void CPHGun::Destroy(){
dGeomDestroy(Ray);
}



///////////////////////////////////////////////////////////////////////////////////////////


void CPHGun::Shoot(const dReal* rayO,const dReal*  rayD){
bulletContact.geom.pos[0]=dInfinity;
RayO[0]=rayO[0];
RayO[1]=rayO[1];
RayO[2]=rayO[2];
RayD[0]=rayD[0];
RayD[1]=rayD[1];
RayD[2]=rayD[2];
dGeomRaySet(Ray,RayO,RayD);

isShooting=true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Implementation for CPhysicsElement
void CPHElement::			add_Box		(const Fobb&		V){
	m_boxes_data.push_back(V);
}

void CPHElement::			create_Box		(Fobb&		V){
														dGeomID geom,trans;
														geom=dCreateBoxA(0,
																		V.m_halfsize.x*2.f,
																		V.m_halfsize.y*2.f,
																		V.m_halfsize.z*2.f);
														
														m_geoms.push_back(geom);
														dGeomSetPosition(geom,
															V.m_translate.x-m_mass_center.x,
															V.m_translate.y-m_mass_center.y,
															V.m_translate.z-m_mass_center.z);
														dMatrix3 R;
														PHDynamicData::FMX33toDMX(V.m_rotate,R);
														dGeomSetRotation(geom,R);
														trans=dCreateGeomTransform(0);
														dGeomTransformSetGeom(trans,geom);
														dGeomSetBody(trans,m_body);
														m_trans.push_back(trans);
														dGeomGroupAdd(m_group,trans);
														dGeomTransformSetInfo(trans,1);
														dGeomCreateUserData(trans);
														dGeomCreateUserData(geom);
														dGeomGetUserData(trans)->friction=friction_table[1];
														//dGeomGetUserData(trans)->friction=dInfinity;
														
														}

void CPHElement::			add_Sphere	(const Fsphere&	V){
	m_spheras_data.push_back(V);
}

void CPHElement::			create_Sphere	(Fsphere&	V){
														dGeomID geom,trans;
														geom=dCreateSphere(0,V.R);
														m_geoms.push_back(geom);
														dGeomSetPosition(geom,V.P.x,V.P.y,V.P.z);
														trans=dCreateGeomTransform(0);
														dGeomTransformSetGeom(trans,m_geoms.back());
														dGeomSetBody(trans,m_body);
														m_trans.push_back(trans);
														dGeomGroupAdd(m_group,m_trans.back());
														dGeomTransformSetInfo(m_trans.back(),1);		
														};
void CPHElement::			build	(dSpaceID space){

m_body=dBodyCreate(phWorld);
dBodySetMass(m_body,&m_mass);
m_group=dCreateGeomGroup(space);
Fvector mc=get_mc_data();
//m_start=mc;

m_inverse_local_transform.identity();
m_inverse_local_transform.c.set(mc);
m_inverse_local_transform.invert();
dBodySetPosition(m_body,mc.x,mc.y,mc.z);
vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
	create_Box(*i_box);
	}
	vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
		create_Sphere(*i_sphere);
		}
}

void CPHElement::			destroy	(){
	vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++){
	dGeomDestroyUserData(*i);
	dGeomDestroy(*i);
	}
	for(i=m_trans.begin();i!=m_trans.end();i++){
	dGeomDestroyUserData(*i);
	dGeomDestroy(*i);
	
	}
	dGeomDestroy(m_group);
	dBodyDestroy(m_body);
	m_geoms.clear();
	m_trans.clear();
}

Fvector CPHElement::			get_mc_data	(){
	Fvector mc,s;
	float pv,volume=0.f;
	mc.set(0,0,0);
	vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
	pv=(*i_box).m_halfsize.x*(*i_box).m_halfsize.y*(*i_box).m_halfsize.z*8;
	s.mul((*i_box).m_translate,pv);
	volume+=pv;
	mc.add(s);
	}
	vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
	pv=(*i_sphere).R*(*i_sphere).R*(*i_sphere).R*4/3*M_PI;
	s.mul((*i_sphere).P,pv);
	volume+=pv;
	mc.add(s);
	}
	m_volume=volume;
	mc.mul(1.f/volume);
	m_mass_center=mc;
	return mc;
}
Fvector CPHElement::			get_mc_geoms	(){
Fvector mc;
mc.set(0.f,0.f,0.f);
return mc;
}

void CPHElement::calculate_it_data(const Fvector& mc,float mas){
	dMass m;
	dMassSetZero(&m_mass);
	vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
	Fvector& hside=(*i_box).m_halfsize;
	Fvector& pos=(*i_box).m_translate;
	Fvector l;
	l.sub(pos,mc);
	dMassSetBox(&m,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
	dMassAdjust(&m,hside.x*hside.y*hside.z*8.f/m_volume*mas);
	dMassTranslate(&m,l.x,l.y,l.z);
	dMassAdd(&m_mass,&m);
	
	}

	vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
	Fvector& pos=(*i_sphere).P;
	Fvector l;
	l.sub(pos,mc);
	dMassSetSphere(&m,mas/m_volume,(*i_sphere).R);
	dMassTranslate(&m,l.x,l.y,l.z);
	dMassAdd(&m_mass,&m);

	}


}


void CPHElement::calculate_it_data_use_density(const Fvector& mc,float density){
	
	dMass m;
	dMassSetZero(&m_mass);
	vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
	Fvector& hside=(*i_box).m_halfsize;
	Fvector& pos=(*i_box).m_translate;
	Fvector l;
	l.sub(pos,mc);
	dMassSetBox(&m,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
	dMassAdjust(&m,hside.x*hside.y*hside.z*8.f*density);
	dMassTranslate(&m,l.x,l.y,l.z);
	dMassAdd(&m_mass,&m);
	
	}

	vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
	Fvector& pos=(*i_sphere).P;
	Fvector l;
	l.sub(pos,mc);
	dMassSetSphere(&m,density,(*i_sphere).R);
	dMassTranslate(&m,l.x,l.y,l.z);
	dMassAdd(&m_mass,&m);

	}


}



void		CPHElement::	setMass		(float M){
//calculate_it_data(get_mc_data(),M);

calculate_it_data_use_density(get_mc_data(),M);

}
void		CPHElement::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	//mXFORM.set(m0);
	//m_m0.set(m0);
	//m_m2.set(m2);
}
void		CPHElement::Start(){
	//mXFORM.set(m0);
	build(m_space);
	//dBodySetPosition(m_body,m_m0.c.x,m_m0.c.y,m_m0.c.z);
	//Fmatrix33 m33;
	//m33.set(m_m0);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
	//dBodySetRotation(m_body,R);
}

void		CPHElement::Deactivate(){
	destroy();
//	bActive=false;
}
void CPHElement::SetTransform(const Fmatrix &m0){
	Fvector mc;
	mc.set(m_mass_center);
	m0.transform_tiny(mc);
	dBodySetPosition(m_body,mc.x,mc.y+0.0f,mc.z);
	Fmatrix33 m33;
	m33.set(m0);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m33,R);
	dBodySetRotation(m_body,R);
		
}
CPHElement::~CPHElement	(){

}
void		CPHElement::Update(){
	//mXFORM.identity();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPHShell::setMass(float M){
	vector<CPHElement*>::iterator i;
	float volume=0.f;
	for(i=elements.begin();i!=elements.end();i++)	volume+=(*i)->get_volume();

	for(i=elements.begin();i!=elements.end();i++)
	(*i)->setMass(
				(*i)->get_volume()/volume*M
				);
}


void CPHShell::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	if(bActive)
		return;
	m_ident=ph_world->AddObject(this);

		vector<CPHElement*>::iterator i;
		
		mXFORM.set(m0);
		for(i=elements.begin();i!=elements.end();i++){
														(*i)->Start();
														(*i)->SetTransform(m0);
			}
	i=elements.begin();
	m_body=(*i)->get_body();
	m_inverse_local_transform.set((*i)->m_inverse_local_transform);
	Fmatrix33 m33;
	Fmatrix m,m1;
	m1.set(m0);
	m1.invert();
	m.mul(m1,m2);
	//m.mul(1.f/dt01);
	m33.set(m);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m33,R);
	dBodySetLinearVel(m_body,m.c.x,m.c.y,m.c.z);
	//dBodySetPosition(m_body,m0.c.x,m0.c.y+1.,m0.c.z);
	bActive=true;

//////////////////////////////////////////////////////////////
//initializing values for disabling//////////////////////////
//////////////////////////////////////////////////////////////
	/*
	mean_w[0]=0.f;
	mean_w[1]=0.f;
	mean_w[2]=0.f;
	mean_v[0]=0.f;
	mean_v[1]=0.f;
	mean_v[2]=0.f;
	*/
	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);
}

void CPHShell::Deactivate(){
if(!bActive)
		return;
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	(*i)->Deactivate();
ph_world->RemoveObject(m_ident);
bActive=false;
}

void CPHShell::PhDataUpdate(dReal step){
//////////////////////////////////////////////////////////////////////
////limit velocity of the main body/////////////////////////////////
/////////////////////////////////////////////////////////////////////
	if( !dBodyIsEnabled(m_body)) {
					if(previous_p[0]!=dInfinity) previous_p[0]=dInfinity;
					return;
				}

				static const dReal u = -0.1f;
				static const dReal w_limit = M_PI/4.f/0.02f;
				static const dReal l_limit = 3.f/0.02f;
				dReal mag;
				const dReal* pos = dBodyGetLinearVel(m_body);
				mag=sqrtf(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]);
				if(mag>l_limit){
					dReal f=mag/l_limit;
					dBodySetLinearVel(m_body,pos[0]/f,pos[1]/f,pos[2]/f);
				}
				//dBodyAddForce(m_body, u * pos[0]*mag, u * pos[1]*mag, u * pos[2]*mag);
				const dReal* rot = dBodyGetAngularVel(m_body);
				mag=sqrtf(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
				if(mag>w_limit){
					dReal f=mag/w_limit;
					dBodySetAngularVel(m_body,rot[0]/f,rot[1]/f,rot[2]/f);
				}

/////////////////////////////////////////////////////////////////////////////////////
////////disabling main body//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
				{
				const dReal* torqu=dBodyGetTorque(m_body);
				const dReal* force=dBodyGetForce(m_body);
				dReal t_m =sqrtf(	torqu[0]*torqu[0]+
										torqu[1]*torqu[1]+
										torqu[2]*torqu[2]);
				dReal f_m=sqrtf(	force[0]*force[0]+
										force[1]*force[1]+
										force[2]*force[2]);
				if(t_m+f_m>0.000000){
					previous_p[0]=dInfinity;
					previous_dev=0;
					previous_v=0;
					dis_count_f=1;
					dis_count_f1=0;
					return;
				}
				}
				if(previous_p[0]==dInfinity&&ph_world->disable_count==0){
					const dReal* position=dBodyGetPosition(m_body);
					memcpy(previous_p,position,sizeof(dVector3));
					memcpy(previous_p1,position,sizeof(dVector3));
					const dReal* rotation=dBodyGetRotation(m_body);
					memcpy(previous_r,rotation,sizeof(dMatrix3));
					memcpy(previous_r1,rotation,sizeof(dMatrix3));
					previous_dev=0;
					previous_v=0;
					dis_count_f=1;
					dis_count_f1=0;
				}

			
				if(ph_world->disable_count==dis_frames){	
					if(previous_p[0]!=dInfinity){
					const dReal* current_p=dBodyGetPosition(m_body);
					dVector3 velocity={current_p[0]-previous_p[0],
									   current_p[1]-previous_p[1],
									   current_p[2]-previous_p[2]};
					dReal mag_v=sqrtf(
						  velocity[0]*velocity[0]+
						  velocity[1]*velocity[1]+
						  velocity[2]*velocity[2]);
					mag_v/=dis_count_f;
					const dReal* current_r=dBodyGetRotation(m_body);
					dMatrix3 rotation_m;
					
					dMULTIPLYOP1_333(rotation_m,=,previous_r,current_r);
			
					dVector3 deviation_v={rotation_m[0]-1.f,
										  rotation_m[5]-1.f,
										  rotation_m[10]-1.f
					};

					dReal deviation =sqrtf(deviation_v[0]*deviation_v[0]+
										   deviation_v[1]*deviation_v[1]+
										   deviation_v[2]*deviation_v[2]);

					deviation/=dis_count_f;
					if(mag_v<0.001* dis_frames && deviation<0.00001*dis_frames)
						dBodyDisable(m_body);
					if((previous_dev>deviation&&previous_v>mag_v)
					  ) 
					{
					dis_count_f++;
					previous_dev=deviation;
					previous_v=mag_v;
					//return;
					}
					else{
					previous_dev=0;
					previous_v=0;
					dis_count_f=1;
					memcpy(previous_p,current_p,sizeof(dVector3));
					memcpy(previous_r,current_r,sizeof(dMatrix3));
					}

					{
					
					dVector3 velocity={current_p[0]-previous_p1[0],
									   current_p[1]-previous_p1[1],
									   current_p[2]-previous_p1[2]};
					dReal mag_v=sqrtf(
						  velocity[0]*velocity[0]+
						  velocity[1]*velocity[1]+
						  velocity[2]*velocity[2]);
					mag_v/=dis_count_f;
					dMatrix3 rotation_m;
					dMULTIPLYOP1_333(rotation_m,=,previous_r1,current_r);
			
					dVector3 deviation_v={rotation_m[0]-1.f,
										  rotation_m[5]-1.f,
										  rotation_m[10]-1.f
					};

					dReal deviation =sqrtf(deviation_v[0]*deviation_v[0]+
										   deviation_v[1]*deviation_v[1]+
										   deviation_v[2]*deviation_v[2]);

					deviation/=dis_count_f;
					if(mag_v<0.04* dis_frames && deviation<0.01*dis_frames)
						dis_count_f1++;
					else{
						memcpy(previous_p1,current_p,sizeof(dVector3));
						memcpy(previous_r1,current_r,sizeof(dMatrix3));
					}
					if(dis_count_f1>10) dis_count_f*=10;
					}


					}
					
				
				}
/////////////////////////////////////////////////////////////////
	
				

}
void CPHShell::PhTune(dReal step){
}

void CPHShell::Update(){
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	(*i)->Update();

if( !dBodyIsEnabled(m_body)) return;
				


				PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),
							  dBodyGetPosition(m_body),
							  mXFORM);
	
				mXFORM.mulB(m_inverse_local_transform);

}