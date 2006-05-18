#include "stdafx.h"
#include "PhysicsShellHolder.h"
#include "PhysicsShell.h"
#include "xrMessages.h"
#include "ph_shell_interface.h"
#include "../skeletoncustom.h"
#include "script_callback_ex.h"
#include "Level.h"
#include "PHCommander.h"
#include "PHScriptCall.h"
#include "CustomRocket.h"
#include "Grenade.h"
#include "phworld.h"

CPhysicsShellHolder::CPhysicsShellHolder()
{
	init();
}

void CPhysicsShellHolder::net_Destroy()
{
	//remove calls
	CPHSriptReqGObjComparer cmpr(this);
	Level().ph_commander_scripts().remove_calls(&cmpr);
	//������� �������� �� ParticlePlayer
	CParticlesPlayer::net_DestroyParticles		();
	inherited::net_Destroy						();
	b_sheduled									=	false;
	deactivate_physics_shell						();
	xr_delete									(m_pPhysicsShell);
}

static enum EEnableState
{
	stEnable			=0	,
	stDisable				,
	stNotDefitnite		
};
static u8 st_enable_state=(u8)stNotDefitnite;
BOOL CPhysicsShellHolder::net_Spawn				(CSE_Abstract*	DC)
{
	CParticlesPlayer::net_SpawnParticles		();
	st_enable_state=(u8)stNotDefitnite;
	BOOL ret=inherited::net_Spawn				(DC);//load
	b_sheduled									=	true;
		//create_physic_shell			();
	if(PPhysicsShell()&&PPhysicsShell()->isFullActive())
	{
		PPhysicsShell()->GetGlobalTransformDynamic(&XFORM());
		switch (EEnableState(st_enable_state))
		{
		case stEnable		:	PPhysicsShell()->Enable()	;break;
		case stDisable		:	PPhysicsShell()->Disable()	;break;
		case stNotDefitnite	:								;break;
		}

		st_enable_state=(u8)stNotDefitnite;
	}
	return ret;
}

void	CPhysicsShellHolder::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /* ALife::eHitTypeWound*/)
{
	if(impulse>0)
		if(m_pPhysicsShell) m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
}

//void	CPhysicsShellHolder::Hit(float P, Fvector &dir, CObject* who, s16 element,
//						 Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
void	CPhysicsShellHolder::Hit					(SHit* pHDS)
{
	PHHit(pHDS->damage(),pHDS->dir,pHDS->who,pHDS->boneID,pHDS->p_in_bone_space,pHDS->impulse,pHDS->hit_type);
}

void CPhysicsShellHolder::create_physic_shell	()
{
	VERIFY(!m_pPhysicsShell);
	IPhysicShellCreator *shell_creator = smart_cast<IPhysicShellCreator*>(this);
	if (shell_creator)
		shell_creator->CreatePhysicsShell();
}

void CPhysicsShellHolder::init			()
{
	m_pPhysicsShell				=	NULL		;
	b_sheduled					=	false		;
}

void CPhysicsShellHolder::activate_physic_shell()
{
	VERIFY						(!m_pPhysicsShell);
	create_physic_shell			();

	Fvector						l_fw, l_up;
	l_fw.set					(XFORM().k);
	l_up.set					(XFORM().j);
	l_fw.mul					(2.f);
	l_up.mul					(2.f);

	Fmatrix						l_p1, l_p2;
	l_p1.set					(XFORM());
	l_p2.set					(XFORM());
	l_fw.mul					(2.f);
	l_p2.c.add					(l_fw);

	m_pPhysicsShell->Activate	(l_p1, 0, l_p2);
	smart_cast<CKinematics*>(Visual())->CalculateBones_Invalidate	();
	smart_cast<CKinematics*>(Visual())->CalculateBones();
	if(!IsGameTypeSingle())
	{
		if(!smart_cast<CCustomRocket*>(this)&&!smart_cast<CGrenade*>(this)) PPhysicsShell()->SetIgnoreDynamic();
	}
//	XFORM().set					(l_p1);
}

void CPhysicsShellHolder::setup_physic_shell	()
{
	VERIFY						(!m_pPhysicsShell);
	create_physic_shell			();
	m_pPhysicsShell->Activate	(XFORM(),0,XFORM());
}

