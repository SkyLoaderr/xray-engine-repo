/////////////////////////////////////////////////////
// ƒл€ персонажей, имеющих инвентарь
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortion.h"
#include "PdaMsg.h"
#include "attachment_owner.h"
#include "script_space.h"

class CInventory;
class CInventoryItem;
class CTrade;
class CPda;

class CInventoryOwner : public CAttachmentOwner {							
public:
					CInventoryOwner				();
	virtual			~CInventoryOwner			();

	virtual BOOL	net_Spawn					(LPVOID DC);
	virtual void	net_Destroy					();
			void	Init						();
	virtual void	Load						(LPCSTR section);
	virtual void	reinit						();
	virtual void	reload						(LPCSTR section);

	//обновление
	virtual void	UpdateInventoryOwner		(u32 deltaT);

	// свойства
	u32					m_dwMoney;
	ALife::EStalkerRank	m_tRank;

	CPda* GetPDA();
	bool IsActivePDA();

	//функци€ через которую
	//другие персонажи отправл€ют объекту PDA сообщение.
	//должна быть переопределена в порожеденных классах
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index);
	//отправка сообщени€ другому владельцу PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, int info_index);


	CInventory	*m_inventory;									// инвентарь
	CInventory	*m_trade_storage;								// склад 

	
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
	const CInventory &inventory() const {return(*m_inventory);}
	CInventory &inventory() {return(*m_inventory);}

	//возвращает текуший разброс стрельбы (в радианах) с учетом движени€
	virtual float GetWeaponAccuracy			() const;

protected:
	// торговл€
	CTrade				*m_pTrade;
	bool				m_bTalking; 
	CInventoryOwner		*m_pTalkPartner;




public:
	virtual void			renderable_Render		();
	virtual void			OnItemTake				(CInventoryItem *inventory_item);
	virtual void			OnItemDrop				(CInventoryItem *inventory_item);
	virtual void			OnItemDropUpdate		() {}
	virtual bool			use_bolts				() const {return(true);}

	//////////////////////////////////////////////////////////////////////////
	// св€зь со скриптами
	//////////////////////////////////////////////////////////////////////////
public:
	void	set_pda_callback	(const luabind::object &lua_object, LPCSTR method);
	void	set_pda_callback	(const luabind::functor<void> &lua_function);
	void	set_info_callback	(const luabind::object &lua_object, LPCSTR method);
	void	set_info_callback	(const luabind::functor<void> &lua_function);
	void	clear_pda_callback	();
	void	clear_info_callback	();

	void	zero_callbacks		();
	void	reset_callbacks		();
protected:
	SMemberCallback m_pPdaCallback;
	SMemberCallback m_pInfoCallback;
};

#include "inventory_owner_inline.h"