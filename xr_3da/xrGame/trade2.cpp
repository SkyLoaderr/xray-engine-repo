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

void CTrade::SellItem(int id)
{
	if (TradeState) m_dwLastTradeTime = Level().timeServer();
	else { Msg("I'm not ready to trade"); return;}

	PIItem	l_pIItem;
	int i=1;

	
	// id - � ������, �.�. ����� ������ ������� �� ������ � ������
	CInventory &pThisInv		= GetTradeInv(pThis);
	CInventory &pPartnerInv		= GetTradeInv(pPartner);

	TIItemContainer::iterator first_in_group_it;
	for (TIItemContainer::iterator  it = first_in_group_it = pThisInv.m_all.begin(); pThisInv.m_all.end() != it; ++it) {
		if ((it != first_in_group_it) && (xr_strcmp((*first_in_group_it)->Name(), (*it)->Name()) != 0)) {
			++i;
			first_in_group_it = it;
		}

		if (i == id) {
			l_pIItem = (*it);

			// ����� ������ �������� ������� �����������
			u32	dwTransferMoney = GetItemPrice(l_pIItem);

			if ((l_pIItem->Weight() + pPartnerInv.TotalWeight() < pPartnerInv.GetMaxWeight()) && 
				(std::find(pPartnerInv.m_all.begin(),pPartnerInv.m_all.end(),l_pIItem) == pPartnerInv.m_all.end()) && 
				(pPartner.inv_owner->m_dwMoney >= dwTransferMoney )) {
				
				if (xr_strcmp(l_pIItem->Name(),"Bolt") == 0) {
#ifdef DEBUG
					Msg("Cannot sell bolt!");
#endif					
					break;
				}

				// ��������� � ���� 
				NET_Packet				P;
				CGameObject				*O = smart_cast<CGameObject *>(pThis.inv_owner);
				O->u_EventGen			(P,GE_TRADE_SELL,O->ID());
				P.w_u16					(u16(l_pIItem->object().ID()));
				O->u_EventSend			(P);

				// �������� ���� �����
				pThis.inv_owner->m_dwMoney += dwTransferMoney;

				// ����� � ��������
				O						= smart_cast<CGameObject *>(pPartner.inv_owner);
				O->u_EventGen			(P,GE_TRADE_BUY,O->ID());
				P.w_u16					(u16(l_pIItem->object().ID()));
				O->u_EventSend			(P);

				// ��������� ����� � ��������
				pPartner.inv_owner->m_dwMoney -= dwTransferMoney;
#ifdef DEBUG
				Msg("--TRADE:: [%s]: Ok, item sold!",*pThis.base->cName());
#endif
				// On Trade Action callback
				if (pThis.type == TT_TRADER) {
					smart_cast<CAI_Trader*>(pThis.base)->callback(GameObject::eTradeSellBuyItem)(l_pIItem->object().lua_game_object(), true, dwTransferMoney);
				} else if (pPartner.type == TT_TRADER) {
					smart_cast<CAI_Trader*>(pPartner.base)->callback(GameObject::eTradeSellBuyItem)(l_pIItem->object().lua_game_object(), false, dwTransferMoney);
				}
			}
			break;
		}
	}
}

void CTrade::SellItem(CInventoryItem* pItem)
{
	// ����� ������ �������� ������� �����������
	// ����� ���� �� ������� �������, ��� ������ �� ����
	u32	dwTransferMoney;

/*	if (TT_ACTOR != pPartner.type) 
		dwTransferMoney = GetPartnerTrade()->GetItemPrice(pItem);
	else*/
	dwTransferMoney = GetItemPrice(pItem);


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
	if (pThis.type == TT_TRADER) 
	{
		pTrader = smart_cast<CAI_Trader*>(pThis.base);
		pTrader->callback(GameObject::eTradeSellBuyItem)(pItem->object().lua_game_object(), true, dwTransferMoney);
	}
	else if (pPartner.type == TT_TRADER) 
	{
		pTrader = smart_cast<CAI_Trader*>(pPartner.base);
		pTrader->callback(GameObject::eTradeSellBuyItem)(pItem->object().lua_game_object(), false, dwTransferMoney);
		
		CArtefact* pArtefact= smart_cast<CArtefact*>(pItem);
		if(pArtefact)
			m_bNeedToUpdateArtefactTasks |= pTrader->BuyArtefact(pArtefact);

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

	// ����������� ������������
	float factor	= 1.0f;
	u32 item_cost	= pItem->Cost();
	
	// condition
	// for "dead" weapon we use 10% from base cost, for "good" weapon we use full base cost
	item_cost		= iFloor	(float(item_cost)*powf(pItem->GetCondition()*0.9f + .1f, 0.75f) + .5f); 

	//��� ������ ���� ���� ������ ���������� 
	//��� �����������
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

	// ����� ������ �������� ������� �����������
	u32	dwTransferMoney = (u32)(((float) item_cost) * factor );

	return dwTransferMoney;
}
