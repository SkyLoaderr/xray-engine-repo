/////////////////////////////////////////////////////
// ��� ����������, ������� ���������
// InventoryOwner.h
//////////////////////////////////////////////////////

#pragma once
#include "inventory.h"


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


	CInventory	m_inventory;									// ���������
	CInventory	m_trade_storage;								// ����� 
	
	CTrade		*m_trade;										// ��������

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

protected:
	
};
