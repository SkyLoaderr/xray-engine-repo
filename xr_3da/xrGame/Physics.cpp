#pragma once

typedef void __stdcall BoneCallbackFun(CBoneInstance* B);
typedef  void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	 void __stdcall ObjectContactCallbackFun(bool& do_colide,dContact& c);
struct Fcylinder;
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
///////////////////////////////////////////////////////////////
#pragma warning(disable:4995)
#include "..\ode\src\collision_kernel.h"
#include <..\ode\src\joint.h>
#include <..\ode\src\objects.h>
#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////
//#include "dRay/include/dRay.h"
#include "ExtendedGeom.h"
union dInfBytes dInfinityValue = {{0,0,0x80,0x7f}};
Shader* CPHElement::hWallmark=NULL;
// #include "contacts.h"




//void _stdcall dGeomTransformSetInfo (dGeomID g, int mode);
/////////////////////////////////////////
//static dContact bulletContact;
//static bool isShooting;
//static dVector3 RayD;
//static dVector3 RayO;

dWorldID phWorld;
/////////////////////////////////////
static void NearCallback(void* /*data*/, dGeomID o1, dGeomID o2);
/*
Log("fgfgfgf","ffff");

Log("fgfgfgf",12);
Fvector f;
Log("dfdf ",f);
Msg("%s","rrr");
*/
//Level().HUD().pFontSmall->Out(x,y,"%s","sdfsdsd");

void CPHWorld::Render()
{
	if (!bDebug)	return;
	/*
	RCache.OnFrameEnd		();
	RCache.set_xform_world	(Fidentity);
	Fmatrix M;
	Fvector scale;


	Jeep.DynamicData.GetWorldMX(M);
	scale.set(Jeep.jeepBox[0]/2.f,Jeep.jeepBox[1]/2.f,Jeep.jeepBox[2]/2.f);
	RCache.dbg_DrawOBB	(M,scale,0xffffffff);

	Jeep.DynamicData.GetTGeomWorldMX(M);
	scale.set(Jeep.cabinBox[0]/2.f,Jeep.cabinBox[1]/2.f,Jeep.cabinBox[2]/2.f);
	RCache.dbg_DrawOBB	(M,scale,0xffffffff);



	scale.set(1.6f/0.8f*0.28f,1.6f/0.8f*0.28f,1.6f/0.8f*0.28f);
	Jeep.DynamicData[0].GetWorldMX(M);

	Fvector t = M.c;
	M.scale(scale);
	M.c = t;

	RCache.dbg_DrawEllipse(M, 0xffffffff);
	Jeep.DynamicData[1].GetWorldMX(M);
	t = M.c;
	M.scale(scale);
	M.c = t;
	RCache.dbg_DrawEllipse(M, 0xffffffff);
	Jeep.DynamicData[2].GetWorldMX(M);
	t = M.c;
	M.scale(scale);
	M.c = t;
	RCache.dbg_DrawEllipse(M, 0xffffffff);
	Jeep.DynamicData[3].GetWorldMX(M);
	t = M.c;
	M.scale(scale);
	M.c = t;
	RCache.dbg_DrawEllipse(M, 0xffffffff);
	*/
}

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
						}


/////////////////////////////////////////////////////////////////////////////
/////////////CPHJeep////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CPHJeep::Create1(dSpaceID space, dWorldID world){
	if(bActive) return;
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
	u32 i;
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
bActive=true;
}

/////////////////////////////////////////////////////////////////////////////
void CPHJeep::Create(dSpaceID space, dWorldID world){
	if(bActive) return;
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
	//dGeomCreateUserData(Geoms[5]);
	//dGeomCreateUserData(Geoms[7]);
	dGeomCreateUserData(Geoms[0]);
	dGeomCreateUserData(Geoms[6]);
	dGeomGetUserData(Geoms[0])->material=GMLib.GetMaterialIdx("materials\\car_cabine");
	dGeomGetUserData(Geoms[6])->material=GMLib.GetMaterialIdx("materials\\car_cabine");
	//dGeomGetUserData(Geoms[5])->friction=500.f;
	//dGeomGetUserData(Geoms[7])->friction=500.f;

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
	u32 i;
	for(i = 1; i <= 4; ++i)
	{
		Bodies[i] = dBodyCreate(world);
		dBodySetMass(Bodies[i], &m);
		dBodySetQuaternion(Bodies[i], q);
		//Geoms[i] = dCreateSphere(0, wheelRadius);
		Geoms[i] = dCreateCylinder(0, wheelRadius,0.19f);
		dGeomCreateUserData(Geoms[i]);
		dGeomGetUserData(Geoms[i])->material=GMLib.GetMaterialIdx("materials\\rubber");
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
		dJointSetHinge2Param(Joints[i], dParamFMax, 10000.f );
		dJointSetHinge2Param(Joints[i], dParamFudgeFactor, 0.001f);

		dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
		dJointSetHinge2Param(Joints[i], dParamFMax2, 500.f);
		dReal k_p=20000000.f;//20000.f;
		dReal k_d=10.f;//1000.f;
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
bActive=true;
}
////////////////////////////////////////////////////////////////
void CPHJeep::JointTune(dReal step){
const	dReal k_p=30000.f;//30000.f;
const	dReal k_d=1000.f;//1000.f;
	for(u32 i = 0; i < 4; ++i)
	{

		
		dJointSetHinge2Param(Joints[i], dParamSuspensionERP, step*k_p / (step*k_p + k_d));
		dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 1.f / (step*k_p + k_d));

	}
	static const dReal w_limit = M_PI/16.f/0.02f;
	const dReal* rot = dBodyGetAngularVel(Bodies[0]);
	dReal mag=_sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
				if(mag>w_limit){
					dReal f=mag/w_limit;
					dBodySetAngularVel(Bodies[0],rot[0]/f,rot[1]/f,rot[2]/f);
				}
}
/////////
void CPHJeep::Destroy(){
	if(!bActive) return;
	for(u32 i=0;i<NofGeoms;i++) dGeomDestroyUserData(Geoms[i]);
	DynamicData.Destroy();
	
	for(u32 i=0;i<NofJoints;i++) dJointDestroy(Joints[i]);
	for(u32 i=0;i<NofBodies;i++) dBodyDestroy(Bodies[i]);

	dGeomDestroyUserData(Geoms[5]);
	dGeomDestroyUserData(Geoms[7]);
	dGeomDestroyUserData(Geoms[0]);
	dGeomDestroyUserData(Geoms[6]);

	for(u32 i=0;i<5;i++) {
		dGeomDestroy(Geoms[i]);
	}
	dGeomDestroy(Geoms[6]);
//	dGeomDestroy(Geoms[5]);
	dGeomDestroy(Geoms[7]);
	dGeomDestroy(GeomsGroup);

	bActive=false;
}

void CPHJeep::Steer1(const char& velocity, const char& steering)
{
	if(!bActive) return;
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
Translate.identity();
Translate.translate(0,-MassShift+0.810f,0);
DynamicData.SetZeroTransform(Translate);

}


void CPHJeep::Steer(const char& steering)
{
	if(!bActive) return;
	static const dReal steeringRate = M_PI * 4 / 5;
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
if(!bActive) return;
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
	if(!bActive) return;

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
		/////////////
	switch(DriveDirection)
	{
	case 1:
        for(i = 2; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -DriveVelocity : DriveVelocity);
		break;

	case -1:
        for(i = 2; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? DriveVelocity : -DriveVelocity);
		break;
	case 0:
		for(i = 2; i < 4; ++i){
			dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
			dJointSetHinge2Param(Joints[i], dParamFMax2, 100);
		}
		return;
	}
		/////////////
		for(i=2;i<4;i++)
			dJointSetHinge2Param(Joints[i], dParamFMax2, DriveForce);
		return;
		}
	
	for(i=0;i<4;i++)
			dJointSetHinge2Param(Joints[i], dParamFMax2, DriveForce);
}
/////////////////////////////////////////
void CPHJeep::NeutralDrive(){
if(!bActive) return;
	//////////////////
	for(u32 i = 0; i < 4; ++i){
			dJointSetHinge2Param(Joints[i], dParamFMax2, 10);
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
			}
}
//////////////////////////////////////////////////////////
void CPHJeep::Revert(){
if(!bActive) return;
dBodyAddForce(Bodies[0], 0, 2*9000, 0);
dBodyAddRelTorque(Bodies[0], 300, 0, 0);
}
////////////////////////////////////////////////////////////////////////////
///////////CPHWorld/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


