#include "stdafx.h"
#include "phcharacter.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\cl_intersect.h"
#include "GameObject.h"
#include "PHCapture.h"
#include "Entity.h"

void CPHCapture::object_contactCallbackFun(bool& do_colide,dContact& c)
{

	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	if(dGeomGetClass(c.geom.g1)==dGeomTransformClass) 
	{
		const dGeomID geom=dGeomTransformGetGeom(c.geom.g1);
		l_pUD1 = dGeomGetUserData(geom);
	}
	else 
		l_pUD1 = dGeomGetUserData(c.geom.g1);

	if(! l_pUD1) return;

	if(dGeomGetClass(c.geom.g2)==dGeomTransformClass) 
	{
		const dGeomID geom=dGeomTransformGetGeom(c.geom.g2);
		l_pUD2 = dGeomGetUserData(geom);
	} 
	else 
		l_pUD2 = dGeomGetUserData(c.geom.g2);

	if(!l_pUD2) return;

	CEntityAlive* capturer=dynamic_cast<CEntityAlive*>(l_pUD1->ph_ref_object);
	if(capturer)
	{
		CPHCapture* capture=capturer->Movement.Capture();
		if(capture)
		{
		if(capture->m_taget->PhysicsRefObject()==l_pUD2->ph_ref_object) 
			do_colide = false;
		}


	}
	
	capturer=dynamic_cast<CEntityAlive*>(l_pUD2->ph_ref_object);
	if(capturer)
	{
		CPHCapture* capture=capturer->Movement.Capture();
		if(capture)
		{
			if(capture->m_taget->PhysicsRefObject()==l_pUD1->ph_ref_object) 
				do_colide = false;
		}


	}

	
}

CPHCapture::CPHCapture									(CPHCharacter   *a_character,
														 CPhysicsElement*a_taget,
														 CBoneInstance  *a_capture_bone,
														 float			a_capture_distance,
														 u32			a_capture_time,
														 float			a_pull_force,
														 float			a_capture_force)
{
	m_character				=a_character;
	m_taget					=a_taget;
	m_capture_bone			=a_capture_bone;
	m_capture_distance		=a_capture_distance;
	m_capture_force			=a_capture_force;
	m_capture_time			=a_capture_time;		
	m_time_start			=Device.dwTimeGlobal;
	
	m_pull_force			=4.f*world_gravity*a_taget->PhysicsShell()->getMass();
	if(m_pull_force>a_pull_force) m_pull_force=a_pull_force;
	m_joint					=NULL;	
	m_ajoint				=NULL;
	m_body					=NULL;
	b_failed				=false;
	e_state					=cstPulling;


	m_taget->set_DynamicLimits(default_l_limit/1000.f,default_w_limit/1000.f);
	m_taget->PhysicsShell()->set_ObjectContactCallback(object_contactCallbackFun);
	CPHObject::Activate();
	
}

void CPHCapture::CreateBody()
{
	m_body= dBodyCreate(phWorld);
	dMass m;
	dMassSetSphere(&m,1.f,1000000.f);
	dMassAdjust(&m,100000.f);
	dBodySetMass(m_body,&m);
	dBodySetGravityMode(m_body,0);
}

CPHCapture::~CPHCapture()
{
	CPHObject::Deactivate();
	if(m_joint) dJointDestroy(m_joint);
	m_joint=NULL;
	if(m_body) dBodyDestroy(m_body);
	m_taget->set_ObjectContactCallback(0);

}
void CPHCapture::PhDataUpdate(dReal step)
{
	if(b_failed) return;
	switch(e_state) 
	{
	case cstPulling:  PullingUpdate();
		break;
	case cstCaptured: CapturedUpdate();
		break;
	default: NODEFAULT;
	}
}


