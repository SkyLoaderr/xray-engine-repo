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
#include "entity_alive.h"
#include "level.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "string_table.h"

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
#ifdef DEBUG
	LastVisPos.clear();
#endif
	m_eItemPlace = eItemPlaceUndefined;
	m_Description = "";
}

CInventoryItem::~CInventoryItem() 
{
#ifdef DEBUG
	LastVisPos.clear();
#endif
}

void CInventoryItem::Load(LPCSTR section) 
{
	if (!frame_check(m_dwFrameLoad))
		return;

	inherited::Load	(section);

	ISpatial*		self				=	smart_cast<ISpatial*> (this);
	if (self)		self->spatial.type	|=	STYPE_VISIBLEFORAI;	

	m_name = pSettings->r_string_wb(section, "inv_name");
	m_nameShort = pSettings->r_string_wb(section, "inv_name_short");
	NameComplex();
	m_weight = pSettings->r_float(section, "inv_weight");
	R_ASSERT(m_weight>=0.f);

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

	// Description
	if (pSettings->line_exist(section, "description"))
		m_Description = CStringTable()(pSettings->r_string(section, "description"));
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
	return *m_name;
}

const char* CInventoryItem::NameShort() 
{
	return *m_nameShort;
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
	// Если IItem нельзя использовать, вернуть false
	return true;
}

bool CInventoryItem::Activate() 
{
	// Вызывается при активации слота в котором находится объект
	// Если объект может быть активирован вернуть true, иначе false
	return false;
}

void CInventoryItem::Deactivate() 
{
	// Вызывается при деактивации слота в котором находится объект
}

void CInventoryItem::Drop() 
{
	if(m_pInventory) m_drop = true;
}

s32 CInventoryItem::Sort(PIItem /**pIItem/**/) 
{
	// Если нужно разместить IItem после this - вернуть 1, если
	// перед - -1. Если пофиг то 0.
	return 0;
}

bool CInventoryItem::Merge(PIItem /**pIItem/**/) 
{
	// Если удалось слить вместе вернуть true
	return false;
}

void CInventoryItem::OnH_B_Independent	()
{
	m_eItemPlace = eItemPlaceUndefined;
	inherited::OnH_B_Independent();
}

void CInventoryItem::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
}

void CInventoryItem::OnH_A_Chield		()
{
	inherited::OnH_A_Chield		();
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
	switch (type)
	{
	case GE_ADDON_ATTACH:
		{
			u32 ItemID;
			P.r_u32			(ItemID);
			CInventoryItem*	 ItemToAttach	= smart_cast<CInventoryItem*>(Level().Objects.net_Find(ItemID));
			if (!ItemToAttach) break;
			Attach(ItemToAttach);
			CActor* pActor = smart_cast<CActor*>(H_Parent());
			if (pActor && pActor->inventory().ActiveItem() == this)
			{
				pActor->inventory().SetPrevActiveSlot(pActor->inventory().GetActiveSlot());
				pActor->inventory().Activate(NO_ACTIVE_SLOT);
				
			}
		}break;
	case GE_ADDON_DETACH:
		{
			string64			i_name;
			P.r_stringZ			(i_name);
			Detach(i_name);
			CActor* pActor = smart_cast<CActor*>(H_Parent());
			if (pActor && pActor->inventory().ActiveItem() == this)
			{
				pActor->inventory().SetPrevActiveSlot(pActor->inventory().GetActiveSlot());
				pActor->inventory().Activate(NO_ACTIVE_SLOT);
			};
		}break;	
	}
}

//процесс отсоединения вещи заключается в спауне новой вещи 
//в инвентаре и установке соответствующих флагов в родительском
//объекте, поэтому функция должна быть переопределена
bool CInventoryItem::Detach(const char* item_section_name) 
{
	if (OnClient()) return true;

	CSE_Abstract*		D	= F_entity_Create(item_section_name);
	R_ASSERT		   (D);
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = 
							 smart_cast<CSE_ALifeDynamicObject*>(D);
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
	D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
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

	m_bInInterpolation = false;
	m_bInterpolate	= false;

	m_useful_for_NPC				= true;
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObject					*alife_object = smart_cast<CSE_ALifeObject*>(e);
	if (alife_object)	{
		m_useful_for_NPC				= !!alife_object->m_flags.test(CSE_ALifeObject::flUsefulForAI);
	}

	CSE_ALifeInventoryItem			*pSE_InventoryItem = smart_cast<CSE_ALifeInventoryItem*>(e);
	if(!pSE_InventoryItem) return res;

	//!!!
	m_fCondition = pSE_InventoryItem->m_fCondition;
	if (GameID() != GAME_SINGLE)
	{
		processing_activate();
	}

	return		res;
}