//dGeomID plane;
void CPHWorld::Create(){

	phWorld = dWorldCreate();
	Space = dHashSpaceCreate(0);
	ContactGroup = dJointGroupCreate(0);		
	dWorldSetGravity(phWorld, 0,-2.f*9.81f, 0);//-2.f*9.81f
	Mesh.Create(Space,phWorld);
	//Jeep.Create(Space,phWorld);//(Space,phWorld)
	//Gun.Create(Space);
	//plane=dCreatePlane(Space,0,1,0,0.1f);
	//dGeomCreateUserData(plane);
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
	//Jeep.Destroy();
	//Gun.Destroy();
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
void CPHWorld::Step(dReal step)
{

	// compute contact joints and forces

	list<CPHObject*>::iterator iter;
	//step+=astep;

	//const  dReal k_p=24000000.f;//550000.f;///1000000.f;
	//const dReal k_d=400000.f;
	u32 it_number;
	frame_time+=step;
	//m_frame_sum+=step;

	if(!(frame_time<fixed_step))
	{
		it_number	=	iFloor	(frame_time/fixed_step);
		frame_time	-=	it_number*fixed_step;
	}
	else return;
	/*
	m_update_delay_count++;

	if(m_update_delay_count==update_delay){
	if(m_delay){
	if(m_delay<m_previous_delay) m_reduce_delay--;
	else 	m_reduce_delay++;
	}
	m_previous_delay=m_delay;
	m_update_delay_count=0;
	}

	m_delay+=(it_number-m_reduce_delay-1);
*/
	//for(UINT i=0;i<(m_reduce_delay+1);i++)
	for(UINT i=0; i<it_number;i++)
		{
				
			disable_count++;		
			if(disable_count==dis_frames+1) disable_count=0;
			
			m_steps_num++;
			//double dif=m_frame_sum-Time();
			//if(fabs(dif)>fixed_step) 
			//	m_start_time+=dif;

		//	dWorldSetERP(phWorld,  fixed_step*k_p / (fixed_step*k_p + k_d));
		//	dWorldSetCFM(phWorld,  1.f / (fixed_step*k_p + k_d));

		//dWorldSetERP(phWorld,  0.8);
		//dWorldSetCFM(phWorld,  0.00000001);



		Device.Statistic.ph_collision.Begin	();
		dSpaceCollide		(Space, 0, &NearCallback); 
		Device.Statistic.ph_collision.End	();

		for(iter=m_objects.begin();iter!=m_objects.end();iter++)
			(*iter)->PhTune(fixed_step);	

		Device.Statistic.ph_core.Begin		();
		dWorldStep			(phWorld, fixed_step);
		//dWorldStepFast (phWorld,fixed_step,10);
		Device.Statistic.ph_core.End		();

		for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhDataUpdate(fixed_step);
		dJointGroupEmpty(ContactGroup);





		//	for(iter=m_objects.begin();iter!=m_objects.end();iter++)
		//			(*iter)->StepFrameUpdate(step);

	}

}

static void NearCallback(void* /*data*/, dGeomID o1, dGeomID o2){
const ULONG N = 100;
dContact contacts[N];

		// get the contacts up to a maximum of N contacts
		ULONG n;
	
		n = dCollide(o1, o2, N, &contacts[0].geom, sizeof(dContact));	
		
	if(n>N-1)
		n=N-1;
	ULONG i;


	for(i = 0; i < n; ++i)
	{
		if(i!=0) {
		  dReal dif=dFabs(contacts[i-1].geom.pos[0]-contacts[i].geom.pos[0])+
				dFabs(contacts[i-1].geom.pos[1]-contacts[i].geom.pos[1])+
				dFabs(contacts[i-1].geom.pos[2]-contacts[i].geom.pos[2]);
		  if(dif==0.f) continue;
		}

       
		if(dGeomGetClass(contacts[i].geom.g1)!=dTriListClass &&
			dGeomGetClass(contacts[i].geom.g2)!=dTriListClass){
											contacts[i].surface.mu =1.f;// 5000.f;
											contacts[i].surface.soft_erp=1.f;//ERP(world_spring,world_damping);
											contacts[i].surface.soft_cfm=1.f;//CFM(world_spring,world_damping);
											contacts[i].surface.bounce = 0.01f;//0.1f;
											contacts[i].surface.mode=0;
											}


		bool pushing_neg=false;
		dxGeomUserData* usr_data_1=NULL;
		dxGeomUserData* usr_data_2=NULL;
		if(dGeomGetClass(contacts[i].geom.g1)==dGeomTransformClass){
				const dGeomID geom=dGeomTransformGetGeom(contacts[i].geom.g1);
				usr_data_1 = dGeomGetUserData(geom);
			}
		else
			usr_data_1 = dGeomGetUserData(contacts[i].geom.g1);

		if(dGeomGetClass(contacts[i].geom.g2)==dGeomTransformClass){
				const dGeomID geom=dGeomTransformGetGeom(contacts[i].geom.g2);
				usr_data_2 = dGeomGetUserData(geom);
			}
		else
			usr_data_2 = dGeomGetUserData(contacts[i].geom.g2);
/////////////////////////////////////////////////////////////////////////////////////////////////
		if(usr_data_2){ 
				pushing_neg=usr_data_2->pushing_b_neg||usr_data_2->pushing_neg;
				contacts[i].surface.mu*=GMLib.GetMaterial(usr_data_2->material)->fPHFriction;
				contacts[i].surface.soft_cfm*=GMLib.GetMaterial(usr_data_2->material)->fPHSpring;
				contacts[i].surface.soft_erp*=GMLib.GetMaterial(usr_data_2->material)->fPHDamping;
			if(usr_data_2->ph_object){
					usr_data_2->ph_object->InitContact(&contacts[i]);
					//if(pushing_neg) contacts[i].surface.mu=dInfinity;
					//dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
					//dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
					//continue;

			}
			if(usr_data_2->object_callback){
				bool do_colide=true;
				usr_data_2->object_callback(do_colide,contacts[i]);
				if(!do_colide) continue;
			}
		}
///////////////////////////////////////////////////////////////////////////////////////
		if(usr_data_1){ 

				pushing_neg=usr_data_1->pushing_b_neg||
				usr_data_1->pushing_neg;
				contacts[i].surface.mu*=GMLib.GetMaterial(usr_data_1->material)->fPHFriction;
				contacts[i].surface.soft_cfm*=GMLib.GetMaterial(usr_data_1->material)->fPHSpring;
				contacts[i].surface.soft_erp*=GMLib.GetMaterial(usr_data_1->material)->fPHDamping;
			if(usr_data_1->ph_object){
					usr_data_1->ph_object->InitContact(&contacts[i]);

					//if(pushing_neg) contacts[i].surface.mu=dInfinity;
					//dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
					//dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
					//continue;
				}
			if(usr_data_1->object_callback){
				bool do_colide=true;
				usr_data_1->object_callback(do_colide,contacts[i]);
				if(!do_colide) continue;
			}
		}

		contacts[i].surface.mode =dContactBounce|dContactApprox1|dContactSoftERP|dContactSoftCFM;
		dReal cfm=1.f/(world_spring*contacts[i].surface.soft_cfm*fixed_step+world_damping*contacts[i].surface.soft_erp);
		
		
		//contacts[i].surface.soft_erp=ERP(world_spring*contacts[i].surface.soft_cfm,
		//								 world_damping*contacts[i].surface.soft_erp);
		//contacts[i].surface.soft_cfm=CFM(world_spring*contacts[i].surface.soft_cfm,
		//								 world_damping*contacts[i].surface.soft_erp);



			//dReal erp1=ERP(world_spring,world_damping);//0.54545456
			//dReal cfm1=CFM(world_spring,world_damping);//1.1363636e-006

		
		contacts[i].surface.soft_erp=fixed_step*world_spring*contacts[i].surface.soft_cfm*cfm;
		contacts[i].surface.soft_cfm=cfm;
		contacts[i].surface.bounce_vel =1.5f;//0.005f;
		

		if(pushing_neg) 
			contacts[i].surface.mu=dInfinity;

		dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
		}
	
	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
static void SaveContacts(dGeomID o1, dGeomID o2,dJointGroupID jointGroup){
const ULONG N = 100;
dContact contacts[N];

		// get the contacts up to a maximum of N contacts
		ULONG n;
	
		n = dCollide(o1, o2, N, &contacts[0].geom, sizeof(dContact));	
		
	if(n>N)
		n=N;
	ULONG i;


	for(i = 0; i < n; ++i)
	{
		if(i!=0) {
		  dReal dif=dFabs(contacts[i-1].geom.pos[0]-contacts[i].geom.pos[0])+
				dFabs(contacts[i-1].geom.pos[1]-contacts[i].geom.pos[1])+
				dFabs(contacts[i-1].geom.pos[2]-contacts[i].geom.pos[2]);
		  if(dif==0.f) continue;
		}

       
		if(dGeomGetClass(contacts[i].geom.g1)!=dTriListClass &&
			dGeomGetClass(contacts[i].geom.g2)!=dTriListClass){
											contacts[i].surface.mu =1.f;// 5000.f;
											contacts[i].surface.soft_erp=1.f;//ERP(world_spring,world_damping);
											contacts[i].surface.soft_cfm=1.f;//CFM(world_spring,world_damping);
											contacts[i].surface.bounce = 0.01f;//0.1f;
											}


		bool pushing_neg=false;
		dxGeomUserData* usr_data_1=NULL;
		dxGeomUserData* usr_data_2=NULL;
		if(dGeomGetClass(contacts[i].geom.g1)==dGeomTransformClass){
				const dGeomID geom=dGeomTransformGetGeom(contacts[i].geom.g1);
				usr_data_1 = dGeomGetUserData(geom);
			}
		else
			usr_data_1 = dGeomGetUserData(contacts[i].geom.g1);

		if(dGeomGetClass(contacts[i].geom.g2)==dGeomTransformClass){
				const dGeomID geom=dGeomTransformGetGeom(contacts[i].geom.g2);
				usr_data_2 = dGeomGetUserData(geom);
			}
		else
			usr_data_2 = dGeomGetUserData(contacts[i].geom.g2);
/////////////////////////////////////////////////////////////////////////////////////////////////
		if(usr_data_2){ 
				pushing_neg=usr_data_2->pushing_b_neg||usr_data_2->pushing_neg;
				contacts[i].surface.mu*=GMLib.GetMaterial(usr_data_2->material)->fPHFriction;
				contacts[i].surface.soft_cfm*=GMLib.GetMaterial(usr_data_2->material)->fPHSpring;
				contacts[i].surface.soft_erp*=GMLib.GetMaterial(usr_data_2->material)->fPHDamping;
			if(usr_data_2->ph_object){
					usr_data_2->ph_object->InitContact(&contacts[i]);
					//if(pushing_neg) contacts[i].surface.mu=dInfinity;
					//dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
					//dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
					//continue;
			}
			if(usr_data_2->object_callback){
				bool do_colide=true;
				usr_data_2->object_callback(do_colide,contacts[i]);
				if(!do_colide) continue;
			}
		}
///////////////////////////////////////////////////////////////////////////////////////
		if(usr_data_1){ 

				pushing_neg=usr_data_1->pushing_b_neg||
				usr_data_1->pushing_neg;
				contacts[i].surface.mu*=GMLib.GetMaterial(usr_data_1->material)->fPHFriction;
				contacts[i].surface.soft_cfm*=GMLib.GetMaterial(usr_data_1->material)->fPHSpring;
				contacts[i].surface.soft_erp*=GMLib.GetMaterial(usr_data_1->material)->fPHDamping;
			if(usr_data_1->ph_object){
					usr_data_1->ph_object->InitContact(&contacts[i]);
					//if(pushing_neg) contacts[i].surface.mu=dInfinity;
					//dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
					//dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
					//continue;
				}
			if(usr_data_1->object_callback){
				bool do_colide=true;
				usr_data_1->object_callback(do_colide,contacts[i]);
				if(!do_colide) continue;
			}

		}

		contacts[i].surface.mode =dContactBounce|dContactApprox1|dContactSoftERP|dContactSoftCFM;
		dReal cfm=1.f/(world_spring*contacts[i].surface.soft_cfm*fixed_step+world_damping*contacts[i].surface.soft_erp);
		
		
		//contacts[i].surface.soft_erp=ERP(world_spring*contacts[i].surface.soft_cfm,
		//								 world_damping*contacts[i].surface.soft_erp);
		//contacts[i].surface.soft_cfm=CFM(world_spring*contacts[i].surface.soft_cfm,
		//								 world_damping*contacts[i].surface.soft_erp);



		//dReal erp1=ERP(world_spring,world_damping);//0.54545456
		//dReal cfm1=CFM(world_spring,world_damping);//1.1363636e-006


		contacts[i].surface.soft_erp=fixed_step*world_spring*contacts[i].surface.soft_cfm*cfm;
		contacts[i].surface.soft_cfm=cfm;
		contacts[i].surface.bounce_vel =1.5f;//0.005f;


		if(pushing_neg) 
			contacts[i].surface.mu=dInfinity;

		dJointID c = dJointCreateContact(phWorld, jointGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
		}
	
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Implementation for CPhysicsElement
void CPHElement::			add_Box		(const Fobb&		V){
	m_boxes_data.push_back(V);
}

void CPHElement::			create_Box	(const Fobb&		V){
														dGeomID geom,trans;

														dVector3 local_position={
															V.m_translate.x-m_mass_center.x,
															V.m_translate.y-m_mass_center.y,
															V.m_translate.z-m_mass_center.z};

														if(dDOT(local_position,local_position)>0.0001||
															m_spheras_data.size()+m_boxes_data.size()>1
															){
														geom=dCreateBox(0,
																		V.m_halfsize.x*2.f,
																		V.m_halfsize.y*2.f,
																		V.m_halfsize.z*2.f);
														
														m_geoms.push_back(geom);
														dGeomSetPosition(geom,
															local_position[0],
															local_position[1],
															local_position[2]);
														dMatrix3 R;
														PHDynamicData::FMX33toDMX(V.m_rotate,R);
														dGeomSetRotation(geom,R);
														trans=dCreateGeomTransform(0);

														dGeomTransformSetGeom(trans,geom);
														dGeomSetBody(trans,m_body);
														m_trans.push_back(trans);
														/////////////////////////////////////////////////////////
														dGeomGroupAdd(m_group,trans);
														/////////////////////////////////////////////////////////
														dGeomTransformSetInfo(trans,1);
														dGeomCreateUserData(geom);
														dGeomGetUserData(geom)->material=ul_material;
														if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
														if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
															}
														else{
													  geom=dCreateBox(0,
																		V.m_halfsize.x*2.f,
																		V.m_halfsize.y*2.f,
																		V.m_halfsize.z*2.f);
														
														m_geoms.push_back(geom);

														
														dGeomSetPosition(geom,
															local_position[0],
															local_position[1],
															local_position[2]);

														dMatrix3 R;
														PHDynamicData::FMX33toDMX(V.m_rotate,R);
														dGeomSetRotation(geom,R);


														trans=dCreateGeomTransform(0);
														dGeomTransformSetInfo(trans,1);
														dGeomTransformSetGeom(trans,geom);
														dGeomSetBody(trans,m_body);
														m_trans.push_back(trans);

														dGeomCreateUserData(geom);
														dGeomGetUserData(geom)->material=ul_material;

														if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
														if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
														}

														
														}

void CPHElement::			add_Sphere	(const Fsphere&	V){
	m_spheras_data.push_back(V);
}

void CPHElement::			create_Sphere	(const Fsphere&	V){
														dGeomID geom,trans;
														dVector3 local_position={
															V.P.x-m_mass_center.x,
															V.P.y-m_mass_center.y,
															V.P.z-m_mass_center.z};
														if(dDOT(local_position,local_position)>0.0001||
															m_spheras_data.size()+m_boxes_data.size()>1
															)
														{
														geom=dCreateSphere(0,V.R);
														m_geoms.push_back(geom);
														dGeomSetPosition(geom,local_position[0],local_position[1],local_position[2]);
														trans=dCreateGeomTransform(0);
														dGeomTransformSetGeom(trans,geom);
														dGeomSetBody(trans,m_body);
														m_trans.push_back(trans);
														dGeomGroupAdd(m_group,trans);
														dGeomTransformSetInfo(trans,1);		
														dGeomCreateUserData(geom);
														//dGeomGetUserData(geom)->material=GMLib.GetMaterialIdx("box_default");
														dGeomGetUserData(geom)->material=0;
														if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
														if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
														}
														else
														{
														geom=dCreateSphere(0,V.R);
														m_geoms.push_back(geom);
														dGeomSetPosition(geom,
															m_mass_center.x,
															m_mass_center.y,
															m_mass_center.z);
														dGeomSetBody(geom,m_body);
														dGeomCreateUserData(geom);
														//dGeomGetUserData(geom)->material=GMLib.GetMaterialIdx("box_default");
														dGeomGetUserData(geom)->material=0;
														if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
														if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
														}
														};
void CPHElement::add_Cylinder(const Fcylinder& V)
{
	m_cylinders_data.push_back(V);
}

void CPHElement::create_Cylinder(const Fcylinder& V)
{
	dGeomID geom,trans;

	dVector3 local_position=
	{
			V.m_translate.x-m_mass_center.x,
			V.m_translate.y-m_mass_center.y,
			V.m_translate.z-m_mass_center.z
	};

		if(m_group){
				geom=dCreateCylinder
					(
					0,
					V.m_halflength*2.f,
					V.m_radius
					);

				m_geoms.push_back(geom);
				dGeomSetPosition(geom,
					local_position[0],
					local_position[1],
					local_position[2]);
				dMatrix3 R;
				PHDynamicData::FMX33toDMX(V.m_rotate,R);
				dGeomSetRotation(geom,R);
				trans=dCreateGeomTransform(0);

				dGeomTransformSetGeom(trans,geom);
				dGeomSetBody(trans,m_body);
				m_trans.push_back(trans);
				/////////////////////////////////////////////////////////
				dGeomGroupAdd(m_group,trans);
				/////////////////////////////////////////////////////////
				dGeomTransformSetInfo(trans,1);
				dGeomCreateUserData(geom);
				dGeomGetUserData(geom)->material=ul_material;
				if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
				if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
			}
		else{
				geom=dCreateCylinder
					(
					0,
					V.m_halflength*2.f,
					V.m_radius
					);

			m_geoms.push_back(geom);


			dGeomSetPosition(geom,
				local_position[0],
				local_position[1],
				local_position[2]);

			dMatrix3 R;
			PHDynamicData::FMX33toDMX(V.m_rotate,R);
			dGeomSetRotation(geom,R);


			trans=dCreateGeomTransform(0);
			dGeomTransformSetInfo(trans,1);
			dGeomTransformSetGeom(trans,geom);
			dGeomSetBody(trans,m_body);
			m_trans.push_back(trans);

			dGeomCreateUserData(geom);
			dGeomGetUserData(geom)->material=ul_material;

			if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
			if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);

		}
}
void CPHElement::			build	(dSpaceID space){

m_body=dBodyCreate(phWorld);
m_saved_contacts=dJointGroupCreate (0);
b_contacts_saved=false;
dBodyDisable(m_body);
//dBodySetFiniteRotationMode(m_body,1);
//dBodySetFiniteRotationAxis(m_body,0,0,0);

dBodySetMass(m_body,&m_mass);

if(m_spheras_data.size()+m_boxes_data.size()>1)
//m_group=dCreateGeomGroup(space);
m_group=dCreateGeomGroup(0);

Fvector mc=get_mc_data();
//m_start=mc;

m_inverse_local_transform.identity();
m_inverse_local_transform.c.set(mc);
m_inverse_local_transform.invert();
dBodySetPosition(m_body,mc.x,mc.y,mc.z);
///////////////////////////////////////////////////////////////////////////////////////
	vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
	create_Box(*i_box);
	}
///////////////////////////////////////////////////////////////////////////////////////
	vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
		create_Sphere(*i_sphere);
		}
///////////////////////////////////////////////////////////////////////////////////////
	vector<Fcylinder>::iterator i_cylinder;
	for(i_cylinder=m_cylinders_data.begin();i_cylinder!=m_cylinders_data.end();i_cylinder++){
		create_Cylinder(*i_cylinder);
	}
/////////////////////////////////////////////////////////////////////////////////////////
}

void CPHElement::RunSimulation()
{
if(push_untill)
	push_untill+=Device.dwTimeGlobal;

	if(m_phys_ref_object)
	{
		vector<dGeomID>::iterator i;
		for(i=m_geoms.begin();i!=m_geoms.end();i++)
			dGeomUserDataSetPhysicsRefObject(*i,m_phys_ref_object);
	}

if(m_group)
dSpaceAdd(m_shell->GetSpace(),m_group);
else
if(m_boxes_data.size()==0)
dSpaceAdd(m_shell->GetSpace(),*m_geoms.begin());
else
dSpaceAdd(m_shell->GetSpace(),*m_trans.begin());

dBodyEnable(m_body);
}

void CPHElement::			destroy	(){
	m_attached_elements.clear();
	dJointGroupDestroy(m_saved_contacts);
	vector<dGeomID>::iterator i;


	for(i=m_geoms.begin();i!=m_geoms.end();i++){
	dGeomDestroyUserData(*i);
	dGeomDestroy(*i);
	}
	for(i=m_trans.begin();i!=m_trans.end();i++){
	dGeomDestroyUserData(*i);

	
	//if(!attached)
	dGeomDestroy(*i);



	if(m_body && !attached)
		{
		dBodyDestroy(m_body);
		m_body=NULL;
		}




	if(m_group){
				dGeomDestroy(m_group);
				m_group=NULL;
				}
	}

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


	vector<Fcylinder>::iterator i_cylider;
	for(i_cylider=m_cylinders_data.begin();i_cylider!=m_cylinders_data.end();i_cylider++){
		pv=M_PI*(*i_cylider).m_radius*(*i_cylider).m_radius*(*i_cylider).m_halflength*2.f;
		s.mul((*i_cylider).m_translate,pv);
		volume+=pv;
		mc.add(s);
	}

	m_volume=volume;
	mc.mul(1.f/volume);
	m_mass_center=mc;
	return mc;
}
Fvector CPHElement::			get_mc_geoms	(){
////////////////////to be implemented
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
	dMatrix3 DMatx;
	PHDynamicData::FMX33toDMX((*i_box).m_rotate,DMatx);
	dMassRotate(&m,DMatx);
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

	vector<Fcylinder>::iterator i_cylinder;
	for(i_cylinder=m_cylinders_data.begin();i_cylinder!=m_cylinders_data.end();i_cylinder++){
		Fvector& pos=(*i_cylinder).m_translate;
		Fvector l;
		l.sub(pos,mc);
		dMassSetCylinder(&m,mas/m_volume,2,(*i_cylinder).m_radius,2.f*(*i_cylinder).m_halflength);
		dMatrix3 DMatx;
		PHDynamicData::FMX33toDMX((*i_cylinder).m_rotate,DMatx);
		dMassRotate(&m,DMatx);
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}
//dMassSetCylinder()
}


void CPHElement::calculate_it_data_use_density(const Fvector& mc,float density){

//	vector<Fobb>::iterator i_box=m_boxes_data.begin();
//	if(m_boxes_data.size()==1,m_spheras_data.size()==0){
//	Fvector& hside=(*i_box).m_halfsize;

//	dMassSetBox(&m_mass,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
//	dMassAdjust(&m_mass,hside.x*hside.y*hside.z*8.f*density);

//	return;
//	}

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
	dMatrix3 DMatx;
	PHDynamicData::FMX33toDMX((*i_box).m_rotate,DMatx);
	dMassRotate(&m,DMatx);	
	dMassTranslate(&m,l.x,l.y,l.z);
	dMassAdd(&m_mass,&m);
	
	}

	vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
	Fvector& pos=(*i_sphere).P;
	Fvector l;
	l.sub(pos,mc);
	dMassSetSphere(&m,1,(*i_sphere).R);
	dMassAdjust(&m,4.f/3.f*M_PI*(*i_sphere).R*(*i_sphere).R*(*i_sphere).R*density);
	dMassTranslate(&m,l.x,l.y,l.z);
	dMassAdd(&m_mass,&m);

	}

	vector<Fcylinder>::iterator i_cylinder;
	for(i_cylinder=m_cylinders_data.begin();i_cylinder!=m_cylinders_data.end();i_cylinder++){
		Fvector& pos=(*i_cylinder).m_translate;
		Fvector l;
		l.sub(pos,mc);
		dMassSetCylinder(&m,1.f,2,(*i_cylinder).m_radius,(*i_cylinder).m_halflength*2.f);
		dMassAdjust(&m,M_PI*(*i_cylinder).m_radius*(*i_cylinder).m_radius*(*i_cylinder).m_halflength*2.f*density);
		dMatrix3 DMatx;
		PHDynamicData::FMX33toDMX((*i_box).m_rotate,DMatx);
		dMassRotate(&m,DMatx);	
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}

}



