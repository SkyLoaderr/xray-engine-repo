////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item.cpp
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "inventory_item.h"
#include "inventory.h"
#include "Physics.h"

CInventoryItem::CInventoryItem() 
{
	m_weight = 100.f;
	m_slot = NO_ACTIVE_SLOT;
	m_belt = false;
	m_pInventory = NULL;
	m_drop = false;
	m_ruck = true;

	m_fCondition = 1.0f;
	m_bUsingCondition = false;

	m_fK_Burn = 1.0f;
	m_fK_Strike = 1.0f;
	m_fK_Wound = 1.0f;
	m_fK_Radiation = 1.0f;
	m_fK_Telepatic = 1.0f;
	m_fK_Shock = 1.0f;
	m_fK_ChemicalBurn = 1.0f;
	m_fK_Explosion = 1.0f;
	m_fK_FireWound = 1.0f;


	m_iGridWidth	= 1;
	m_iGridHeight	= 1;

	m_iXPos = 0;
	m_iYPos = 0;
	////////////////////////////////////
	m_bHasUpdate = false;
	m_bInInterpolation = false;
	m_inventory_mask = u64(-1);
}

CInventoryItem::~CInventoryItem() 
{
}

void CInventoryItem::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_name = pSettings->r_string(section, "inv_name");
	m_nameShort = pSettings->r_string(section, "inv_name_short");
	NameComplex();
	m_weight = pSettings->r_float(section, "inv_weight");

	m_cost = pSettings->r_u32(section, "cost");

	//properties used by inventory menu
	m_iGridWidth	= pSettings->r_u32(section, "inv_grid_width");
	m_iGridHeight	= pSettings->r_u32(section, "inv_grid_height");

	m_iXPos			= pSettings->r_u32(section, "inv_grid_x");
	m_iYPos			= pSettings->r_u32(section, "inv_grid_y");

	if(pSettings->line_exist(section, "slot"))
		m_slot = pSettings->r_u32(section, "slot");
	else
		m_slot = NO_ACTIVE_SLOT;
}

void  CInventoryItem::ChangeCondition(float fDeltaCondition)
{
	m_fCondition += fDeltaCondition;

	if(m_fCondition <0.0f) m_fCondition = 0.0f;
	else if(m_fCondition >1.0f) m_fCondition = 1.0f;
}

void CInventoryItem::Hit(float P, Fvector &dir,	
						 CObject* who, s16 element,
						 Fvector position_in_object_space, 
						 float impulse, 
						 ALife::EHitType hit_type)
{
	inherited::Hit(P, dir, who, element, position_in_object_space, 
					  impulse, hit_type);


	if(!m_bUsingCondition) return;

	float hit_power = P/100.f;

	switch(hit_type)
	{
	case eHitTypeBurn:
		hit_power *= m_fK_Burn;
		break;
	case eHitTypeChemicalBurn:
		hit_power *= m_fK_ChemicalBurn;
		break;
	case eHitTypeStrike:
		hit_power *= m_fK_Strike;
		break;
	case eHitTypeTelepatic:
		hit_power *= m_fK_Telepatic;
		break;
	case eHitTypeShock:
		hit_power *= m_fK_Shock;
		break;
	case eHitTypeExplosion:
		hit_power *= m_fK_Explosion;
		break;
	case eHitTypeFireWound:
		hit_power *= m_fK_FireWound;
		break;
	case eHitTypeWound:
		hit_power *= m_fK_Wound;
		break;
	}

	ChangeCondition(-hit_power);
}

const char* CInventoryItem::Name() 
{
	return m_name;
}

const char* CInventoryItem::NameShort() 
{
	return m_nameShort;
}

LPCSTR CInventoryItem::NameComplex() 
{
	const char *l_name = Name();
	if(l_name) 
		strcpy(m_nameComplex, l_name); 
	else 
		m_nameComplex[0] = 0;

	/*for(PPIItem l_it = m_subs.begin(); m_subs.end() != l_it; ++l_it) 
	{
		const char *l_subName = (*l_it)->NameShort();
		if(l_subName)
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], l_subName);
	}*/


	if(m_bUsingCondition)
	{
		/*if(GetCondition()<0.33)
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], " bad");
		else if(GetCondition()<0.66)
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], " good");
		else
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], " excelent");
		*/
		sprintf(&m_nameComplex[xr_strlen(m_nameComplex)]," %f",GetCondition());
	}


	return m_nameComplex;
}

