/////////////////////////////////////////////////////
// ��� ����������, ������� ���������
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortionDefs.h"
#include "pda_space.h"
#include "attachment_owner.h"
#include "script_space_forward.h"
#include "character_info_defs.h"

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
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_INDEX info_index);
	//�������� ��������� ������� ��������� PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, INFO_INDEX info_index);

	// ���������
	CInventory	*m_inventory;			
	
	////////////////////////////////////
	//�������� � ������� � ����������


	//������������� ������� ��������
	void InitTrade();
	CTrade* GetTrade();

	//��� ��������� ���������
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
	virtual bool OnReceiveInfo	(INFO_INDEX info_index);
	//������ ����������
	virtual void OnDisableInfo	(INFO_INDEX info_index);
	//��������/������� ���������� ����� ������
	virtual void TransferInfo	(INFO_INDEX info_index, bool add_info) const;
	//���� �� ���������� � ���������
	virtual bool HasInfo		(INFO_INDEX info_index) const;


	CInfoPortionWrapper			*m_known_info_registry;

	//////////////////////////////////////////////////////////////////////////
	// ��������� 
public:
	const CInventory &inventory() const {VERIFY (m_inventory); return(*m_inventory);}
	CInventory		 &inventory()		{VERIFY (m_inventory); return(*m_inventory);}

	//���������� ������� ������� �������� (� ��������) � ������ ��������
	virtual float GetWeaponAccuracy			() const;
	//������������ ���������� ���
	virtual float MaxCarryWeight			() const;

	//////////////////////////////////////////////////////////////////////////
	// ������ � �����������
	virtual void FoundZone				(CCustomZone*);
	virtual void LostZone				(CCustomZone*);

	//////////////////////////////////////////////////////////////////////////
	//������� �������������� ���������
public:
	virtual CCharacterInfo& CharacterInfo	() const {VERIFY(m_pCharacterInfo); return *m_pCharacterInfo;}
	//��������� ����������� �� ���������� � ��������� ������
	virtual void			SetCommunity	(CHARACTER_COMMUNITY_INDEX);
	virtual void			SetRank			(CHARACTER_RANK_VALUE);
	virtual void			ChangeRank		(CHARACTER_RANK_VALUE);
	virtual void			SetReputation	(CHARACTER_REPUTATION_VALUE);
	virtual void			ChangeReputation(CHARACTER_REPUTATION_VALUE);

	//��� ������ � relation system
	u16								object_id	() const;
	CHARACTER_COMMUNITY_INDEX		Community	() const;
	CHARACTER_RANK_VALUE			Rank		() const;
	CHARACTER_REPUTATION_VALUE		Reputation	() const;

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