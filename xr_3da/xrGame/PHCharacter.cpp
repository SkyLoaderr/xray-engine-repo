#include "stdafx.h"
#include "phcharacter.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
CPHCharacter::CPHCharacter(void)
{
m_body=NULL;
m_wheel_body=NULL;
m_geom_shell=NULL;
m_wheel=NULL;
m_cap=NULL;
m_geom_group=NULL;
m_wheel_transform=NULL;
m_shell_transform=NULL;
m_cap_transform=NULL;
m_wheel_joint=NULL;
m_control_accel[0]=1.f;
m_control_accel[1]=0.f;
m_control_accel[2]=0.f;
m_contact_normal[0]=0.f;
m_contact_normal[1]=1.f;
m_contact_normal[2]=0.f;
is_contact=false;
was_contact=false;
is_control=false;
b_depart=false;
b_meet=false;
}

CPHCharacter::~CPHCharacter(void)
{
}



void CPHSimpleCharacter::Create(){

ph_world->AddObject(this);
m_geom_shell=dCreateCylinder(0,0.4f,2.f);
m_cap=dCreateSphere(0,0.5f);
m_wheel=dCreateSphere(ph_world->GetSpace(),0.5f);

m_cap_transform=dCreateGeomTransform(0);
m_shell_transform=dCreateGeomTransform(0);
m_wheel_transform=dCreateGeomTransform(0);
dGeomTransformSetInfo(m_cap_transform,1);
dGeomTransformSetInfo(m_shell_transform,1);
dGeomTransformSetInfo(m_wheel_transform,1);
////////////////////////////////////////////////////////////////////////
dMatrix3 RZ;
dRFromAxisAndAngle (RZ,0.f, 0.f, 1.f, M_PI/2.f);
dGeomSetRotation(m_cap,RZ);
dGeomSetPosition(m_cap,0.f,2.f,0.f);
////////////////////////////////////////////////////////////////////////

//dGeomSetPosition(chSphera2,0.f,-0.7f,0.f);

////////////////////////////////////////////////////////////////////////
dGeomSetPosition(m_geom_shell,0.f,1.f,0.f);
dGeomTransformSetGeom(m_cap_transform,m_cap);
dGeomTransformSetGeom(m_wheel_transform,m_wheel);
dGeomTransformSetGeom(m_shell_transform,m_geom_shell);
m_body=dBodyCreate(phWorld);
dGeomSetBody(m_cap_transform,m_body);
dGeomSetBody(m_shell_transform,m_body);
dGeomSetBody(m_wheel,m_body);
//dBodySetPosition(m_body,-10,3,0);
dGeomCreateUserData(m_wheel_transform);
dGeomUserDataSetPhObject(m_wheel_transform,(CPHObject*)this);
/////////////////////////////////////////////////////////////////////////
dMass m;
dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
dMassAdjust(&m,70.f);
dBodySetMass(m_body,&m);

}


void CPHSimpleCharacter::Destroy(){
	if(m_wheel_joint){
		dJointDestroy(m_wheel_joint);
		m_wheel_joint=NULL;
	}

	if(m_body) {
		dBodyDestroy(m_body);
		m_body=NULL;
	}

	if(m_wheel_body){
		dBodyDestroy(m_wheel_body);
		m_wheel_body=NULL;
	}

	if(m_geom_shell){
		dGeomDestroy(m_geom_shell);
		m_geom_shell=NULL;
	}

	if(m_wheel) {
		dGeomDestroyUserData(m_wheel);
			dGeomDestroy(m_wheel);
		m_wheel=NULL;
	}


	if(m_cap) {
		dGeomDestroy(m_cap);
		m_cap=NULL;
	}

	if(m_wheel_transform){
		dGeomDestroy(m_wheel_transform);
			m_wheel_transform=NULL;
	}
	if(m_shell_transform){
		dGeomDestroy(m_shell_transform);
		m_shell_transform=NULL;
	}
	if(m_cap_transform){
		dGeomDestroy(m_cap_transform);
		m_cap_transform=NULL;
	}
	if(m_geom_group){
		dGeomDestroy(m_geom_group);
		m_geom_group=NULL;
	}

}



