////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_trader.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Jim
//	Description : AI Behaviour for monster "Trader"
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

	//fHealth							= pSettings->r_float	(section,"Health");
	fEntityHealth = pSettings->r_float	(section,"Health");

	m_inventory.m_maxWeight			= pSettings->r_float	(section,"max_item_mass");
	m_inventory.m_maxRuck			= 10000;
	
	m_trade_storage.m_maxWeight		= m_inventory.m_maxWeight;
	m_trade_storage.m_maxRuck		= m_inventory.m_maxRuck;
}

BOOL CAI_Trader::net_Spawn			(LPVOID DC)
{
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeTrader					*l_tpTrader = dynamic_cast<CSE_ALifeTrader*>(e);
	
	m_tpOrderedArtefacts			= l_tpTrader->m_tpOrderedArtefacts;
	
	R_ASSERT						(l_tpTrader);
	cNameVisual_set					(l_tpTrader->get_visual());
	
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	//r_torso_current.yaw			= r_torso_target.yaw	= -tpTrader->o_Angle.y;
	//r_torso_current.pitch			= r_torso_target.pitch	= 0;
	
	m_tAnimation					= PKinematics(Visual())->ID_Cycle("trade");

	setVisible						(TRUE);
	setEnabled						(TRUE);

	m_dwMoney						= l_tpTrader->m_dwMoney;
	m_tRank							= l_tpTrader->m_tRank;

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
	CObject* Obj;

	switch (type) {
		case GE_OWNERSHIP_TAKE:
			P.r_u16		(id);
			Obj = Level().Objects.net_Find	(id);
			if(g_Alive() && m_inventory.Take(dynamic_cast<CGameObject*>(Obj))) Obj->H_SetParent(this);
			break;
		case GE_OWNERSHIP_REJECT:
			P.r_u16		(id);
			Obj = Level().Objects.net_Find	(id);
			if(m_inventory.Drop(dynamic_cast<CGameObject*>(Obj))) Obj->H_SetParent(0);
			break;
		case GE_TRANSFER_AMMO:
			break;
		
			// Trade is accomplishing through 'm_trade_storage'
		case GE_TRADE_BUY:		// equal to GE_OWNERSHIP_TAKE
			P.r_u16		(id);
			Obj			= Level().Objects.net_Find	(id);
			
			if(g_Alive() && m_trade_storage.Take(dynamic_cast<CGameObject*>(Obj), true)) Obj->H_SetParent(this);
			break;
		case GE_TRADE_SELL:
			P.r_u16		(id);
			Obj			= Level().Objects.net_Find	(id);
			if	(m_trade_storage.Drop(dynamic_cast<CGameObject*>(Obj))) Obj->H_SetParent(0);
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
	m_trade->UpdateTrade		();

	Think();
}

void CAI_Trader::g_WeaponBones	(int &L, int &R1, int &R2)
{
	CKinematics *V	= PKinematics(Visual());
	R1				= V->LL_BoneID("bip01_r_hand");
	R2				= V->LL_BoneID("bip01_r_finger2");
	L				= V->LL_BoneID("bip01_l_finger1");
}

void CAI_Trader::renderable_Render	()
{
	inherited::renderable_Render	();
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

void CAI_Trader::Die ( )
{
	m_inventory.DropAll();
}

