/////////////////////////////////////////////////////
// ƒл€ персонажей, имеющих инвентарь
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "inventory.h"


class CTrade;
class CPda;


//типы сообщений PDA
enum EPdaMsg {
	ePdaMsgTrade  = u32(0),		//торговать
	ePdaMsgGoAway,				//убирайс€ с территории
	ePdaMsgNeedHelp,			//просьба о помощи
	
	ePdaMsgAccept,				//прин€ть предложение
	ePdaMsgDecline,				//отказатьс€
	ePdaMsgDeclineRude,			//грубо отказатьс€
	ePdaMessageMax
};

//уровень "злости" в сообщении
enum EPdaMsgAnger {
	ePdaMsgAngerNone = u32(0),
	ePdaMsgAngerLow,
	ePdaMsgAngerHigh,
	ePdaMsgAngerMax
};

class CInventoryOwner		
{							
public:
	CInventoryOwner();
	virtual ~CInventoryOwner();

	virtual BOOL net_Spawn		(LPVOID DC);


	// свойства
	u32					m_dwMoney;
	EStalkerRank		m_tRank;

	CPda* GetPDA();
	bool IsActivePDA();

	//функци€ через которую
	//другие персонажи отправл€ют объекту PDA сообщение.
	//должна быть переопределена в порожеденных классах
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger);
	//отправка сообщени€ другому владельцу PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger);


	CInventory	m_inventory;									// »нвентарь
	CInventory	m_trade_storage;								// склад 

	void InitTrade();
	CTrade* GetTrade();
protected:
	// торговл€
	CTrade*	m_pTrade;
	
};
