////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_trader.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Jim
//	Description : AI Behaviour for monster "Trader"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_trader.h"
#include "../../bolt.h"
#include "../../trade.h"
#include "../../script_entity_action.h"
#include "../../script_game_object.h"
#include "../../inventory.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "../../script_space.h"
#include "../../artifact.h"
#include "../../xrserver.h"

CAI_Trader::CAI_Trader()
{
	InitTrade();
	init();
} 

CAI_Trader::~CAI_Trader()
{
	delete_data						(m_tpOrderedArtefacts);
}

void CAI_Trader::Load(LPCSTR section)
{
	setEnabled						(false);
	inherited::Load					(section);

	//fHealth							= pSettings->r_float	(section,"Health");
	fEntityHealth = pSettings->r_float	(section,"Health");

	float max_weight = pSettings->r_float	(section,"max_item_mass");
	inventory().SetMaxWeight(max_weight*1000);
	inventory().SetMaxRuck(1000000);
	inventory().CalcTotalWeight();
}

void CAI_Trader::init()
{
}

void CAI_Trader::reinit	()
{
	CEntityAlive::reinit	();
	CInventoryOwner::reinit	();
	CScriptMonster::reinit	();
	CSoundPlayer::reinit	();

	m_OnStartCallback.clear ();
	m_OnStopCallback.clear	();
	m_OnTradeCallback.clear ();

	m_tpHeadDef				= 0;
	m_tpGlobalDef			= 0;
	m_cur_head_anim_type	= MonsterSpace::eHeadAnimNone;
}

void CAI_Trader::reload	(LPCSTR section)
{
	CEntityAlive::reload	(section);
	CInventoryOwner::reload	(section);
	CScriptMonster::reload	(section);
	CSoundPlayer::reload	(section);

	head_anims.clear		();	

	add_head_anim(MonsterSpace::eHeadAnimNormal, "talk_");
	add_head_anim(MonsterSpace::eHeadAnimAngry,  "talk_angry_");
	add_head_anim(MonsterSpace::eHeadAnimGlad,   "talk_glad_");
	add_head_anim(MonsterSpace::eHeadAnimKind,   "talk_kind_");
}

/////////////////////////////////////////////////////////////////////////////////////////
// Animation management
/////////////////////////////////////////////////////////////////////////////////////////

void CAI_Trader::add_head_anim(u32 index, LPCSTR anim_name)
{
	SAnimInfo val;
	
	val.name	= anim_name;
	val.count	= get_anim_count(anim_name);

	head_anims.insert(mk_pair(index, val));
}

u8 CAI_Trader::get_anim_count(LPCSTR anim)
{
	string128	s, s_temp; 
	u8 count = 0;

	for (int i=0; ; ++i) {
		if (0 != PSkeletonAnimated(Visual())->ID_Cycle_Safe(strconcat(s_temp, anim,itoa(i,s,10))))  count++;
		else break;
	}

	if (count == 0) {
		sprintf(s, "Error! No animation: %s for monster %s", anim, *cName());
		R_ASSERT2(count != 0, s);
	} 

	return count;
}

void CAI_Trader::select_head_anim(u32 type)
{
	MOTION_MAP_IT it = head_anims.find(type);
	VERIFY(it != head_anims.end());

	// get index
	u32 index = Random.randI(it->second.count);

	// construct name
	string128 s1,s2;
	m_tpHeadDef = PSkeletonAnimated(Visual())->ID_Cycle_Safe(strconcat(s2,*it->second.name,itoa(index,s1,10)));
}

// Animation Callbacks
void __stdcall CAI_Trader::AnimGlobalCallback(CBlend* B)
{
	CAI_Trader *trader = (CAI_Trader*)B->CallbackParam;
	trader->m_tpGlobalDef = 0;
}

void __stdcall CAI_Trader::AnimHeadCallback(CBlend* B)
{
	CAI_Trader *trader = (CAI_Trader*)B->CallbackParam;
	trader->m_tpHeadDef = 0;
}


