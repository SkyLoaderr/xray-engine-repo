///////////////////////////////////////////////////////////////
// PDA.h
// PDA - пейджер сталкеров и солдат
// используется для общения, идентификации на карте 
// и как хранилище всей важной "сюжетной" информации для
// всех персонажей игры
///////////////////////////////////////////////////////////////


#pragma once

#include "../feel_touch.h"
#include "inventory_item_object.h"

#include "InfoPortionDefs.h"
#include "character_info_defs.h"

#include "PdaMsg.h"


class CInventoryOwner;
class CPda;

DEF_LIST (PDA_LIST, CPda*);

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

	virtual void SendMessage(u32 pda_num, EPdaMsg msg, INFO_ID info_id);
	virtual void SendMessageID(u32 pda_ID, EPdaMsg msg, INFO_ID info_id);

	virtual bool IsNewMessage(){return m_bNewMessage;}
	virtual void NoNewMessage(){m_bNewMessage = false;}

	virtual bool NeedToAnswer(u32 pda_ID);
	virtual bool WaitForReply(u32 pda_ID);

	virtual u32 ActiveContactsNum() {return m_PDAList.size();}

	//список PDA в зоне достигаемости
	PDA_LIST m_PDAList;

	//список только что вошедших и вышедших PDA из зоны
	PDA_LIST m_NewPDAList;
	PDA_LIST m_DeletedPDAList;

	//для ведения логов переговоров
	//с другими PDA в зоне досягаемости
	DEF_LIST (PDA_MESSAGE_LIST, SPdaMessage);
	DEFINE_MAP (u32, PDA_MESSAGE_LIST, PDA_LOG_MAP, PDA_LOG_PAIR_IT);
	PDA_LOG_MAP m_mapPdaLog;


	//получить последнее сообщение из лога  (false - если лог пуст)
	bool GetLastMessageFromLog(u32 pda_ID,  SPdaMessage& pda_message);

	virtual void		SetInfoPortion (INFO_ID);
	virtual INFO_ID		GetInfoPortion ();

	virtual LPCSTR		Name			();
	virtual LPCSTR		NameComplex		();

protected:
	//поступление нового сообщения на PDA
	bool m_bNewMessage;

	void PdaEventSend(u32 pda_ID, EPdaMsg msg, INFO_ID info_id);
	void AddMessageToLog(u32 pda_ID, EPdaMsg msg, INFO_ID info_id, bool receive);


	//радиус обнаружения других PDA
	float m_fRadius;

	//первый владелец PDA
	u16 m_idOriginalOwner;
	SPECIFIC_CHARACTER_ID m_SpecificChracterOwner;
	//присоединенный info portion
	INFO_ID		m_InfoPortion;
	//полное название PDA с именем владельца
	xr_string m_sFullName;

	//пассивный режим работы PDA
	bool m_bPassiveMode;
	//PDA выключено
	bool m_bTurnedOff;
};
