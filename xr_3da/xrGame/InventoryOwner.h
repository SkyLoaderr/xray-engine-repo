/////////////////////////////////////////////////////
// ��� ����������, ������� ���������
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "InfoPortion.h"
#include "PdaMsg.h"
#include "attachment_owner.h"

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

	//����������
	virtual void	UpdateInventoryOwner		(u32 deltaT);

	// ��������
	u32					m_dwMoney;
	ALife::EStalkerRank	m_tRank;

	CPda* GetPDA();
	bool IsActivePDA();

	//������� ����� �������
	//������ ��������� ���������� ������� PDA ���������.
	//������ ���� �������������� � ������������ �������
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index);
	//�������� ��������� ������� ��������� PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, int info_index);


	CInventory	*m_inventory;									// ���������
	CInventory	*m_trade_storage;								// ����� 

	
	////////////////////////////////////
	//�������� � ������� � ����������

	//������������� ������� ��������
	void InitTrade();
	CTrade* GetTrade();

	//��� ��������� ���������
	virtual bool OfferTalk(CInventoryOwner* talk_partner);
	void StartTalk(CInventoryOwner* talk_partner);
	void StopTalk();
	bool IsTalking();
	CInventoryOwner* GetTalkPartner() {return m_pTalkPartner;}
	//������� ������ ������, ���� �������� ������� ��������,
	//�� �� ������ ��� ����������, ��� � ���� ���� �� ���� ����
	//false - ���� ������������ �������� ��� ���������� ���
	//������ �������� �������� ���������� � index_list
	virtual bool AskQuestion(SInfoQuestion& question, INFO_INDEX_LIST& index_list);
	//�������� ������� ����� ������ ����������
	virtual void OnReceiveInfo(int /**info_index/**/) {};

	//������� �������������� ���������
	virtual LPCSTR GetGameName();
	virtual LPCSTR GetGameRank();
	virtual LPCSTR GetGameCommunity();
	const CInventory &inventory() const {return(*m_inventory);}
	CInventory &inventory() {return(*m_inventory);}

	//���������� ������� ������� �������� (� ��������) � ������ ��������
	virtual float GetWeaponAccuracy			() const;

protected:
	// ��������
	CTrade				*m_pTrade;
	bool				m_bTalking; 
	CInventoryOwner		*m_pTalkPartner;




public:
	virtual void			renderable_Render		();
	virtual void			OnItemTake				(CInventoryItem *inventory_item);
	virtual void			OnItemDrop				(CInventoryItem *inventory_item);
	virtual void			OnItemDropUpdate		() {}
	virtual bool			use_bolts				() const {return(true);}
};

#include "inventory_owner_inline.h"