void		CPHElement::	setDensity		(float M){

//calculate_it_data(get_mc_data(),M);

calculate_it_data_use_density(get_mc_data(),M);

}

void		CPHElement::	setMass		(float M){

	calculate_it_data(get_mc_data(),M);

//calculate_it_data_use_density(get_mc_data(),M);

}

void		CPHElement::Start(){
	//mXFORM.set(m0);
	build(m_space);
	RunSimulation();

	//dBodySetPosition(m_body,m_m0.c.x,m_m0.c.y,m_m0.c.z);
	//Fmatrix33 m33;
	//m33.set(m_m0);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
	//dBodySetRotation(m_body,R);
}

void		CPHElement::Deactivate(){
	if(!bActive) return;
	if(push_untill)
	{
			push_untill-=Device.dwTimeGlobal;
			if(push_untill<=0)
				unset_Pushout();
	}
	destroy();
	bActive=false;
	bActivating=false;
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
	m_boxes_data.clear();
	m_spheras_data.clear();
	m_cylinders_data.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPHShell::setDensity(float M){
	vector<CPHElement*>::iterator i;
	//float volume=0.f;
	//for(i=elements.begin();i!=elements.end();i++)	volume+=(*i)->get_volume();

	for(i=elements.begin();i!=elements.end();i++)
	(*i)->setDensity(M);
}


void CPHShell::setMass(float M){
	vector<CPHElement*>::iterator i;
	float volume=0.f;
	for(i=elements.begin();i!=elements.end();i++)	volume+=(*i)->get_volume();

	for(i=elements.begin();i!=elements.end();i++)
		(*i)->setMass(
						(*i)->get_volume()/volume*M
					);
}

float CPHShell::getMass()
{
float m=0.f;

vector<CPHElement*>::iterator i;

for(i=elements.begin();i!=elements.end();i++)	m+=(*i)->getMass();

return m;
}
void CPHShell::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	if(bActive)
		return;
	m_ident=ph_world->AddObject(this);

		vector<CPHElement*>::iterator i;
		
		mXFORM.set(m0);
		m_space=dSimpleSpaceCreate(ph_world->GetSpace());
		//dSpaceSetCleanup (m_space, 0);
		for(i=elements.begin();i!=elements.end();i++){
														//(*i)->Start();
														//(*i)->SetTransform(m0);
														(*i)->Activate(m0,dt01, m2, disable);
			}
	//SetPhObjectInElements();
	bActive=true;
}



