#include "stdafx.h"
#include "grenade.h"
#include "PhysicsShell.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "ParticlesObject.h"
#include "actor.h"
#include "inventory.h"
#include "level.h"
#include "xrmessages.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"

#define GRENADE_REMOVE_TIME		30000
const float default_grenade_detonation_threshold_hit=100;
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

	//////////////////////////////////////
	//время убирания оружия с уровня
	if(pSettings->line_exist(section,"grenade_remove_time"))
		m_dwGrenadeRemoveTime = pSettings->r_u32(section,"grenade_remove_time");
	else
		m_dwGrenadeRemoveTime = GRENADE_REMOVE_TIME;
	m_grenade_detonation_threshold_hit=READ_IF_EXISTS(pSettings,r_float,section,"detonation_threshold_hit",default_grenade_detonation_threshold_hit);
	//////////////////////////////////////
}

//void CGrenade::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector position_in_object_space, float impulse,ALife::EHitType hit_type)
void CGrenade::Hit					(SHit* pHDS)
{
	if( ALife::eHitTypeExplosion==pHDS->hit_type && m_grenade_detonation_threshold_hit<pHDS->damage()&&CExplosive::Initiator()==u16(-1)) 
	{
		CExplosive::SetCurrentParentID(pHDS->who->ID());
		Destroy();
	}
//	inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);
	inherited::Hit(pHDS);
}

BOOL CGrenade::net_Spawn(CSE_Abstract* DC) 
{
	m_dwGrenadeIndependencyTime = 0;
	BOOL ret= inherited::net_Spawn(DC);/* && CInventoryItemObject::net_Spawn(DC)*/
	Fvector box;BoundingBox().getsize(box);
	float max_size=max(max(box.x,box.y),box.z);
	box.set(max_size,max_size,max_size);
	box.mul(3.f);
	CExplosive::SetExplosionSize(box);
	return ret;
}

void CGrenade::net_Destroy() 
{
	inherited::net_Destroy	();
	//CInventoryItemObject::net_Destroy();
	CExplosive::net_Destroy();
}

void CGrenade::OnH_B_Independent() 
{
	m_pHUD->StopCurrentAnimWithoutCallback();
	if (Local() && (m_state == MS_THREATEN || m_state == MS_READY || m_state == MS_THROW))
	{
		if (m_fake_missile)
		{
			CGrenade					*pGrenade = smart_cast<CGrenade*>(m_fake_missile);
			if (pGrenade)
			{
				m_constpower = false;
				m_fThrowForce = 0;
				Throw();

				///////////////////////////////

				m_dwDestroyTime = 0xffffffff;
				DestroyObject		();
//				NET_Packet			P;
//				u_EventGen			(P,GE_DESTROY,ID());
//				u_EventSend			(P);
			};
		};
	};
	inherited::OnH_B_Independent();
}

void CGrenade::OnH_A_Independent() 
{
	m_dwGrenadeIndependencyTime = Level().timeServer();
	inherited::OnH_A_Independent();	
}

void CGrenade::OnH_A_Chield()
{
	m_dwGrenadeIndependencyTime = 0;
	m_dwDestroyTime = 0xffffffff;
	inherited::OnH_A_Chield();
}

u32 CGrenade::State(u32 state) 
{
	switch (state)
	{
	case MS_THREATEN:
		{
			//#pragma todo("Oles to Yura: position can be under level, use 'Center' instead")
			Fvector		C;
			Center		(C);
			PlaySound	(sndCheckout,C);	// 
		}break;
	case MS_HIDDEN:
		{
			if (m_state == MS_END)
			{
//				Msg("MS_HIDDEN grenade");
				if (m_pPhysicsShell)
					m_pPhysicsShell->Deactivate();
				xr_delete(m_pPhysicsShell);

				m_dwDestroyTime = 0xffffffff;

				PutNextToSlot();

				if (Local())
				{
					DestroyObject		();
//					NET_Packet			P;
//					u_EventGen			(P,GE_DESTROY,ID());
//					u_EventSend			(P);
				};
			};
		}break;
	};
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

	CGrenade					*pGrenade = smart_cast<CGrenade*>(m_fake_missile);
	VERIFY						(pGrenade);
	
	if (pGrenade) {
//		pGrenade->m_dwDestroyTime = m_dwDestroyTimeMax + Level().timeServer();
		pGrenade->set_destroy_time(m_dwDestroyTimeMax);
		//установить ID того кто кинул гранату
		pGrenade->SetInitiator( H_Parent()->ID() );
	}
//	Msg("Twrow grenade id=[%d] time=%f",ID(),Device.fTimeGlobal);
	inherited::Throw			();
	m_fake_missile->processing_activate();//@sliph 
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
	return m_dwDestroyTime == 0xffffffff;
}

