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
#include "Level.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "string_table.h"
#include "../skeletoncustom.h"
#include "ai_object_location.h"
#include "HUDManager.h"
#include "UIGameCustom.h"

CInventoryItem::CInventoryItem() 
{
	m_weight = 100.f;
	m_slot = NO_ACTIVE_SLOT;
	m_belt = false;
	m_pInventory = NULL;
	m_drop = false;
	m_ruck = true;
	m_bRuckDefault = true;

	m_bCanTake = true;
	m_bCanTrade = true;

	m_fCondition = 1.0f;
	m_bUsingCondition = false;

	m_iGridWidth	= 1;
	m_iGridHeight	= 1;

	m_iXPos = 0;
	m_iYPos = 0;
	////////////////////////////////////	
	m_name = m_nameShort = NULL;

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
	CHitImmunity::LoadImmunities	(section);

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

	if(pSettings->line_exist(section, "belt"))
		m_belt = !!pSettings->r_bool(section, "belt");


	if(pSettings->line_exist(section, "can_take"))
		m_bCanTake = !!pSettings->r_bool(section, "can_take");
	else
		m_bCanTake = true;

	if(pSettings->line_exist(section, "can_trade"))
		m_bCanTrade = !!pSettings->r_bool(section, "can_trade");
	else
		m_bCanTrade = true;

	if(pSettings->line_exist(section, "default_to_ruck"))
		m_bRuckDefault = !!pSettings->r_bool(section, "default_to_ruck");
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
	if(l_name) 	m_nameComplex = l_name; 
	else 		m_nameComplex = 0;

	/*for(PPIItem l_it = m_subs.begin(); m_subs.end() != l_it; ++l_it) 
	{
		const char *l_subName = (*l_it)->NameShort();
		if(l_subName)
			strcpy(&m_nameComplex[xr_strlen(m_nameComplex)], l_subName);
	}*/


	if(m_bUsingCondition){
		string32		cond;
		if(GetCondition()<0.33)		strcpy		(cond, "[poor]");
		else if(GetCondition()<0.66)strcpy		(cond, "[bad]");
		else						strcpy		(cond, "[good]");
		string256		temp;
		sprintf			(temp,"%s %s",*m_nameComplex,cond);
		m_nameComplex	= temp;
	}


	return *m_nameComplex;
}

bool CInventoryItem::Useful() const
{
	// Если IItem нельзя использовать, вернуть false
	return CanTake();
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
	m_eItemPlace = eItemPlaceUndefined ;
	if (HUD().GetUI() && object().H_Parent() == Level().CurrentViewEntity())
	{
		HUD().GetUI()->UIGame()->ReInitInventoryWnd();
	};
}

void CInventoryItem::OnH_A_Independent	()
{
	m_eItemPlace = eItemPlaceUndefined;	
}

void CInventoryItem::OnH_B_Chield		()
{
}

void CInventoryItem::OnH_A_Chield		()
{
	if (HUD().GetUI() && object().H_Parent() == Level().CurrentViewEntity())
	{
		HUD().GetUI()->UIGame()->ReInitInventoryWnd();
	};
}

void CInventoryItem::UpdateCL()
{
}

