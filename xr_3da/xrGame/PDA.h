///////////////////////////////////////////////////////////////
// PDA.h
// PDA - ������� ��������� � ������
// ������������ ��� �������, ������������� �� ����� 
// � ��� ��������� ���� ������ "��������" ���������� ���
// ���� ���������� ����
///////////////////////////////////////////////////////////////


#pragma once

#include "../feel_touch.h"
#include "inventory.h"
#include "InfoPortion.h"


class CInventoryOwner;


//��������� ��� �������� ��������� PDA,
//������������ ��� ������� �����
typedef struct tagSPdaMessage 
{
	EPdaMsg			msg;
	EPdaMsgAnger	anger;
	
	//true ���� �� �������� ��������� 
	//� false ���� �� ��� ��������
	bool			receive;
	
	//true, ���� ��������� - ������
	//� false, ���� �����
	bool			question;

	//����� ���������/�������� ���������
	_TIME_ID		time;
		
} SPdaMessage;


DEF_LIST (PDA_LIST, CPda*);


DEFINE_MAP (int, BOOL, KNOWN_INFO_MAP, KNOWN_INFO_PAIR_IT);

class CPda :
	public CInventoryItem,
	public Feel::Touch
{
typedef	CInventoryItem	inherited;
public:
	CPda(void);
	virtual ~CPda(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void feel_touch_new	(CObject* O);
	virtual void feel_touch_delete	(CObject* O);
	virtual BOOL feel_touch_contact	(CObject* O);

	void OnEvent(NET_Packet& P, u16 type);


	virtual u32 GetOriginalOwnerID() {return m_idOriginalOwner;}
	virtual CInventoryOwner* GetOriginalOwner();
	virtual CObject*		 GetOwnerObject();


	virtual void TurnOn() {m_bTurnedOff = false;}
	virtual void TurnOff() {m_bTurnedOff = true;}
	
	virtual bool IsActive() {return (!m_bTurnedOff && !m_bPassiveMode);}
	virtual bool IsPassive() {return (m_bTurnedOff || m_bPassiveMode);}
	virtual bool IsOn() {return !m_bTurnedOff;}
	virtual bool IsOff() {return m_bTurnedOff;}

	virtual void SendMessage(u32 pda_num, EPdaMsg msg, EPdaMsgAnger anger);
	virtual void SendMessageID(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger);

	virtual bool IsNewMessage(){return m_bNewMessage;}
	virtual void NoNewMessage(){m_bNewMessage = false;}

	virtual bool NeedToAnswer(u32 pda_ID);
	virtual bool WaitForReply(u32 pda_ID);

	virtual u32 ActiveContactsNum() {return m_PDAList.size();}

	//������ PDA � ���� �������������
	PDA_LIST m_PDAList;

	//������ ������ ��� �������� � �������� PDA �� ����
	PDA_LIST m_NewPDAList;
	PDA_LIST m_DeletedPDAList;

	//��� ������� ����� �����������
	//� ������� PDA � ���� ������������
	DEF_LIST (PDA_MESSAGE_LIST, SPdaMessage);
	DEFINE_MAP (u32, PDA_MESSAGE_LIST, PDA_LOG_MAP, PDA_LOG_PAIR_IT);
	PDA_LOG_MAP m_mapPdaLog;

	
	///////////////////////////////////////
	// ���������� � �������� ���������� ���
	// ������ PDA

	//����� �� � ���-��� � �������� �������
	bool IsKnowAbout(int info_index);
	//�������� ���������� ������� PDA
	bool TransferInfoToID(u32 pda_ID, int info_index);
	//������� ��������� ����� ������ ����������
	void OnReceiveInfo(int info_index);


	//�������� �� ��������� ����������
	KNOWN_INFO_MAP m_mapKnownInfo;

	//��� ������� ��������� � ���� ������
	INFO_QUESTIONS_LIST m_ActiveQuestionsList;
	void UpdateQuestions();

protected:
	//����������� ������ ��������� �� PDA
	bool m_bNewMessage;

	void PdaEventSend(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger);
	void AddMessageToLog(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger, bool receive);


	//������ ����������� ������ PDA
	float m_fRadius;

	//������ �������� PDA
	u32 m_idOriginalOwner;

	//��������� ����� ������ PDA
	bool m_bPassiveMode;
	//PDA ���������
	bool m_bTurnedOff;
};