void CGrenade::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	CExplosive::OnEvent(P,type);
}

void CGrenade::PutNextToSlot	()
{
//	R_ASSERT(m_pInventory);
//	R_ASSERT(H_Parent());
	VERIFY	(!getDestroy());
	//выкинуть гранату из инвентаря
	if (m_pInventory)
	{
		m_pInventory->Ruck(this);
		m_pInventory->SetActiveSlot(NO_ACTIVE_SLOT);
	}
	else
		Msg ("! PutNextToSlot : m_pInventory = 0");	

	if(smart_cast<CActor*>(H_Parent()) && m_pInventory)
	{
		//найти такую же гранату и положить в рюкзак
		CGrenade *pNext = smart_cast<CGrenade*>(m_pInventory->Same(this,false));
		//или найти любую другую гранату на поясе
		if(!pNext) pNext = smart_cast<CGrenade*>(m_pInventory->SameSlot(m_slot,false));

		VERIFY(pNext != this);

		if(pNext)
			m_pInventory->Slot(pNext);
	}
	else
		Msg ("! PutNextToSlot : Parent = 0");
}

void CGrenade::OnAnimationEnd() 
{
	switch(inherited::State()){
	case MS_END: SwitchState(MS_HIDDEN);	break;
		default : inherited::OnAnimationEnd();
	}
}


void CGrenade::UpdateCL() 
{
	if (OnServer())
	{		
		if (m_constpower && CMissile::State() == MS_READY)
		{
//			m_throw = true; 
//			SwitchState(MS_THROW);
		}
	};
	////////////////////////////////////
	inherited::UpdateCL();
	
	CExplosive::UpdateCL();	
	////////////////////////////////////
	if(!IsGameTypeSingle())
		make_Interpolation();
}

void CGrenade::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
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
					//перебираем все предметы на поясе 
					//пока не встретим гарнату другого типа
					TIItemContainer::iterator it = m_pInventory->m_belt.begin();
					while(m_pInventory->m_belt.end() != it) 
					{
						CGrenade *pGrenade = smart_cast<CGrenade*>(*it);
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
//	case kDROP:
//		setup_throw_params();
//		return true;
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
	//CInventoryItemObject::reinit			();
}

void CGrenade::reload					(LPCSTR section)
{
	inherited::reload			(section);
	//CInventoryItemObject::reload			(section);
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

bool CGrenade::NeedToDestroyObject()	const
{
	if (GameID() == GAME_SINGLE) return false;
	if (Remote()) return false;
	if (TimePassedAfterIndependant() > m_dwGrenadeRemoveTime)
		return true;

	return false;
}

ALife::_TIME_ID	 CGrenade::TimePassedAfterIndependant()	const
{
	if(!H_Parent() && m_dwGrenadeIndependencyTime != 0)
		return Level().timeServer() - m_dwGrenadeIndependencyTime;
	else
		return 0;
}

BOOL CGrenade::UsedAI_Locations		()
{
#pragma todo("Dima to Yura : It crashes, because on net_Spawn object doesn't use AI locations, but on net_Destroy it does use them")
	return TRUE;//m_dwDestroyTime == 0xffffffff;
}

void CGrenade::net_Relcase(CObject* O )
{
	CExplosive::net_Relcase(O);
	inherited::net_Relcase(O);
}