void CInventoryItem::net_Destroy		()
{
	if (!frame_check(m_dwFrameDestroy))
		return;

	//инвентарь которому мы принадлежали
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
	if (CSE_ALifeInventoryItem::FLAG_NO_POSITION != NumItems)
		P.r_vec3			( N.State.position);

	if (!NumItems || (CSE_ALifeInventoryItem::FLAG_NO_POSITION == NumItems)) return;

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

	Level().AddObject_To_Objects4CrPr(this);
	CrPr_SetActivated(false);
	CrPr_SetActivationStep(0);

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
	if (H_Parent())
		NumItems = CSE_ALifeInventoryItem::FLAG_NO_POSITION;
	else
		if (GameID() == GAME_SINGLE)
			NumItems = 0;

	P.w_u16					( NumItems		);
	if (NumItems != CSE_ALifeInventoryItem::FLAG_NO_POSITION)
		P.w_vec3			( State.position);

	if (!NumItems || (NumItems == CSE_ALifeInventoryItem::FLAG_NO_POSITION))
		return;

	P.w_u8					( State.enabled );

	P.w_vec3				( State.angular_vel);
	P.w_vec3				( State.linear_vel);

	P.w_vec3				( State.force	);
	P.w_vec3				( State.torque	);

	P.w_float				( State.quaternion.x );
	P.w_float				( State.quaternion.y );
	P.w_float				( State.quaternion.z );
	P.w_float				( State.quaternion.w );
};


void CInventoryItem::save(NET_Packet &packet)
{
	inherited::save(packet);
	packet.w_u8 ((u8)m_eItemPlace);
}
void CInventoryItem::load(IReader &packet)
{
	inherited::load(packet);
	m_eItemPlace = (EItemPlace)packet.r_u8 ();
/*	if(m_pInventory)
		m_pInventory->Replace(this);*/
}

///////////////////////////////////////////////
void CInventoryItem::PH_B_CrPr		()
{
	//just set last update data for now
	if (CrPr_IsActivated()) return;
	if (CrPr_GetActivationStep() > ph_world->m_steps_num) return;
	CrPr_SetActivated(true);

	///////////////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	///////////////////////////////////////////////
	pSyncObj->get_State(LastState);
	///////////////////////////////////////////////
	net_update_IItem N_I = NET_IItem.back();

	pSyncObj->set_State(N_I.State);

	PHUnFreeze();
	///////////////////////////////////////////////
	if (Level().InterpolationDisabled())
	{
		m_bInInterpolation = false;
	};
	///////////////////////////////////////////////
};	

void CInventoryItem::PH_I_CrPr		()		// actions & operations between two phisic prediction steps
{
	//store recalculated data, then we able to restore it after small future prediction
	if (!CrPr_IsActivated()) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	pSyncObj->get_State(RecalculatedState);
	///////////////////////////////////////////////
	Fmatrix xformX;
	pSyncObj->cv2obj_Xfrom(RecalculatedState.quaternion, RecalculatedState.position, xformX);

	VERIFY2								(_valid(xformX),*cName());
	pSyncObj->cv2obj_Xfrom(RecalculatedState.quaternion, RecalculatedState.position, xformX);

	IRecRot.set(xformX);
	IRecPos.set(xformX.c);
	VERIFY2								(_valid(IRecPos),*cName());
}; 

