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
	//удалить партиклы из ParticlePlayer
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
	BOOL ret=inherited::net_Spawn				(DC);
	b_sheduled									=	true;
		//create_physic_shell			();

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
	
	
	switch (EEnableState(st_enable_state))
	{
	case stEnable		:	PPhysicsShell()->Enable()	;
	case stDisable		:	PPhysicsShell()->Disable()	;
	case stNotDefitnite	:								;
	}
	
	st_enable_state=(u8)stNotDefitnite;
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
	//обновить присоединенные партиклы
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