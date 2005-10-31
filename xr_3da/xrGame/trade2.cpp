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

class CInventoryOwner;

bool CTrade::CanTrade()
{
	CEntity *pEntity;

	xr_vector<CObject*>		q_nearest;			// = Level().ObjectSpace.q_nearest; 

	Level().ObjectSpace.GetNearest	(q_nearest,pThis.base->Position(),2.f, NULL);
	if (!q_nearest.empty()) 
	{
		for (u32 i=0, n = q_nearest.size(); i<n; ++i) 
		{
			// Может ли объект торговать
			pEntity = smart_cast<CEntity *>(q_nearest[i]);
			if (pEntity && !pEntity->g_Alive()) return false;
			if (SetPartner(pEntity)) break;
		}
	} 
	
	if (!pPartner.base) return false;

	// Объект рядом
	float dist = pPartner.base->Position().distance_to(pThis.base->Position());
	if (dist < 0.5f || dist > 4.5f)  
	{
		RemovePartner();
		return false;
	}

	// Объект смотрит на меня
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
	// сумма сделки учитывая ценовой коэффициент
	// актер цену не говорит никогда, все делают за него
	u32	dwTransferMoney;

	dwTransferMoney = GetItemPrice(pItem);


	// выбросить у себя 
	NET_Packet				P;
	CGameObject				*O = smart_cast<CGameObject *>(pThis.inv_owner);
	O->u_EventGen			(P,GE_TRADE_SELL,O->ID());
	P.w_u16					(pItem->object().ID());
	O->u_EventSend			(P);

	// добавить себе денег
	pThis.inv_owner->m_dwMoney += dwTransferMoney;

	// взять у партнера
	O						= smart_cast<CGameObject *>(pPartner.inv_owner);
	O->u_EventGen			(P,GE_TRADE_BUY,O->ID());
	P.w_u16					(pItem->object().ID());
	O->u_EventSend			(P);

	// уменьшить денег у партнера
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

u32	CTrade::GetItemPrice(PIItem pItem)
{
	CArtefact* pArtefact = smart_cast<CArtefact*>(pItem);

	// определение коэффициента
	float factor	= 1.0f;
	u32 item_cost	= pItem->Cost();
	
	// condition
	// for "dead" weapon we use 10% from base cost, for "good" weapon we use full base cost
	item_cost		= iFloor	(float(item_cost)*powf(pItem->GetCondition()*0.9f + .1f, 0.75f) + .5f); 

	//для актера цену вещи всегда определяют 
	//его собеседники
	if (pThis.type == TT_ACTOR)
	{
		if(pPartner.type == TT_TRADER && pArtefact)
		{
			CAI_Trader* pTrader = smart_cast<CAI_Trader*>(pPartner.inv_owner); VERIFY(pTrader);
			if (pTrader)
				item_cost = pTrader->ArtefactPrice(pArtefact);
		}


		CHARACTER_GOODWILL attitude = RELATION_REGISTRY().GetAttitude(pPartner.inv_owner, pThis.inv_owner);
		float goodwill_factor;

		attitude += 100;
		
		if(NO_GOODWILL == attitude)
			goodwill_factor = 0.f;
		else
			goodwill_factor = float(attitude)/200.f;

		factor = m_tTradeFactors.fBuyFactorHostile +
			(m_tTradeFactors.fBuyFactorFriendly - m_tTradeFactors.fBuyFactorHostile)*goodwill_factor;
		clamp(factor, m_tTradeFactors.fBuyFactorHostile, m_tTradeFactors.fBuyFactorFriendly);
	}
	else if(pPartner.type == TT_ACTOR)
	{
		if(pThis.type == TT_TRADER && pArtefact)
		{
			CAI_Trader* pTrader = smart_cast<CAI_Trader*>(pThis.inv_owner); VERIFY(pTrader);
			if (pTrader)
				item_cost = pTrader->ArtefactPrice(pArtefact);
		}


		CHARACTER_GOODWILL attitude = RELATION_REGISTRY().GetAttitude(pPartner.inv_owner, pThis.inv_owner);

		attitude += 100;
		float goodwill_factor;

		if(NO_GOODWILL == attitude)
			goodwill_factor = 0.f;
		else
			goodwill_factor = float(attitude)/200.f;

		factor = m_tTradeFactors.fSellFactorFriendly +
			(m_tTradeFactors.fSellFactorHostile - m_tTradeFactors.fSellFactorFriendly) *goodwill_factor;
		clamp(factor, m_tTradeFactors.fSellFactorFriendly, m_tTradeFactors.fSellFactorHostile);

	}

	// сумма сделки учитывая ценовой коэффициент
	u32	dwTransferMoney = (u32)(((float) item_cost) * factor );

	return dwTransferMoney;
}
