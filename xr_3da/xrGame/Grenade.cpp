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
	return inherited::net_Spawn	(DC);
}

void CGrenade::net_Destroy() 
{
	inherited::net_Destroy	();
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



bool CGrenade::Useful() 
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
			if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
			xr_delete(m_pPhysicsShell);
			
			//выкинуть гранату из инвентар€
			m_pInventory->Ruck(this); 
			m_dwDestroyTime = 0;
			
			if (Local())
			{
				NET_Packet			P;
				u_EventGen			(P,GE_DESTROY,ID());
				u_EventSend			(P);
			};

			
			if(dynamic_cast<CActor*>(H_Parent()))
			{
				//найти такую же гранату и положить в рюкзак
				CGrenade *pNext = dynamic_cast<CGrenade*>(m_pInventory->Same(this,false));
				//или найти любую другую гранату на по€се
				if(!pNext) pNext = dynamic_cast<CGrenade*>(m_pInventory->SameSlot(m_slot,false));

				if(pNext) 
				{ 
					m_pInventory->Slot(pNext, false); 
					//m_pInventory->Activate(pNext->m_slot); 
				}
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
	case kWPN_NEXT:
		{
            if(flags&CMD_START) 
			{
				if(m_pInventory)
				{

					//ищем на по€се такую же гранату
					PPIItem it = m_pInventory->m_belt.begin();
					while(m_pInventory->m_belt.end() != it && 
						  xr_strcmp((*it)->cNameSect(), cNameSect())) ++it;
					
					if(m_pInventory->m_belt.end() != it) 
					{
						while(m_pInventory->m_belt.end() != it) 
						{
							CGrenade *pGrenade = dynamic_cast<CGrenade*>(*it);
							if(pGrenade && xr_strcmp(pGrenade->cNameSect(), cNameSect())) 
							{
								m_pInventory->Ruck(this); 
								m_pInventory->Slot(pGrenade); 
								m_pInventory->Belt(this); 
								m_pInventory->Activate(pGrenade->m_slot);
								return true;
							}
							++it;
						}

						
						it = m_pInventory->m_belt.begin();
						CGrenade *pGrenade = dynamic_cast<CGrenade*>(*it);

						while(this != pGrenade) 
						{
							if(pGrenade && xr_strcmp(pGrenade->cNameSect(), cNameSect())) 
							{
								m_pInventory->Ruck(this); 
								m_pInventory->Slot(pGrenade); 
								m_pInventory->Belt(this); 
								m_pInventory->Activate(pGrenade->m_slot);
								return true;
							}
							++it;
							if(it == m_pInventory->m_belt.end()) break;
							
							pGrenade = dynamic_cast<CGrenade*>(*it);
						}
						
					}
					return true;
				}
			}
		} return true;
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
	CMissile::reinit			();
	CExplosive::reinit			();
}