void CPHShell::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	if(bActive)
		return;
	m_ident=ph_world->AddObject(this);

	vector<CPHElement*>::iterator i;

	mXFORM.set(transform);
	m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	//dSpaceSetCleanup (m_space, 0);
	for(i=elements.begin();i!=elements.end();i++){
		//(*i)->Start();
		//(*i)->SetTransform(m0);
		(*i)->Activate(transform,lin_vel, ang_vel);
	}
	//SetPhObjectInElements();
	bActive=true;
}

void CPHElement::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	mXFORM.set(m0);
	Start();
	SetTransform(m0);
	//i=elements.begin();
	//m_body=(*i)->get_body();
	//m_inverse_local_transform.set((*i)->m_inverse_local_transform);
	//Fmatrix33 m33;
	//Fmatrix m,m1;
	//m1.set(m0);
	//m1.identity();
	//m1.invert();
	//m.mul(m1,m2);
	//m.mul(1.f/dt01);
	//m33.set(m);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
	dBodySetLinearVel(m_body,m2.c.x-m0.c.x,m2.c.y-m0.c.y,m2.c.z-m0.c.z);


	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


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

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);
	bActive=true;
}

void CPHElement::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	mXFORM.set(transform);
	Start();
	SetTransform(transform);
	//i=elements.begin();
	//m_body=(*i)->get_body();
	//m_inverse_local_transform.set((*i)->m_inverse_local_transform);
	//Fmatrix33 m33;
	//Fmatrix m,m1;
	//m1.set(m0);
	//m1.identity();
	//m1.invert();
	//m.mul(m1,m2);
	//m.mul(1.f/dt01);
	//m33.set(m);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
	dBodySetLinearVel(m_body,lin_vel.x,lin_vel.y,lin_vel.z);
	
	dBodySetAngularVel(m_body,ang_vel.x,ang_vel.y,ang_vel.z);


	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


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

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);
	bActive=true;
}

void CPHShell::Deactivate(){
if(!bActive)
		return;
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	(*i)->Deactivate();

	vector<CPHJoint*>::iterator j;
	for(j=joints.begin();j!=joints.end();j++)
	(*j)->Deactivate();

ph_world->RemoveObject(m_ident);
if(m_space) {
dSpaceDestroy(m_space);
m_space=NULL;
}
bActive=false;
bActivating=false;
}

void CPHShell::PhDataUpdate(dReal step){

vector<CPHElement*>::iterator i;
for(i=elements.begin();i!=elements.end();i++)
					(*i)->PhDataUpdate(step);
}

void CPHElement::PhDataUpdate(dReal step){
//m_body_interpolation.UpdatePositions();
//m_body_interpolation.UpdateRotations();
//return;

///////////////skip body effecting update for attached elements////////////////
	if(attached) {
		if( !dBodyIsEnabled(m_body)) return;
		m_body_interpolation.UpdatePositions();
		m_body_interpolation.UpdateRotations();
		return;
	}

///////////////skip for disabled elements////////////////////////////////////////////////////////////
	if( !dBodyIsEnabled(m_body)) {
					if(previous_p[0]!=dInfinity) previous_p[0]=dInfinity;//disable
					return;
				}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////scale velocity///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
				const dReal scale=1.01f;
				const dReal scalew=1.01f;
				const dReal* vel1= dBodyGetLinearVel(m_body);
				dBodySetLinearVel(m_body,vel1[0]/scale,vel1[1]/scale,vel1[2]/scale);
				const dReal* wvel1 = dBodyGetAngularVel(m_body);
				dBodySetAngularVel(m_body,wvel1[0]/scalew,wvel1[1]/scalew,wvel1[2]/scalew);
			

				
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////limit velocity & secure /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				static const dReal u = -0.1f;
				static const dReal w_limit = M_PI/16.f/fixed_step;
				static const dReal l_limit = 3.f/fixed_step;
////////////////limit linear vel////////////////////////////////////////////////////////////////////////////////////////
				const dReal* pos = dBodyGetLinearVel(m_body);
				dReal mag;
				mag=_sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]);
				if(!(mag>-dInfinity && mag<dInfinity)){
					dBodySetLinearVel(m_body,0.f,0.f,0.f);
					mag=0.f;
				}
				else if(mag>l_limit){
					dReal f=mag/l_limit;
					dBodySetLinearVel(m_body,pos[0]/f,pos[1]/f,pos[2]/f);
				}

////////////////secure position///////////////////////////////////////////////////////////////////////////////////
				const dReal* position=dBodyGetPosition(m_body);
				if(!(position[0]<dInfinity && position[0]>-dInfinity &&
					 position[1]<dInfinity && position[1]>-dInfinity &&
					 position[2]<dInfinity && position[2]>-dInfinity)
					 ) 
					 dBodySetPosition(m_body,m_safe_position[0]-m_safe_velocity[0]*fixed_step,
											   m_safe_position[1]-m_safe_velocity[1]*fixed_step,
											   m_safe_position[2]-m_safe_velocity[2]*fixed_step);
				else{
					Memory.mem_copy(m_safe_position,position,sizeof(dVector3));
					Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));
					}

