#include "stdafx.h"
#pragma hdrstop
#include "physicsshell.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "PHJoint.h"
#include "PHShell.h"
#include "PHJoint.h"
#include "PHJointDestroyInfo.h"
#include "PHSplitedShell.h"
CPhysicsElement*			P_create_Element		()
{
	CPHElement* element=xr_new<CPHElement>	();
	return element;
}

CPhysicsShell*				P_create_Shell			()
{
	CPhysicsShell* shell=xr_new<CPHShell>	();
	return shell;
}

CPhysicsShell*				P_create_splited_Shell	()
{
	CPhysicsShell* shell=xr_new<CPHSplitedShell>	();
	return shell;
}

CPhysicsJoint*				P_create_Joint			(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)
{
	CPhysicsJoint* joint=xr_new<CPHJoint>	( type , first, second);
	return joint;
}


CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map)
{


	CKinematics* pKinematics=PKinematics(obj->Visual());

	CPhysicsShell* pPhysicsShell		= P_create_Shell();



	pPhysicsShell->build_FromKinematics(pKinematics,bone_map);

	pPhysicsShell->set_PhysicsRefObject(obj);
	pPhysicsShell->mXFORM.set(obj->XFORM());
	pPhysicsShell->Activate(true,not_active_state);//,
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	pPhysicsShell->SetAirResistance();//0.0014f,1.5f

	return pPhysicsShell;
}


CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,BONE_P_MAP* p_bone_map,LPCSTR	fixed_bones)
{
	CPhysicsShell* pPhysicsShell;
	CKinematics* pKinematics=PKinematics(obj->Visual());
	if(fixed_bones)
	{


		int count =					_GetItemCount(fixed_bones);
		for (int i=0 ;i<count; ++i) 
		{
			string64					fixed_bone							;
			_GetItem					(fixed_bones,i,fixed_bone)			;
			u16 fixed_bone_id=pKinematics->LL_BoneID(fixed_bone)			;
			R_ASSERT2(BI_NONE!=fixed_bone_id,"wrong fixed bone")			;
			p_bone_map->insert(mk_pair(fixed_bone_id,physicsBone()))			;
		}

		pPhysicsShell=P_build_Shell(obj,not_active_state,p_bone_map);

		//m_pPhysicsShell->add_Joint(P_create_Joint(CPhysicsJoint::enumType::full_control,0,fixed_element));
	}
	else
		pPhysicsShell=P_build_Shell(obj,not_active_state);


	BONE_P_PAIR_IT i=p_bone_map->begin(),e=p_bone_map->end();
	for(;i!=e;i++)
	{
		CPhysicsElement* fixed_element=i->second.element;
		R_ASSERT2(fixed_element,"fixed bone has no physics");
		FixBody(fixed_element->get_body());
	}
return pPhysicsShell;
}

static BONE_P_MAP bone_map=BONE_P_MAP();
CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,LPCSTR	fixed_bones)
{
	bone_map					.clear();
	return P_build_Shell			(obj,not_active_state,&bone_map,fixed_bones);
}