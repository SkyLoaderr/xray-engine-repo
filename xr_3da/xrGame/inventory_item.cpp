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
#include "xrserver_objects_alife.h"
#include "xrserver_objects_alife_items.h"

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

	m_HitTypeK.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;

	m_iGridWidth	= 1;
	m_iGridHeight	= 1;

	m_iXPos = 0;
	m_iYPos = 0;
	////////////////////////////////////	
	m_inventory_mask = u64(-1);

	m_name = m_nameShort = NULL;

	m_dwFrameLoad		= u32(-1);
	m_dwFrameReload		= u32(-1);
	m_dwFrameReinit		= u32(-1);
	m_dwFrameSpawn		= u32(-1);
	m_dwFrameDestroy	= u32(-1);
	m_dwFrameClient		= u32(-1);
}

CInventoryItem::~CInventoryItem() 
{
}

void CInventoryItem::Load(LPCSTR section) 
{
	if (!frame_check(m_dwFrameLoad))
		return;

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
	clamp(m_fCondition, 0.f, 1.f);
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
	hit_power *= m_HitTypeK[hit_type];

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

bool CInventoryItem::Useful() const
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
}

void CInventoryItem::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
}

void CInventoryItem::OnH_A_Chield		()
{
	inherited::OnH_A_Chield		();
	if (m_pInventory)
		BuildInventoryMask		(m_pInventory);
}

