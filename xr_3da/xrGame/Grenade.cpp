#include "stdafx.h"
#include "grenade.h"
#include "PhysicsShell.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"

CGrenade::CGrenade(void) 
{
	m_pFake = NULL;
	m_eSoundCheckout = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
}

CGrenade::~CGrenade(void) 
{
	inherited::SoundDestroy(sndCheckout);
}

void CGrenade::Load(LPCSTR section) 
{
	inherited::Load(section);
	CExplosive::Load(section);

	ref_str snd_name = pSettings->r_string(section,"snd_checkout");
	sndCheckout.create(TRUE,*snd_name, m_eSoundCheckout);
}

BOOL CGrenade::net_Spawn(LPVOID DC) 
{
	return inherited::net_Spawn	(DC);
}

void CGrenade::net_Destroy() 
{
	inherited::net_Destroy	();
}

void CGrenade::OnH_A_Chield() 
{
	inherited::OnH_A_Chield();

	if(!m_pFake && !dynamic_cast<CGrenade*>(H_Parent())) 
	{
		CSE_Abstract		*D	= F_entity_Create(cNameSect());
		R_ASSERT			(D);
		CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT							(l_tpALifeDynamicObject);
		l_tpALifeDynamicObject->m_tNodeID	= level_vertex_id();
		// Fill
		strcpy				(D->s_name,cNameSect());
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}

void CGrenade::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

u32 CGrenade::State(u32 state) 
{
	if(state == MS_THREATEN) 
	{
		Sound->play_at_pos(sndCheckout, 0, Position(), false);
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
	if(!m_pFake) return;
	
	inherited::Throw();
	
	CGrenade *pGrenade = m_pFake;
	if(pGrenade) 
	{
		pGrenade->m_destroyTime = 3500;
		pGrenade->m_force = m_force; 
		//установить ID того кто кинул гранату
		pGrenade->m_iCurrentParentID = H_Parent()->ID();

		m_force = 0;
		if (Local())
		{
			NET_Packet P;
			u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
			P.w_u16(u16(pGrenade->ID()));
			u_EventSend(P);
		}
	}
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
	return m_destroyTime == 0xffffffff;
}

void CGrenade::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE: 
		{
			P.r_u16(id);
			CGrenade *pGrenade = dynamic_cast<CGrenade*>(Level().Objects.net_Find(id));
			m_pFake = pGrenade;
			pGrenade->H_SetParent(this);
		} 
		break;
	case GE_OWNERSHIP_REJECT: 
		{
			P.r_u16(id);
			m_pFake = NULL;
			CGrenade *pGrenade = dynamic_cast<CGrenade*>(Level().Objects.net_Find(id));			
			if (NULL == pGrenade) break;
			pGrenade->H_SetParent(0);
		} 
		break;
	case GE_GRENADE_EXPLODE:
		{
			Explode();
			m_expoldeTime = EXPLODE_TIME_MAX;
		}break;
	}
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
			m_destroyTime = 0;
			
			if (Local())
			{
				NET_Packet P;
//				u_EventGen(P, GE_OWNERSHIP_REJECT, H_Parent()->ID());
//				P.w_u16(u16(ID()));
//				u_EventSend(P);
				u_EventGen			(P,GE_DESTROY,ID());
				u_EventSend			(P);
			};

			//найти такую же гранату и положить в рюкзак
			CGrenade *pNext = dynamic_cast<CGrenade*>(m_pInventory->Same(this));
			//или найти любую другую гранату на по€се
			if(!pNext) pNext = dynamic_cast<CGrenade*>(m_pInventory->SameSlot(m_slot));
						
			if(pNext) 
			{ 
				m_pInventory->Slot(pNext); 
				m_pInventory->Activate(pNext->m_slot); 
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
						  strcmp((*it)->cNameSect(), cNameSect())) ++it;
					
					if(m_pInventory->m_belt.end() != it) 
					{
						while(m_pInventory->m_belt.end() != it) 
						{
							CGrenade *pGrenade = dynamic_cast<CGrenade*>(*it);
							if(pGrenade && strcmp(pGrenade->cNameSect(), cNameSect())) 
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
							if(pGrenade && strcmp(pGrenade->cNameSect(), cNameSect())) 
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

