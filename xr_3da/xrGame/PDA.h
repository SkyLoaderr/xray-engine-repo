///////////////////////////////////////////////////////////////
// PDA.h
// PDA - пейджер сталкеров и солдат
///////////////////////////////////////////////////////////////


#pragma once
#include "inventory.h"
#include "..\feel_touch.h"
#include "customzone.h"



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




	//типы сообщений PDA
	enum EPDAMsg {
		ePDAMsg = u32(0),
		ePDAMsgTrade,
		ePDAMsgGoAway,
		ePDAMsgNeedHelp,
		ePDAMsgDecline,			//принять предложение
		ePDAMsgAccept,			//отклонить
		ePDAMessageMax,
	};





	virtual void SendMessage(u32 pda_num, EPDAMsg msg);

protected:
    DEF_LIST (PDA_LIST, CPda*);

	//список PDA в зоне достигаемости
	PDA_LIST m_PDAList;

	//радиус обнаружения других PDA
	float m_fRadius;
};
