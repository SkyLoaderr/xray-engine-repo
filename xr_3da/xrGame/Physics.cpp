#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
//#include "dRay/include/dRay.h"
#include "ExtendedGeom.h"
union dInfBytes dInfinityValue = {{0,0,0x80,0x7f}};
// #include "contacts.h"




//void _stdcall dGeomTransformSetInfo (dGeomID g, int mode);
/////////////////////////////////////////
static dContact bulletContact;
static bool isShooting;
static dVector3 RayD;
static dVector3 RayO;

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
	UINT i;
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

}
////////////////////////////////////////////////////////////////
void CPHJeep::JointTune(dReal step){
const	dReal k_p=30000.f;//30000.f;
const	dReal k_d=1000.f;//1000.f;
	for(UINT i = 0; i < 4; ++i)
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
	for(UINT i=0;i<NofGeoms;i++) dGeomDestroyUserData(Geoms[i]);
	DynamicData.Destroy();
	
	for(UINT i=0;i<NofJoints;i++) dJointDestroy(Joints[i]);
	for(UINT i=0;i<NofBodies;i++) dBodyDestroy(Bodies[i]);

	dGeomDestroyUserData(Geoms[5]);
	dGeomDestroyUserData(Geoms[7]);
	dGeomDestroyUserData(Geoms[0]);
	dGeomDestroyUserData(Geoms[6]);

	for(UINT i=0;i<5;i++) {
		dGeomDestroy(Geoms[i]);
	}
	dGeomDestroy(Geoms[6]);
//	dGeomDestroy(Geoms[5]);
	dGeomDestroy(Geoms[7]);
	dGeomDestroy(GeomsGroup);


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
Translate.identity();
Translate.translate(0,-MassShift+0.810f,0);
DynamicData.SetZeroTransform(Translate);

}

void CPHJeep::Steer(const char& steering)
{
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
	//////////////////
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
	Space = dHashSpaceCreate(0);
	ContactGroup = dJointGroupCreate(0);		
	dWorldSetGravity(phWorld, 0,-2.f*9.81f, 0);//-2.f*9.81f
	Mesh.Create(Space,phWorld);
	//Jeep.Create(Space,phWorld);//(Space,phWorld)
	//Gun.Create(Space);
	//dCreatePlane(Space,0,1,0,102);
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
	UINT it_number;
	frame_time+=step;
	//m_frame_sum+=step;
	
	if(!(frame_time<fixed_step)){
	it_number=(UINT)(frame_time/fixed_step);
	frame_time-=it_number*fixed_step;
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
			double dif=m_frame_sum-Time();
			if(fabs(dif)>fixed_step) 
				m_start_time+=dif;

		//	dWorldSetERP(phWorld,  fixed_step*k_p / (fixed_step*k_p + k_d));
		//	dWorldSetCFM(phWorld,  1.f / (fixed_step*k_p + k_d));

			//dWorldSetERP(phWorld,  0.8);
			//dWorldSetCFM(phWorld,  0.00000001);



			dSpaceCollide(Space, 0, &NearCallback); 

		for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhTune(fixed_step);	
	


			dWorldStep(phWorld, fixed_step);
			dJointGroupEmpty(ContactGroup);
		for(iter=m_objects.begin();iter!=m_objects.end();iter++)
				(*iter)->PhDataUpdate(fixed_step);
	




		

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

		}

		contacts[i].surface.mode =dContactBounce|dContactApprox1|dContactSoftERP|dContactSoftCFM;

		contacts[i].surface.soft_erp=ERP(world_spring*contacts[i].surface.soft_cfm,
										 world_damping*contacts[i].surface.soft_erp);
		contacts[i].surface.soft_cfm=CFM(world_spring*contacts[i].surface.soft_cfm,
										 world_damping*contacts[i].surface.soft_erp);
		contacts[i].surface.bounce_vel =1.5f;//0.005f;


		if(pushing_neg) 
			contacts[i].surface.mu=dInfinity;

		dJointID c = dJointCreateContact(phWorld, ContactGroup, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
		}
	
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Implementation for CPhysicsElement
void CPHElement::			add_Box		(const Fobb&		V){
	m_boxes_data.push_back(V);
}

void CPHElement::			create_Box		(Fobb&		V){
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
														dGeomGroupAdd(m_group,trans);
														dGeomTransformSetInfo(trans,1);
														//dGeomCreateUserData(trans);
														dGeomCreateUserData(geom);
														//dGeomGetUserData(geom)->material=GMLib.GetMaterialIdx("box_default");
														dGeomGetUserData(geom)->material=ul_material;
														//dGeomGetUserData(trans)->friction=friction_table[1];
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
														//dGeomGetUserData(geom)->material=GMLib.GetMaterialIdx("materials\\box_default");
														dGeomGetUserData(geom)->material=ul_material;

														
														}
														//dGeomGetUserData(trans)->friction=dInfinity;
														
														}

