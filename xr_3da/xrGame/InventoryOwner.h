/////////////////////////////////////////////////////
// Для персонажей, имеющих инвентарь
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortionDefs.h"
#include "pda_space.h"
#include "attachment_owner.h"
#include "script_space_forward.h"

class CScriptCallback;
class CInventory;
class CInventoryItem;
class CTrade;
class CPda;
class CCharacterInfo;
class CGameObject;
class CEntityAlive;
class CCustomZone;
class CInfoPortionWrapper;
class NET_Packet;

class CInventoryOwner : public CAttachmentOwner {							
public:
					CInventoryOwner				();
	virtual			~CInventoryOwner			();

public:
	virtual CInventoryOwner*	cast_inventory_owner	()						{return this;}
public:

	//////////////////////////////////////////////////////////////////////////
	// общие функции

	virtual BOOL	net_Spawn					(LPVOID DC);
	virtual void	net_Destroy					();
			void	Init						();
	virtual void	Load						(LPCSTR section);
	virtual void	reinit						();
	virtual void	reload						(LPCSTR section);
	virtual void	OnEvent						(NET_Packet& P, u16 type);

	//serialization
	virtual void	save						(NET_Packet &output_packet);
	virtual void	load						(IReader &input_packet);

	
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
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_INDEX info_index);
	//отправка сообщения другому владельцу PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, INFO_INDEX info_index);

	// инвентарь
	CInventory	*m_inventory;			
	
	////////////////////////////////////
	//торговля и общение с персонажем


	//инициализация объекта торговли
	void InitTrade();
	CTrade* GetTrade();

	//для включения разговора
	virtual bool OfferTalk		(CInventoryOwner* talk_partner);
	virtual void StartTalk		(CInventoryOwner* talk_partner, bool start_trade = true);
	virtual void StopTalk		();
	virtual bool IsTalking		();
	
	virtual void EnableTalk		()		{m_bAllowTalk = true;}
	virtual void DisableTalk	()		{m_bAllowTalk = false;}
	virtual bool IsTalkEnabled	()		{ return m_bAllowTalk;}

	CInventoryOwner* GetTalkPartner()	{return m_pTalkPartner;}
	virtual void	 NewPdaContact		(CInventoryOwner*);
	virtual void	 LostPdaContact		(CInventoryOwner*);

	//игровое имя 
	virtual LPCSTR	Name        () const;

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
	virtual bool OnReceiveInfo	(INFO_INDEX info_index);
	//убрать информацию
	virtual void OnDisableInfo	(INFO_INDEX info_index);
	//передать/удалить информацию через сервер
	virtual void TransferInfo	(INFO_INDEX info_index, bool add_info) const;
	//есть ли информация у персонажа
	virtual bool HasInfo		(INFO_INDEX info_index) const;


	CInfoPortionWrapper			*m_known_info_registry;

	//////////////////////////////////////////////////////////////////////////
	// инвентарь 
public:
	const CInventory &inventory() const {VERIFY (m_inventory); return(*m_inventory);}
	CInventory		 &inventory()		{VERIFY (m_inventory); return(*m_inventory);}

	//возвращает текуший разброс стрельбы (в радианах) с учетом движения
	virtual float GetWeaponAccuracy			() const;
	//максимальный переносимы вес
	virtual float MaxCarryWeight			() const;

	//////////////////////////////////////////////////////////////////////////
	// работа с детекторами
	virtual void FoundZone				(CCustomZone*);
	virtual void LostZone				(CCustomZone*);

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