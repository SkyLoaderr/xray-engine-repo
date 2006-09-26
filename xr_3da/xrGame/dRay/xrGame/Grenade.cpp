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

	HUD_SOUND::LoadSound(section,"snd_checkout",sndCheckout,m_eSoundCheckout);

	//////////////////////////////////////
	//����� �������� ������ � ������
	if(pSettings->line_exist(section,"grenade_remove_time"))
		m_dwGrenadeRemoveTime = pSettings->r_u32(section,"grenade_remove_time");
	else
		m_dwGrenadeRemoveTime = GRENADE_REMOVE_TIME;
	m_grenade_detonation_threshold_hit=READ_IF_EXISTS(pSettings,r_float,section,"detonation_threshold_hit",default_grenade_detonation_threshold_hit);
}

void CGrenade::Hit					(SHit* pHDS)
{
	if( ALife::eHitTypeExplosion==pHDS->hit_type && m_grenade_detonation_threshold_hit<pHDS->damage()&&CExplosive::Initiator()==u16(-1)) 
	{
		CExplosive::SetCurrentParentID(pHDS->who->ID());
		Destroy();
	}
	inherited::Hit(pHDS);
}

BOOL CGrenade::net_Spawn(CSE_Abstract* DC) 
{
	m_dwGrenadeIndependencyTime			= 0;
	BOOL ret= inherited::net_Spawn		(DC);
	Fvector box;BoundingBox().getsize	(box);
	float max_size						=max(max(box.x,box.y),box.z);
	box.set								(max_size,max_size,max_size);
	box.mul								(3.f);
	CExplosive::SetExplosionSize		(box);
	m_thrown							= false;
	return								ret;
}

void CGrenade::net_Destroy() 
{
	inherited::net_Destroy				();
	CExplosive::net_Destroy				();
}

void CGrenade::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CGrenade::OnH_A_Independent() 
{
	m_dwGrenadeIndependencyTime			= Level().timeServer();
	inherited::OnH_A_Independent		();	
}

void CGrenade::OnH_A_Chield()
{
	m_dwGrenadeIndependencyTime			= 0;
	m_dwDestroyTime						= 0xffffffff;
	inherited::OnH_A_Chield				();
}

void CGrenade::State(u32 state) 
{
	switch (state)
	{
	case MS_THREATEN:
		{
			Fvector						C;
			Center						(C);
			PlaySound					(sndCheckout,C);
		}break;
	case MS_HIDDEN:
		{
			if(m_thrown)
			{
				if (m_pPhysicsShell)	m_pPhysicsShell->Deactivate();
				xr_delete				(m_pPhysicsShell);
				m_dwDestroyTime			= 0xffffffff;
				PutNextToSlot			();
				if (Local())			DestroyObject		();
				
			};
		}break;
	};
	inherited::State(state);
}
/*
bool CGrenade::Activate() 
{
	Show								();
	return								true;
}

void CGrenade::Deactivate() 
{
	Hide								();
}
*/
void CGrenade::Throw() 
{
	if (!m_fake_missile)
		return;

	CGrenade					*pGrenade = smart_cast<CGrenade*>(m_fake_missile);
	VERIFY						(pGrenade);
	
	if (pGrenade) {
		pGrenade->set_destroy_time(m_dwDestroyTimeMax);
		//���������� ID ���� ��� ����� �������
		pGrenade->SetInitiator( H_Parent()->ID() );
	}
	inherited::Throw			();
	m_fake_missile->processing_activate();//@sliph
	m_thrown = true;
}



void CGrenade::Destroy() 
{
	//Generate Expode event
	Fvector						normal;
	FindNormal					(normal);
	CExplosive::GenExplodeEvent	(Position(), normal);
}



bool CGrenade::Useful() const
{
	return m_dwDestroyTime == 0xffffffff;
}

void CGrenade::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent			(P,type);
	CExplosive::OnEvent			(P,type);
}

void CGrenade::PutNextToSlot()
{
	if (OnClient()) return;
//	Msg ("* PutNextToSlot : %d", ID());	
	VERIFY									(!getDestroy());
	//�������� ������� �� ���������
	NET_Packet						P;
	if (m_pInventory)
	{
		m_pInventory->Ruck					(this);
//.		m_pInventory->SetActiveSlot			(NO_ACTIVE_SLOT);

		this->u_EventGen				(P, GEG_PLAYER_ITEM2RUCK, this->H_Parent()->ID());
		P.w_u16							(this->ID());
		this->u_EventSend				(P);
	}
	else
		Msg ("! PutNextToSlot : m_pInventory = 0");	

	if (smart_cast<CInventoryOwner*>(H_Parent()) && m_pInventory)
	{
		CGrenade *pNext						= smart_cast<CGrenade*>(	m_pInventory->Same(this,true)		);
		if(!pNext) pNext					= smart_cast<CGrenade*>(	m_pInventory->SameSlot(this,true)	);

		VERIFY								(pNext != this);

		if(pNext && m_pInventory->Slot(pNext) ){

			pNext->u_EventGen				(P, GEG_PLAYER_ITEM2SLOT, pNext->H_Parent()->ID());
			P.w_u16							(pNext->ID());
			pNext->u_EventSend				(P);
//			if(IsGameTypeSingle())			
				m_pInventory->SetActiveSlot			(pNext->GetSlot());
		}

		m_thrown				= false;
	}
}

void CGrenade::OnAnimationEnd(u32 state) 
{
	switch(state){
	case MS_END: SwitchState(MS_HIDDEN);	break;
//.	case MS_END: SwitchState(MS_RESTORE);	break;
		default : inherited::OnAnimationEnd(state);
	}
}


void CGrenade::UpdateCL() 
{
	inherited::UpdateCL			();
	CExplosive::UpdateCL		();

	if(!IsGameTypeSingle())	make_Interpolation();
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
					TIItemContainer::iterator it = m_pInventory->m_ruck.begin();
					TIItemContainer::iterator it_e = m_pInventory->m_ruck.end();
					for(;it!=it_e;++it) 
					{
						CGrenade *pGrenade = smart_cast<CGrenade*>(*it);
						if(pGrenade && xr_strcmp(pGrenade->cNameSect(), cNameSect())) 
						{
							m_pInventory->Ruck(this);
							m_pInventory->SetActiveSlot(NO_ACTIVE_SLOT);
							m_pInventory->Slot(pGrenade);
							return true;
						}
					}
					return true;
				}
			}
			return true;
		};
	}
	return false;
}


bool CGrenade::NeedToDestroyObject()	const
{
	if ( IsGameTypeSingle()			) return false;
	if ( Remote()					) return false;
	if ( TimePassedAfterIndependant() > m_dwGrenadeRemoveTime)
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

void			CGrenade::Deactivate			()
{
	//Drop grenade if primed
	m_pHUD->StopCurrentAnimWithoutCallback();
	if (Local() && (GetState() == MS_THREATEN || GetState() == MS_READY || GetState() == MS_THROW))
	{
		if (m_fake_missile)
		{
			CGrenade*		pGrenade	= smart_cast<CGrenade*>(m_fake_missile);
			if (pGrenade)
			{
				if (m_pInventory->GetOwner())
				{
					CActor* pActor = smart_cast<CActor*>(m_pInventory->GetOwner());
					if (pActor)
					{
						if (!pActor->g_Alive())
						{
							m_constpower			= false;
							m_fThrowForce			= 0;
						}
					}
					
				}				
				Throw					();
				m_dwDestroyTime			= 0xffffffff;
			};
		};
	};

	inherited::Deactivate();
}
