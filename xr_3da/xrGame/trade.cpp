#include "stdafx.h"
#include "trade.h"
#include "inventory.h"
#include "actor.h"
#include "ai\\stalker\\ai_stalker.h"
#include "ai\\trader\\ai_trader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// CTrade class //////////////////////////////////////////////////////////////////////////
CTrade::CTrade(CInventoryOwner	*p_io) 
{
	pInvOwner = p_io;

	TradeState = false;
	pPartner.Set(TT_NONE,0);
	pThis.Set(TT_NONE,0);
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


void CTrade::StartTrade() 
{
	TradeState = true;
	Msg("Trade Started!!!");
}

bool CTrade::SetPartner(CEntity *p)
{
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	pTrader = dynamic_cast<CAI_Trader *>(p);
	if (pTrader && (pTrader != pThis.base))  pPartner.Set(TT_TRADER, pTrader);
	else {
		pActor = dynamic_cast<CActor *>(p);
		if (pActor && (pActor != pThis.base)) pPartner.Set(TT_ACTOR, pActor);
		else {
			pStalker = dynamic_cast<CAI_Stalker *>(p);
			if (pStalker && (pStalker != pThis.base)) pPartner.Set(TT_STALKER, pStalker);
			else return false;
		}
	}
	return true;
}

void CTrade::UpdateInventoryOwnerInfo()
{
	// Заполнить pThis
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	// Определяем потомка этого экземпляра класса
	pTrader = dynamic_cast<CAI_Trader *>(pInvOwner);
	if (pTrader) pThis.Set(TT_TRADER, pTrader);
	else {
		pActor = dynamic_cast<CActor *>(pInvOwner);
		if (pActor) pThis.Set(TT_ACTOR, pActor);
		else {
			pStalker = dynamic_cast<CAI_Stalker *>(pInvOwner);
			if (pStalker) pThis.Set(TT_STALKER, pStalker);
			else Msg("Incorrect Inventory owner!");
		}
	}
}