#ifdef DEBUG
void CInventoryItem::PH_Ch_CrPr			()
{
	//restore recalculated data and get data for interpolation	
	if (!CrPr_IsActivated()) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	pSyncObj->get_State(CheckState);
	if (!H_Parent() && getVisible())
	{
		if (CheckState.enabled == false && RecalculatedState.enabled == true)
		{
			///////////////////////////////////////////////////////////////////
			pSyncObj->set_State(LastState);
			pSyncObj->set_State(RecalculatedState);//, N_A.State.enabled);

			PHUnFreeze();
			///////////////////////////////////////////////////////////////////
			ph_world->Step();
			///////////////////////////////////////////////////////////////////
			PH_Ch_CrPr();
			////////////////////////////////////
		};
	};	
};
#endif

void CInventoryItem::PH_A_CrPr		()
{
	//restore recalculated data and get data for interpolation	
	if (!CrPr_IsActivated()) return;
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
	////////////////////////////////////
	Fmatrix xformX;
	pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, PredictedState.position, xformX);

	VERIFY2								(_valid(xformX),*cName());
	pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, PredictedState.position, xformX);
	
	IEndRot.set(xformX);
	IEndPos.set(xformX.c);
	VERIFY2								(_valid(IEndPos),*cName());
	/////////////////////////////////////////////////////////////////////////
	CalculateInterpolationParams();
	///////////////////////////////////////////////////
};

extern	float		g_cl_lvInterp;

void CInventoryItem::CalculateInterpolationParams()
{
	IStartPos.set(Position());
	IStartRot.set(XFORM());

	Fvector P0, P1, P2, P3;

	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = PHGetSyncItem(0);
	
	Fmatrix xformX0, xformX1;	

	if (m_bInInterpolation)
	{
		u32 CurTime = Level().timeServer();
		float factor	= float(CurTime - m_dwIStartTime)/(m_dwIEndTime - m_dwIStartTime);
		if (factor > 1.0f) factor = 1.0f;

		float c = factor;
		for (u32 k=0; k<3; k++)
		{
			P0[k] = c*(c*(c*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
			P1[k] = (c*c*SCoeff[k][0]*3+c*SCoeff[k][1]*2+SCoeff[k][2])/3; // сокрость из формулы в 3 раза превышает скорость при расчете коэффициентов !!!!
		};
		P0.set(IStartPos);
		P1.add(IStartPos);
	}	
	else
	{
		P0 = IStartPos;

		if (LastState.linear_vel.x == 0 && LastState.linear_vel.y == 0 && LastState.linear_vel.z == 0)
		{
			pSyncObj->cv2obj_Xfrom(RecalculatedState.previous_quaternion, RecalculatedState.previous_position, xformX0);
			pSyncObj->cv2obj_Xfrom(RecalculatedState.quaternion, RecalculatedState.position, xformX1);
		}
		else
		{
			pSyncObj->cv2obj_Xfrom(LastState.previous_quaternion, LastState.previous_position, xformX0);
			pSyncObj->cv2obj_Xfrom(LastState.quaternion, LastState.position, xformX1);
		};

		P1.sub(xformX1.c, xformX0.c);
		P1.add(IStartPos);
	}

	P2.sub(PredictedState.position, PredictedState.linear_vel);
	pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, P2, xformX0);
	P2.set(xformX0.c);

	pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, PredictedState.position, xformX1);
	P3.set(xformX1.c);
	/////////////////////////////////////////////////////////////////////////////
	Fvector TotalPath;
	TotalPath.sub(P3, P0);
	float TotalLen = TotalPath.magnitude();
	
	SPHNetState	State0 = (NET_IItem.back()).State;
	SPHNetState	State1 = PredictedState;

	float lV0 = State0.linear_vel.magnitude();
	float lV1 = State1.linear_vel.magnitude();

	u32		ConstTime = u32((fixed_step - ph_world->m_frame_time)*1000)+ Level().GetInterpolationSteps()*u32(fixed_step*1000);
	
	m_dwIStartTime = m_dwILastUpdateTime;

	if (( lV0 + lV1) > 0.000001 && g_cl_lvInterp == 0)
	{
		u32		CulcTime = iCeil(TotalLen*2000/( lV0 + lV1));
		m_dwIEndTime = m_dwIStartTime + min(CulcTime, ConstTime);
	}
	else
		m_dwIEndTime = m_dwIStartTime + ConstTime;
	/////////////////////////////////////////////////////////////////////////////
	Fvector V0, V1;
	V0.sub(P1, P0);
	V1.sub(P3, P2);
	lV0 = V0.magnitude();
	lV1 = V1.magnitude();

	if (TotalLen != 0)
	{
		if (V0.x != 0 || V0.y != 0 || V0.z != 0)
		{
			if (lV0 > TotalLen/3)
			{
				V0.normalize();
				V0.mul(TotalLen/3);
				P1.add(V0, P0);
			}
		}
		
		if (V1.x != 0 || V1.y != 0 || V1.z != 0)
		{
			if (lV1 > TotalLen/3)
			{
				V1.normalize();
				V1.mul(TotalLen/3);
				P2.sub(P3, V1);
			};
		}
	};
	/////////////////////////////////////////////////////////////////////////////
	for( u32 i =0; i<3; i++)
	{
		SCoeff[i][0] = P3[i]	- 3*P2[i] + 3*P1[i] - P0[i];
		SCoeff[i][1] = 3*P2[i]	- 6*P1[i] + 3*P0[i];
		SCoeff[i][2] = 3*P1[i]	- 3*P0[i];
		SCoeff[i][3] = P0[i];
	};
	/////////////////////////////////////////////////////////////////////////////
	m_bInInterpolation = true;

	if (m_pPhysicsShell) m_pPhysicsShell->NetInterpolationModeON();

};

