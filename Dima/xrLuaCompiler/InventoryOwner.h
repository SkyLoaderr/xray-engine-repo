/////////////////////////////////////////////////////
// ƒл€ персонажей, имеющих инвентарь
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "inventory.h"

#include "InfoPortion.h"

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

	//обновление
	virtual void UpdateInventoryOwner(u32 deltaT);

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


	CInventory	m_inventory;									// инвентарь
	CInventory	m_trade_storage;								// склад 

	
	////////////////////////////////////
	//торговл€ и общение с персонажем

	//инициализаци€ объекта торговли
	void InitTrade();
	CTrade* GetTrade();

	//дл€ включени€ разговора
	virtual bool OfferTalk(CInventoryOwner* talk_partner);
	void StartTalk(CInventoryOwner* talk_partner);
	void StopTalk();
	bool IsTalking();
	CInventoryOwner* GetTalkPartner() {return m_pTalkPartner;}
	//партнер задает вопрос, если персонаж захочет отвечать,
	//то он вернет всю информацию, что у него есть по этой теме
	//false - если отказываемс€ говорить или информации нет
	//список индексов выданной информации в index_list
	virtual bool AskQuestion(SInfoQuestion& question, INFO_INDEX_LIST& index_list);
	//персонаж получил новую порцию информации
	virtual void OnReceiveInfo(int info_index) {};



protected:
	// торговл€
	CTrade*	m_pTrade;

	bool m_bTalking; 
	CInventoryOwner* m_pTalkPartner;
};