void CAI_Trader::SelectAnimation		(const Fvector& /**_view/**/, const Fvector& /**_move/**/, float /**speed/**/)
{
	if (!g_Alive()) return;

	// назначить глобальную анимацию
	if (!m_tpGlobalDef) {
		// выбор анимации
		m_tpGlobalDef = PSkeletonAnimated(Visual())->ID_Cycle("rot_5");
		PSkeletonAnimated(Visual())->PlayCycle(m_tpGlobalDef,TRUE,AnimGlobalCallback,this);
	}

	AssignHeadAnimation();
}

void CAI_Trader::AssignHeadAnimation()
{
	// назначить анимацию головы
	if (!m_tpHeadDef)	{
		if ( m_cur_head_anim_type != u32(-1)) {
			select_head_anim(m_cur_head_anim_type);
			PSkeletonAnimated(Visual())->PlayCycle(m_tpHeadDef,TRUE,AnimHeadCallback,this);	
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////

bool CAI_Trader::bfAssignSound(CScriptEntityAction *tpEntityAction)
{
	if (!CScriptMonster::bfAssignSound(tpEntityAction))
		return			(false);

	CScriptSoundAction	&l_tAction	= tpEntityAction->m_tSoundAction;
	m_cur_head_anim_type = l_tAction.m_tHeadAnimType;

	return				(true);
}

//////////////////////////////////////////////////////////////////////////

void __stdcall CAI_Trader::BoneCallback(CBoneInstance *B)
{
	CAI_Trader*	this_class = dynamic_cast<CAI_Trader*> (static_cast<CObject*>(B->Callback_Param));

	this_class->LookAtActor(B);
}

void CAI_Trader::LookAtActor(CBoneInstance *B)
{
	Fvector dir;
	dir.sub(Level().CurrentEntity()->Position(),Position());

	float yaw,pitch;
	dir.getHP(yaw, pitch);

	float h,p,b;
	XFORM().getHPB(h,p,b);
	float cur_yaw	= h;
	float dy		= _abs(angle_normalize_signed(yaw - cur_yaw));

	if (angle_normalize_signed(yaw - cur_yaw) > 0) dy *= -1.f;

	Fmatrix M;
	M.setXYZi (dy, 0.f, 0.f);
	B->mTransform.mulB(M);
}

BOOL CAI_Trader::net_Spawn			(LPVOID DC)
{
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeTrader					*l_tpTrader = dynamic_cast<CSE_ALifeTrader*>(e);
	
	clone							(l_tpTrader->m_tpOrderedArtefacts,m_tpOrderedArtefacts);
	
	R_ASSERT						(l_tpTrader);


	//проспавнить PDA у InventoryOwner
	if (!CInventoryOwner::net_Spawn(DC)) return FALSE;

	if (!inherited::net_Spawn(DC) || !CScriptMonster::net_Spawn(DC))	return FALSE;

	//m_body.current.yaw			= m_body.target.yaw	= -tpTrader->o_Angle.y;
	//m_body.current.pitch			= m_body.target.pitch	= 0;
	
	//m_tAnimation					= PSkeletonAnimated(Visual())->ID_Cycle("rot_5");

	setVisible						(TRUE);
	setEnabled						(TRUE);

	m_dwMoney						= l_tpTrader->m_dwMoney;
	m_tRank							= l_tpTrader->m_tRank;

	// Установка callback на кости
	CBoneInstance *bone_head =	&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head"));
	bone_head->set_callback(BoneCallback,this);

	return	TRUE;
}

void CAI_Trader::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	P.w_float						(inventory().TotalWeight());
	P.w_u32							(m_dwMoney);
	P.w_u32							(0);
}

void CAI_Trader::net_Import		(NET_Packet& P)
{
	R_ASSERT						(Remote());

	float							fDummy;
	u32								dwDummy;
	P.r_float						(fDummy);
	m_dwMoney						= P.r_u32();
	P.r_u32							(dwDummy);

	setVisible						(TRUE);
	setEnabled						(TRUE);
}



void CAI_Trader::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent			(P,type);
	CInventoryOwner::OnEvent	(P,type);

	u16 id;
	CObject* Obj;

	switch (type) {
		case GE_TRADE_BUY:
		case GE_OWNERSHIP_TAKE:
			P.r_u16		(id);
			Obj = Level().Objects.net_Find	(id);
			if(inventory().Take(dynamic_cast<CGameObject*>(Obj), false, false)) 
				Obj->H_SetParent(this);
			else
			{
				NET_Packet				P;
				u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
				P.w_u16					(u16(Obj->ID()));
				u_EventSend				(P);
			}
			break;
		case GE_TRADE_SELL:
		case GE_OWNERSHIP_REJECT:
			P.r_u16		(id);
			Obj = Level().Objects.net_Find	(id);
			if(inventory().Drop(dynamic_cast<CGameObject*>(Obj))) 
				Obj->H_SetParent(0);
			break;
		case GE_TRANSFER_AMMO:
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
		Msg("Taking item %s!",*I->cName());
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->ID()));
		u_EventSend		(P);
	}
}

