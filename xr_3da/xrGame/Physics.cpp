
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "contrib/dRay/include/dRay.h"

void CPHWorld::Render()
{
//	Device.Shader.OnFrameEnd		();
//	Device.Primitive.dbg_DrawAABB	(vCenter,sx,sy,sz,0xffffffff);
//	Device.Primitive.dbg_DrawEllipse(M, 0xfffffff);
}

//////////////////////////////////////////////////////////////
//////////////CPHMesh///////////////////////////////////////////
///////////////////////////////////////////////////////////

void CPHMesh ::Create(dSpaceID space, dWorldID world){
Geom = dCreateTriList(space, 0, 0);
/*
 uint i,j;
  for( i=0,j=0; i<FACE_COUNT;i++){
	  Indices[j++]= GroundFacesC[i][0];
	  Indices[j++]= GroundFacesC[i][1];
	  Indices[j++]= GroundFacesC[i][2];
  }
  for( i=0; i<POINT_COUNT;i++){
	   Vertices[i][0]=GroundVerticesC[i][0];
	    Vertices[i][1]=GroundVerticesC[i][1];
		 Vertices[i][2]=GroundVerticesC[i][2];
  }

  dGeomTriListBuild(TriList, Vertices, VertexCount, Indices, IndexCount);
  */
}

////////////////////////////////////////////////////////////////////////////

void CPHMesh ::Destroy(){
						dGeomDestroy(Geom);
						}


/////////////////////////////////////////////////////////////////////////////
/////////////CPHJeep////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CPHJeep::Create(dSpaceID space, dWorldID world){
	
	static const dReal scaleParam=1.6/0.8;
	static const dVector3 scaleBox={REAL(2.4)*scaleParam, REAL(0.4)*scaleParam, REAL(1.5)*scaleParam};
	static const dVector3 jeepBox={scaleBox[0],scaleBox[0],scaleBox[0]};
	static const dVector3 cabinBox={scaleBox[0]/1.7,scaleBox[1]*2.,scaleBox[2]/1.01};

	static const dReal wheelRadius = REAL(0.28)* scaleParam, wheelWidth = REAL(0.25)* scaleParam;
	static const wheelSegments = 24;

	static const dVector3 startPosition={0,3,15};
	static const dReal weelSepX=jeepBox[0]/2.f-jeepBox[0]/8.f,weelSepZ=jeepBox[2]/2.f-wheelRadius/2.f,weelSepY=jeepBox[2];

	dMass m;


//	dMassSetBox(&m, 1, 5,1, 5); // density,lx,ly,lz
//	dMassAdjust(&m, 1); // mass
/*
	bodytestGeom = dBodyCreate(world);
	dBodySetMass(bodytestGeom, &m);

	testGeom=dCreateBox(space,1,1,1);
		dGeomSetBody(testGeom,bodytestGeom);
	dGeomSetPosition(testGeom,startPosition[0]-1, startPosition[1]+2.33, startPosition[2]);


	testGeomMesh.CreateBox(oVector3(1,1,1));
	dynamicMeshes.Push(new GeomMesh(testGeom, &testGeomMesh));
*/
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
	dGeomSetPosition(Geoms[6], -jeepBox[0]/2+cabinBox[0]/2+0.1, cabinBox[1]/2+jeepBox[1]/2, 0); // x,y,z
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