void CPHElement::			add_Sphere	(const Fsphere&	V){
	m_spheras_data.push_back(V);
}

void CPHElement::			create_Sphere	(Fsphere&	V){
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
														}
														};
void CPHElement::			build	(dSpaceID space){

m_body=dBodyCreate(phWorld);

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
vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
	create_Box(*i_box);
	}
	vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
		create_Sphere(*i_sphere);
		}
//////////////////////////////////////

}

void CPHElement::RunSimulation()
{
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
	vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++){
	dGeomDestroyUserData(*i);
	dGeomDestroy(*i);
	}
	for(i=m_trans.begin();i!=m_trans.end();i++){
	dGeomDestroyUserData(*i);
	dGeomDestroy(*i);
	
	}
	if(m_spheras_data.size()+m_boxes_data.size()>1)
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
	//density*=40.f;
	vector<Fobb>::iterator i_box=m_boxes_data.begin();
	if(m_boxes_data.size()==1,m_spheras_data.size()==0){
	Fvector& hside=(*i_box).m_halfsize;
	//dReal hs=_min(_max(hside.x,hside.y),hside.z);
	dMassSetBox(&m_mass,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
	dMassAdjust(&m_mass,hside.x*hside.y*hside.z*8.f*density);
	//dMassSetBox(&m_mass,1.f,hs*2.f,hs*2.f,hs*2.f);
	//dMassAdjust(&m_mass,hs*hs*hs*8.f*density);
	return;
	}

	dMass m;
	dMassSetZero(&m_mass);
	
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
	dMassSetSphere(&m,1,(*i_sphere).R);
	dMassAdjust(&m,4.f/3.f*M_PI*(*i_sphere).R*(*i_sphere).R*(*i_sphere).R*density);
	dMassTranslate(&m,l.x,l.y,l.z);
	dMassAdd(&m_mass,&m);

	}


}