void CAI_Trader::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (this != O->H_Parent()))
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
	UpdateInventoryOwner(dt);

	if (GetScriptControl()) ProcessScripts();
	else Think();
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

void CAI_Trader::g_fireParams(const CHudItem* pHudItem, Fvector& P, Fvector& D)
{
	if (g_Alive() && inventory().ActiveItem()) {
		Center(P);
		D.setHP(0,0);
		D.normalize_safe();
	}
}

void CAI_Trader::Think()
{
}

void CAI_Trader::Die ()
{
	inherited::Die ();
}

void CAI_Trader::net_Destroy()
{
	inherited::net_Destroy		();
	CScriptMonster::net_Destroy ();
	m_OnStartCallback.clear		();
	m_OnStopCallback.clear		();
	m_OnTradeCallback.clear		();
	delete_data					(m_tpOrderedArtefacts);
}

void TraderScriptCallBack(CBlend* B)
{
	CScriptMonster	*l_tpScriptMonster = dynamic_cast<CScriptMonster*> (static_cast<CObject*>(B->CallbackParam));
	R_ASSERT		(l_tpScriptMonster);
	if (l_tpScriptMonster->GetCurrentAction()) 
		l_tpScriptMonster->GetCurrentAction()->m_tAnimationAction.m_bCompleted = true;
}

void CAI_Trader::UpdateCL()
{ 
	inherited::UpdateCL		();
	CSoundPlayer::update	(Device.fTimeDelta);

	if (!bfScriptAnimation()) {
		SelectAnimation		(XFORM().k,XFORM().k,0.f);
	}

	if (!m_current_sound) {
		m_tpHeadDef				= 0;
		m_cur_head_anim_type	= MonsterSpace::eHeadAnimNone;
	}

	AssignHeadAnimation();
}

