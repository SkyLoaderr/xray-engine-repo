#include "stdafx.h"
#include "grenade.h"
#include "PhysicsShell.h"
#include "WeaponHUD.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"
#include "Physics.h"
#include "HUDManager.h"

CGrenade::CGrenade(void) 
{
	m_pFake = NULL;
}

CGrenade::~CGrenade(void) 
{
}

void CGrenade::Load(LPCSTR section) 
{
	inherited::Load(section);

	CExplosive::Load(section);
}

BOOL CGrenade::net_Spawn(LPVOID DC) 
{
	CExplosive::net_Spawn(DC);
	return inherited::net_Spawn	(DC);
}

void CGrenade::net_Destroy() 
{
	CExplosive::net_Destroy();
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
	// Если IItem еще не полностью использованый, вернуть true
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
			
			//выкинуть гранату из инвентаря
			m_pInventory->Ruck(this); 
			m_destroyTime = 0;
			
			if (Local())
			{
				NET_Packet P;
				u_EventGen(P, GE_OWNERSHIP_REJECT, H_Parent()->ID());
				P.w_u16(u16(ID()));
				u_EventSend(P);
			};

			//найти такую же гранату и положить в рюкзак
			CGrenade *pNext = dynamic_cast<CGrenade*>(m_pInventory->Same(this));
			//или найти любую другую гранату на поясе
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
	if (Remote() && NET.size()>1)
	{
		net_update N = NET.back();
		NET_Last = N;
		N = *(NET.end()-2);

//		XFORM().setHPB(NET_Last.angles.x, NET_Last.angles.y, NET_Last.angles.z);
//		Position().set(NET_Last.pos);

		u16 NumItems = PHGetSyncItemsNumber();

		CPHSynchronize* pSyncObj = NULL;
		SPHNetState	State;

		for (u16 i=0; i<NumItems; i++)
		{
			pSyncObj = PHGetSyncItem(i);
			if (!pSyncObj) continue;

			pSyncObj->get_State(State);
 
			State.angular_vel			= NET_Last.State.angular_vel;
			State.linear_vel			= NET_Last.State.linear_vel;
			State.enabled				= true;//(!N.State.enabled && !NET_Last.State.enabled) ? false : true;

			State.force					= NET_Last.State.force;
			State.torque				= NET_Last.State.torque;
			
			State.position				= NET_Last.State.position;
			State.previous_position		= NET_Last.State.position;

			State.quaternion			= NET_Last.State.quaternion;
			State.previous_quaternion	= NET_Last.State.quaternion;
			
			pSyncObj->set_State(NET_Last.State);

			State.enabled				= NET_Last.State.enabled;
			pSyncObj->set_State(NET_Last.State);

			HUD().outMessage		(0xffffffff,"client","Physics C desync: %d steps A desync %d steps",
										 NET_Last.CurPhStep	- LastCPhStep,
										 NET_Last.RPhStep	- LastAPhStep);

			LastCPhStep = NET_Last.CurPhStep;
			LastAPhStep = NET_Last.RPhStep;
		};

		while (NET.size() > 1)
		{
			NET.pop_front();
		};
	};
	inherited::UpdateCL();
	
	CExplosive::UpdateCL();
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

					//ищем на поясе такую же гранату
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

static DWORD DTime = 0;
void CGrenade::net_Import			(NET_Packet& P) 
{
	net_update			N;

	P.r_u32					( N.dwTimeStamp );
	return;
	N.CurPhStep				= ph_world->m_steps_num;
	P.r_u64					(N.RPhStep);

	P.r_vec3				( N.State.angular_vel);
	P.r_vec3				( N.State.linear_vel);

	P.r_vec3				( N.State.force);
	P.r_vec3				( N.State.torque);

	P.r_vec3				( N.State.position);
	P.r_vec3				( N.State.previous_position);

	P.r_float				( N.State.quaternion.x );
	P.r_float				( N.State.quaternion.y );
	P.r_float				( N.State.quaternion.z );
	P.r_float				( N.State.quaternion.w );

	P.r_float				( N.State.previous_quaternion.x );
	P.r_float				( N.State.previous_quaternion.y );
	P.r_float				( N.State.previous_quaternion.z );
	P.r_float				( N.State.previous_quaternion.w );

	P.r_u8					( *((u8*)&(N.State.enabled)) );

	if (NET.empty() || (NET.back().dwTimeStamp+DTime<N.dwTimeStamp))	
	{
		NET.push_back			(N);
	}
};

void CGrenade::net_Export			(NET_Packet& P) 
{
	P.w_u32				(Level().timeServer());
	return;
	P.w_u64				(ph_world->m_steps_num);
//	P.w_vec3			(Position()	);

//	float					_x,_y,_z;
//	XFORM().getHPB			(_x,_y,_z);

	u16 NumItems = PHGetSyncItemsNumber();

	CPHSynchronize* pSyncObj = NULL;
	SPHNetState	State;

	for (u16 i=0; i<NumItems; i++)
	{
		pSyncObj = PHGetSyncItem(i);
		if (!pSyncObj) continue;
		pSyncObj->get_State(State);

		P.w_vec3				( State.angular_vel);
		P.w_vec3				( State.linear_vel);

		P.w_vec3				( State.force);
		P.w_vec3				( State.torque);

		P.w_vec3				( State.position);
		P.w_vec3				( State.previous_position);

		P.w_float				( State.quaternion.x );
		P.w_float				( State.quaternion.y );
		P.w_float				( State.quaternion.z );
		P.w_float				( State.quaternion.w );

		P.w_float				( State.previous_quaternion.x );
		P.w_float				( State.previous_quaternion.y );
		P.w_float				( State.previous_quaternion.z );
		P.w_float				( State.previous_quaternion.w );

		P.w_u8					( State.enabled );
	};
};