#include "stdafx.h"
#include "phcharacter.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\cl_intersect.h"
#include "GameObject.h"
#include "PHCapture.h"


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
	m_pull_force=			1.3f*world_gravity*a_taget->getMass();
	if(m_pull_force>a_pull_force) m_pull_force=a_pull_force;
	m_joint=NULL;	
	
	b_failed				=false;
	e_state					=cstPulling;
	CPHObject::Activate();
	
}

CPHCapture::~CPHCapture()
{
	CPHObject::Deactivate();
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
if(Device.dwTimeGlobal-m_time_start>m_capture_time)
{
b_failed=true;
return;
}


Fvector dir;
const Fvector& bone_position=m_capture_bone->mTransform.c;
m_taget->GetGlobalPositionDynamic(&dir);
dir.sub(bone_position,dir);
float dist=dir.magnitude();
if(dist<m_capture_distance)
{
m_joint=dJointCreateBall(phWorld,0);
dJointSetBallAnchor(m_joint,bone_position.x,bone_position.y,bone_position.z);
dJointAttach(m_joint,m_character->GetBody(),m_taget->get_body());
e_state=cstCaptured;
return;
}
dir.normalize();
m_taget->applyForce(dir,m_pull_force);
}

void CPHCapture::CapturedUpdate()
{

}
