/////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "phcharacter.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "GameObject.h"
#include "PHCapture.h"
#include "Entity.h"
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
CPHCapture::CPHCapture	(CPHCharacter   *a_character, CGameObject	*a_taget_object)
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
#pragma todo("-- OLES to SLIPCH: a_taget_element should be u16, not int. BTW, -1 = BI_NONE :)")

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