bool CInventoryItem::Useful() 
{
	// ���� IItem ������ ������������, ������� false
	return true;
}
/*
bool CInventoryItem::Attach(PIItem pIItem, bool force) 
{
	// �������� force ������ ����� false
	// ��������� ������ �������� ��� �� true
	// ���� ������ IItem ����� ���� � ���� �����������,
	// � ������� return CInventoryItem::Attach(pIItem, force);
	if(force) 
	{
		m_subs.insert(m_subs.end(), pIItem);
		return true;
	} 
	else 
		return false;
}*/
/*
bool CInventoryItem::Detach(PIItem pIItem, bool force) 
{
	// �������� force ������ ����� true
	// ��������� ������ �������� ��� �� false
	// ���� ������ IItem �� ����� ���� ����������,
	// � ������� return CInventoryItem::Detach(pIItem, force);
	if(force) 
	{
		if(m_subs.erase(std::find(m_subs.begin(), m_subs.end(), pIItem)) != m_subs.end()) 
		{
			return true;
		}
		return false;
	}
	else return false; 
}*/

bool CInventoryItem::DetachAll()
{
	if(!m_pInventory || std::find(m_pInventory->m_ruck.begin(), 
								    m_pInventory->m_ruck.end(), this) == 
									m_pInventory->m_ruck.end()) return false;

/*	for(PPIItem it = m_subs.begin(); m_subs.end() != it; ++it) 
	{
		Detach(*it);
		//m_pInventory->m_ruck.insert(m_pInventory->m_ruck.end(), *l_it);
		(*it)->DetachAll();
	}*/
	return true;
}

bool CInventoryItem::Activate() 
{
	// ���������� ��� ��������� ����� � ������� ��������� ������
	// ���� ������ ����� ���� ����������� ������� true, ����� false
	return false;
}

void CInventoryItem::Deactivate() 
{
	// ���������� ��� ����������� ����� � ������� ��������� ������
}

void CInventoryItem::Drop() 
{
	if(m_pInventory) m_drop = true;
}

s32 CInventoryItem::Sort(PIItem /**pIItem/**/) 
{
	// ���� ����� ���������� IItem ����� this - ������� 1, ����
	// ����� - -1. ���� ����� �� 0.
	return 0;
}

bool CInventoryItem::Merge(PIItem /**pIItem/**/) 
{
	// ���� ������� ����� ������ ������� true
	return false;
}

void CInventoryItem::OnH_B_Independent	()
{
	inherited::OnH_B_Independent();

/*
	if(m_pPhysicsShell) 
	{
		Fvector l_fw; l_fw.set(XFORM().k); l_fw.mul(2.f);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM());
		l_p2.set(XFORM()); l_fw.mul(2.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
	}*/
}

void CInventoryItem::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	/*
	setVisible					(false);
	setEnabled					(false);
	if (m_pPhysicsShell)		m_pPhysicsShell->Deactivate	();

	if(Local()) OnStateSwitch(eHiding);


	if (Local() && (0xffff!=respawnPhantom)) 
	{
	NET_Packet		P;
	u_EventGen		(P,GE_RESPAWN,respawnPhantom);
	u_EventSend		(P);
	respawnPhantom	= 0xffff;
	}
	*/
}

void CInventoryItem::OnH_A_Chield		()
{
	inherited::OnH_A_Chield		();
	if (m_pInventory)
		BuildInventoryMask		(m_pInventory);
	/*
	setVisible					(false);
	setEnabled					(false);
	if (m_pPhysicsShell)		m_pPhysicsShell->Deactivate	();

	if(Local()) OnStateSwitch(eHiding);


	if (Local() && (0xffff!=respawnPhantom)) 
	{
	NET_Packet		P;
	u_EventGen		(P,GE_RESPAWN,respawnPhantom);
	u_EventSend		(P);
	respawnPhantom	= 0xffff;
	}
	*/
}

