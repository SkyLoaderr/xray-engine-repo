#include "stdafx.h"
#include "trade.h"
#include "actor.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/trader/ai_trader.h"
#include "artifact.h"
#include "inventory.h"
#include "xrmessages.h"
#include "character_info.h"
#include "relation_registry.h"
#include "level.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "game_object_space.h"
#include "trade_parameters.h"

bool CTrade::CanTrade()
{
	CEntity *pEntity;

	xr_vector<CObject*>		q_nearest;			// = Level().ObjectSpace.q_nearest; 

	Level().ObjectSpace.GetNearest	(q_nearest,pThis.base->Position(),2.f, NULL);
	if (!q_nearest.empty()) 
	{
		for (u32 i=0, n = q_nearest.size(); i<n; ++i) 
		{
			// ����� �� ������ ���������
			pEntity = smart_cast<CEntity *>(q_nearest[i]);
			if (pEntity && !pEntity->g_Alive()) return false;
			if (SetPartner(pEntity)) break;
		}
	} 
	
	if (!pPartner.base) return false;

	// ������ �����
	float dist = pPartner.base->Position().distance_to(pThis.base->Position());
	if (dist < 0.5f || dist > 4.5f)  
	{
		RemovePartner();
		return false;
	}

	// ������ ������� �� ����
	float yaw, pitch;
	float yaw2, pitch2;

	pThis.base->Direction().getHP(yaw,pitch);
	pPartner.base->Direction().getHP(yaw2,pitch2);
	yaw = angle_normalize(yaw);
	yaw2 = angle_normalize(yaw2);

	float Res = rad2deg(_abs(yaw - yaw2) < PI ? _abs(yaw - yaw2) : 
								 PI_MUL_2 - _abs(yaw - yaw2));
	if (Res < 165.f || Res > 195.f) 
	{
		RemovePartner();
		return false;
	}

	return true;
}

void CTrade::SellItem(CInventoryItem* pItem)
{
	// ����� ������ �������� ������� �����������
	// ����� ���� �� ������� �������, ��� ������ �� ����
	u32						dwTransferMoney = GetItemPrice(pItem);

	pThis.inv_owner->on_before_sell		(pItem);
	pPartner.inv_owner->on_before_buy	(pItem);

	// ��������� � ���� 
	NET_Packet				P;
	CGameObject				*O = smart_cast<CGameObject *>(pThis.inv_owner);
	O->u_EventGen			(P,GE_TRADE_SELL,O->ID());
	P.w_u16					(pItem->object().ID());
	O->u_EventSend			(P);

	// �������� ���� �����
	pThis.inv_owner->m_dwMoney += dwTransferMoney;

	// ����� � ��������
	O						= smart_cast<CGameObject *>(pPartner.inv_owner);
	O->u_EventGen			(P,GE_TRADE_BUY,O->ID());
	P.w_u16					(pItem->object().ID());
	O->u_EventSend			(P);

	// ��������� ����� � ��������
	pPartner.inv_owner->m_dwMoney -= dwTransferMoney;

	CAI_Trader* pTrader = NULL;

	if (pPartner.type == TT_TRADER) 
	{
		CArtefact* pArtefact= smart_cast<CArtefact*>(pItem);
		if(pArtefact){
			pTrader = smart_cast<CAI_Trader*>(pPartner.base);
			m_bNeedToUpdateArtefactTasks |= pTrader->BuyArtefact(pArtefact);
		}

	}
	if((pPartner.type==TT_ACTOR) || (pThis.type==TT_ACTOR)){
		bool bDir = (pThis.type==TT_ACTOR);
		Actor()->callback(GameObject::eTradeSellBuyItem)(pItem->object().lua_game_object(), bDir, dwTransferMoney);
	}
}


CInventory& CTrade::GetTradeInv(SInventoryOwner owner)
{
	R_ASSERT(TT_NONE != owner.type);

	//return ((TT_TRADER == owner.type) ? (*owner.inv_owner->m_trade_storage) : (owner.inv_owner->inventory()));
	return owner.inv_owner->inventory();
}

CTrade*	CTrade::GetPartnerTrade()
{
	return pPartner.inv_owner->GetTrade();
}
CInventory*	CTrade::GetPartnerInventory()
{
	return &GetTradeInv(pPartner);
}

CInventoryOwner* CTrade::GetPartner()
{
	return pPartner.inv_owner;
}

u32	CTrade::GetItemPrice	(PIItem pItem)
{
	CArtefact				*pArtefact = smart_cast<CArtefact*>(pItem);

	// computing base_cost
	float					base_cost;
	if (pArtefact && (pThis.type == TT_ACTOR) && (pPartner.type == TT_TRADER)) {
		CAI_Trader			*pTrader = smart_cast<CAI_Trader*>(pPartner.inv_owner);
		VERIFY				(pTrader);
		base_cost			= (float)pTrader->ArtefactPrice(pArtefact);
	}
	else
		base_cost			= (float)pItem->Cost();
	
	// computing condition factor
	// for "dead" weapon we use 10% from base cost, for "good" weapon we use full base cost
	float					condition_factor = powf(pItem->GetCondition()*0.9f + .1f, 0.75f); 
	
	// computing relation factor
	float					relation_factor;

	CHARACTER_GOODWILL		attitude = RELATION_REGISTRY().GetAttitude(pPartner.inv_owner, pThis.inv_owner);

	if (NO_GOODWILL == attitude)
		relation_factor		= 0.f;
	else
		relation_factor		= float(attitude + 1000.f)/2000.f;

	clamp					(relation_factor,0.f,1.f);

	const SInventoryOwner	*_partner = 0;
	bool					buying = true;
	bool					is_actor = (pThis.type == TT_ACTOR) || (pPartner.type == TT_ACTOR);
	if (is_actor) {
		buying				= (pPartner.type == TT_ACTOR);
		_partner			= &(buying ? pThis : pPartner);
	}
	else {
		// rare case
		_partner			= &pPartner;
	}
	const SInventoryOwner	&partner = *_partner;

	// computing action factor
	const CTradeFactors		*p_trade_factors;
	if (buying)
		p_trade_factors		= &partner.inv_owner->trade_parameters().factors(CTradeParameters::action_buy(0),pItem->object().cNameSect());
	else
		p_trade_factors		= &partner.inv_owner->trade_parameters().factors(CTradeParameters::action_sell(0),pItem->object().cNameSect());
	
	const CTradeFactors		&trade_factors = *p_trade_factors;

	float					action_factor = 
		trade_factors.friend_factor() +
		(
			trade_factors.enemy_factor() -
			trade_factors.friend_factor()
		)*
		relation_factor;

	clamp					(action_factor,trade_factors.enemy_factor(),trade_factors.friend_factor());
	
	// computing deficit_factor
	float					deficit_factor = partner.inv_owner->deficit_factor(pItem->object().cNameSect());

	// total price calculation
	u32						result = 
		iFloor	(
			base_cost*
			condition_factor*
			action_factor*
			deficit_factor
		);

	return					(result);
}