/////////////////limit & secure angular vel///////////////////////////////////////////////////////////////////////////////
				const dReal* rot = dBodyGetAngularVel(m_body);
				dReal w_mag=_sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
				if(!(w_mag>-dInfinity && w_mag<dInfinity)) 
					dBodySetAngularVel(m_body,0.f,0.f,0.f);
				else if(w_mag>w_limit){
					dReal f=w_mag/w_limit;
					dBodySetAngularVel(m_body,rot[0]/f,rot[1]/f,rot[2]/f);
				}

////////////////secure rotation////////////////////////////////////////////////////////////////////////////////////////
				{
				const dReal* rotation=dBodyGetRotation(m_body);
				for(int i=0;i<12;i++)
				{
					if(!(rotation[i]>-dInfinity&&rotation[i]<dInfinity))
					{
					dMatrix3 m;
					dRSetIdentity(m);
					dBodySetRotation(m_body,m);
					break;
					}
				}
				}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////disable///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				ReEnable();
				Disabling();
			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////air resistance/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//const dReal k_w=0.05f;
				//const dReal k_l=0.0002f;//1.8f;
				dBodyAddTorque(m_body,-rot[0]*k_w,-rot[1]*k_w,-rot[2]*k_w);
				dMass mass;
				dBodyGetMass(m_body,&mass);
				dReal l_air=mag*k_l;
				//if(mag*mag*k_l/fixed_step>mass.mass*mag) k_l=mass.mass/mag/fixed_step;
				if(mag*l_air/fixed_step>mass.mass*mag) l_air=mass.mass/fixed_step;
				//dBodyAddForce(m_body,-pos[0]*mag*k_l,-pos[1]*mag*k_l,-pos[2]*mag*k_l);			
				dBodyAddForce(m_body,-pos[0]*l_air,-pos[1]*l_air,-pos[2]*l_air);
				
				m_body_interpolation.UpdatePositions();
				m_body_interpolation.UpdateRotations();

}

void	CPHElement::Disabling(){
//return;
/////////////////////////////////////////////////////////////////////////////////////
///////////////////disabling main body//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
				{
				const dReal* torqu=dBodyGetTorque(m_body);
				const dReal* force=dBodyGetForce(m_body);
				dReal t_m =_sqrt(	torqu[0]*torqu[0]+
										torqu[1]*torqu[1]+
										torqu[2]*torqu[2]);
				dReal f_m=_sqrt(	force[0]*force[0]+
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
					Memory.mem_copy(previous_p,position,sizeof(dVector3));
					Memory.mem_copy(previous_p1,position,sizeof(dVector3));
					const dReal* rotation=dBodyGetRotation(m_body);
					Memory.mem_copy(previous_r,rotation,sizeof(dMatrix3));
					Memory.mem_copy(previous_r1,rotation,sizeof(dMatrix3));
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
					dReal mag_v=_sqrt(
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

					dReal deviation =_sqrt(deviation_v[0]*deviation_v[0]+
										   deviation_v[1]*deviation_v[1]+
										   deviation_v[2]*deviation_v[2]);

					deviation/=dis_count_f;
					if(mag_v<0.001f* dis_frames && deviation<0.00001f*dis_frames)
						Disable();//dBodyDisable(m_body);//
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
					Memory.mem_copy(previous_p,current_p,sizeof(dVector3));
					Memory.mem_copy(previous_r,current_r,sizeof(dMatrix3));
					}

					{
					
					dVector3 velocity={current_p[0]-previous_p1[0],
									   current_p[1]-previous_p1[1],
									   current_p[2]-previous_p1[2]};
					dReal mag_v=_sqrt(
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

					dReal deviation =_sqrt(deviation_v[0]*deviation_v[0]+
										   deviation_v[1]*deviation_v[1]+
										   deviation_v[2]*deviation_v[2]);

					deviation/=dis_count_f;
					if(mag_v<0.04* dis_frames && deviation<0.01*dis_frames)
						dis_count_f1++;
					else{
						Memory.mem_copy(previous_p1,current_p,sizeof(dVector3));
						Memory.mem_copy(previous_r1,current_r,sizeof(dMatrix3));
						}

					if(dis_count_f1>10) dis_count_f*=10;

					}


					}
	
				}
/////////////////////////////////////////////////////////////////

}

void CPHElement::ResetDisable(){
	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f	 =0;
	dis_count_f1 =0;
}

void CPHElement::Enable(){

	if(!bActive) return;
	if(dBodyIsEnabled(m_body)) return;
	ResetDisable();
	dBodyEnable(m_body);
	}


void CPHElement::Disable(){
	//return;
	/*
	if(!b_contacts_saved){
		int num=dBodyGetNumJoints(m_body);
		for(int i=0;i<num;i++){
			dJointID joint=	dBodyGetJoint (m_body, i);
			if(dJointGetType (joint)==dJointTypeContact){
				dxJointContact* contact=(dxJointContact*) joint;
				dBodyID b1=dGeomGetBody(contact->contact.geom.g1);
				dBodyID b2=dGeomGetBody(contact->contact.geom.g2);
				if(b1==0 || b2==0){
					dJointID c = dJointCreateContact(phWorld, m_saved_contacts, &(contact->contact));
					dJointAttach(c, b1, b2);
					b_contacts_saved=true;
				}

			}

		}
	}
	
*/
if(!dBodyIsEnabled(m_body)) return;
if(m_group)
	 SaveContacts(ph_world->GetMeshGeom(),m_group,m_saved_contacts);
else 
	SaveContacts(ph_world->GetMeshGeom(),m_trans[0],m_saved_contacts);

vector<CPHElement*>::iterator i;
for(i=m_attached_elements.begin();i!=m_attached_elements.end();i++){


if((*i)->m_group)
	 SaveContacts(ph_world->GetMeshGeom(),(*i)->m_group,m_saved_contacts);
else 
	SaveContacts(ph_world->GetMeshGeom(),(*i)->m_trans[0],m_saved_contacts);

}
			
	dBodyDisable(m_body);
}


void CPHElement::ReEnable(){
	//if(b_contacts_saved && dBodyIsEnabled(m_body))
	//{
		dJointGroupEmpty(m_saved_contacts);
		b_contacts_saved=false;
	//}
}

void CPHShell::PhTune(dReal step){
}

void CPHShell::Update(){
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	(*i)->Update();
	mXFORM.set((*elements.begin())->mXFORM);
}

void CPHElement::Update(){
if( !dBodyIsEnabled(m_body)) return;
				
		//		PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),
		//					  dBodyGetPosition(m_body),
		//					  mXFORM);

				m_body_interpolation.InterpolateRotation(mXFORM);	
				m_body_interpolation.InterpolatePosition(mXFORM.c);


				mXFORM.mulB(m_inverse_local_transform);

				if(push_untill)//temp_for_push_out||(!temp_for_push_out&&object_contact_callback)
							if(push_untill<Device.dwTimeGlobal) unset_Pushout();

}

void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val){
	if(!bActive) return;
	(*elements.begin())->applyImpulseTrace		( pos,  dir,  val);
}

void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const s16 element){
	if(!bActive) return;
	VERIFY(p_kinematics);
	CBoneInstance& instance=p_kinematics->LL_GetInstance				(element);
	if(!instance.Callback_Param) return;
	((CPHElement*)instance.Callback_Param)->applyImpulseTrace		( pos,  dir,  val);

}

void	CPHElement::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val){

	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	val/=fixed_step;
	Fvector body_pos;
	body_pos.sub(pos,m_inverse_local_transform.c);
	dBodyAddForceAtRelPos       (m_body, dir.x*val,dir.y*val,dir.z*val,body_pos.x, body_pos.y, body_pos.z);
}

void __stdcall CPHShell:: BonesCallback				(CBoneInstance* B){
CPHElement*	E			= dynamic_cast<CPHElement*>	(static_cast<CObject*>(B->Callback_Param));

E->CallBack(B);
}

void CPHElement::CallBack(CBoneInstance* B){
	
	if(!bActive && !bActivating){
		mXFORM.set(B->mTransform);
		bActivating=true;
		bActive=true;
		m_start_time=Device.fTimeGlobal;

		if(!m_parent_element) m_shell->CreateSpace();
		build(m_space);

	
		Fmatrix global_transform;
		global_transform.set(m_shell->mXFORM);
		global_transform.mulB(mXFORM);
		SetTransform(global_transform);
		m_body_interpolation.SetBody(m_body);
		return;
	}
	Fmatrix parent;
	if(bActivating){

		if(!m_parent_element)
		{	
		m_shell->Activate();
		//parent.set(B->mTransform);
		//parent.invert();
		//m_inverse_local_transform.mulB(parent);
		}
	if(!m_shell->bActive) return;
	RunSimulation();

	
	//Fmatrix m,m1,m2;
	//m1.set(mXFORM);
	//m2.set(B->mTransform);
	
	//m1.invert();
	//m.mul(m1,m2);
	//float dt01=Device.fTimeGlobal-m_start_time;
	//m.mul(1.f/dt01);

	//Fmatrix33 m33;
	//m33.set(m);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
//	dBodySetLinearVel(m_body,m.c.x,m.c.y,m.c.z);
	PSGP.memCopy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	PSGP.memCopy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


//////////////////////////////////////////////////////////////
//initializing values for disabling//////////////////////////
//////////////////////////////////////////////////////////////

	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;


	bActivating=false;
	//previous_f[0]=dInfinity;
	return;
	}


	
	//if(!dBodyIsEnabled(m_body)){}

	if(m_parent_element){
	InterpolateGlobalTransform(&B->mTransform);
	parent.set(m_shell->mXFORM);
	parent.invert();
	B->mTransform.mulA(parent);
	}
	else{

		InterpolateGlobalTransform(&m_shell->mXFORM);
		B->mTransform.identity();
		//parent.set(B->mTransform);
		//parent.invert();
		//m_shell->mXFORM.mulB(parent);
		
	}
}

void CPHElement::InterpolateGlobalTransform(Fmatrix* m){
	m_body_interpolation.InterpolateRotation(*m);
	m_body_interpolation.InterpolatePosition(m->c);
	m->mulB(m_inverse_local_transform);
	
}

