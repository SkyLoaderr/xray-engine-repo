#include "stdafx.h"
#include "trade.h"
#include "actor.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/trader/ai_trader.h"
#include "artifact.h"
#include "inventory.h"
#include "xrmessages.h"

class CInventoryOwner;

TradeFactors CTrade::m_tTradeFactors;

//////////////////////////////////////////////////////////////////////////////////////////
// CTrade class //////////////////////////////////////////////////////////////////////////
CTrade::CTrade(CInventoryOwner	*p_io) 
{
	TradeState = false;
	m_dwLastTradeTime	= 0;
	pPartner.Set(TT_NONE,0,0);

	// Заполнить pThis
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	// Определяем потомка этого экземпляра класса
	pTrader = dynamic_cast<CAI_Trader *>(p_io);
	if (pTrader) 
		pThis.Set(TT_TRADER, pTrader, p_io);
	else 
	{
		pActor = dynamic_cast<CActor *>(p_io);
		if (pActor) 
			pThis.Set(TT_ACTOR, pActor, p_io);
		else 
		{
			pStalker = dynamic_cast<CAI_Stalker *>(p_io);
			if (pStalker) pThis.Set(TT_STALKER, pStalker, p_io);
			else Msg("Incorrect Inventory owner!");
		}
	}
	
	// Загрузка коэффициентов торговли
	if (!m_tTradeFactors.Loaded) 
	{
		m_tTradeFactors.TraderBuyPriceFactor			= pSettings->r_float("trade","trader_buy_price_factor");
		m_tTradeFactors.TraderSellPriceFactor			= pSettings->r_float("trade","trader_sell_price_factor");
		m_tTradeFactors.StalkerNeutralBuyPriceFactor	= pSettings->r_float("trade","stalker_neutral_buy_price_factor");
		m_tTradeFactors.StalkerNeutralSellPriceFactor	= pSettings->r_float("trade","stalker_neutral_sell_price_factor");
		m_tTradeFactors.StalkerFriendBuyPriceFactor		= pSettings->r_float("trade","stalker_friend_buy_price_factor");
		m_tTradeFactors.StalkerFriendSellPriceFactor	= pSettings->r_float("trade","stalker_friend_sell_price_factor");
		m_tTradeFactors.Loaded = true;
	}
}

CTrade::~CTrade()
{
	
}

