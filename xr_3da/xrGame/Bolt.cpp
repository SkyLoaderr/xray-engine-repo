#include "stdafx.h"
#include "bolt.h"
#include "ParticlesObject.h"
#include "PhysicsShell.h"
#include "xr_level_controller.h"

CBolt::CBolt(void) 
{
	//m_belt = true;
	m_weight = .1f;
	m_slot = BOLT_SLOT;
	m_ruck = false;
}

CBolt::~CBolt(void) 
{
}

void CBolt::OnH_A_Chield() 
{
	inherited::OnH_A_Chield();
}

void CBolt::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
}

bool CBolt::Activate() 
{
	Show();
	return true;
}

void CBolt::Deactivate() 
{
	Hide();
}

void CBolt::Throw() 
{
	CBolt						*l_pBolt = smart_cast<CBolt*>(m_fake_missile);
	VERIFY						(l_pBolt);
	l_pBolt->m_dwDestroyTime = m_dwDestroyTimeMax;
	inherited::Throw			();
	spawn_fake_missile			();
}

bool CBolt::Useful() const
{
	// ≈сли IItem еще не полностью использованый, вернуть true
	return m_dwDestroyTime == 0xffffffff;
}

bool CBolt::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) 
	{
	case kDROP:
		{
			if(flags&CMD_START) 
			{
				m_throw = false;
				if(State() == MS_IDLE) State(MS_THREATEN);
			} 
			else if(State() == MS_READY || State() == MS_THREATEN) 
			{
				m_throw = true; 
				if(State() == MS_READY) State(MS_THROW);
			}
		} 
		return true;
	}
	return false;
}

void CBolt::Destroy()
{
	inherited::Destroy();
}

void CBolt::OnH_B_Independent()
{
	inherited::OnH_B_Independent();
	m_pPhysicsShell->SetAirResistance(.0001f);
}