void CInventoryItem::UpdateCL()
{
	inherited::UpdateCL();

/*	if (0==H_Parent() && m_pPhysicsShell)		
	{
		m_pPhysicsShell->Update	();
		XFORM().set			(m_pPhysicsShell->mXFORM);
		XFORM().set			(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}*/
}


void CInventoryItem::OnEvent (NET_Packet& P, u16 type)
{
	inherited::OnEvent(P, type);
}

//������� ������������ ���� ����������� � ������ ����� ���� 
//� ��������� � ��������� ��������������� ������ � ������������
//�������, ������� ������� ������ ���� ��������������
bool CInventoryItem::Detach(const char* item_section_name) 
{
	CSE_Abstract*		D	= F_entity_Create(item_section_name);
	R_ASSERT		   (D);
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = 
							 dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT			(l_tpALifeDynamicObject);
	
	l_tpALifeDynamicObject->m_tNodeID = this->level_vertex_id();
		
	// Fill
	strcpy				(D->s_name, item_section_name);
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	u16(this->H_Parent()->ID());
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	Position();
	D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);

	return true;
}

/////////// network ///////////////////////////////
void CInventoryItem::net_Import			(NET_Packet& P) 
{	
	net_update_IItem			N;

	P.r_u32					( N.dwTimeStamp );

	u16	NumItems = 0;
	P.r_u16					( NumItems);
	if (!NumItems) return;

	P.r_u8					( *((u8*)&(N.State.enabled)) );

	P.r_vec3				( N.State.angular_vel);
	P.r_vec3				( N.State.linear_vel);

	P.r_vec3				( N.State.force);
	P.r_vec3				( N.State.torque);

	P.r_vec3				( N.State.position);

	P.r_float				( N.State.quaternion.x );
	P.r_float				( N.State.quaternion.y );
	P.r_float				( N.State.quaternion.z );
	P.r_float				( N.State.quaternion.w );

	N.State.previous_position	= N.State.position;
	N.State.previous_quaternion = N.State.quaternion;

	if (NET_IItem.empty() || (NET_IItem.back().dwTimeStamp<N.dwTimeStamp))
	{
		long dTime = Level().timeServer() - N.dwTimeStamp;
		if (dTime < (fixed_step*500)) return;
		u32 NumSteps = ph_world->CalcNumSteps(dTime);

		m_bHasUpdate = true;
		Level().m_bNeed_CrPr = true;
		Level().m_dwNumSteps = NumSteps;

		NET_IItem.push_back			(N);
	}
};

void CInventoryItem::net_Export			(NET_Packet& P) 
{	
	
//	inherited::net_Export(P);

	P.w_u32				(Level().timeServer());	
	
	u16 NumItems = PHGetSyncItemsNumber();
	if (H_Parent() || GameID() == 1) NumItems = 0;
//	if (H_Parent()) NumItems = 0;

	P.w_u16				(NumItems);
	if (!NumItems) return;

//	Msg("CInventoryItem net_export %s - Items %d", cName(), NumItems);

	CPHSynchronize* pSyncObj = NULL;
	SPHNetState	State;

	for (u16 i=0; i<NumItems; i++)
	{
		pSyncObj = PHGetSyncItem(i);
		if (!pSyncObj) continue;
		pSyncObj->get_State(State);

		P.w_u8					( State.enabled );

		P.w_vec3				( State.angular_vel);
		P.w_vec3				( State.linear_vel);

		P.w_vec3				( State.force);
		P.w_vec3				( State.torque);

		P.w_vec3				( State.position);

		P.w_float				( State.quaternion.x );
		P.w_float				( State.quaternion.y );
		P.w_float				( State.quaternion.z );
		P.w_float				( State.quaternion.w );
	};
};