bool CTrade::CanTrade()
{
	CEntity *pEntity;

	Level().ObjectSpace.GetNearest(pThis.base->Position(),2.f);
	if (!Level().ObjectSpace.q_nearest.empty()) 
	{
		for (u32 i=0, n = Level().ObjectSpace.q_nearest.size(); i<n; ++i) 
		{
			// Может ли объект торговать
			pEntity = dynamic_cast<CEntity *>(Level().ObjectSpace.q_nearest[i]);
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

	float Res = R2D(_abs(yaw - yaw2) < PI ? _abs(yaw - yaw2) : 
								 PI_MUL_2 - _abs(yaw - yaw2));
	if (Res < 165.f || Res > 195.f) 
	{
		RemovePartner();
		return false;
	}

	return true;
}

void CTrade::RemovePartner()
{
	pPartner.Set(TT_NONE,0,0);
}
// предложение торговли
void CTrade::Communicate() 
{
	// Вывести приветствие
	Msg("--TRADE::----------------------------------------------");
	Msg("--TRADE::          TRADE ACIVATED                      ");
	Msg("--TRADE::----------------------------------------------");
	Msg("--TRADE:: - Hello, my name is [%s]", *pThis.base->cName());
	Msg("--TRADE::   Wanna trade with me?" );

	if (pPartner.inv_owner->GetTrade()->OfferTrade(pThis)) { 
		StartTrade();
	}

}

bool CTrade::SetPartner(CEntity *p)
{
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	pTrader = dynamic_cast<CAI_Trader *>(p);
	if (pTrader && (pTrader != pThis.base))  
		pPartner.Set(TT_TRADER, pTrader, pTrader);
	else 
	{
		pActor = dynamic_cast<CActor *>(p);
		if (pActor && (pActor != pThis.base)) 
			pPartner.Set(TT_ACTOR, pActor, pActor);
		else 
		{
			pStalker = dynamic_cast<CAI_Stalker *>(p);
			if (pStalker && (pStalker != pThis.base)) 
				pPartner.Set(TT_STALKER, pStalker, pStalker);
			else return false;
		}
	}
	return true;
}


// Man предлагает торговать 
// возвращает true, если данный trader готов торговать с man
// т.е. принятие торговли
bool CTrade::OfferTrade(SInventoryOwner man)
{
	StartTrade();
	pPartner.Set(man.type,man.base,man.inv_owner); 
	Msg("--TRADE:: - My name is [%s]", *pThis.base->cName());
	Msg("--TRADE:: [%s]: I know smth about you...", *pThis.base->cName());
	Msg("--TRADE:: [%s]: a. Your name is %s", *pThis.base->cName(),pPartner.base->cName());
	
	string64	s;
	switch (pPartner.type) 
	{
		case TT_TRADER: strcpy(s, "trader"); break;
		case TT_STALKER: 
		case TT_ACTOR: strcpy(s, "stalker"); break;
	}
	
	Msg("--TRADE:: [%s]: b. You are a %s", *pThis.base->cName(),s);
	
	switch (pPartner.inv_owner->m_tRank) 
	{
		case ALife::eStalkerRankNone: strcpy(s,"NO_RANK"); break;
		case ALife::eStalkerRankNovice: strcpy(s,"NOVICE"); break;
		case ALife::eStalkerRankExperienced: strcpy(s,"EXPERIENCED"); break;
		case ALife::eStalkerRankVeteran: strcpy(s,"VETERAN"); break;
		case ALife::eStalkerRankMaster: strcpy(s,"MASTER"); break;
		case ALife::eStalkerRankDummy: strcpy(s,"DUMMY"); break;
	}
	
	Msg("--TRADE:: [%s]: c. Your rank is %s", *pThis.base->cName(),s);
	Msg("--TRADE:: [%s]: d. You have %i money", *pThis.base->cName(),pPartner.inv_owner->m_dwMoney);
	strcpy(s,"POSITIVE");
	Msg("--TRADE:: [%s]: e. My attitude to you is %s", *pThis.base->cName(),s);
	Msg("--TRADE:: [%s]: So, lets trade...",*pThis.base->cName());

	return true;
}


void CTrade::StartTrade()
{
	TradeState = true;
	m_dwLastTradeTime =  Level().timeServer();
}

void CTrade::StartTrade(CInventoryOwner* pInvOwner)
{
	SetPartner(dynamic_cast<CEntity*>(pInvOwner));
	StartTrade();

	if (pThis.type == TT_TRADER) dynamic_cast<CAI_Trader*>(pThis.base)->OnStartTrade();
}

void CTrade::StopTrade()
{
	TradeState = false;
	m_dwLastTradeTime = 0;
	RemovePartner();
	Msg("--TRADE:: [%s]: Trade stopped...",*pThis.base->cName());

	if (pThis.type == TT_TRADER) dynamic_cast<CAI_Trader*>(pThis.base)->OnStopTrade();
}

void CTrade::UpdateTrade()
{
	/*if (TradeState) 
	{
		u32 cur_time = Level().timeServer();
		
		// если через 10 сек нет команды - прервать
		if (m_dwLastTradeTime + 20000 < cur_time) 
		{ 
			StopTrade();
		}
	}*/
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Commands
void CTrade::ShowItems()
{
	if (TradeState) m_dwLastTradeTime = Level().timeServer();
	else { Msg("I'm not ready to trade"); return;}

	Msg("--TRADE:: ----------------------------------------------");
	Msg("--TRADE:: [%s]: Here are my items: ",*pThis.base->cName());

	// определение коэффициента
	float factor = 1.0f;
	
	if (pThis.type == TT_TRADER) {
		factor = m_tTradeFactors.TraderSellPriceFactor;
	} else {
		if (pPartner.type == TT_TRADER) {
			factor = m_tTradeFactors.TraderBuyPriceFactor;
		} else if (pPartner.type == TT_STALKER) {
			CEntityAlive	*pEAThis = dynamic_cast<CEntityAlive*> (pThis.base);
			CEntityAlive    *pPAPartner = dynamic_cast<CEntityAlive*> (pPartner.base);
			if (pEAThis->tfGetRelationType(pPAPartner) == ALife::eRelationTypeFriend) {
				factor = m_tTradeFactors.StalkerFriendBuyPriceFactor;
			} else factor = m_tTradeFactors.StalkerNeutralBuyPriceFactor;
		}
	}

	string128	CurName;
	u32			num = 0;
	u32			i = 0;
	u32			l_dwCost = 0;
	CurName[0]	= 0;

	PSPIItem	B = GetTradeInv(pThis).m_all.begin(), I = B;
	PSPIItem	E = GetTradeInv(pThis).m_all.end();

	for ( ; I != E; ++I) {
		if (0 != xr_strcmp(CurName, (*I)->Name())) {
			if (!num) {
				Msg("--TRADE:: [%s]: %i. %s ($%i) /%i items/ ",
					*pThis.base->cName(),++i,
					CurName,l_dwCost, num);
			} 
			l_dwCost = (int)(((float) (*I)->Cost()) * factor );
			num = 1;
			strcpy(CurName,(*I)->Name());
		} else {
			++num;
		}
	}

	// Вывести последний item
	if (B != E) 
		Msg("--TRADE:: [%s]: %i. %s ($%i) /%i items/ ",*pThis.base->cName(),++i,CurName,l_dwCost, num);
	
}

void CTrade::ShowMoney()
{
	if (TradeState) m_dwLastTradeTime = Level().timeServer();
	else { Msg("I'm not ready to trade"); return;}

	Msg("--TRADE:: [%s]: Money = %i ",*pThis.base->cName(),pThis.inv_owner->m_dwMoney);
}

void CTrade::ShowArtifactPrices()
{
	if (TradeState) m_dwLastTradeTime = Level().timeServer();
	else { Msg("I'm not ready to trade"); return;}

	if (pThis.type == TT_TRADER) 
	{
		CAI_Trader					*l_pTrader = dynamic_cast<CAI_Trader *>(pThis.inv_owner);
		R_ASSERT					(l_pTrader);
		Msg							("--TRADE:: [%s]: I need the following artefacts :",*pThis.base->cName());
		ALife::ARTEFACT_TRADER_ORDER_PAIR_IT	ii = l_pTrader->m_tpOrderedArtefacts.begin();
		ALife::ARTEFACT_TRADER_ORDER_PAIR_IT	ee = l_pTrader->m_tpOrderedArtefacts.end();
		for ( ; ii != ee; ++ii) {
			u32						l_dwAlreadyPurchased = GetTradeInv(pThis).dwfGetSameItemCount((*ii).second->m_caSection);
			R_ASSERT				(l_dwAlreadyPurchased <= (*ii).second->m_dwTotalCount);
			Msg						("-   Artefact %s (total %d items) :",pSettings->r_string((*ii).second->m_caSection,"inv_name"),(*ii).second->m_dwTotalCount - l_dwAlreadyPurchased);
			ALife::ARTEFACT_ORDER_IT	iii = (*ii).second->m_tpOrders.begin();
			ALife::ARTEFACT_ORDER_IT	eee = (*ii).second->m_tpOrders.end();
			for ( ; iii != eee; ++iii)
				if (l_dwAlreadyPurchased < (*iii).m_count) {
					Msg				("-       %d items for $%d for organization %s",(*iii).m_count - l_dwAlreadyPurchased,(*iii).m_price,(*iii).m_section);
					l_dwAlreadyPurchased = 0;
				}else
					l_dwAlreadyPurchased -= (*iii).m_count;
		}
	}
	else 
		Msg("--TRADE:: [%s]: I don't buy artefacts! Go to trader",*pThis.base->cName());
}

void CTrade::SellItem(int id)
{
	if (TradeState) m_dwLastTradeTime = Level().timeServer();
	else { Msg("I'm not ready to trade"); return;}

	PIItem	l_pIItem;
	int i=1;

	
	// id - в списке, т.е. найти первый элемент из группы в списке
	CInventory &pThisInv		= GetTradeInv(pThis);
	CInventory &pPartnerInv		= GetTradeInv(pPartner);

	PSPIItem first_in_group_it;
	for (PSPIItem  it = first_in_group_it = pThisInv.m_all.begin(); pThisInv.m_all.end() != it; ++it) {
		if ((it != first_in_group_it) && (xr_strcmp((*first_in_group_it)->Name(), (*it)->Name()) != 0)) {
			++i;
			first_in_group_it = it;
		}

		if (i == id) {
			l_pIItem = (*it);

			// сумма сделки учитывая ценовой коэффициент
			u32	dwTransferMoney = GetItemPrice(l_pIItem);

			if ((l_pIItem->Weight() + pPartnerInv.TotalWeight() < pPartnerInv.GetMaxWeight()) && 
				(pPartnerInv.m_all.find(l_pIItem) == pPartnerInv.m_all.end()) && 
				(pPartner.inv_owner->m_dwMoney >= dwTransferMoney )) {
				
				if (xr_strcmp(l_pIItem->Name(),"Bolt") == 0) {
					Msg("Cannot sell bolt!");
					break;
				}

				// выбросить у себя 
				NET_Packet				P;
				CGameObject				*O = dynamic_cast<CGameObject *>(pThis.inv_owner);
				O->u_EventGen			(P,GE_TRADE_SELL,O->ID());
				P.w_u16					(u16(l_pIItem->ID()));
				O->u_EventSend			(P);

				// добавить себе денег
				pThis.inv_owner->m_dwMoney += dwTransferMoney;

				// взять у партнера
				O						= dynamic_cast<CGameObject *>(pPartner.inv_owner);
				O->u_EventGen			(P,GE_TRADE_BUY,O->ID());
				P.w_u16					(u16(l_pIItem->ID()));
				O->u_EventSend			(P);

				// уменьшить денег у партнера
				pPartner.inv_owner->m_dwMoney -= dwTransferMoney;
				
				Msg("--TRADE:: [%s]: Ok, item sold!",*pThis.base->cName());

				if (pThis.type == TT_TRADER) dynamic_cast<CAI_Trader*>(pThis.base)->OnTradeAction(l_pIItem, true);
				else if (pPartner.type == TT_TRADER) dynamic_cast<CAI_Trader*>(pPartner.base)->OnTradeAction(l_pIItem, false);
			}
			break;
		}
	}
}

void CTrade::SellItem(CInventoryItem* pItem)
{
	// сумма сделки учитывая ценовой коэффициент
	// актер цену не говорит никогда, все делают за него
	u32	dwTransferMoney;

	if (TT_ACTOR != pPartner.type) 
		dwTransferMoney = GetPartnerTrade()->GetItemPrice(pItem);
	else
		dwTransferMoney = GetItemPrice(pItem);


	// выбросить у себя 
	NET_Packet				P;
	CGameObject				*O = dynamic_cast<CGameObject *>(pThis.inv_owner);
	O->u_EventGen			(P,GE_TRADE_SELL,O->ID());
	P.w_u16					(u16(pItem->ID()));
	O->u_EventSend			(P);

	// добавить себе денег
	pThis.inv_owner->m_dwMoney += dwTransferMoney;

	// взять у партнера
	O						= dynamic_cast<CGameObject *>(pPartner.inv_owner);
	O->u_EventGen			(P,GE_TRADE_BUY,O->ID());
	P.w_u16					(u16(pItem->ID()));
	O->u_EventSend			(P);

	// уменьшить денег у партнера
	pPartner.inv_owner->m_dwMoney -= dwTransferMoney;

	if (pThis.type == TT_TRADER) dynamic_cast<CAI_Trader*>(pThis.base)->OnTradeAction(pItem, true);
	else if (pPartner.type == TT_TRADER) dynamic_cast<CAI_Trader*>(pPartner.base)->OnTradeAction(pItem, false);
}


CInventory& CTrade::GetTradeInv(SInventoryOwner owner)
{
	R_ASSERT(TT_NONE != owner.type);

	return ((TT_TRADER == owner.type) ? (*owner.inv_owner->m_trade_storage) : (owner.inv_owner->inventory()));
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
	// определение коэффициента
	float factor = 1.0f;
	
	if (pThis.type == TT_TRADER) 
	{
		factor = m_tTradeFactors.TraderSellPriceFactor;
	}
	else 
	{
		if (pPartner.type == TT_TRADER) 
		{
			factor = m_tTradeFactors.TraderBuyPriceFactor;
		} 
		else if (pPartner.type == TT_STALKER) 
		{
			CEntityAlive	*pEAThis = dynamic_cast<CEntityAlive*> (pThis.base);
			CEntityAlive    *pPAPartner = dynamic_cast<CEntityAlive*> (pPartner.base);
			if (pEAThis->tfGetRelationType(pPAPartner) == ALife::eRelationTypeFriend) 
			{
				factor = m_tTradeFactors.StalkerFriendBuyPriceFactor;
			} 
			else 
				factor = m_tTradeFactors.StalkerNeutralBuyPriceFactor;
		}
	}

	// сумма сделки учитывая ценовой коэффициент
	u32	dwTransferMoney = (u32)(((float) pItem->Cost()) * factor );

	return dwTransferMoney;
}