void CInventoryItem::OnEvent (NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_ADDON_ATTACH:
		{
			u32 ItemID;
			P.r_u32			(ItemID);
			CInventoryItem*	 ItemToAttach	= smart_cast<CInventoryItem*>(Level().Objects.net_Find(ItemID));
			if (!ItemToAttach) break;
			Attach(ItemToAttach);
			CActor* pActor = smart_cast<CActor*>(object().H_Parent());
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
			CActor* pActor = smart_cast<CActor*>(object().H_Parent());
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
	
	l_tpALifeDynamicObject->m_tNodeID = object().ai_location().level_vertex_id();
		
	// Fill
	D->s_name			=	item_section_name;
	D->set_name_replace	("");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	u16(object().H_Parent()->ID());
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	object().Position();
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
BOOL CInventoryItem::net_Spawn			(CSE_Abstract* DC)
{
	VERIFY(!m_pInventory);

	m_bInInterpolation = false;
	m_bInterpolate	= false;

	m_useful_for_NPC				= true;
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObject					*alife_object = smart_cast<CSE_ALifeObject*>(e);
	if (alife_object)	{
		m_useful_for_NPC				= !!alife_object->m_flags.test(CSE_ALifeObject::flUsefulForAI);
	}

	CSE_ALifeInventoryItem			*pSE_InventoryItem = smart_cast<CSE_ALifeInventoryItem*>(e);
	if (!pSE_InventoryItem) return TRUE;

	//!!!
	m_fCondition = pSE_InventoryItem->m_fCondition;
	if (GameID() != GAME_SINGLE)
	{
		object().processing_activate();
	}

	return		TRUE;
}

void CInventoryItem::net_Destroy		()
{
	//инвентарь которому мы принадлежали
	m_pInventory = NULL;
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

	Level().AddObject_To_Objects4CrPr(m_object);
	object().CrPr_SetActivated(false);
	object().CrPr_SetActivationStep(0);

	NET_IItem.push_back			(N);
	while (NET_IItem.size() > 2)
	{
		NET_IItem.pop_front();
	};
};

void CInventoryItem::net_Export			(NET_Packet& P) 
{	
	P.w_float			(m_fCondition);
	P.w_u32				(Level().timeServer());	

	///////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	SPHNetState	State;
	pSyncObj = object().PHGetSyncItem(0);
	if (pSyncObj && !object().H_Parent()) 
		pSyncObj->get_State(State);
	else 	
		State.position.set(object().Position());
	///////////////////////////////////////	
	u16 NumItems = object().PHGetSyncItemsNumber();
	if (object().H_Parent())
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
	packet.w_u8 ((u8)m_eItemPlace);
}

void CInventoryItem::load(IReader &packet)
{
	m_eItemPlace = (EItemPlace)packet.r_u8 ();
}

///////////////////////////////////////////////
void CInventoryItem::PH_B_CrPr		()
{
	//just set last update data for now
	if (object().CrPr_IsActivated()) return;
	if (object().CrPr_GetActivationStep() > ph_world->m_steps_num) return;
	object().CrPr_SetActivated(true);

	///////////////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object().PHGetSyncItem(0);
	if (!pSyncObj) return;
	///////////////////////////////////////////////
	pSyncObj->get_State(LastState);
	///////////////////////////////////////////////
	net_update_IItem N_I = NET_IItem.back();

	pSyncObj->set_State(N_I.State);

	object().PHUnFreeze();
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
	if (!object().CrPr_IsActivated()) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object().PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	pSyncObj->get_State(RecalculatedState);
	///////////////////////////////////////////////
	Fmatrix xformX;
	pSyncObj->cv2obj_Xfrom(RecalculatedState.quaternion, RecalculatedState.position, xformX);

	VERIFY2								(_valid(xformX),*object().cName());
	pSyncObj->cv2obj_Xfrom(RecalculatedState.quaternion, RecalculatedState.position, xformX);

	IRecRot.set(xformX);
	IRecPos.set(xformX.c);
	VERIFY2								(_valid(IRecPos),*object().cName());
}; 

#ifdef DEBUG
void CInventoryItem::PH_Ch_CrPr			()
{
	//restore recalculated data and get data for interpolation	
	if (!object().CrPr_IsActivated()) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object().PHGetSyncItem(0);
	if (!pSyncObj) return;
	////////////////////////////////////
	pSyncObj->get_State(CheckState);
	if (!object().H_Parent() && object().getVisible())
	{
		if (CheckState.enabled == false && RecalculatedState.enabled == true)
		{
			///////////////////////////////////////////////////////////////////
			pSyncObj->set_State(LastState);
			pSyncObj->set_State(RecalculatedState);//, N_A.State.enabled);

			object().PHUnFreeze();
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
	if (!object().CrPr_IsActivated()) return;
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object().PHGetSyncItem(0);
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

	VERIFY2								(_valid(xformX),*object().cName());
	pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, PredictedState.position, xformX);
	
	IEndRot.set(xformX);
	IEndPos.set(xformX.c);
	VERIFY2								(_valid(IEndPos),*object().cName());
	/////////////////////////////////////////////////////////////////////////
	CalculateInterpolationParams();
	///////////////////////////////////////////////////
};

extern	float		g_cl_lvInterp;

void CInventoryItem::CalculateInterpolationParams()
{
	IStartPos.set(object().Position());
	IStartRot.set(object().XFORM());

	Fvector P0, P1, P2, P3;

	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object().PHGetSyncItem(0);
	
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

	if (object().m_pPhysicsShell) object().m_pPhysicsShell->NetInterpolationModeON();

};

void CInventoryItem::make_Interpolation	()
{
	m_dwILastUpdateTime = Level().timeServer();
	
	if(!object().H_Parent() && object().getVisible() && object().m_pPhysicsShell && m_bInInterpolation) 
	{
		u32 CurTime = Level().timeServer();
		if (CurTime >= m_dwIEndTime) 
		{
			m_bInInterpolation = false;

			object().m_pPhysicsShell->NetInterpolationModeOFF();

			CPHSynchronize* pSyncObj = NULL;
			pSyncObj = object().PHGetSyncItem(0);
			pSyncObj->set_State(PredictedState);
			Fmatrix xformI;
			pSyncObj->cv2obj_Xfrom(PredictedState.quaternion, PredictedState.position, xformI);
			VERIFY2								(_valid(object().renderable.xform),*object().cName());
			object().XFORM().set(xformI);
			VERIFY2								(_valid(object().renderable.xform),*object().cName());
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

			VERIFY2								(_valid(IPos),*object().cName());
			IRot.slerp(IStartRot, IEndRot, factor);
			VERIFY2								(_valid(IRot),*object().cName());
			object().XFORM().rotation(IRot);
			VERIFY2								(_valid(object().renderable.xform),*object().cName());
			object().Position().set(IPos);
			VERIFY2								(_valid(object().renderable.xform),*object().cName());
		};
	}
	else
	{
		m_bInInterpolation = false;
	};

#ifdef DEBUG
	Fvector iPos = object().Position();

	if (!object().H_Parent() && object().getVisible()) 
	{
		LastVisPos.push_back(iPos);
	};
#endif
}


void CInventoryItem::renderable_Render	()
{
}

void CInventoryItem::reload		(LPCSTR section)
{
	m_affected_holder_range	= READ_IF_EXISTS(pSettings,r_float,section,"affected_holder_range",1.f);
	m_affected_holder_fov	= READ_IF_EXISTS(pSettings,r_float,section,"affected_holder_fov",1.f);
}

void CInventoryItem::reinit		()
{
	m_pInventory		= 0;
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
	CEntityAlive*	E		= smart_cast<CEntityAlive*>(object().H_Parent());
	if (!E) {
		on_activate_physic_shell();
		return;
	};

	UpdateXForm();

	object().CPhysicsShellHolder::activate_physic_shell();
}

void CInventoryItem::UpdateXForm	()
{
	if (0==object().H_Parent())	return;

	// Get access to entity and its visual
	CEntityAlive*	E		= smart_cast<CEntityAlive*>(object().H_Parent());

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
	object().Position().set(mRes.c);
}



#ifdef DEBUG
extern	Flags32	dbg_net_Draw_Flags;
void CInventoryItem::OnRender()
{
	if (bDebug && object().Visual())
	{
		if (!(dbg_net_Draw_Flags.is_any((1<<4)))) return;

		Fvector bc,bd; 
		object().Visual()->vis.box.get_CD	(bc,bd);
		Fmatrix	M = object().XFORM();
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

DLL_Pure *CInventoryItem::_construct	()
{
	m_object	= smart_cast<CPhysicsShellHolder*>(this);
	VERIFY		(m_object);
	return		(m_object);
}

void CInventoryItem::fill_eye_params	(float &range, float &fov) const
{
	range		*= m_affected_holder_range;
	fov			*= m_affected_holder_fov;
}