void CInventoryItem::make_Interpolation	()
{
	m_dwILastUpdateTime = Level().timeServer();
	
	if(!H_Parent() && getVisible() && m_pPhysicsShell && m_bInInterpolation) 
	{
		u32 CurTime = Level().timeServer();
		if (CurTime >= m_dwIEndTime) 
		{
			m_bInInterpolation = false;

			m_pPhysicsShell->NetInterpolationModeOFF();

			CPHSynchronize* pSyncObj = NULL;
			pSyncObj = PHGetSyncItem(0);
			pSyncObj->set_State(PredictedState);
			Fmatrix xformI;
			pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, PredictedState.position, xformI);
			VERIFY2								(_valid(renderable.xform),*cName());
			XFORM().set(xformI);
			VERIFY2								(_valid(renderable.xform),*cName());
		}
		else 
		{
			VERIFY			(CurTime <= m_dwIEndTime);
			float factor	= float(CurTime - m_dwIStartTime)/(m_dwIEndTime - m_dwIStartTime);
			if (factor > 1) factor = 1.0f;
			else if (factor < 0) factor = 0;

			Fvector IPos;
			Fquaternion IRot;

			float c = factor;
			for (u32 k=0; k<3; k++)
			{
				IPos[k] = c*(c*(c*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
			};

			VERIFY2								(_valid(IPos),*cName());
			IRot.slerp(IStartRot, IEndRot, factor);
			VERIFY2								(_valid(IRot),*cName());
			XFORM().rotation(IRot);
			VERIFY2								(_valid(renderable.xform),*cName());
			Position().set(IPos);
			VERIFY2								(_valid(renderable.xform),*cName());
		};
	}
	else
	{
		m_bInInterpolation = false;
	};

#ifdef DEBUG
	Fvector iPos = Position();

	if (!H_Parent() && getVisible()) 
	{
		LastVisPos.push_back(iPos);
	};
#endif
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
	if (!frame_check(m_dwFrameReinit))
		return;

	m_pInventory		= 0;

	inherited::reinit	();
}

bool CInventoryItem::can_kill			() const
{
	return				(false);
}

CInventoryItem *CInventoryItem::can_kill	(CInventory *inventory) const
{
	return				(0);
}

const CInventoryItem *CInventoryItem::can_kill			(const xr_vector<const CGameObject*> &items) const
{
	return				(0);
}

CInventoryItem *CInventoryItem::can_make_killing	(const CInventory *inventory) const
{
	return				(0);
}

bool CInventoryItem::ready_to_kill		() const
{
	return				(false);
}

void CInventoryItem::activate_physic_shell()
{
	CEntityAlive*	E		= smart_cast<CEntityAlive*>(H_Parent());
	if (!E)
	{
		inherited::activate_physic_shell();
		return;
	};
	UpdateXForm();

	CPhysicsShellHolder::activate_physic_shell();
}

void CInventoryItem::UpdateXForm	()
{
	if (0==H_Parent())	return;

	// Get access to entity and its visual
	CEntityAlive*	E		= smart_cast<CEntityAlive*>(H_Parent());

	if(!E) return;
	R_ASSERT		(E);
	CKinematics*	V		= smart_cast<CKinematics*>	(E->Visual());
	VERIFY			(V);

	// Get matrices
	int				boneL,boneR,boneR2;
	E->g_WeaponBones(boneL,boneR,boneR2);
	//	if ((HandDependence() == hd1Hand) || (STATE == eReload) || (!E->g_Alive()))
	//		boneL = boneR2;
#pragma todo("TO ALL: serious performance problem")
	V->CalculateBones	();
	Fmatrix& mL			= V->LL_GetTransform(u16(boneL));
	Fmatrix& mR			= V->LL_GetTransform(u16(boneR));
	// Calculate
	Fmatrix			mRes;
	Fvector			R,D,N;
	D.sub			(mL.c,mR.c);	D.normalize_safe();

	if(fis_zero(D.magnitude()))
	{
		mRes.set(E->XFORM());
		mRes.c.set(mR.c);
	}
	else
	{		
		D.normalize();
		R.crossproduct	(mR.j,D);

		N.crossproduct	(D,R);
		N.normalize();

		mRes.set		(R,N,D,mR.c);
		mRes.mulA_43	(E->XFORM());
	}

	//	UpdatePosition	(mRes);
	Position().set(mRes.c);
}



#ifdef DEBUG
extern	Flags32	dbg_net_Draw_Flags;
void CInventoryItem::OnRender()
{
	if (bDebug && Visual())
	{
		if (!(dbg_net_Draw_Flags.is_any((1<<4)))) return;

		Fvector bc,bd; 
		Visual()->vis.box.get_CD	(bc,bd);
		Fmatrix	M = XFORM();
		M.c.add (bc);
		RCache.dbg_DrawOBB			(M,bd,color_rgba(0,0,255,255));
/*
		u32 Color;
		if (processing_enabled())
		{
			if (m_bInInterpolation)
				Color = color_rgba(0,255,255, 255);
			else
				Color = color_rgba(0,255,0, 255);
		}
		else
		{
			if (m_bInInterpolation)
				Color = color_rgba(255,0,255, 255);
			else
				Color = color_rgba(255, 0, 0, 255);
		};

//		RCache.dbg_DrawOBB			(M,bd,Color);
		float size = 0.01f;
		if (!H_Parent())
		{
			RCache.dbg_DrawAABB			(Position(), size, size, size, color_rgba(0, 255, 0, 255));

			Fvector Pos1, Pos2;
			VIS_POSITION_it It = LastVisPos.begin();
			Pos1 = *It;
			for (; It != LastVisPos.end(); It++)
			{
				Pos2 = *It;
				RCache.dbg_DrawLINE(Fidentity, Pos1, Pos2, color_rgba(255, 255, 0, 255));
				Pos1 = Pos2;
			};

		}
		//---------------------------------------------------------
		if (OnClient() && !H_Parent() && m_bInInterpolation)
		{

			Fmatrix xformI;

			xformI.rotation(IRecRot);
			xformI.c.set(IRecPos);
			RCache.dbg_DrawAABB			(IRecPos, size, size, size, color_rgba(255, 0, 255, 255));

			xformI.rotation(IEndRot);
			xformI.c.set(IEndPos);
			RCache.dbg_DrawOBB			(xformI,bd,color_rgba(0, 255, 0, 255));

			///////////////////////////////////////////////////////////////////////////
			Fvector point0 = IStartPos, point1;			
			
			float c = 0;
			for (float i=0.1f; i<1.1f; i+= 0.1f)
			{
				c = i;// * 0.1f;
				for (u32 k=0; k<3; k++)
				{
					point1[k] = c*(c*(c*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
				};
				RCache.dbg_DrawLINE(Fidentity, point0, point1, color_rgba(0, 0, 255, 255));
				point0.set(point1);
			};
		};
		*/
	};
}
#endif