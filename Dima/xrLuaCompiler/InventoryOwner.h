/////////////////////////////////////////////////////
// ��� ����������, ������� ���������
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "inventory.h"

#include "InfoPortion.h"

class CTrade;
class CPda;


//���� ��������� PDA
enum EPdaMsg {
	ePdaMsgTrade  = u32(0),		//���������
	ePdaMsgGoAway,				//�������� � ����������
	ePdaMsgNeedHelp,			//������� � ������
	
	ePdaMsgAccept,				//������� �����������
	ePdaMsgDecline,				//����������
	ePdaMsgDeclineRude,			//����� ����������
	ePdaMessageMax
};

//������� "������" � ���������
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

	//����������
	virtual void UpdateInventoryOwner(u32 deltaT);

	// ��������
	u32					m_dwMoney;
	EStalkerRank		m_tRank;

	CPda* GetPDA();
	bool IsActivePDA();

	//������� ����� �������
	//������ ��������� ���������� ������� PDA ���������.
	//������ ���� �������������� � ������������ �������
	virtual void ReceivePdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger);
	//�������� ��������� ������� ��������� PDA
	virtual void SendPdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger);


	CInventory	m_inventory;									// ���������
	CInventory	m_trade_storage;								// ����� 

	
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
	virtual void OnReceiveInfo(int info_index) {};



protected:
	// ��������
	CTrade*	m_pTrade;

	bool m_bTalking; 
	CInventoryOwner* m_pTalkPartner;
};
