#include "stdafx.h"
#include "trade.h"
#include "inventory.h"
#include "actor.h"
#include "ai\\stalker\\ai_stalker.h"
#include "ai\\trader\\ai_trader.h"

//////////////////////////////////////////////////////////////////////////////////////////
// CTrade class //////////////////////////////////////////////////////////////////////////
CTrade::CTrade(CInventoryOwner	*p_io) 
{
	TradeState = false;
	pPartner.Set(TT_NONE,0,0);

	// Заполнить pThis
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	// Определяем потомка этого экземпляра класса
	pTrader = dynamic_cast<CAI_Trader *>(p_io);
	if (pTrader) pThis.Set(TT_TRADER, pTrader, p_io);
	else {
		pActor = dynamic_cast<CActor *>(p_io);
		if (pActor) pThis.Set(TT_ACTOR, pActor, p_io);
		else {
			pStalker = dynamic_cast<CAI_Stalker *>(p_io);
			if (pStalker) pThis.Set(TT_STALKER, pStalker, p_io);
			else Msg("Incorrect Inventory owner!");
		}
	}
	
}

CTrade::~CTrade()
{
	
}

bool CTrade::CanTrade()
{
	CEntity *pEntity;

	Level().ObjectSpace.GetNearest(pThis.base->Position(),2.f);
	if (!Level().ObjectSpace.q_nearest.empty()) {
		for (u32 i=0, n = Level().ObjectSpace.q_nearest.size(); i<n; i++) {
			// Может ли объект торговать
			pEntity = dynamic_cast<CEntity *>(Level().ObjectSpace.q_nearest[i]);
			if (SetPartner(pEntity)) break;
		}
	} 

	if (!pPartner.base) return false;

	// Объект рядом
	float dist = pPartner.base->Position().distance_to(pThis.base->Position());
	if (dist < 0.5f || dist > 1.5f) return false;

	// Объект смотрит на меня
	float yaw, pitch;
	float yaw2, pitch2;

	pThis.base->Direction().getHP(yaw,pitch);
	pPartner.base->Direction().getHP(yaw2,pitch2);
	yaw = angle_normalize(yaw);
	yaw2 = angle_normalize(yaw2);

	float Res = R2D(_abs(yaw - yaw2) < PI ? _abs(yaw - yaw2) : PI_MUL_2 - _abs(yaw - yaw2));
	if (Res < 165.f || Res > 195.f) return false;

	return true;
}


void CTrade::Communicate() 
{
	// Вывести приветствие
	Msg("--TRADE::----------------------------------------------");
	Msg("--TRADE::          TRADE ACIVATED                      ");
	Msg("--TRADE::----------------------------------------------");
	Msg("--TRADE:: - Hello, my name [%s]", pThis.base->cName());
	Msg("--TRADE::   Wanna trade with me?" );

	if (pPartner.inv_owner->m_trade->OfferTrade(pThis)) { 
		//StartTrade();
		//pPartner.inv_owner->m_trade->StartTrade();
	}

}

bool CTrade::SetPartner(CEntity *p)
{
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	pTrader = dynamic_cast<CAI_Trader *>(p);
	if (pTrader && (pTrader != pThis.base))  pPartner.Set(TT_TRADER, pTrader, pTrader);
	else {
		pActor = dynamic_cast<CActor *>(p);
		if (pActor && (pActor != pThis.base)) pPartner.Set(TT_ACTOR, pActor, pActor);
		else {
			pStalker = dynamic_cast<CAI_Stalker *>(p);
			if (pStalker && (pStalker != pThis.base)) pPartner.Set(TT_STALKER, pStalker, pStalker);
			else return false;
		}
	}
	return true;

}

// Man предлагает торговать 
// возвращает true, если данный trader готов торговать с man
bool CTrade::OfferTrade(SInventoryOwner man)
{
	pPartner.Set(man.type,man.base,man.inv_owner); 
	Msg("--TRADE:: - My name is [%s]", pThis.base->cName());
	Msg("--TRADE:: [%s]: I know smth about you...", pThis.base->cName());
	Msg("--TRADE:: [%s]: a. Your name is %s", pThis.base->cName(),pPartner.base->cName());
	
	string64	s;
	switch (pPartner.type) {
		case TT_TRADER: strcpy(s, "trader"); break;
		case TT_STALKER: 
		case TT_ACTOR: strcpy(s, "stalker"); break;
	}
	
	Msg("--TRADE:: [%s]: b. You are a %s", pThis.base->cName(),s);
	
	switch (pPartner.inv_owner->m_tRank) {
		case eStalkerRankNone: strcpy(s,"NO_RANK"); break;
		case eStalkerRankNovice: strcpy(s,"NOVICE"); break;
		case eStalkerRankExperienced: strcpy(s,"EXPERIENCED"); break;
		case eStalkerRankVeteran: strcpy(s,"VETERAN"); break;
		case eStalkerRankMaster: strcpy(s,"MASTER"); break;
		case eStalkerRankDummy: strcpy(s,"DUMMY"); break;
	}
	
	Msg("--TRADE:: [%s]: c. Your rank is %s", pThis.base->cName(),s);
	Msg("--TRADE:: [%s]: d. You have %i money", pThis.base->cName(),pPartner.inv_owner->m_dwMoney);
	strcpy(s,"POSITIVE");
	Msg("--TRADE:: [%s]: e. My attitude to you is %s", pThis.base->cName(),s);
	Msg("--TRADE:: [%s]: So, lets trade...",pThis.base->cName());

	return true;
}


void CTrade::StartTrade()
{
	TradeState = true;
}

void CTrade::StopTrade()
{
	TradeState = false;
}


void CTrade::ShowItems()
{

	Msg("--TRADE:: ----------------------------------------------");
	Msg("--TRADE:: [%s]: Here are my items: ",pThis.base->cName());

	int i=0;
	for (PSPIItem it = pThis.inv_owner->m_inventory.m_all.begin(); it != pThis.inv_owner->m_inventory.m_all.end(); it ++, i++) {
		Msg("--TRADE:: [%s]: %i. %s",pThis.base->cName(),i,(*it)->NameShort());
	}
	
}

void CTrade::ShowMoney()
{
	Msg("--TRADE:: [%s]: Money = %i ",pThis.base->cName(),pThis.inv_owner->m_dwMoney);
}
