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
	HUD_SOUND::DestroySound(sndCheckout);
}

void CGrenade::Load(LPCSTR section) 
{
	inherited::Load(section);
	CExplosive::Load(section);

	HUD_SOUND::LoadSound(section,"snd_checkout",sndCheckout,TRUE,m_eSoundCheckout);
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
	m_pHUD->StopCurrentAnimWithoutCallback();
	inherited::OnH_B_Independent();
}

u32 CGrenade::State(u32 state) 
{
	if(state == MS_THREATEN) 
	{
		//UpdateFP();
#pragma todo("Oles to Yura: position can be under level, use 'Center' instead")
		PlaySound	(sndCheckout,Center());	// 
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
		//���������� ID ���� ��� ����� �������
		pGrenade->m_iCurrentParentID = H_Parent()->ID();
	}

	inherited::Throw			();
}



void CGrenade::Destroy() 
{
	//Generate Expode event


	Fvector  normal;
	FindNormal(normal);
	CExplosive::GenExplodeEvent(Position(), normal);
}



bool CGrenade::Useful() const
{
	// ���� IItem ��� �� ��������� �������������, ������� true
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
//				Msg					("ge_destroy: [%d] - %s",ID(),*cName());
				u_EventSend			(P);
			};

			//�������� ������� �� ���������
			if (m_pInventory)
			{
				m_pInventory->Ruck(this);
				m_pInventory->SetActiveSlot(NO_ACTIVE_SLOT);
			}
		
			if(dynamic_cast<CActor*>(H_Parent()))
			{
				//����� ����� �� ������� � �������� � ������
				CGrenade *pNext = dynamic_cast<CGrenade*>(m_pInventory->Same(this,false));
				//��� ����� ����� ������ ������� �� �����
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
	//������������ ���� �������
	case kWPN_NEXT:
		{
            if(flags&CMD_START) 
			{
				if(m_pInventory)
				{
					//���������� ��� �������� �� ����� 
					//���� �� �������� ������� ������� ����
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
