#include "stdafx.h"
#include "PhysicsShellHolder.h"
#include "PhysicsShell.h"
#include "xrMessages.h"
#include "Level.h"
#include "ph_shell_interface.h"


	CPhysicsShellHolder::CPhysicsShellHolder()
	{
		init();
	}

void CPhysicsShellHolder::net_Destroy()
{
	inherited::net_Destroy						();
	xr_delete									(m_pPhysicsShell);
}
BOOL CPhysicsShellHolder::net_Spawn				(LPVOID	DC)
{
	BOOL ret=inherited::net_Spawn(DC);
	create_physic_shell			();
	return ret;
}

void	CPhysicsShellHolder::PHHit(float P,Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /* ALife::eHitTypeWound*/)
{
	if(impulse>0)
		if(m_pPhysicsShell) m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
}

void	CPhysicsShellHolder::Hit(float P, Fvector &dir, CObject* /**who/**/, s16 element,
						 Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	PHHit(P,dir,element,p_in_object_space,impulse,hit_type);
}

void CPhysicsShellHolder::create_physic_shell	()
{
	IPhysicShellCreator *shell_creator = smart_cast<IPhysicShellCreator*>(this);
	if (shell_creator)
		shell_creator->CreatePhysicsShell();
}

void CPhysicsShellHolder::init			()
{
	m_pPhysicsShell				= NULL;
}

void CPhysicsShellHolder::activate_physic_shell()
{
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
	PKinematics			(Visual())->CalculateBones_Invalidate	();
	PKinematics			(Visual())->CalculateBones();
	///XFORM().set					(l_p1);
}

void CPhysicsShellHolder::setup_physic_shell	()
{
	m_pPhysicsShell->Activate	(XFORM(),0,XFORM());
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


void CPhysicsShellHolder::PHSetPushOut(u32 time /* = 5000 */)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->set_PushOut(time,PushOutCallback1);
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