void CPHElement::DynamicAttach(CPHElement* E)
{
	dVector3 p1;
	dMatrix3 R1;
	Memory.mem_copy(p1,dBodyGetPosition(m_body),sizeof(dVector3));
	const dReal* p2=dBodyGetPosition(E->m_body);
	Memory.mem_copy( R1,dBodyGetRotation(m_body),sizeof(dMatrix3));
	const dReal* R2=dBodyGetRotation(E->m_body);
	dVector3 pp={p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
	dMatrix3 RR;
	dMULTIPLY1_333(RR,R1,R2);
	dVector3 ppr;
	dMULTIPLY1_331(ppr,R1,pp);
	vector<dGeomID>::iterator i;
	Fmatrix RfRf;
	PHDynamicData::DMXPStoFMX(RR,ppr,RfRf);
	E->m_inverse_local_transform.mulA(RfRf);
	//E->fixed_position.set(RfRf);
	for(i=E->m_trans.begin();i!=E->m_trans.end();i++){
	//	if(!m_group) {
	//		m_group=dCreateGeomGroup(0);
	//		dSpaceRemove (ph_world->GetSpace(), m_trans[0]);
	//		dGeomGroupAdd(m_group,m_trans[0]);
	//	}
	//	dSpaceRemove (ph_world->GetSpace(), *i);
	//	dGeomGroupAdd(m_group,*i);
		dGeomID geom=dGeomTransformGetGeom(*i);
		const dReal* pos=dGeomGetPosition(geom);
		const dReal* rot=dGeomGetRotation(geom);
		dMatrix3 rr;
		dMULTIPLY0_333(rr,RR,rot);

		dGeomSetRotation(geom,rr);
		dGeomSetPosition(geom,pos[0]+ppr[0],pos[1]+ppr[1],pos[2]+ppr[2]);

		dGeomSetBody(*i,m_body);
		dBodySetPosition(m_body,p1[0],p1[1],p1[2]);
		dBodySetRotation(m_body,R1);

		}
	for(i=E->m_geoms.begin();i!=E->m_geoms.end();i++)
		if(dGeomGetBody(*i)){
		dGeomID trans=dCreateGeomTransform(0);
		dGeomSetBody((*i),0);
		dGeomSetPosition((*i),pp[0],pp[1],pp[2]);
		dGeomSetRotation((*i),RR);
		dGeomTransformSetGeom(trans,(*i));
		dGeomTransformSetInfo(trans,1);
		dGeomSetBody(trans,m_body);
		dBodySetPosition(m_body,p1[0],p1[1],p1[2]);
		dBodySetRotation(m_body,R1);
		E->m_trans.push_back(trans);
		}



		dMass m1,m2;
		dBodyGetMass(E->m_body,&m1);
		dBodyGetMass(m_body,&m2);
		dMassAdd(&m1,&m2);
		dBodySetMass(m_body,&m1);
		dBodyDestroy(E->m_body);
		E->m_body=m_body;
		E->m_body_interpolation.SetBody(m_body);
		E->attached=true;
		m_attached_elements.push_back(E);
	//E->m_body;

}

void CPHShell::Activate(bool place_current_forms,bool disable){
	if(bActive)
		return;

		m_ident=ph_world->AddObject(this);
		{
		vector<CPHElement*>::iterator i;
		if(place_current_forms)
		for(i=elements.begin();i!=elements.end();i++)	{
														//(*i)->Start();
														//(*i)->SetTransform(m0);
														(*i)->Activate(mXFORM,disable);
														}
		}

		{
		
		vector<CPHJoint*>::iterator i;
		for(i=joints.begin();i!=joints.end();i++) (*i)->Activate();
		}
	//SetPhObjectInElements();/////////////////////////////////////////////////////////////////////
	bActive=true;
	bActivating=true;
}
void CPHElement::Activate(bool place_current_forms,bool disable){

	Start();
	if(place_current_forms)
	{
	SetTransform(mXFORM);
	}
	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


//////////////////////////////////////////////////////////////
//initializing values for disabling//////////////////////////
//////////////////////////////////////////////////////////////

	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);


}

void CPHElement::Activate(const Fmatrix& start_from,bool disable){

	Start();
//	if(place_current_forms)
	{
		Fmatrix globe;
		globe.mul(mXFORM,start_from);
		SetTransform(globe);
	}
	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


	//////////////////////////////////////////////////////////////
	//initializing values for disabling//////////////////////////
	//////////////////////////////////////////////////////////////

	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);


}
void CPHJoint::CreateBall()
{

m_joint=dJointCreateBall(phWorld,0);
Fvector pos;
Fmatrix location;
CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);
first->InterpolateGlobalTransform(&location);

location.transform_tiny(pos,anchor);
dJointSetBallAnchor(m_joint,pos.x,pos.y,pos.z);
dJointAttach(m_joint,first->get_body(),second->get_body());

}

void CPHJoint::CreateCarWeel()
{
}

void CPHJoint::CreateHinge()
{

m_joint=dJointCreateHinge(phWorld,0);
Fvector pos;
Fmatrix first_matrix,second_matrix;
Fvector axis;
CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);
first->InterpolateGlobalTransform(&first_matrix);
second->InterpolateGlobalTransform(&second_matrix);

switch(vs_anchor){
case vs_first :first_matrix.transform_tiny(pos,anchor); break;
case vs_second:second_matrix.transform_tiny(pos,anchor); break;
case vs_global:					
default:pos.set(anchor);	
}



switch(axes[0].vs){

case vs_first :first_matrix.transform_dir(axis,axes[0].direction);	break;
case vs_second:second_matrix.transform_dir(axis,axes[0].direction); break;
case vs_global:
default:		axis.set(axes[0].direction);							
}


first_matrix.invert();

Fmatrix rotate;
rotate.mul(first_matrix,second_matrix);
//rotate.mul(second_matrix,first_matrix);
//rotate.mulB(axes[0].zero_transform);
//rotate.mulA(axes[0].zero_transform);

//rotate.transform_dir(axis);
float shift_angle;
axis_angleA(rotate,axes[0].direction,shift_angle);
//axis_angleB(rotate,axis,shift_angle);
shift_angle-=axes[0].zero;

//Fvector own_ax;
//own_axis_angle(rotate,own_ax,shift_angle);

if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;





float lo=axes[0].low+shift_angle;
float hi=axes[0].high+shift_angle;
if(lo<-M_PI){ 
			hi-=(lo+M_PI);
			lo=-M_PI;
			}
if(lo>0.f) {
			hi-=lo;
			lo=0.f;
			}
if(hi>M_PI) {
			lo-=(hi-M_PI);
			hi=M_PI;
			}
if(hi<0.f) {
			lo-=hi;
			hi=0.f;
			}



//Fvector the_own_axes;
//float angle;

//own_axis_angle(rotate,the_own_axes,angle);
//the_own_axes.normalize();
//float dotpr=the_own_axes.dotproduct(axis);
//axis_angleA(rotate,axis,angle);
//axis_angleB(rotate,axis,angle);

//rotate.transform_dir(the_own_axes);
dJointAttach(m_joint,first->get_body(),second->get_body());
dJointSetHingeAnchor(m_joint,pos.x,pos.y,pos.z);
dJointSetHingeAxis(m_joint,axis.x,axis.y,axis.z);

dJointSetHingeParam(m_joint,dParamLoStop ,lo);
dJointSetHingeParam(m_joint,dParamHiStop ,hi);
if(axes[0].force>0.f){
dJointSetHingeParam(m_joint,dParamFMax ,axes[0].force);
dJointSetHingeParam(m_joint,dParamVel ,axes[0].velocity);
}
}


void CPHJoint::CreateHinge2()
{

	m_joint=dJointCreateHinge2(phWorld,0);

	Fvector pos;
	Fmatrix first_matrix,second_matrix;
	Fvector axis;
	CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
	CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);
	first->InterpolateGlobalTransform(&first_matrix);
	second->InterpolateGlobalTransform(&second_matrix);

	switch(vs_anchor)
	{
	case vs_first :first_matrix.transform_tiny(pos,anchor); break;
	case vs_second:second_matrix.transform_tiny(pos,anchor); break;
	case vs_global:					
	default:pos.set(anchor);	
	}
	//////////////////////////////////////


	dJointAttach(m_joint,first->get_body(),second->get_body());
	dJointSetHinge2Anchor(m_joint,pos.x,pos.y,pos.z);

	dJointAttach(m_joint,first->get_body(),second->get_body());

	/////////////////////////////////////////////

	Fmatrix first_matrix_inv;
	first_matrix_inv.set(first_matrix);
	first_matrix_inv.invert();
	Fmatrix rotate;
	rotate.mul(first_matrix_inv,second_matrix);
	/////////////////////////////////////////////
	float shift_angle;
	float lo;
	float hi;
	//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	switch(axes[0].vs)
	{
	case vs_first :first_matrix.transform_dir(axis,axes[0].direction);	break;
	case vs_second:second_matrix.transform_dir(axis,axes[0].direction); break;
	case vs_global:
	default:		axis.set(axes[0].direction);							
	}



	axis_angleA(rotate,axes[0].direction,shift_angle);

	shift_angle-=axes[0].zero;

	if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
	if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;

	lo=axes[0].low+shift_angle;
	hi=axes[0].high+shift_angle;
	if(lo<-M_PI){ 
		hi-=(lo+M_PI);
		lo=-M_PI;
	}
	if(lo>0.f) {
		hi-=lo;
		lo=0.f;
	}
	if(hi>M_PI) {
		lo-=(hi-M_PI);
		hi=M_PI;
	}
	if(hi<0.f) {
		lo-=hi;
		hi=0.f;
	}



	dJointSetHinge2Axis1 (m_joint, axis.x, axis.y, axis.z);


	dJointSetHinge2Param(m_joint,dParamLoStop ,lo);
	dJointSetHinge2Param(m_joint,dParamHiStop ,hi);

	if(!(axes[0].force<0.f)){
		dJointSetHinge2Param(m_joint,dParamFMax,axes[0].force);
		dJointSetHinge2Param(m_joint,dParamVel ,axes[0].velocity);
	}



	switch(axes[1].vs)
	{
	case vs_first :first_matrix.transform_dir(axis,axes[1].direction);	break;
	case vs_second:second_matrix.transform_dir(axis,axes[1].direction); break;
	case vs_global:
	default		  :axis.set(axes[1].direction);							
	}



	axis_angleA(rotate,axes[1].direction,shift_angle);

	shift_angle-=axes[1].zero;

	if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
	if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;





	lo=axes[1].low+shift_angle;
	hi=axes[1].high+shift_angle;
	if(lo<-M_PI){ 
		hi-=(lo+M_PI);
		lo=-M_PI;
	}
	if(lo>0.f) {
		hi-=lo;
		lo=0.f;
	}
	if(hi>M_PI) {
		lo-=(hi-M_PI);
		hi=M_PI;
	}
	if(hi<0.f) {

		lo-=hi;
		hi=0.f;
	}

	dJointSetHinge2Axis2 (m_joint, axis.x, axis.y, axis.z);

	dJointSetAMotorParam(m_joint,dParamLoStop2 ,lo);
	dJointSetAMotorParam(m_joint,dParamHiStop2 ,hi);
	if(!(axes[1].force<0.f)){
		dJointSetAMotorParam(m_joint,dParamFMax2 ,axes[1].force);
		dJointSetAMotorParam(m_joint,dParamVel2 ,axes[1].velocity);
	}
	//////////////////////////////////////////////////////////////////

}