void CInventoryItem::UpdateCL()
{
	if (!frame_check(m_dwFrameClient))
		return;

	inherited::UpdateCL();
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
BOOL CInventoryItem::net_Spawn			(LPVOID DC)
{
	VERIFY(!m_pInventory);

	if (!frame_check(m_dwFrameSpawn))
		return	(TRUE);

	BOOL res = inherited::net_Spawn(DC);

	m_bHasUpdate = false;
	m_bInInterpolation = false;
	m_bInterpolate	= false;

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeInventoryItem			*pSE_InventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(e);
	if(!pSE_InventoryItem) return res;
	
	m_fCondition = pSE_InventoryItem->m_fCondition;

	return		res;
}

void CInventoryItem::net_Destroy		()
{
	if (!frame_check(m_dwFrameDestroy))
		return;

	//��������� �������� �� ������������
	m_pInventory = NULL;

	inherited::net_Destroy	();
}

void CInventoryItem::net_Import			(NET_Packet& P) 
{	
	P.r_float				(m_fCondition);

	net_update_IItem			N;
	P.r_u32					( N.dwTimeStamp );


	u16	NumItems = 0;
	P.r_u16					( NumItems);
	P.r_vec3				( N.State.position);

	if (!NumItems) return;

	P.r_u8					( *((u8*)&(N.State.enabled)) );

	P.r_vec3				( N.State.angular_vel);
	P.r_vec3				( N.State.linear_vel);

	P.r_vec3				( N.State.force);
	P.r_vec3				( N.State.torque);


	P.r_float				( N.State.quaternion.x );
	P.r_float				( N.State.quaternion.y );
	P.r_float				( N.State.quaternion.z );
	P.r_float				( N.State.quaternion.w );

		

	N.State.previous_position	= N.State.position;
	N.State.previous_quaternion = N.State.quaternion;

	if (!NET_IItem.empty() && (NET_IItem.back().dwTimeStamp>=N.dwTimeStamp)) return;

	if (!NET_IItem.empty()) m_bInterpolate = true;

	long dTime = Level().timeServer() - N.dwTimeStamp;
	u32 NumSteps = 0;
	if (dTime < (fixed_step*500))
		NumSteps = 0;
	else
		NumSteps = ph_world->CalcNumSteps(dTime);

	m_bHasUpdate = true;
	Level().SetNumCrSteps(NumSteps);

	NET_IItem.push_back			(N);
	while (NET_IItem.size() > 2)
	{
		NET_IItem.pop_front();
	};
};

void CInventoryItem::net_Export			(NET_Packet& P) 
{	
	
//	inherited::net_Export(P);
	P.w_float			(m_fCondition);
	P.w_u32				(Level().timeServer());	

	///////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	SPHNetState	State;
	pSyncObj = PHGetSyncItem(0);
	if (pSyncObj && !H_Parent()) 
		pSyncObj->get_State(State);
	else 	
		State.position.set(Position());
	///////////////////////////////////////	
	u16 NumItems = PHGetSyncItemsNumber();
	if (H_Parent() || GameID() == 1) NumItems = 0;

	P.w_u16				(NumItems);
	P.w_vec3			( State.position);

	if (!NumItems) return;

	P.w_u8					( State.enabled );

	P.w_vec3				( State.angular_vel);
	P.w_vec3				( State.linear_vel);

	P.w_vec3				( State.force);
	P.w_vec3				( State.torque);


	P.w_float				( State.quaternion.x );
	P.w_float				( State.quaternion.y );
	P.w_float				( State.quaternion.z );
	P.w_float				( State.quaternion.w );

};

void CInventoryItem::PH_B_CrPr		()
{
	//just set last update data for now
	if (!m_bHasUpdate) return;	
	///////////////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	///////////////////////////////////////////////
	IStartPos.set(Position());
	IStartRot.set(XFORM());

	m_bInInterpolation = false;
	net_update_IItem N_I = NET_IItem.back();
	if (!N_I.State.enabled) 
	{
		pSyncObj->set_State(N_I.State);//, N_A.State.enabled);
	}
	else
	{
		PHUnFreeze();

		pSyncObj->set_State(N_I.State);//, N_A.State.enabled);
	};
	///////////////////////////////////////////////
	if (Level().InterpolationDisabled())
	{
		m_bHasUpdate = false;
		m_bInInterpolation = false;
	};
	///////////////////////////////////////////////
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
	pSyncObj->get_State(PredictedState);
	////////////////////////////////////
	pSyncObj->set_State(RecalculatedState);
	////////////////////////////////////

	if (!m_bInterpolate) return;

	Fmatrix xformX;
	pSyncObj->cv2obj_Xfrom(PredictedState.previous_quaternion, PredictedState.previous_position, xformX);

	IEndRot.set(xformX);
	IEndPos.set(xformX.c);

	m_bInInterpolation = true;
	m_dwIStartTime = m_dwILastUpdateTime;
	m_dwIEndTime = Level().timeServer() + u32((fixed_step - ph_world->m_frame_time)*1000)+ Level().GetInterpolationSteps()*u32(fixed_step*1000);
};

void CInventoryItem::make_Interpolation	()
{
	m_dwILastUpdateTime = Level().timeServer();
	if (!m_bInInterpolation) return;
	
	if(!H_Parent() && getVisible() && m_pPhysicsShell) {
		u32 CurTime = Level().timeServer();
		if (CurTime >= m_dwIEndTime) {
			m_bInInterpolation = false;

			CPHSynchronize* pSyncObj = NULL;
			pSyncObj = PHGetSyncItem(0);
			pSyncObj->set_State(PredictedState);//, PredictedState.enabled);
//			Position().set(PredictedState.position);
			Fmatrix xformI;
			pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, PredictedState.position, xformI);
			XFORM().set(xformI);
		}
		else {
			VERIFY			(CurTime <= m_dwIEndTime);
			float factor	= float(CurTime - m_dwIStartTime)/(m_dwIEndTime - m_dwIStartTime);
			if (factor > 1) factor = 1.0f;
			else if (factor < 0) factor = 0;

			Fvector IPos;
			Fquaternion IRot;
			IPos.lerp(IStartPos, IEndPos, factor);
			IRot.slerp(IStartRot, IEndRot, factor);

			XFORM().rotation(IRot);
			Position().set(IPos);
			
		};
		/*
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
		*/
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

void CInventoryItem::renderable_Render	()
{
	inherited::renderable_Render();
}

void CInventoryItem::reload		(LPCSTR section)
{
	if (!frame_check(m_dwFrameReload))
		return;

	inherited::reload	(section);
}

void CInventoryItem::reinit		()
{
	if (!frame_check(m_dwFrameReload))
		return;

	m_pInventory		= 0;

	inherited::reinit	();
}
