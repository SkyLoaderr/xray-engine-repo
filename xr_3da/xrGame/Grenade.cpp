#include "stdafx.h"
#include "grenade.h"
#include "PhysicsShell.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"
#include "actor.h"
#include "inventory.h"

CGrenade::CGrenade(void) 
{
	m_eSoundCheckout = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
}

CGrenade::~CGrenade(void) 
{
	DestroySound(sndCheckout);
}

void CGrenade::Load(LPCSTR section) 
{
	inherited::Load(section);
	CExplosive::Load(section);

	LoadSound(section,"snd_checkout",sndCheckout,TRUE,m_eSoundCheckout);
}

BOOL CGrenade::net_Spawn(LPVOID DC) 
{
	return (inherited::net_Spawn(DC) && CExplosive::net_Spawn(DC));
}

void CGrenade::net_Destroy() 
{
	inherited::net_Destroy	();
	CExplosive::net_Destroy();
}

void CGrenade::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

u32 CGrenade::State(u32 state) 
{
	if(state == MS_THREATEN) 
	{
		//UpdateFP();
		PlaySound(sndCheckout,Position());
	}
	return inherited::State(state);
}

bool CGrenade::Activate() 
{
	Show();
	return true;
}

void CGrenade::Deactivate() 
{
	Hide();
}

void CGrenade::Throw() 
{
	if (!m_fake_missile)
		return;

	CGrenade					*pGrenade = dynamic_cast<CGrenade*>(m_fake_missile);
	VERIFY						(pGrenade);
	
	if (pGrenade) {
		pGrenade->m_dwDestroyTime = m_dwDestroyTimeMax;
		//установить ID того кто кинул гранату
		pGrenade->m_iCurrentParentID = H_Parent()->ID();
	}

	inherited::Throw			();
}

void CGrenade::Destroy() 
{
	//Generate Expode event
	if (Local()) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	};
}



bool CGrenade::Useful() const
{
	// ≈сли IItem еще не полностью использованый, вернуть true
	return m_dwDestroyTime == 0xffffffff;
}

void CGrenade::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	CExplosive::OnEvent(P,type);
}

void CGrenade::OnAnimationEnd() 
{
	switch(inherited::State()) 
	{

	case MS_END:
		{
			if (m_pPhysicsShell)
				m_pPhysicsShell->Deactivate();
			xr_delete(m_pPhysicsShell);
			
			m_dwDestroyTime = 0;
			
			if (Local())
			{
				NET_Packet			P;
				u_EventGen			(P,GE_DESTROY,ID());
				Msg					("ge_destroy: [%d] - %s",ID(),*cName());
				u_EventSend			(P);
			};

			//выкинуть гранату из инвентар€
			if (m_pInventory)
			{
				m_pInventory->Ruck(this);
				m_pInventory->SetActiveSlot(NO_ACTIVE_SLOT);
			}
		
			if(dynamic_cast<CActor*>(H_Parent()))
			{
				//найти такую же гранату и положить в рюкзак
				CGrenade *pNext = dynamic_cast<CGrenade*>(m_pInventory->Same(this,false));
				//или найти любую другую гранату на по€се
				if(!pNext) pNext = dynamic_cast<CGrenade*>(m_pInventory->SameSlot(m_slot,false));

				R_ASSERT(pNext != this);

				if(pNext) 
					m_pInventory->Slot(pNext);
			}
		}
		break;
		default : inherited::OnAnimationEnd();
	}
}


void CGrenade::UpdateCL() 
{
	////////////////////////////////////
	inherited::UpdateCL();
	
	CExplosive::UpdateCL();	
	////////////////////////////////////
	make_Interpolation();
}



bool CGrenade::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	switch(cmd) 
	{
	//переключение типа гранаты
	case kWPN_NEXT:
		{
            if(flags&CMD_START) 
			{
				if(m_pInventory)
				{
					//перебираем все предметы на по€се 
					//пока не встретим гарнату другого типа
					PPIItem it = m_pInventory->m_belt.begin();
					while(m_pInventory->m_belt.end() != it) 
					{
						CGrenade *pGrenade = dynamic_cast<CGrenade*>(*it);
						if(pGrenade && xr_strcmp(pGrenade->cNameSect(), cNameSect())) 
						{
							m_pInventory->Ruck(this);
							m_pInventory->Belt(this);
							m_pInventory->SetActiveSlot(NO_ACTIVE_SLOT);
							m_pInventory->Slot(pGrenade);
							//m_pInventory->Activate(pGrenade->m_slot);
							return true;
						}
						++it;
					}
					return true;
				}
			}
			return true;
		};
	case kDROP:
		setup_throw_params();
		return true;
	}
	return false;
}

void CGrenade::net_Import			(NET_Packet& P) 
{
	inherited::net_Import (P);	
};

void CGrenade::net_Export			(NET_Packet& P) 
{
	inherited::net_Export (P);
};

void CGrenade::make_Interpolation ()
{
	inherited::make_Interpolation();
}

void CGrenade::PH_B_CrPr			()
{
	inherited::PH_B_CrPr		();
}

void CGrenade::PH_I_CrPr			()
{
	inherited::PH_I_CrPr		();
}

void CGrenade::PH_A_CrPr			()
{
	inherited::PH_A_CrPr		();
}

void CGrenade::reinit				()
{
	inherited::reinit			();
	CExplosive::reinit			();
}

void CGrenade::reload					(LPCSTR section)
{
	inherited::reload			(section);
	CExplosive::reload			(section);
}

void CGrenade::activate_physic_shell	()
{
	inherited::activate_physic_shell();
}

void CGrenade::setup_physic_shell		()
{
	inherited::setup_physic_shell();
}

void CGrenade::create_physic_shell		()
{
	inherited::create_physic_shell();
}
