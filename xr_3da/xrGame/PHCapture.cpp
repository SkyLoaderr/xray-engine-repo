#include "stdafx.h"
#include "phcharacter.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\cl_intersect.h"
#include "GameObject.h"
#include "PHCapture.h"
#include "Entity.h"

///////////////////////////////////////////////////////////////////////////////////
CPHCapture::CPHCapture									(CPHCharacter   *a_character, CGameObject	*a_taget_object) 
{
	CPHObject::Activate();

	m_joint					=NULL;	
	m_ajoint				=NULL;
	m_body					=NULL;
	b_failed				=false;
	e_state					=cstPulling;

	if(!a_taget_object||!a_taget_object->m_pPhysicsShell||!a_taget_object->m_pPhysicsShell->bActive) 
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!a_character||!a_character->b_exist)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}
	m_taget_object			=a_taget_object;
	m_character				=a_character;

	CObject* capturer_object=dynamic_cast<CObject*>(m_character->PhysicsRefObject());

	if(!capturer_object)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CKinematics* p_kinematics=PKinematics(capturer_object->Visual());

	if(!p_kinematics)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CInifile* ini=p_kinematics->LL_UserData();

	if(!ini)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!ini->section_exist("capture"))
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	m_capture_bone=&p_kinematics->LL_GetInstance(
		p_kinematics->LL_BoneID(ini->r_string("capture","bone"))
		);


	m_taget_element					=m_taget_object->m_pPhysicsShell->NearestToPoint(m_capture_bone->mTransform.c);

	Init(ini);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CPHCapture::CPHCapture(CPHCharacter   *a_character,CGameObject	*a_taget_object,int a_taget_element) 
{
	CPHObject::Activate();
	m_joint					=NULL;	
	m_ajoint				=NULL;
	m_body					=NULL;
	b_failed				=false;
	e_state					=cstPulling;

	if(!a_taget_object||!a_taget_object->m_pPhysicsShell||!a_taget_object->m_pPhysicsShell->bActive) 
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!a_character||!a_character->b_exist)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}
	m_taget_object			=a_taget_object;
	m_character				=a_character;

	CObject* capturer_object=dynamic_cast<CObject*>(m_character->PhysicsRefObject());

	if(!capturer_object)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CKinematics* p_kinematics=PKinematics(capturer_object->Visual());

	if(!p_kinematics)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CInifile* ini=p_kinematics->LL_UserData();

	if(!ini)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(a_taget_element==-1)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	if(!ini->section_exist("capture"))
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	m_capture_bone=&p_kinematics->LL_GetInstance(
		p_kinematics->LL_BoneID(ini->r_string("capture","bone"))
		);


	IRender_Visual* V=m_taget_object->Visual();

	if(!V)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CKinematics* K=	PKinematics(V);

	if(!K)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	CBoneInstance& tag_bone=K->LL_GetInstance(a_taget_element);

	if(!tag_bone.Callback_Param)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	m_taget_element					=(CPhysicsElement*)tag_bone.Callback_Param;

	if(!m_taget_element)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}

	Init(ini);

}



void CPHCapture::Init(CInifile* ini)
{
	Fvector dir;
	Fvector capture_bone_position;
	capture_bone_position.set(m_capture_bone->mTransform.c);
	dynamic_cast<CObject*>(m_character->PhysicsRefObject())->XFORM().transform_tiny(capture_bone_position);
	m_taget_element->GetGlobalPositionDynamic(&dir);
	dir.sub(capture_bone_position,dir);


	m_pull_distance=ini->r_float("capture","pull_distance");
	if(dir.magnitude()>m_pull_distance)
	{
		m_taget_object=NULL;
		b_failed=true;
		return;
	}


	m_capture_distance		=ini->r_float("capture","distance");					//distance
	m_capture_force			=ini->r_float("capture","capture_force");				//capture force
	m_capture_time			=ini->r_u32("capture","time_limit")*1000;				//time;		
	m_time_start			=Device.dwTimeGlobal;
	float max_pull_force    =ini->r_float("capture","pull_force");					//pull force
	m_pull_force			=4.f*world_gravity*m_taget_element->PhysicsShell()->getMass();
	if(m_pull_force>max_pull_force) m_pull_force=max_pull_force;



	float pulling_vel_scale =ini->r_float("capture","velocity_scale");				//
	m_taget_element->set_DynamicLimits(default_l_limit*pulling_vel_scale,default_w_limit*pulling_vel_scale);
	//m_taget_element->PhysicsShell()->set_ObjectContactCallback(object_contactCallbackFun);
	m_character->SetObjectContactCallback(object_contactCallbackFun);
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
	Deactivate();
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
	case cstReleased: ReleasedUpdate();
		break;
	default: NODEFAULT;
	}
}

