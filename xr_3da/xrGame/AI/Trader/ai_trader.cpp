////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_trader.h"
#include "..\\..\\bolt.h"

#include "..\\..\\trade.h"

CAI_Trader::CAI_Trader()
{
	m_bPlaying						= false;

	m_trade							= xr_new<CTrade>(this);
} 

CAI_Trader::~CAI_Trader()
{
	xr_delete(m_trade);
}

void CAI_Trader::Load(LPCSTR section)
{
	setEnabled						(false);
	inherited::Load					(section);


	fHealth							= pSettings->r_float	(section,"Health");
	m_inventory.m_maxWeight			= pSettings->r_float	(section,"max_item_mass");
	m_inventory.m_maxRuck			= 10000;
}

BOOL CAI_Trader::net_Spawn			(LPVOID DC)
{
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeTrader					*tpTrader = dynamic_cast<CSE_ALifeTrader*>(e);
	
	R_ASSERT						(tpTrader);
	cNameVisual_set					(tpTrader->get_visual());
	
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	//r_torso_current.yaw			= r_torso_target.yaw	= -tpTrader->o_Angle.y;
	//r_torso_current.pitch			= r_torso_target.pitch	= 0;
	
	m_tAnimation					= PKinematics(Visual())->ID_Cycle("trade");

	setVisible						(TRUE);
	setEnabled						(TRUE);

	CSE_ALifeTraderAbstract		*pTA	= dynamic_cast<CSE_ALifeTraderAbstract*>(e);
	m_dwMoney	= pTA->m_dwMoney;
	m_tRank		= pTA->m_tRank;

	return	TRUE;
}

void CAI_Trader::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	P.w_float						(m_inventory.TotalWeight());
	P.w_u32							(0);
	P.w_u32							(0);
}

void CAI_Trader::net_Import		(NET_Packet& P)
{
	R_ASSERT						(Remote());

	float							fDummy;
	u32								dwDummy;
	P.r_float						(fDummy);
	P.r_u32							(dwDummy);
	P.r_u32							(dwDummy);

	setVisible						(TRUE);
	setEnabled						(TRUE);
}

void CAI_Trader::SelectAnimation		(const Fvector& _view, const Fvector& _move, float speed)
{
	if (g_Alive()) 
		if (!m_bPlaying) {
				PKinematics(Visual())->PlayCycle	(m_tAnimation,TRUE,AnimCallback,this);
				m_bPlaying	= true;
		}
}

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CAI_Trader::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			Log("Trying to take - ", O->cName());
			if(g_Alive() && m_inventory.Take(dynamic_cast<CGameObject*>(O))) {
				O->H_SetParent(this);
				Log("TAKE - ", O->cName());
			}
		}
		break;
	case GE_OWNERSHIP_REJECT:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			
			if(m_inventory.Drop(dynamic_cast<CGameObject*>(O))) {
				O->H_SetParent(0);
				//feel_touch_deny(O,2000);
			}

		}
		break;
	case GE_TRANSFER_AMMO:
		{
		}
		break;
	}
}

void CAI_Trader::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= dynamic_cast<CInventoryItem*>	(O);
	CBolt				*E	= dynamic_cast<CBolt*>			(O);

	if (I && !E) {
		Msg("Taking item %s!",I->cName());
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->ID()));
		u_EventSend		(P);
	}
}

void CAI_Trader::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (O->H_Parent() != this))
		return;

	Msg("Dropping item!");
	// We doesn't have similar weapon - pick up it
	NET_Packet				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);
}

void CAI_Trader::shedule_Update	(u32 dt)
{
	inherited::shedule_Update	(dt);
	m_inventory.Update			(dt);

	Think();

}

void CAI_Trader::g_WeaponBones	(int &L, int &R1, int &R2)
{
	if (g_Alive() && m_inventory.ActiveItem()) {
		CKinematics *V	= PKinematics(Visual());
		R1				= V->LL_BoneID("bip01_r_hand");
		R2				= V->LL_BoneID("bip01_r_finger2");
		L				= V->LL_BoneID("bip01_l_finger1");
	}
}

void CAI_Trader::renderable_Render	()
{
	inherited::renderable_Render	();
	if(m_inventory.ActiveItem())
		m_inventory.ActiveItem()->renderable_Render();
}

void CAI_Trader::g_fireParams(Fvector& P, Fvector& D)
{
	if (g_Alive() && m_inventory.ActiveItem()) {
		Center(P);
		D.setHP(0,0);
		D.normalize_safe();
	}
}

void CAI_Trader::Think()
{
	SelectAnimation(XFORM().k,XFORM().k,0);
}
