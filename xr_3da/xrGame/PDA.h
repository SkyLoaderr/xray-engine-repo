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

DEF_VECTOR (PDA_LIST, CPda*);

class CPda :
	public CInventoryItemObject,
	public Feel::Touch
{
	typedef	CInventoryItemObject inherited;
public:
											CPda					();
	virtual									~CPda					();

	virtual BOOL 							net_Spawn				(CSE_Abstract* DC);
	virtual void 							Load					(LPCSTR section);
	virtual void 							net_Destroy				();

	virtual void 							OnH_A_Chield			();
	virtual void 							OnH_B_Independent		();

	virtual void 							shedule_Update			(u32 dt);
	virtual void 							UpdateCL				();
	virtual void 							renderable_Render		();

	virtual void 							feel_touch_new			(CObject* O);
	virtual void 							feel_touch_delete		(CObject* O);
	virtual BOOL 							feel_touch_contact		(CObject* O);


	virtual u16								GetOriginalOwnerID		() {return m_idOriginalOwner;}
	virtual CInventoryOwner*				GetOriginalOwner		();
	virtual CObject*						GetOwnerObject			();


	virtual void							TurnOn					() {m_bTurnedOff = false;}
	virtual void							TurnOff					() {m_bTurnedOff = true;}
	
	virtual bool 							IsActive				() {return (!m_bTurnedOff && !m_bPassiveMode);}
	virtual bool 							IsPassive				() {return (m_bTurnedOff || m_bPassiveMode);}
	virtual bool 							IsOn					() {return !m_bTurnedOff;}
	virtual bool 							IsOff					() {return m_bTurnedOff;}


			xr_vector<CObject*>				ActiveContacts			();
			void							ActiveContacts			(xr_vector<CPda*>& res);
			CPda*							GetPdaFromOwner			(CObject* owner);
	virtual u32								ActiveContactsNum		()							{return ActiveContacts().size();}


	virtual void							save					(NET_Packet &output_packet);
	virtual void							load					(IReader &input_packet);

	virtual LPCSTR							Name					();
	virtual LPCSTR							NameComplex				();

protected:

	void PdaEventSend(u32 pda_ID, EPdaMsg msg, INFO_ID info_id);
	void AddMessageToLog(u32 pda_ID, EPdaMsg msg, INFO_ID info_id, bool receive);


	//радиус обнаружения других PDA
	float								m_fRadius;

	//первый владелец PDA
	u16									m_idOriginalOwner;
	SPECIFIC_CHARACTER_ID				m_SpecificChracterOwner;
	//полное название PDA с именем владельца
	xr_string							m_sFullName;

	//пассивный режим работы PDA
	bool m_bPassiveMode;
	//PDA выключено
	bool m_bTurnedOff;
};