void CPHJoint::CreateShoulder1()
{
}

void CPHJoint::CreateShoulder2()
{
}

void CPHJoint::CreateUniversalHinge()
{


}

void CPHJoint::CreateWelding()
{
dynamic_cast<CPHElement*>(pFirst_element)
->DynamicAttach(dynamic_cast<CPHElement*>(pSecond_element));

}

void CPHJoint::CreateFullControl()
{
	m_joint=dJointCreateBall(phWorld,0);
	m_joint1=dJointCreateAMotor(phWorld,0);
	
Fvector pos;
Fmatrix first_matrix,second_matrix;
Fvector axis;
CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);
first->InterpolateGlobalTransform(&first_matrix);
second->InterpolateGlobalTransform(&second_matrix);

switch(vs_anchor){
case vs_first :first_matrix.transform_tiny(pos,anchor); break;
case vs_second:second_matrix.transform_tiny(pos,anchor); break;
case vs_global:					
default:pos.set(anchor);	
}
//////////////////////////////////////



//dJointSetAMotorMode (m_joint1, dAMotorUser);
dJointSetAMotorMode (m_joint1, dAMotorEuler);
dJointSetAMotorNumAxes (m_joint1, 3);

dJointAttach(m_joint,first->get_body(),second->get_body());
dJointSetBallAnchor(m_joint,pos.x,pos.y,pos.z);

dJointAttach(m_joint1,first->get_body(),second->get_body());

/////////////////////////////////////////////

Fmatrix first_matrix_inv;
first_matrix_inv.set(first_matrix);
first_matrix_inv.invert();
Fmatrix rotate;
rotate.mul(first_matrix_inv,second_matrix);
/////////////////////////////////////////////
float shift_angle;
float lo;
float hi;
//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
switch(axes[0].vs){

case vs_first :first_matrix.transform_dir(axis,axes[0].direction);	break;
case vs_second:second_matrix.transform_dir(axis,axes[0].direction); break;
case vs_global:
default:		axis.set(axes[0].direction);							
}



axis_angleA(rotate,axes[0].direction,shift_angle);

shift_angle-=axes[0].zero;

if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;

lo=axes[0].low+shift_angle;
hi=axes[0].high+shift_angle;
if(lo<-M_PI){ 
			hi-=(lo+M_PI);
			lo=-M_PI;
			}
if(lo>0.f) {
			hi-=lo;
			lo=0.f;
			}
if(hi>M_PI) {
			lo-=(hi-M_PI);
			hi=M_PI;
			}
if(hi<0.f) {
			lo-=hi;
			hi=0.f;
			}





dJointSetAMotorAxis (m_joint1, 0, 1, axis.x, axis.y, axis.z);
//dJointSetAMotorAxis (m_joint1, 0, 1, axes[0].direction.x, axes[0].direction.y, axes[0].direction.z);

dJointSetAMotorParam(m_joint1,dParamLoStop ,lo);
dJointSetAMotorParam(m_joint1,dParamHiStop ,hi);

if(!(axes[0].force<0.f)){
dJointSetAMotorParam(m_joint1,dParamFMax ,axes[0].force);
dJointSetAMotorParam(m_joint1,dParamVel ,axes[0].velocity);
}

//dJointSetAMotorAngle (m_joint1, 0, 0.0f);

switch(axes[1].vs){

case vs_first :first_matrix.transform_dir(axis,axes[1].direction);	break;
case vs_second:second_matrix.transform_dir(axis,axes[1].direction); break;
case vs_global:
default:		axis.set(axes[1].direction);							
}



axis_angleA(rotate,axes[1].direction,shift_angle);

shift_angle-=axes[1].zero;

if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;





lo=axes[1].low+shift_angle;
hi=axes[1].high+shift_angle;
if(lo<-M_PI){ 
			hi-=(lo+M_PI);
			lo=-M_PI;
			}
if(lo>0.f) {
			hi-=lo;
			lo=0.f;
			}
if(hi>M_PI) {
			lo-=(hi-M_PI);
			hi=M_PI;
			}
if(hi<0.f) {
			lo-=hi;
			hi=0.f;
			}

//dJointSetAMotorAxis (m_joint1, 1, 2, axis.x, axis.y, axis.z);
//dJointSetAMotorAngle (m_joint1, 1, 0.f);

dJointSetAMotorParam(m_joint1,dParamLoStop2 ,lo);
dJointSetAMotorParam(m_joint1,dParamHiStop2 ,hi);
if(!(axes[1].force<0.f)){
dJointSetAMotorParam(m_joint1,dParamFMax2 ,axes[1].force);
dJointSetAMotorParam(m_joint1,dParamVel2 ,axes[1].velocity);
}
//////////////////////////////////////////////////////////////////
switch(axes[2].vs){

case vs_first :first_matrix.transform_dir(axis,axes[2].direction);	break;
case vs_second:second_matrix.transform_dir(axis,axes[2].direction); break;
case vs_global:
default:		axis.set(axes[2].direction);							
}



axis_angleA(rotate,axes[2].direction,shift_angle);

shift_angle-=axes[2].zero;

if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;





lo=axes[2].low+shift_angle;
hi=axes[2].high+shift_angle;
if(lo<-M_PI){ 
			hi-=(lo+M_PI);
			lo=-M_PI;
			}
if(lo>0.f) {
			hi-=lo;
			lo=0.f;
			}
if(hi>M_PI) {
			lo-=(hi-M_PI);
			hi=M_PI;
			}
if(hi<0.f) {
			lo-=hi;
			hi=0.f;
			}

dJointSetAMotorAxis (m_joint1, 2, 2, axis.x, axis.y, axis.z);
//dJointSetAMotorAngle (m_joint1, 2, 0.f);

dJointSetAMotorParam(m_joint1,dParamLoStop3 ,lo);
dJointSetAMotorParam(m_joint1,dParamHiStop3 ,hi);	
if(!(axes[2].force<0.f)){
dJointSetAMotorParam(m_joint1,dParamFMax3 ,axes[2].force);
dJointSetAMotorParam(m_joint1,dParamVel3 ,axes[2].velocity);
}

}


void CPHJoint::SetAnchor(const float x,const float y,const float z)
{
vs_anchor=vs_global;
anchor.set(x,y,z);
}

void CPHJoint::SetAnchorVsFirstElement(const float x,const float y,const float z)
{
	vs_anchor=vs_first;
	anchor.set(x,y,z);
}

void CPHJoint::SetAnchorVsSecondElement(const float x,const float y,const float z)
{
vs_anchor=vs_second;
anchor.set(x,y,z);	
}

void CPHJoint::SetAxis(const float x,const float y,const float z,const int axis_num)
{
	int ax=axis_num;

	switch(eType){
	case ball:
	case welding:
								return;						break;
	case hinge:					ax=0;
															break;
	case hinge2:
	
	
	case universal_hinge:		
														
	case shoulder1:	
														
	case shoulder2:	
														
	case car_wheel:	
								if(ax>1) ax=1;
														break;
	case full_control:
								if(ax>2) ax=2;
														break;
	}
			axes[ax].vs=vs_global;
			axes[ax].direction.set(x,y,z);
}

void CPHJoint::SetAxisVsFirstElement(const float x,const float y,const float z,const int axis_num)
{
	int ax=axis_num;

	switch(eType){
	case ball:					return;						break;
	case hinge:					ax=0;
															break;
	case hinge2:
	
	
	case universal_hinge:		
														
	case shoulder1:	
														
	case shoulder2:	
														
	case car_wheel:	
								if(ax>1) ax=1;
														break;
	case full_control:
								if(ax>2) ax=2;
														break;
	}
			axes[ax].vs=vs_first;
			axes[ax].direction.set(x,y,z);
}

void CPHJoint::SetAxisVsSecondElement(const float x,const float y,const float z,const int axis_num)
{
	int ax=axis_num;

	switch(eType){
	case ball:					
	case welding:
								return;						break;
	case hinge:					ax=0;
															break;
	case hinge2:
	
	
	case universal_hinge:		
														
	case shoulder1:	
														
	case shoulder2:	
														
	case car_wheel:	
								if(ax>1) ax=1;
														break;
	case full_control:
								if(ax>2) ax=2;
														break;
	}
			axes[ax].vs=vs_second;
			axes[ax].direction.set(x,y,z);

}

void CPHJoint::SetLimits(const float low, const float high, const int axis_num)
{
	int ax=axis_num;

	switch(eType){
	case ball:					
	case welding:
								return;						break;
	case hinge:					ax=0;
															break;
	case hinge2:
	
	
	case universal_hinge:		
														
	case shoulder1:	
														
	case shoulder2:	
														
	case car_wheel:	
								if(ax>1) ax=1;
														break;
	case full_control:
								if(ax>2) ax=2;
														break;
	}


			Fvector axis;
			switch(axes[ax].vs){
			case vs_first :pFirst_element->mXFORM.transform_dir(axis,axes[ax].direction);	break;
			case vs_second:pSecond_element->mXFORM.transform_dir(axis,axes[ax].direction); break;
			case vs_global:
			default:		axis.set(axes[0].direction);							
			}

			axes[ax].low=low;
			axes[ax].high=high;
			Fmatrix m1,m2;
			m1.set(pFirst_element->mXFORM);
			m1.invert();
			m2.mul(m1,pSecond_element->mXFORM);
			//m2.mul(pSecond_element->mXFORM,m1);

			
			
			float zer;
			//axis_angleB(m2,axis,zer);
			axis_angleA(m2,axes[ax].direction,zer);
	
			axes[ax].zero=zer;
			m2.invert();
			axes[ax].zero_transform.set(m2);

}