void CPHCapture::PullingUpdate()
{
if(!m_taget->bActive||Device.dwTimeGlobal-m_time_start>m_capture_time)
{
	b_failed=true;
	return;
}

Fvector dir;
Fvector capture_bone_position;
CObject* object=dynamic_cast<CObject*>(m_character->PhysicsRefObject());
capture_bone_position.set(m_capture_bone->mTransform.c);
object->XFORM().transform_tiny(capture_bone_position);
m_taget->GetGlobalPositionDynamic(&dir);
dir.sub(capture_bone_position,dir);
float dist=dir.magnitude();
dir.mul(1.f/dist);
if(dist<m_capture_distance)
{
m_joint=dJointCreateBall(phWorld,0);

m_ajoint=dJointCreateAMotor(phWorld,0);
dJointSetAMotorMode (m_ajoint, dAMotorEuler);
dJointSetAMotorNumAxes (m_ajoint, 3);



//dJointSetAMotorParam(m_ajoint,dParamFMax ,m_capture_force*0.2f);
//dJointSetAMotorParam(m_ajoint,dParamVel  ,0.f);

dJointSetAMotorParam(m_ajoint,dParamFMax2 ,m_capture_force*0.2f);
dJointSetAMotorParam(m_ajoint,dParamVel2  ,0.f);

dJointSetAMotorParam(m_ajoint,dParamFMax3 ,m_capture_force*0.2f);
dJointSetAMotorParam(m_ajoint,dParamVel3  ,0.f);





CreateBody();
dBodySetPosition(m_body,capture_bone_position.x,capture_bone_position.y,capture_bone_position.z);
dJointAttach(m_joint,m_body,m_taget->get_body());
dJointAttach(m_ajoint,m_body,m_taget->get_body());
dJointSetFeedback (m_joint, &m_joint_feedback);
dJointSetBallAnchor(m_joint,capture_bone_position.x,capture_bone_position.y,capture_bone_position.z);


dJointSetAMotorAxis (m_ajoint, 0, 1, dir.x, dir.y, dir.z);

if(dir.x>EPS)
{
if(dir.y>EPS)
	{
	float mag=dir.x*dir.x+dir.y*dir.y;
	dJointSetAMotorAxis (m_ajoint, 2, 2, -dir.y/mag, dir.x/mag, 0.f);
	}
else if(dir.z>EPS)
	{
	float mag=dir.x*dir.x+dir.z*dir.z;
	dJointSetAMotorAxis (m_ajoint, 2, 2, -dir.z/mag,0.f,dir.x/mag);
	}
else
	{
	dJointSetAMotorAxis (m_ajoint, 2, 2, 1.f,0.f,0.f);
	}
}
else
{
	if(dir.y>EPS)
	{

		if(dir.z>EPS)
		{
			float mag=dir.y*dir.y+dir.z*dir.z;
			dJointSetAMotorAxis (m_ajoint, 2, 2,0.f,-dir.z/mag,dir.y/mag);
		}
		else
		{
			dJointSetAMotorAxis (m_ajoint, 2, 2, 0.f,1.f,0.f);
		}
	}
	else 
	{
			dJointSetAMotorAxis (m_ajoint, 2, 2, 0.f,0.f,1.f);
	}
}


dJointSetAMotorParam(m_ajoint,dParamLoStop ,0.f);
dJointSetAMotorParam(m_ajoint,dParamHiStop ,0.f);	

m_taget->set_DynamicLimits();
e_state=cstCaptured;
return;
}

m_taget->applyForce(dir,m_pull_force);

}

void CPHCapture::CapturedUpdate()
{

	if(!m_taget->bActive||dDOT(m_joint_feedback.f2,m_joint_feedback.f2)>m_capture_force*m_capture_force) 
	{
		b_failed=true;
		dJointDestroy(m_joint);
		m_joint=NULL;
		return;
	}

	Fvector capture_bone_position;
	CObject* object=dynamic_cast<CObject*>(m_character->PhysicsRefObject());
	capture_bone_position.set(m_capture_bone->mTransform.c);
	object->XFORM().transform_tiny(capture_bone_position);
	dBodySetPosition(m_body,capture_bone_position.x,capture_bone_position.y,capture_bone_position.z);
}