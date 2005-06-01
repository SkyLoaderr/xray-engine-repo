///////////////////////////////////////////////////////////////
// PDA.h
// PDA - ������� ��������� � ������
// ������������ ��� �������, ������������� �� ����� 
// � ��� ��������� ���� ������ "��������" ���������� ���
// ���� ���������� ����
///////////////////////////////////////////////////////////////


#pragma once

#include "../feel_touch.h"
#include "inventory_item_object.h"

#include "InfoPortionDefs.h"
#include "character_info_defs.h"

#include "PdaMsg.h"


class CInventoryOwner;
class CPda;

DEF_VECTOR (PDA_LIST, CPda*);

class CPda :
	public CInventoryItemObject,
	public Feel::Touch
{
	typedef	CInventoryItemObject inherited;
public:
	CPda(void);
	virtual ~CPda(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
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


	virtual u16 GetOriginalOwnerID() {return m_idOriginalOwner;}
	virtual CInventoryOwner* GetOriginalOwner();
	virtual CObject*		 GetOwnerObject();


	virtual void TurnOn() {m_bTurnedOff = false;}
	virtual void TurnOff() {m_bTurnedOff = true;}
	
	virtual bool IsActive() {return (!m_bTurnedOff && !m_bPassiveMode);}
	virtual bool IsPassive() {return (m_bTurnedOff || m_bPassiveMode);}
	virtual bool IsOn() {return !m_bTurnedOff;}
	virtual bool IsOff() {return m_bTurnedOff;}

//	virtual void SendMessage(u32 pda_num, EPdaMsg msg, INFO_ID info_id);
	virtual void SendMessageID(u32 pda_ID, EPdaMsg msg, INFO_ID info_id);

	virtual bool IsNewMessage(){return m_bNewMessage;}
	virtual void NoNewMessage(){m_bNewMessage = false;}

	virtual bool NeedToAnswer(u32 pda_ID);
	virtual bool WaitForReply(u32 pda_ID);

	const xr_vector<CObject*>&	ActiveContacts				()	{return	feel_touch;}
	void			ActiveContacts		(xr_vector<CPda*>& res);
	CPda*		GetPdaFromOwner								(CObject* owner);
	virtual u32 ActiveContactsNum() {return ActiveContacts().size();}

	//������ PDA � ���� �������������
//	PDA_LIST m_PDAList;

	//������ ������ ��� �������� � �������� PDA �� ����
//	PDA_LIST m_NewPDAList;
//	PDA_LIST m_DeletedPDAList;

	//��� ������� ����� �����������
	//� ������� PDA � ���� ������������
	DEF_LIST (PDA_MESSAGE_LIST, SPdaMessage);
	DEFINE_MAP (u32, PDA_MESSAGE_LIST, PDA_LOG_MAP, PDA_LOG_PAIR_IT);
	PDA_LOG_MAP m_mapPdaLog;


	//�������� ��������� ��������� �� ����  (false - ���� ��� ����)
	bool GetLastMessageFromLog(u32 pda_ID,  SPdaMessage& pda_message);

//	virtual void		SetInfoPortion (INFO_ID);
//	virtual INFO_ID		GetInfoPortion ();

	virtual LPCSTR		Name			();
	virtual LPCSTR		NameComplex		();

protected:
	//����������� ������ ��������� �� PDA
	bool m_bNewMessage;

	void PdaEventSend(u32 pda_ID, EPdaMsg msg, INFO_ID info_id);
	void AddMessageToLog(u32 pda_ID, EPdaMsg msg, INFO_ID info_id, bool receive);


	//������ ����������� ������ PDA
	float								m_fRadius;

	//������ �������� PDA
	u16									m_idOriginalOwner;
	SPECIFIC_CHARACTER_ID				m_SpecificChracterOwner;
	//�������������� info portion
	//	INFO_ID		m_InfoPortion;
	//������ �������� PDA � ������ ���������
	xr_string							m_sFullName;

	//��������� ����� ������ PDA
	bool m_bPassiveMode;
	//PDA ���������
	bool m_bTurnedOff;
};