void CPHSimpleCharacter::PhDataUpdate(dReal step){

dReal* chVel=const_cast<dReal*>(dBodyGetLinearVel(m_body));
dReal rk=10.f;


			//if(chControlFlags[0]==1.f) chVel[0]=0.f;
			//if(chControlFlags[2]==1.f) chVel[2]=0.f;

			if(!is_contact) rk=0.f;

			//-chVel[1]*1000.*chDepart//-30.f*70.f
			if(is_control){
			dVector3 sidedir;
			dVector3 y={0.,1.,0.};
				dCROSS(sidedir,=,m_control_accel,y);
				dNormalize3(sidedir);
				dReal vProj=dDOT(sidedir,chVel);
				dBodyAddForce(m_body,m_control_accel[0],m_control_accel[1],m_control_accel[2]);
				dBodyAddForce(m_body,-sidedir[0]*vProj*3000.f,-sidedir[1]*vProj*3000.f,-sidedir[2]*vProj*3000.f);
			}

			dBodyAddForce(m_body,-chVel[0]*rk,-20.f*70.f*(!is_contact),-chVel[2]*rk);
	
			if(b_depart) chVel[1]=0.f;
			is_control=false;
			b_depart=false;

			dBodyAddForce(m_body,0,-10.f*70.f,0);

			{
				static const dReal l_limit = 5.f/0.02f;
				dReal mag;
				const dReal* pos = dBodyGetLinearVel(m_body);
				mag=sqrtf(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]);

				if(mag>l_limit){
								dReal f=mag/l_limit;
								dBodySetLinearVel(m_body,pos[0]/f,pos[1]/f,pos[2]/f);
								}
			}
			//chControlFlags[0]=1.f;
			//chControlFlags[2]=1.f;
		//	rayD[0]=0;
		//	rayD[1]=0;
		//	rayD[2]=0;
			/*
			if(isShootting){
				dGeomDestroy(ray);
				isShootting=false;
				//ray=NULL;
			}
			*/
		//	jeepDynamicData.CalculateData();

			was_contact=is_contact;
			is_contact=false;
}

void CPHSimpleCharacter::PhTune(dReal step){
	b_depart=was_contact&&(!is_contact);
	b_meet=(!was_contact)&&(is_contact);
}

void CPHSimpleCharacter::InitContact(dContact* c){
	bool bo1=(c->geom.g1==m_wheel_transform);
	if(!bo1 || c->geom.g2!=m_wheel_transform) return;
	//dGeomID geom;

	//if(bo1) geom=dGeomTransformGetGeom(c->geom.g1);
	//else	geom=dGeomTransformGetGeom(c->geom.g2);

	if(bo1){
		if(c->geom.normal[1]>0.1f)
		is_contact=true;
		memcpy(m_contact_normal,c->geom.normal,sizeof(dVector3));
	}
	else{
		if(c->geom.normal[1]<0.1f)
		is_contact=true;
		m_contact_normal[0]=-c->geom.normal[0];
		m_contact_normal[1]=-c->geom.normal[1];
		m_contact_normal[2]=-c->geom.normal[2];
	}

        
	if(is_control&&is_contact){
					c->surface.mode =dContactApprox1|dContactFDir1|dContactSoftCFM|dContactSoftERP;// dContactBounce;
					c->surface.mu = 0.00f;
					c->surface.mu2 = dInfinity;
					c->surface.soft_cfm=0.0001f;
					c->surface.soft_erp=0.2f;

					memcpy(c->fdir1,m_control_accel,sizeof(dVector3));
					dNormalize3(c->fdir1);
					
					}
		else
		{
		c->surface.mode =dContactApprox1|dContactSoftCFM|dContactSoftERP;
		c->surface.soft_cfm=0.0001f;
		c->surface.soft_erp=0.2f;

		c->surface.mu = 3.f;
		}




}

void CPHSimpleCharacter::SetAcceleration(Fvector accel){
is_control=true;
if(!is_contact) return;

//dReal* pos=const_cast<dReal*>(dBodyGetPosition((dBodyID)chBody));
dReal* R= const_cast<dReal*>(dBodyGetRotation((dBodyID)m_body));


dVector3 fvdir,sidedir;
accel.y=0.f;//just in case
dCROSS(sidedir,=,m_contact_normal,accel);
dCROSS(fvdir,=,m_contact_normal,sidedir);
dReal mag=accel.magnitude();
dRFrom2Axes(R,sidedir[0],sidedir[1],sidedir[2],accel.x,accel.y,accel.z);

dNormalize3(fvdir);dNormalize3(sidedir);

m_control_accel[0]=fvdir[0]*mag;
m_control_accel[1]=fvdir[1]*mag;
m_control_accel[2]=fvdir[2]*mag;

//dBodyAddForce(chBody,pos[0],pos[1],pos[2]);


}

void CPHSimpleCharacter::SetPosition(Fvector pos){
	dBodySetPosition(m_body,pos.x,pos.y,pos.z);
}

Fvector CPHSimpleCharacter::GetPosition(){
	const dReal* pos=dBodyGetPosition(m_body);
	Fvector vpos;
	memcpy(&vpos,pos,sizeof(Fvector));
	return vpos;
}

Fvector CPHSimpleCharacter::GetVelocity(){
	const dReal* vel=dBodyGetLinearVel(m_body);
	Fvector vvel;
	memcpy(&vvel,vel,sizeof(Fvector));
	return vvel;
}


void CPHSimpleCharacter::SetVelocity(Fvector vel){
	dBodySetLinearVel(m_body,vel.x,vel.y,vel.z);
}