void		CPHElement::	setMass		(float M){
//calculate_it_data(get_mc_data(),M);

calculate_it_data_use_density(get_mc_data(),M);

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
		m_space=dSimpleSpaceCreate(ph_world->GetSpace());
		for(i=elements.begin();i!=elements.end();i++){
														//(*i)->Start();
														//(*i)->SetTransform(m0);
														(*i)->Activate(m0,dt01, m2, disable);
			}
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


	if( !dBodyIsEnabled(m_body)) {
					if(previous_p[0]!=dInfinity) previous_p[0]=dInfinity;
					return;
				}
   ////////////////////////////////////////////////////////////////////
  //limit velocity of the main body///////////////////////////////////
 ////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
				const dReal scale=1.01f;
				const dReal scalew=1.01f;
				const dReal* vel1= dBodyGetLinearVel(m_body);
				dBodySetLinearVel(m_body,vel1[0]/scale,vel1[1]/scale,vel1[2]/scale);
				const dReal* wvel1 = dBodyGetAngularVel(m_body);
				dBodySetAngularVel(m_body,wvel1[0]/scalew,wvel1[1]/scalew,wvel1[2]/scalew);
/////////////////////////////////////////////////////////////////
				static const dReal u = -0.1f;
				static const dReal w_limit = M_PI/16.f/fixed_step;
				static const dReal l_limit = 3.f/fixed_step;
				

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
				//const dReal k_l=0.1f;
				//dBodyAddForce(m_body,-pos[0]*k_l,-pos[1]*k_l,-pos[2]*k_l);
				//dBodyAddForce(m_body, u * pos[0]*mag, u * pos[1]*mag, u * pos[2]*mag);
				const dReal* rot = dBodyGetAngularVel(m_body);
				dReal w_mag=_sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
				if(!(w_mag>-dInfinity && w_mag<dInfinity)) 
					dBodySetAngularVel(m_body,0.f,0.f,0.f);
				else if(w_mag>w_limit){
					dReal f=w_mag/w_limit;
					dBodySetAngularVel(m_body,rot[0]/f,rot[1]/f,rot[2]/f);
				}
				//const dReal k_w=0.1f;
				//dBodyAddTorque(m_body,-rot[0]*k_w,-rot[1]*k_w,-rot[2]*k_w);
				Disable();

	
				const dReal k_w=0.05f;
				dBodyAddTorque(m_body,-rot[0]*k_w,-rot[1]*k_w,-rot[2]*k_w);
	
				const dReal k_l=0.0002f;//1.8f;
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

void	CPHElement::Disable(){
//return;
/////////////////////////////////////////////////////////////////////////////////////
////////disabling main body//////////////////////////////////////////////////////////
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

}

void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val){
	(*elements.begin())->applyImpulseTrace		( pos,  dir,  val);
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

	if(bActivating){

	if(!m_parent_element)m_shell->Activate();
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
	

//	Fmatrix bone,inv_shell;
//	InterpolateGlobalTransform(&bone);
//	inv_shell.set(m_shell->mXFORM);
//	inv_shell.invert();
//	bone.mulB(inv_shell);
//	B->mTransform.set(bone);
	Fmatrix parent;
	//if(!dBodyIsEnabled(m_body)) return;
	if(m_parent_element){
	//if(m_parent_element->bActivating || !m_parent_element->bActive) return;
	InterpolateGlobalTransform(&B->mTransform);
	parent.set(m_shell->mXFORM);
	//m_parent_element->InterpolateGlobalTransform(&parent);
	parent.invert();
	B->mTransform.mulA(parent);
	}
	else{
		//InterpolateGlobalTransform(&B->mTransform);
		//parent.set(m_shell->mXFORM);
		//parent.invert();
		//B->mTransform.mulA(parent);


		InterpolateGlobalTransform(&m_shell->mXFORM);
		parent.set(B->mTransform);
		parent.invert();
		m_shell->mXFORM.mulB(parent);
		
	}
}

void CPHElement::InterpolateGlobalTransform(Fmatrix* m){
	m_body_interpolation.InterpolateRotation(*m);
	m_body_interpolation.InterpolatePosition(m->c);
	m->mulB(m_inverse_local_transform);
	
}

void CPHShell::Activate(){
	if(bActive)
		return;
	m_ident=ph_world->AddObject(this);

		//vector<CPHElement*>::iterator i;

		//for(i=elements.begin();i!=elements.end();i++){
														//(*i)->Start();
														//(*i)->SetTransform(m0);
											//			(*i)->Activate();
			//}
		vector<CPHJoint*>::iterator i;

		for(i=joints.begin();i!=joints.end();i++){
														(*i)->Activate();
										
			}
	bActive=true;
	bActivating=true;
}
void CPHElement::Activate(){
	//mXFORM.set(m0);
	Start();
	//SetTransform(m0);
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
	//dBodySetLinearVel(m_body,m2.c.x-m0.c.x,m2.c.y-m0.c.y,m2.c.z-m0.c.z);
	//dBodySetPosition(m_body,m0.c.x,m0.c.y+1.,m0.c.z);

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
//	if(disable) dBodyDisable(m_body);


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
float shift_angle;
axis_angleA(rotate,axes[0].direction,shift_angle);
shift_angle-=axes[0].zero;
if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;


dJointAttach(m_joint,first->get_body(),second->get_body());
dJointSetHingeAnchor(m_joint,pos.x,pos.y,pos.z);
dJointSetHingeAxis(m_joint,axis.x,axis.y,axis.z);

float lo=axes[0].low+shift_angle;
float hi=axes[0].high+shift_angle;
if(lo<-M_PI) 
			lo=-M_PI;
if(lo>0.f) 
			lo=0.f;
if(hi>M_PI)  
			hi=M_PI;
if(hi<0.f) 
			hi=0.f;

rotate.mulB(axes[0].zero_transform);
Fvector the_own_axes;
float angle;
/*
own_axis_angle(rotate,the_own_axes,angle);
the_own_axes.normalize();
angle=the_own_axes.dotproduct(axis);
axis_angleA(rotate,axis,angle);
axis_angleB(rotate,axis,angle);

rotate.transform_dir(the_own_axes);
*/
dJointSetHingeParam(m_joint,dParamLoStop ,lo);
dJointSetHingeParam(m_joint,dParamHiStop ,hi);
}


void CPHJoint::CreateHinge2()
{
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
	}
			axes[ax].vs=vs_first;
			axes[ax].direction.set(x,y,z);
}

void CPHJoint::SetAxisVsSecondElement(const float x,const float y,const float z,const int axis_num)
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
	}
			axes[ax].vs=vs_second;
			axes[ax].direction.set(x,y,z);

}

void CPHJoint::SetLimits(const float low, const float high, const int axis_num)
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
			
			
			float zer;
			//axis_angleA(m2,axis,zer);
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
SPHAxis axis,axis2;
axis2.set_direction(1,0,0);
vs_anchor=vs_first;

	switch(eType){
	case ball:					;						break;
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
	}
	bActive=true;
}

void CPHJoint::Deactivate()
{
if(!bActive) return;
dJointDestroy(m_joint);
bActive=false;
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

