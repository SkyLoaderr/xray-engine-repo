/////////////////////////////////////////////////////
// ƒл€ персонажей, имеющих инвентарь
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortion.h"
#include "PdaMsg.h"
#include "attachment_owner.h"
#include "ai_script_callback.h"


class CInventory;
class CInventoryItem;
class CTrade;
class CPda;
class CCharacterInfo;
class CEntityAlive;

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

	CPda* GetPDA		() const;
	bool IsActivePDA	() const;

	//функци€ через которую
	//другие персонажи отправл€ют объекту PDA сообщение.
	//должна быть переопределена в порожеденных классах
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index);
	//отправка сообщени€ другому владельцу PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index);


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
	
	void EnableTalk()		{m_bAllowTalk = true;}
	void DisableTalk()		{m_bAllowTalk = false;}
	bool IsTalkEnabled()	{ return m_bAllowTalk;}

	CInventoryOwner* GetTalkPartner() {return m_pTalkPartner;}
	//персонаж получил новую порцию информации
	virtual void OnReceiveInfo(INFO_ID info_index);
	//убрать информацию
	virtual void OnDisableInfo(INFO_ID info_index);
	//передать/удалить информацию через сервер
	virtual void TransferInfo(INFO_ID info_index, bool add_info) const;
	//есть ли информаци€ у персонажа
	virtual bool HasInfo(INFO_ID info_index) const;
	
	const CInventory &inventory() const {return(*m_inventory);}
	CInventory &inventory() {return(*m_inventory);}

	//возвращает текуший разброс стрельбы (в радианах) с учетом движени€
	virtual float GetWeaponAccuracy			() const;

protected:
	// торговл€
	CTrade				*m_pTrade;
	bool				m_bTalking; 
	CInventoryOwner		*m_pTalkPartner;

	bool				m_bAllowTalk;

	//игровые характеристики персонажа
public:
	virtual CCharacterInfo& CharacterInfo	() const {VERIFY(m_pCharacterInfo); return *m_pCharacterInfo;}
protected:
	CCharacterInfo*		m_pCharacterInfo;

public:
	virtual void			renderable_Render		();
	virtual void			OnItemTake				(CInventoryItem *inventory_item);
	virtual void			OnItemDrop				(CInventoryItem *inventory_item);
	virtual void			OnItemDropUpdate		() {}
	virtual bool			use_bolts				() const {return(true);}
	virtual	void			spawn_supplies			();

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

protected:
	CScriptCallback m_pPdaCallback;
	CScriptCallback m_pInfoCallback;
};

#include "inventory_owner_inline.h"