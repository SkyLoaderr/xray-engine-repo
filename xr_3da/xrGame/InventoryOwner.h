/////////////////////////////////////////////////////
// ��� ����������, ������� ���������
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortion.h"
#include "PdaMsg.h"
#include "attachment_owner.h"
#include "script_space_forward.h"
#include "alife_registry_container.h"
#include "alife_registry_wrapper.h"

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
	// ����� �������

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

	
	//����������
	virtual void	UpdateInventoryOwner		(u32 deltaT);

	// ��������
	u32					m_dwMoney;
	ALife::EStalkerRank	m_tRank;

	CPda* GetPDA		() const;
	bool IsActivePDA	() const;

	//������� ����� �������
	//������ ��������� ���������� ������� PDA ���������.
	//������ ���� �������������� � ������������ �������
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index);
	//�������� ��������� ������� ��������� PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index);

	// ���������
	CInventory	*m_inventory;			
	
	////////////////////////////////////
	//�������� � ������� � ����������


	//������������� ������� ��������
	void InitTrade();
	CTrade* GetTrade();

	//��� ��������� ���������
	virtual bool OfferTalk		(CInventoryOwner* talk_partner);
	virtual void StartTalk		(CInventoryOwner* talk_partner);
	virtual void StopTalk		();
	virtual bool IsTalking		();
	
	virtual void EnableTalk		()		{m_bAllowTalk = true;}
	virtual void DisableTalk	()		{m_bAllowTalk = false;}
	virtual bool IsTalkEnabled	()		{ return m_bAllowTalk;}

	CInventoryOwner* GetTalkPartner()	{return m_pTalkPartner;}

	//������� ��� 
	virtual LPCSTR	Name        () const;

protected:
	// ��������
	CTrade*				m_pTrade;
	bool				m_bTalking; 
	CInventoryOwner*	m_pTalkPartner;

	bool				m_bAllowTalk;



	//////////////////////////////////////////////////////////////////////////
	// �������� ����������
public:
	//�������� ������� ����� ������ ����������
	virtual void OnReceiveInfo	(INFO_ID info_index);
	//������ ����������
	virtual void OnDisableInfo	(INFO_ID info_index);
	//��������/������� ���������� ����� ������
	virtual void TransferInfo	(INFO_ID info_index, bool add_info) const;
	//���� �� ���������� � ���������
	virtual bool HasInfo		(INFO_ID info_index) const;


	typedef CALifeRegistryWrapper<CInfoPortionRegistry> KNOWN_INFO_REGISTRY;
	KNOWN_INFO_REGISTRY known_info_registry;

	//////////////////////////////////////////////////////////////////////////
	// ��������� 
public:
	const CInventory &inventory() const {return(*m_inventory);}
	CInventory		 &inventory()		{return(*m_inventory);}

	//���������� ������� ������� �������� (� ��������) � ������ ��������
	virtual float GetWeaponAccuracy			() const;
	//������������ ���������� ���
	virtual float MaxCarryWeight			() const;

	//////////////////////////////////////////////////////////////////////////
	//������� �������������� ���������
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
	// ����� �� ���������
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