void CInventoryItem::PH_B_CrPr		()
{
	//just set last update data for now
//	u32 CurTime = Level().timeServer();
	if (m_bHasUpdate)
	{
		m_bInInterpolation = false;
		///////////////////////////////////////////////
		CPHSynchronize* pSyncObj = NULL;
		pSyncObj = PHGetSyncItem(0);
		if (!pSyncObj) return;
		///////////////////////////////////////////////
		PHUnFreeze();
		///////////////////////////////////////////////
		while (NET_IItem.size() > 2)
		{
			NET_IItem.pop_front();
		};

		net_update_IItem N = NET_IItem.back();
		///////////////////////////////////////////////
		IStartPos.set(Position());
		IStartRot.set(XFORM());
		///////////////////////////////////////////////
		pSyncObj->set_State(N.State);
		if (!N.State.enabled) PHFreeze();
		///////////////////////////////////////////////
		if (int(Level().InterpolationSteps()) < 0)
		{
			m_bHasUpdate = false;
			m_bInInterpolation = false;
		};
	};
};	


void CInventoryItem::PH_I_CrPr		()		// actions & operations between two phisic prediction steps
{
	//store recalculated data, then we able to restore it after small future prediction
	if (!m_bHasUpdate) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	pSyncObj->get_State(RecalculatedState);
	///////////////////////////////////////////////
	if (!getVisible())
	{
		m_bHasUpdate = false;
		
		PHFreeze();
	};
}; 

void CInventoryItem::PH_A_CrPr		()
{
	//restore recalculated data and get data for interpolation	
	if (!m_bHasUpdate) return;
	m_bHasUpdate = false;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	SPHNetState		PredictedState;
	pSyncObj->get_State(PredictedState);
	
	Fmatrix xformX;
	pSyncObj->cv2obj_Xfrom(PredictedState.previous_quaternion, PredictedState.previous_position, xformX);

	IEndRot.set(xformX);
	IEndPos.set(xformX.c);

	m_bInInterpolation = true;
	m_dwIStartTime = Level().timeServer();
	m_dwIEndTime = m_dwIStartTime + Level().InterpolationSteps()*u32(fixed_step*1000);
	m_u64IEndStep = ph_world->m_steps_num + Level().InterpolationSteps();
	////////////////////////////////////
	pSyncObj->set_State(RecalculatedState);
};

void CInventoryItem::make_Interpolation	()
{
	if (!m_bInInterpolation) return;

	u32 CurrentTime = Level().timeServer();
	if(!H_Parent() && getVisible() && m_pPhysicsShell) 
	{		
		if (ph_world->m_steps_num > m_u64IEndStep)
		{
			m_bInInterpolation = false;
		}
		else
		{
			Fvector IPos;
			Fquaternion IRot;

			if (ph_world->m_steps_num < m_u64IEndStep)
			{
				float f = float(CurrentTime - m_dwIStartTime)/(m_dwIEndTime - m_dwIStartTime);
				if (f < 0 || f > 1)
				{
					if (f < 0) f = 0.0f;
					else f = 1.0f;
				}

				IPos.lerp(IStartPos, IEndPos, f);
				IRot.slerp(IStartRot, IEndRot, f);
			}
			else
			{
				CPHSynchronize* pSyncObj = NULL;
				pSyncObj = PHGetSyncItem(0);
				if (!pSyncObj) return;
				SPHNetState	State;
				pSyncObj->get_State(State);

				Fmatrix xformI;
				pSyncObj->cv2obj_Xfrom(State.quaternion, State.position, xformI);

				Fvector tmpP;
				Fquaternion tmpR;

				tmpR.set(xformI);
				tmpP.set(xformI.c);

				IPos.lerp(IEndPos, tmpP, ph_world->m_frame_time/fixed_step);
				IRot.slerp(IEndRot, tmpR, ph_world->m_frame_time/fixed_step);
			}

			XFORM().rotation(IRot);
			Position().set(IPos);
		}
	}
}

void CInventoryItem::BuildInventoryMask	(const CInventory	*inventory)
{
	R_ASSERT					(inventory);
	R_ASSERT2					((GetWidth() <= (int)inventory->RuckWidth()) && (GetHeight() <= (int)inventory->RuckHeight()),"Invalid inventory grid sizes");
	m_inventory_mask			= 0;
	for (int i=0; i<GetHeight(); ++i)
		m_inventory_mask		|= ((u64(1) << GetWidth()) - 1) << (i*inventory->RuckWidth());
}