void CPhysicsShellHolder::deactivate_physics_shell()
{
	if (m_pPhysicsShell)
		m_pPhysicsShell->Deactivate	();
	xr_delete(m_pPhysicsShell);
}
void CPhysicsShellHolder::PHSetMaterial(u16 m)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CPhysicsShellHolder::PHSetMaterial(LPCSTR m)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CPhysicsShellHolder::PHGetLinearVell		(Fvector& velocity)
{
	if(!m_pPhysicsShell)
	{
		velocity.set(0,0,0);
		return;
	}
	m_pPhysicsShell->get_LinearVel(velocity);
}

void CPhysicsShellHolder::PHSetLinearVell(Fvector& velocity)
{
	if(!m_pPhysicsShell)
	{
		return;
	}
	m_pPhysicsShell->set_LinearVel(velocity);
}



f32 CPhysicsShellHolder::GetMass()
{
	return m_pPhysicsShell ? m_pPhysicsShell->getMass() : 0;
}

u16	CPhysicsShellHolder::PHGetSyncItemsNumber()
{
	if(m_pPhysicsShell)	return m_pPhysicsShell->get_ElementsNumber();
	else				return 0;
}

CPHSynchronize*	CPhysicsShellHolder::PHGetSyncItem	(u16 item)
{
	if(m_pPhysicsShell) return m_pPhysicsShell->get_ElementSync(item);
	else				return 0;
}
void	CPhysicsShellHolder::PHUnFreeze	()
{
	if(m_pPhysicsShell) m_pPhysicsShell->UnFreeze();
}


void	CPhysicsShellHolder::PHFreeze()
{
	if(m_pPhysicsShell) m_pPhysicsShell->Freeze();
}

void CPhysicsShellHolder::OnChangeVisual()
{
	inherited::OnChangeVisual();
	if (0==renderable.visual) 
	{
		if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
		VERIFY(0==m_pPhysicsShell);
	}
}

void CPhysicsShellHolder::UpdateCL	()
{
	inherited::UpdateCL	();
	//�������� �������������� ��������
	if (m_bActiveBones)	UpdateParticles	();
}
float CPhysicsShellHolder::EffectiveGravity()
{
	return ph_world->Gravity();
}

void		CPhysicsShellHolder::	save				(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	u8 enable_state=(u8)stNotDefitnite;
	if(PPhysicsShell()&&PPhysicsShell()->isActive())
	{
		enable_state=u8(PPhysicsShell()->isEnabled() ? stEnable:stDisable);
	}
	output_packet.w_u8(enable_state);
}

void		CPhysicsShellHolder::	load				(IReader &input_packet)
{
	inherited::load(input_packet);
	st_enable_state=input_packet.r_u8();

}

void CPhysicsShellHolder::PHSaveState(NET_Packet &P)
{

	//CPhysicsShell* pPhysicsShell=PPhysicsShell();
	CKinematics* K	=smart_cast<CKinematics*>(Visual());
	//Flags8 lflags;
	//if(pPhysicsShell&&pPhysicsShell->isActive())			lflags.set(CSE_PHSkeleton::flActive,pPhysicsShell->isEnabled());

//	P.w_u8 (lflags.get());
	if(K)
	{
		P.w_u64(K->LL_GetBonesVisible());
		P.w_u16(K->LL_GetBoneRoot());
	}
	else
	{
		P.w_u64(u64(-1));
		P.w_u16(0);
	}
	/////////////////////////////
	Fvector min,max;

	min.set(flt_max,flt_max,flt_max);
	max.set(-flt_max,-flt_max,-flt_max);
	/////////////////////////////////////

	u16 bones_number=PHGetSyncItemsNumber();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
		Fvector& p=state.position;
		if(p.x<min.x)min.x=p.x;
		if(p.y<min.y)min.y=p.y;
		if(p.z<min.z)min.z=p.z;

		if(p.x>max.x)max.x=p.x;
		if(p.y>max.y)max.y=p.y;
		if(p.z>max.z)max.z=p.z;
	}

	///min.sub(2.f*EPS_S);
	//max.add(2.f*EPS_S);
	P.w_vec3(min);
	P.w_vec3(max);

	P.w_u16(bones_number);

	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
		state.net_Save(P,min,max);
	}
}
void
CPhysicsShellHolder::PHLoadState(IReader &P)
{
	
//	Flags8 lflags;
	CKinematics* K=smart_cast<CKinematics*>(Visual());
//	P.r_u8 (lflags.flags);
	if(K)
	{
		K->LL_SetBonesVisible(P.r_u64());
		K->LL_SetBoneRoot(P.r_u16());
	}

	Fvector min=P.r_vec3();
	Fvector max=P.r_vec3();
	u16 bones_number=P.r_u16();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		state.net_Load(P,min,max);
		PHGetSyncItem(i)->set_State(state);
	}
}