BOOL CAI_Trader::UsedAI_Locations()
{
	return					(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Trade callbacks
///////////////////////////////////////////////////////////////////////////////////////////////
void CAI_Trader::set_callback(const luabind::functor<void> &lua_function, bool bOnStart)
{
	CScriptCallback	 &callback = bOnStart ? m_OnStartCallback : m_OnStopCallback;
	callback.set(lua_function);
}

void CAI_Trader::set_callback(const luabind::object &lua_object, LPCSTR method, bool bOnStart)
{
	CScriptCallback	&callback = bOnStart ? m_OnStartCallback : m_OnStopCallback;
	callback.set(lua_object,method);
}

void CAI_Trader::clear_callback(bool bOnStart)
{
	CScriptCallback	&callback = bOnStart ? m_OnStartCallback : m_OnStopCallback;
	callback.clear	();
}

void CAI_Trader::OnStartTrade()
{
	m_OnStartCallback.callback();
}

void CAI_Trader::OnStopTrade()
{
	m_OnStopCallback.callback();
}

void CAI_Trader::set_trade_callback(const luabind::functor<void> &lua_function)
{
	m_OnTradeCallback.set(lua_function);
}

void CAI_Trader::set_trade_callback(const luabind::object &lua_object, LPCSTR method)
{
	m_OnTradeCallback.set(lua_object,method);
}

void CAI_Trader::clear_trade_callback()
{
	m_OnTradeCallback.clear();
}

void CAI_Trader::OnTradeAction(CGameObject *O, bool bSell)
{
	if (!O) return;
	SCRIPT_CALLBACK_EXECUTE_2(m_OnTradeCallback,O->lua_game_object(), bSell);
}
////////////////////////////////////////////////////////////////////////////////////////////


bool CAI_Trader::can_attach				(const CInventoryItem *inventory_item) const
{
	return				(false);
}

bool CAI_Trader::use_bolts				() const
{
	return				(false);
}

void CAI_Trader::spawn_supplies			()
{
	inherited::spawn_supplies		();
	CInventoryOwner::spawn_supplies	();
}


void CAI_Trader::save (NET_Packet &output_packet)
{
	inherited::save(output_packet);
	CInventoryOwner::save(output_packet);
}
void CAI_Trader::load (IReader &input_packet)		
{
	inherited::load(input_packet);
	CInventoryOwner::load(input_packet);
}


//проверяет список артефактов в заказах
u32 CAI_Trader::ArtefactPrice (CArtefact* pArtefact)
{
	ALife::ARTEFACT_TRADER_ORDER_MAP::const_iterator i = m_tpOrderedArtefacts.begin();
	ALife::ARTEFACT_TRADER_ORDER_MAP::const_iterator e = m_tpOrderedArtefacts.end();
	for ( ; i != e; ++i) 
	{
		//			Msg     ("Artefact : section[%s] total_count[%d]",(*i).second->m_caSection,(*i).second->m_dwTotalCount);
		//			Msg     ("Orders : ");
		if(pArtefact->cNameSect() == (*i).second->m_caSection)
		{
			ALife::ARTEFACT_ORDER_VECTOR::const_iterator    II = (*i).second->m_tpOrders.begin();
			ALife::ARTEFACT_ORDER_VECTOR::const_iterator    EE = (*i).second->m_tpOrders.end();
			for ( ; II != EE; ++II)
			{
				if((*II).m_count>0)
					return (*II).m_price; 
			}
		}
	}

	return pArtefact->Cost();
}

//продажа артефакта, с последуещим изменением списка заказов (true - если артефакт был в списке)
bool CAI_Trader::BuyArtefact (CArtefact* pArtefact)
{
	VERIFY(pArtefact);
	ALife::ARTEFACT_TRADER_ORDER_MAP::iterator i = m_tpOrderedArtefacts.begin();
	ALife::ARTEFACT_TRADER_ORDER_MAP::iterator e = m_tpOrderedArtefacts.end();
	for ( ; i != e; ++i) 
	{
		//			Msg     ("Artefact : section[%s] total_count[%d]",(*i).second->m_caSection,(*i).second->m_dwTotalCount);
		//			Msg     ("Orders : ");
		if(pArtefact->cNameSect() == (*i).second->m_caSection)
		{
			ALife::ARTEFACT_ORDER_VECTOR::iterator    II = (*i).second->m_tpOrders.begin();
			ALife::ARTEFACT_ORDER_VECTOR::iterator    EE = (*i).second->m_tpOrders.end();
			for ( ; II != EE; ++II)
			{
				if((*II).m_count>0)
				{
					(*II).m_count--;
					return true; 
				}
			}
		}
	}

	return false;
}

void CAI_Trader::SyncArtefactsWithServer	()
{
	CSE_Abstract					*e	= Level().Server->game->get_entity_from_eid(ID()); VERIFY(e);
    CSE_ALifeTrader					*l_tpTrader = dynamic_cast<CSE_ALifeTrader*>(e);
	delete_data						(l_tpTrader->m_tpOrderedArtefacts);
	clone							(m_tpOrderedArtefacts, l_tpTrader->m_tpOrderedArtefacts);
}