///uuu
//	const dReal* qq=dBodyGetQuaternion(Bodies[1]);


	dBodySetPosition(Bodies[1], startPosition[0]-weelSepX, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z
	dBodySetPosition(Bodies[2], startPosition[0]-weelSepX, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z-0.9, 2.6,   9.3); // x,y,z
	dBodySetPosition(Bodies[3], startPosition[0]+weelSepX, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z 0.9, 2.6,  10.7); // x,y,z
	dBodySetPosition(Bodies[4], startPosition[0]+weelSepX, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z 0.9, 2.6,   9.3); // x,y,z

	//cabin body
//	dMassSetBox(&m, 1, cabinBox[0], cabinBox[1], cabinBox[2]); // density,lx,ly,lz
//	dMassAdjust(&m, 1); // mass

	//Bodies[5] = dBodyCreate(world);
	//dBodySetMass(Bodies[5], &m);
	//dBodySetPosition(Bodies[5], startPosition[0]-jeepBox[0]/2+cabinBox[0]/2, startPosition[1]+cabinBox[1]/2+jeepBox[1]/2, startPosition[2]); // x,y,z



	// wheel joints
	for(i = 0; i < 4; ++i)
	{
		Joints[i] = dJointCreateHinge2(world, 0);
		dJointAttach(Joints[i], Bodies[0], Bodies[i+1]);
		const dReal* const wPos = dBodyGetPosition(Bodies[i+1]);
		dJointSetHinge2Anchor(Joints[i], wPos[0], wPos[1], wPos[2]);
		dJointSetHinge2Axis1(Joints[i], 0, 1, 0);
		dJointSetHinge2Axis2(Joints[i], 0, 0, ((i % 2) == 0) ? -1 : 1);

		dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
		dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
		dJointSetHinge2Param(Joints[i], dParamFMax, 50);

		dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		dJointSetHinge2Param(Joints[i], dParamFMax2, 80);

		dJointSetHinge2Param(Joints[i], dParamSuspensionERP, 0.4f);
		dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 0.005f);
	}
	//cabin joint
	/*
	Joints[4]=dJointCreateSlider(world,0);
	dJointAttach(Joints[4],Bodies[0],Bodies[5]);
	dJointSetSliderAxis (Joints[4],0,1,0);
	dJointSetSliderParam (Joints[4], dParamLoStop, 0);
	dJointSetSliderParam (Joints[4], dParamHiStop, 0);
	// geometry goup for the jeep
	*/
	GeomsGroup = dCreateGeomGroup(space);  
	for(i = 0; i < NofGeoms-1; ++i)
		dGeomGroupAdd(GeomsGroup, Geoms[i]);
//	dGeomGroupAdd(GeomsGroup, Geoms[7]);
//	dGeomSetBody(GeomsGroup,Bodies[0]);



	//dynamic data
CreateDynamicData();

}

/////////////////////////////////////////////////////////////////////////////


void CPHJeep::Destroy(){
	for(UINT i=0;i<NofJoints;i++) dJointDestroy(Joints[i]);
	for(UINT i=0;i<NofBodies;i++) dBodyDestroy(Bodies[i]);
	for(UINT i=0;i<NofGeoms;i++) dGeomDestroy(Geoms[i]);
	DynamicData.Destroy();

}

void CPHJeep::Steer(const char& velocity, const char& steering)
{
	static const dReal steeringRate = M_PI * 4 / 3;
	static const dReal steeringLimit = M_PI / 6;
	static const dReal wheelVelocity = 16*12 * M_PI;
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
////////////////////////////////////////////////////////////////

void CPHJeep::CreateDynamicData(){
DynamicData.Create(4,Bodies[0]);
DynamicData.SetChild(0,0,Bodies[1]);
DynamicData.SetChild(1,0,Bodies[2]);
DynamicData.SetChild(2,0,Bodies[3]);
DynamicData.SetChild(3,0,Bodies[4]);

}
////////////////////////////////////////////////////////////////////////////
///////////CPHWorld/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CPHWorld::Create(){

	World = dWorldCreate();
	Space = dHashSpaceCreate();
	ContactGroup = dJointGroupCreate(0);		
	dWorldSetGravity(World, 0, -9.81f, 0);
	Mesh.Create(Space,World);
	Jeep.Create(Space,World);
	dWorldSetCFM(World, 0.001f);
	dWorldSetERP(World, 0.99f);
}

/////////////////////////////////////////////////////////////////////////////

void CPHWorld::Destroy(){
	Mesh.Destroy();
	Jeep.Destroy();
	dJointGroupEmpty(ContactGroup);
	dSpaceDestroy(Space);
	dWorldDestroy(World);
	dCloseODE();

}

//////////////////////////////////////////////////////////////////////////////

void CPHWorld::Step(dReal step){
			// compute contact joints and forces
			dSpaceCollide(Space, 0, &NearCallback);
		
			
			/*if(isShootting&&bulletContact.geom.pos[0]!=dInfinity){
				
				dJointAttach(bulletJoint, dGeomGetBody(bulletContact.geom.g1), dGeomGetBody(bulletContact.geom.g2));
		

				dJointID c = dJointCreateContact(world, contactGroup, &bulletContact);
	
				
				}
				*/

			dWorldStep(World, step);
			dJointGroupEmpty(ContactGroup);
	
			//isShootting=false;
			//bulletContact.geom.pos[0]=dInfinity;
		}

//////////////////////////////////////////////////////////////////////////////////