//////////////////////////////////////////////////////////////////////////
/////////////////////CPHWheeledCharacter//////////////////////////////////
//////////////////////////////////////////////////////////////////////////




void CPHWheeledCharacter::Create(){

//chRCylinder=dCreateBox(0,0.8f,2.f,0.8f);
m_cap=dCreateSphere(0,0.5f);
m_wheel=dCreateSphere(0,0.5f);
m_geom_shell=dCreateCylinder(0,0.4f,2.f);


//dGeomTransformSetInfo(chTransform1,1);
//dGeomTransformSetInfo(chTransform2,1);
dMatrix3 RZ;
dRFromAxisAndAngle (RZ,0.f, 0.f, 1.f, M_PI/2.f);
dGeomSetRotation(m_cap,RZ);
dGeomSetPosition(m_cap,0.f,1.f,0.f);
m_cap_transform=dCreateGeomTransform(0);
m_shell_transform=dCreateGeomTransform(0);
dGeomTransformSetInfo(m_cap_transform,1);
dGeomTransformSetInfo(m_shell_transform,1);

//dGeomSetPosition(chSphera2,0.f,-0.7f,0.f);
//dGeomSetPosition(chRCylinder,0.f,0.f,0.f);
dGeomTransformSetGeom(m_cap_transform,m_cap);
dGeomTransformSetGeom(m_shell_transform,m_geom_shell);
m_body=dBodyCreate(phWorld);
m_wheel_body=dBodyCreate(phWorld);




dGeomSetBody(m_cap_transform,m_body);
dGeomSetBody(m_shell_transform,m_body);
//dGeomSetBody(chRCylinder,chRBody);
dGeomSetBody(m_wheel,m_wheel_body);
dBodySetPosition(m_body,-9,4,0);
dBodySetPosition(m_wheel_body,-9,2.5,0);
dMass m;

dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
dMassAdjust(&m,70.f);
dBodySetMass(m_body,&m);

dMassSetSphere(&m,1,0.5);
dMassAdjust(&m,1.f);
dBodySetMass(m_wheel_body,&m);

m_wheel_joint=dJointCreateHinge2(phWorld,0);
dJointAttach(m_wheel_joint,m_body,m_wheel_body);

dJointSetHinge2Anchor (m_wheel_joint,-9 , 2.5, 0);
dJointSetHinge2Axis1 (m_wheel_joint, 0, 1, 0);
dJointSetHinge2Axis2 (m_wheel_joint, 0, 0, 1);


//dJointSetHinge2Param(chRJoint, dParamLoStop, 0);
//dJointSetHinge2Param(chRJoint, dParamHiStop, 0);
dJointSetHinge2Param(m_wheel_joint, dParamFMax, 5000);

dJointSetHinge2Param(m_wheel_joint, dParamVel2, 0);
dJointSetHinge2Param(m_wheel_joint, dParamFMax2, 1000);

		dReal k_p=4000.f;
		dReal k_d=500.f;
		dReal h=0.025f;
			


//	dWorldSetCFM(world,  1.f / (h*k_p + k_d));
//	dWorldSetERP(world,  h*k_p / (h*k_p + k_d));

dJointSetHinge2Param(m_wheel_joint, dParamSuspensionERP, h*k_p / (h*k_p + k_d));
dJointSetHinge2Param(m_wheel_joint, dParamSuspensionCFM, 1.f / (h*k_p + k_d));


m_geom_group=dCreateGeomGroup(ph_world->GetSpace());


dGeomGroupAdd(m_geom_group,m_wheel);
dGeomGroupAdd(m_geom_group,m_cap_transform);
dGeomGroupAdd(m_geom_group,m_shell_transform);
//dGeomGroupAdd(chRGeomGroup,chRCylinder);



}




void CPHWheeledCharacter::Destroy(){

	if(m_body) {
		dBodyDestroy(m_body);
		m_body=NULL;
	}

	if(m_wheel_body){
		dBodyDestroy(m_wheel_body);
		m_wheel_body=NULL;
	}

	if(m_geom_shell){
		dGeomDestroy(m_geom_shell);
		m_geom_shell=NULL;
	}

	if(m_wheel) {
		dGeomDestroyUserData(m_wheel);
			dGeomDestroy(m_wheel);
		m_wheel=NULL;
	}


	if(m_cap) {
		dGeomDestroy(m_cap);
		m_cap=NULL;
	}

	if(m_wheel_transform){
		dGeomDestroy(m_wheel_transform);
			m_wheel_transform=NULL;
	}
	if(m_shell_transform){
		dGeomDestroy(m_shell_transform);
		m_shell_transform=NULL;
	}
	if(m_cap_transform){
		dGeomDestroy(m_cap_transform);
		m_cap_transform=NULL;
	}
	if(m_geom_group){
		dGeomDestroy(m_geom_group);
		m_geom_group=NULL;
	}


}