CPHJoint::CPHJoint(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)
{

pFirst_element=first;
pSecond_element=second; 
eType=type;
bActive=false;
SPHAxis axis,axis2,axis3;
axis2.set_direction(1,0,0);
axis3.direction.crossproduct(axis.direction,axis3.direction);
vs_anchor=vs_first;

	switch(eType){
	case ball:					;						break;
	case welding:				;						break;
	case hinge:					axes.push_back(axis);	
														break;
	case hinge2:
	
	
	case universal_hinge:		
														
	case shoulder1:	
														
	case shoulder2:	
														
	case car_wheel:	
								axes.push_back(axis);
								axes.push_back(axis2);	
														break;
	case full_control:
								axes.push_back(axis);
								axes.push_back(axis2);	
								axes.push_back(axis3);
	}

}

void CPHJoint::SetLimitsVsFirstElement(const float low, const float high,const  int axis_num)
{
}

void CPHJoint::SetLimitsVsSecondElement(const float low, const float high,const  int axis_num)
{
}

void CPHJoint::Activate()
{
if(bActive) return;
	switch(eType){
	case ball:					CreateBall();			break;
	case hinge:					CreateHinge();			break;
	case hinge2:				CreateHinge2();			break;
	case universal_hinge:		CreateUniversalHinge(); break;
	case shoulder1:				CreateShoulder1();		break;
	case shoulder2:				CreateShoulder2();		break;
	case car_wheel:				CreateCarWeel();		break;
	case welding:				CreateWelding();		break;
	case full_control:			CreateFullControl();	break;
	}
	bActive=true;
}

void CPHJoint::Deactivate()
{
if(!bActive) return;
	switch(eType){
	case welding:				; break;
	case ball:					;
	case hinge:					;
	case hinge2:				;
	case universal_hinge:		;
	case shoulder1:				;
	case shoulder2:				;
	case car_wheel:				dJointDestroy(m_joint); 
								break;
	
	case full_control:			dJointDestroy(m_joint);
								dJointDestroy(m_joint1);
								break;
	}
		
bActive=false;
}

void CPHJoint::SetForceAndVelocity		(const float force,const float velocity,const int axis_num){
	int ax;
	ax=axis_num;
	if(ax<-1) ax=-1;

	if(ax==-1) 
		switch(eType){
					case welding:				; 
					case ball:					break;
					case hinge:					axes[0].force=force;
												axes[0].velocity=velocity;
						break;
					case hinge2:				;
					case universal_hinge:		;
					case shoulder1:				;
					case shoulder2:				;
					case car_wheel:				axes[0].force=force;
												axes[0].velocity=velocity;
												axes[1].force=force;
												axes[1].velocity=velocity;
						break;

					case full_control:			axes[0].force=force;
												axes[0].velocity=velocity;
												axes[1].force=force;
												axes[1].velocity=velocity;
												axes[2].force=force;
												axes[2].velocity=velocity;
						break;
		}

	else{
		switch(eType){

						case welding:				; 
						case ball:					break;
						case hinge:					ax=0;
							break;
						case hinge2:				;
						case universal_hinge:		;
						case shoulder1:				;
						case shoulder2:				;
						case car_wheel:				ax= axis_num>1 ? 1 : axis_num; 
							break;

						case full_control:			ax= axis_num>2 ? 2 : axis_num; 
							break;
		}
		axes[ax].force=force;
		axes[ax].velocity=velocity;
	}

	if(bActive)
	{
		switch(eType){

						case hinge2:switch(ax)
									{
									case -1:
												dJointSetHinge2Param(m_joint,dParamFMax ,axes[0].force);
												dJointSetHinge2Param(m_joint,dParamFMax2 ,axes[1].force);
									case 0:		dJointSetHinge2Param(m_joint,dParamFMax ,axes[0].force);break;
									case 1:		dJointSetHinge2Param(m_joint,dParamFMax2 ,axes[1].force);break;
									}
							break;
						case universal_hinge:		;
						case shoulder1:				;
						case shoulder2:				;
						case car_wheel:				;
						case welding:				; 
						case ball:					break;
						case hinge:					dJointSetHingeParam(m_joint,dParamFMax ,axes[0].force);
							break;



						case full_control:
							switch(ax){
								case -1:
										dJointSetAMotorParam(m_joint1,dParamFMax ,axes[0].force);
										dJointSetAMotorParam(m_joint1,dParamFMax2 ,axes[1].force);
										dJointSetAMotorParam(m_joint1,dParamFMax3 ,axes[2].force);
								case 0:dJointSetAMotorParam(m_joint1,dParamFMax ,axes[0].force);break;
								case 1:dJointSetAMotorParam(m_joint1,dParamFMax2 ,axes[1].force);break;
								case 2:dJointSetAMotorParam(m_joint1,dParamFMax3 ,axes[2].force);break;
							}
							break;
		}
	}
}


void CPHShell::SetTransform(Fmatrix m){
Fmatrix init;
vector<CPHElement*>::iterator i=elements.begin();
(*i)->InterpolateGlobalTransform(&init);
init.invert();
Fmatrix add;
add.mul(init,m);
(*i)->SetTransform(m);
i++;
Fmatrix element_transform;
for( ;i!=elements.end(); i++)
{
(*i)->InterpolateGlobalTransform(&element_transform);
element_transform.mulA(add);
(*i)->SetTransform(element_transform);
}


 }


void CPHShell::Enable()
{
if(!bActive)
		return;
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	(*i)->Enable();
}

void CPHElement::set_ContactCallback(ContactCallbackFun* callback)
{
contact_callback=callback;
if(!bActive)
	{
	return;
	}
	vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
	{
		dGeomUserDataSetContactCallback(*i,callback);
	}
}

void CPHElement::set_PhysicsRefObject(CPhysicsRefObject* ref_object)
{
	m_phys_ref_object=ref_object;
	if(!bActive)
	{
		return;
	}
	vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
	{
		dGeomUserDataSetPhysicsRefObject(*i,ref_object);
	}
}

void CPHShell::set_PhysicsRefObject	 (CPhysicsRefObject* ref_object)
{
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->set_PhysicsRefObject(ref_object);
	}
}

void CPHElement::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	object_contact_callback= callback;
	if(!bActive)
	{
		return;
	}
	vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
	{
		dGeomUserDataSetObjectContactCallback(*i,callback);
	}
}


void CPHShell::set_ContactCallback(ContactCallbackFun* callback)
{
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
					(*i)->set_ContactCallback(callback);
}


void CPHShell::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->set_ObjectContactCallback(callback);
}

void __stdcall ContactShotMark(CDB::TRI* T,dContactGeom* c)
{
dBodyID b=dGeomGetBody(c->g1);
dxGeomUserData* data;
if(!b) 
{
	b=dGeomGetBody(c->g2);
	data=dGeomGetUserData(c->g2);
}
else
{
	data=dGeomGetUserData(c->g1);
}

dVector3 vel;
dMass m;
dBodyGetMass(b,&m);

 dBodyGetPointVel(b,c->pos[0],c->pos[1],c->pos[2],vel);
 dReal vel_cret=dFabs(dDOT(vel,c->normal))* _sqrt(m.mass);
{
	 if(data)
	 {
	 	SGameMtlPair* mtl_pair		= GMLib.GetMaterialPair(T->material,data->material);
	//	char buf[40];
	//	R_ASSERT3(mtl_pair,strconcat(buf,"Undefined material pair:  # ", GMLib.GetMaterial(T->material)->name),GMLib.GetMaterial(data->material)->name);
		if(mtl_pair)
		{
		 if(vel_cret>30.f && !mtl_pair->HitMarks.empty())
			::Render->add_Wallmark	(
			 SELECT_RANDOM(mtl_pair->HitMarks),
			 *((Fvector*)c->pos),
			 0.09f,
			 T);
		 if(vel_cret>15.f && !mtl_pair->HitSounds.empty())
		 {
			  ::Sound->play_at_pos(
			  SELECT_RANDOM(mtl_pair->HitSounds) ,0,*((Fvector*)c->pos)
			  );
		 }
		}
	
	 }

//			::Render->add_Wallmark	(
//				CPHElement::hWallmark,
//				*((Fvector*)c->pos),
//				0.09f,
//				T);
	 
 } 
}

float CPHJeep::GetSteerAngle()
{
	if(!bActive) return 0;
	return dJointGetHinge2Angle1 (Joints[2]);
}

void CPHElement::SetPhObjectInGeomData(CPHObject* O)
{
	if(!bActive) return;
	vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
				dGeomGetUserData(*i)->ph_object=O;
}

void CPHShell::SetPhObjectInElements()
{
if(!bActive) return;
vector<CPHElement*>::iterator i;
for(i=elements.begin();i!=elements.end();i++ )
		(*i)->SetPhObjectInGeomData((CPHObject*)this);
}

void CPHElement::SetMaterial(u32 m)
{
	ul_material=m;
	if(!bActive) return;
	vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
					dGeomGetUserData(*i)->material=m;
}

void CPHShell::SetMaterial(LPCSTR m)
{
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->SetMaterial(m);
	}
}

void CPHShell::SetMaterial(u32 m)
{
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->SetMaterial(m);
	}
}

void CPHElement::get_LinearVel(Fvector& velocity)
{
	if(!bActive)
	{
		velocity.set(0,0,0);
		return;
	}
	Memory.mem_copy(&velocity,dBodyGetLinearVel(m_body),sizeof(Fvector));
}

void CPHShell::get_LinearVel(Fvector& velocity)
{

(*elements.begin())->get_LinearVel(velocity);
}

void CPHShell::set_PushOut(u32 time)
{
	vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->set_PushOut(time);
	}
}

void CPHElement::set_PushOut(u32 time)
{
	temp_for_push_out=object_contact_callback;

	set_ObjectContactCallback(PushOutCallback);
	if(bActive) push_untill=Device.dwTimeGlobal+time;
	else		push_untill=time;

}

void CPHElement::unset_Pushout()
{
	object_contact_callback=temp_for_push_out;
	temp_for_push_out=NULL;
	set_ObjectContactCallback(object_contact_callback);
	push_untill=0;
}