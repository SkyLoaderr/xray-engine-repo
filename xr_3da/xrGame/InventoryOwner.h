/////////////////////////////////////////////////////
// Для персонажей, имеющих инвентарь
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortion.h"
#include "PdaMsg.h"
#include "attachment_owner.h"
#include "script_space_forward.h"

class CScriptCallback;

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

	//////////////////////////////////////////////////////////////////////////
	// общие функции

	virtual BOOL	net_Spawn					(LPVOID DC);
	virtual void	net_Destroy					();
			void	Init						();
	virtual void	Load						(LPCSTR section);
	virtual void	reinit						();
	virtual void	reload						(LPCSTR section);
	virtual void	OnEvent						(NET_Packet& P, u16 type);

	//обновление
	virtual void	UpdateInventoryOwner		(u32 deltaT);

	// свойства
	u32					m_dwMoney;
	ALife::EStalkerRank	m_tRank;

	CPda* GetPDA		() const;
	bool IsActivePDA	() const;

	//функция через которую
	//другие персонажи отправляют объекту PDA сообщение.
	//должна быть переопределена в порожеденных классах
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index);
	//отправка сообщения другому владельцу PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index);

	// инвентарь
	CInventory	*m_inventory;			
	
	////////////////////////////////////
	//торговля и общение с персонажем

	//инициализация объекта торговли
	void InitTrade();
	CTrade* GetTrade();

	//для включения разговора
	virtual bool OfferTalk		(CInventoryOwner* talk_partner);
	virtual void StartTalk		(CInventoryOwner* talk_partner);
	virtual void StopTalk		();
	virtual bool IsTalking		();
	
	virtual void EnableTalk		()		{m_bAllowTalk = true;}
	virtual void DisableTalk	()		{m_bAllowTalk = false;}
	virtual bool IsTalkEnabled	()		{ return m_bAllowTalk;}

	CInventoryOwner* GetTalkPartner()	{return m_pTalkPartner;}
protected:
	// торговля
	CTrade*				m_pTrade;
	bool				m_bTalking; 
	CInventoryOwner*	m_pTalkPartner;

	bool				m_bAllowTalk;



	//////////////////////////////////////////////////////////////////////////
	// сюжетная информация
public:
	//персонаж получил новую порцию информации
	virtual void OnReceiveInfo	(INFO_ID info_index);
	//убрать информацию
	virtual void OnDisableInfo	(INFO_ID info_index);
	//передать/удалить информацию через сервер
	virtual void TransferInfo	(INFO_ID info_index, bool add_info) const;
	//есть ли информация у персонажа
	virtual bool HasInfo		(INFO_ID info_index) const;

	//возвращает существующий вектор из реестра, или добавляет новый
	KNOWN_INFO_VECTOR&			KnownInfo		();
	//возвращает NULL, если вектора с информацией не добавлено
	const KNOWN_INFO_VECTOR*	KnownInfoPtr	() const;
protected:

#ifdef _DEBUG
	//для отладки без alife simulator
	KNOWN_INFO_VECTOR m_KnowInfoWithoutAlife;
#endif	

	//////////////////////////////////////////////////////////////////////////
	// инвентарь 
public:
	const CInventory &inventory() const {return(*m_inventory);}
	CInventory		 &inventory()		{return(*m_inventory);}

	//возвращает текуший разброс стрельбы (в радианах) с учетом движения
	virtual float GetWeaponAccuracy			() const;
	//максимальный переносимы вес
	virtual float MaxCarryWeight			() const;

	//////////////////////////////////////////////////////////////////////////
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
	// связь со скриптами
	//////////////////////////////////////////////////////////////////////////
public:
	void	set_pda_callback	(const luabind::object &lua_object, LPCSTR method);
	void	set_pda_callback	(const luabind::functor<void> &lua_function);
	void	set_info_callback	(const luabind::object &lua_object, LPCSTR method);
	void	set_info_callback	(const luabind::functor<void> &lua_function);
	void	clear_pda_callback	();
	void	clear_info_callback	();

protected:
	CScriptCallback		*m_pPdaCallback;
	CScriptCallback		*m_pInfoCallback;
};

#include "inventory_owner_inline.h"