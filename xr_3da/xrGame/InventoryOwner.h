/////////////////////////////////////////////////////
// ƒл€ персонажей, имеющих инвентарь
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "inventory.h"

#include "InfoPortion.h"
#include "PdaMsg.h"

class CTrade;
class CPda;



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
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index);
	//отправка сообщени€ другому владельцу PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, int info_index);


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
	virtual void OnReceiveInfo(int /**info_index/**/) {};

	//игровые характеристики персонажа
	virtual LPCSTR GetGameName();
	virtual LPCSTR GetGameRank();
	virtual LPCSTR GetGameCommunity();

protected:
	// торговл€
	CTrade*	m_pTrade;

	bool m_bTalking; 
	CInventoryOwner* m_pTalkPartner;
};