void CPHCapture::PhTune(dReal step)
{

}

void CPHCapture::PullingUpdate()
{
	if(!m_taget_element->bActive||Device.dwTimeGlobal-m_time_start>m_capture_time)
	{
		Release();
		return;
	}

	Fvector dir;
	Fvector capture_bone_position;
	CObject* object=dynamic_cast<CObject*>(m_character->PhysicsRefObject());
	capture_bone_position.set(m_capture_bone->mTransform.c);
	object->XFORM().transform_tiny(capture_bone_position);
	m_taget_element->GetGlobalPositionDynamic(&dir);
	dir.sub(capture_bone_position,dir);
	float dist=dir.magnitude();
	if(dist>m_pull_distance)
	{
		Release();
		return;
	}
	dir.mul(1.f/dist);
	if(dist<m_capture_distance)
	{
		m_back_force=0.f;

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
		dJointAttach(m_joint,m_body,m_taget_element->get_body());
		dJointAttach(m_ajoint,m_body,m_taget_element->get_body());
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


		//dJointSetAMotorParam(m_ajoint,dParamLoStop ,0.f);
		//dJointSetAMotorParam(m_ajoint,dParamHiStop ,0.f);	

		m_taget_element->set_DynamicLimits();
		e_state=cstCaptured;
		return;
	}

	m_taget_element->applyForce(dir,m_pull_force);

	m_character->ApplyForce(dir,-m_pull_force/2.f);

}

void CPHCapture::CapturedUpdate()
{


	if(!m_taget_element->bActive||dDOT(m_joint_feedback.f2,m_joint_feedback.f2)>m_capture_force*m_capture_force) 
	{
		Release();
		return;
	}

	float mag=dSqrt(dDOT(m_joint_feedback.f1,m_joint_feedback.f1));

	//m_back_force=m_back_force*0.999f+ ((mag<m_capture_force/5.f) ? mag : (m_capture_force/5.f))*0.001f;
	//
	if(mag>m_capture_force/2.2f)
	{
		float f=mag/(m_capture_force/15.f);
		m_character->ApplyForce(m_joint_feedback.f1[0]/f,m_joint_feedback.f1[1]/f,m_joint_feedback.f1[2]/f);
	}

	Fvector capture_bone_position;
	CObject* object=dynamic_cast<CObject*>(m_character->PhysicsRefObject());
	capture_bone_position.set(m_capture_bone->mTransform.c);
	object->XFORM().transform_tiny(capture_bone_position);
	dBodySetPosition(m_body,capture_bone_position.x,capture_bone_position.y,capture_bone_position.z);
}

void CPHCapture::ReleasedUpdate()
{
	if(!b_collide) b_failed=true;
	b_collide=false;
}

void CPHCapture::ReleaseInCallBack()
{
	//	if(!b_failed) return;
	b_collide=true;
}
void CPHCapture::Release()
{
	if(b_failed) return;
	if(e_state==cstReleased) return;
	if(m_joint) dJointDestroy(m_joint);
	m_joint=NULL;
	if(m_body) dBodyDestroy(m_body);
	m_body=NULL;

	if(m_taget_element&&m_taget_object->m_pPhysicsShell)
	{
		m_taget_element->set_DynamicLimits();
	}

	e_state=cstReleased;
	b_collide=true;
}

void CPHCapture::Deactivate()
{
	Release();
	if(m_taget_object&&m_taget_element&&m_taget_object->m_pPhysicsShell&&m_taget_object->m_pPhysicsShell->bActive)
	{
		//m_taget_element->set_ObjectContactCallback(0);
		m_character->SetObjectContactCallback(0);
	}
}



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
		CPHCapture* capture=capturer->Movement.PHCapture();
		if(capture)
		{
			if(capture->m_taget_element->PhysicsRefObject()==l_pUD2->ph_ref_object) 
				do_colide = false;
			if(capture->e_state==CPHCapture::cstReleased) capture->ReleaseInCallBack();
		}


	}

	capturer=dynamic_cast<CEntityAlive*>(l_pUD2->ph_ref_object);
	if(capturer)
	{
		CPHCapture* capture=capturer->Movement.PHCapture();
		if(capture)
		{
			if(capture->m_taget_element->PhysicsRefObject()==l_pUD1->ph_ref_object) 
				do_colide = false;
			if(capture->e_state==CPHCapture::cstReleased) capture->ReleaseInCallBack();
		}


	}


}