#include "stdafx.h"
#include "UIGameTutorial.h"
#include "UIWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "../../xr_input.h"
#include "../xr_level_controller.h"

CUIGameTutorial::CUIGameTutorial()
{
	m_bActive					= false;
	m_bPlayEachItem				= false;
}

void CUIGameTutorial::Start(LPCSTR tutor_name)
{
	VERIFY(m_items.size()==0);
	Device.seqFrame.Add			(this,REG_PRIORITY_LOW-10000);
	Device.seqRender.Add		(this,REG_PRIORITY_LOW-10000);
	
	m_UIWindow					= xr_new<CUIWindow>();

	CUIXml uiXml;
	uiXml.Init					(CONFIG_PATH, UI_PATH, "game_tutorials.xml");
	
	int items_count				= uiXml.GetNodesNum	(tutor_name,0,"item");	VERIFY(items_count>0);
	uiXml.SetLocalRoot			(uiXml.NavigateToNode(tutor_name,0));

	m_bPlayEachItem				= !!uiXml.ReadInt("play_each_item",0,0);

	CUIXmlInit xml_init;
	xml_init.InitWindow			(uiXml, "global_wnd", 0,	m_UIWindow);
	xml_init.InitAutoStaticGroup(uiXml, "global_wnd",		m_UIWindow);

	for(int i=0;i<items_count;++i){
		CTutorialItem* pItem	= xr_new<CTutorialItem>();
		m_items.push_back		(pItem);
		pItem->Load				(&uiXml,i);
	}

	CTutorialItem* pCurrItem	= m_items.front();
	pCurrItem->Start			(this);
	m_pStoredInputReceiver		= pInput->CurrentIR();
	IR_Capture					();
	m_bActive					= true;
}

void CUIGameTutorial::Stop()
{
	if(m_items.size()){
		if (m_bPlayEachItem){
			Next				();
			return;
		}else{
			CTutorialItem* pCurrItem	= m_items.front();
			pCurrItem->Stop		(this, true);
		}
	}

	Device.seqFrame.Remove		(this);
	Device.seqRender.Remove		(this);
	delete_data					(m_items);
	delete_data					(m_UIWindow);
	IR_Release					();
	m_bActive					= false;
	m_pStoredInputReceiver		= NULL;
}

void CUIGameTutorial::OnFrame()
{  
	if(!m_items.size()){
		Stop			();
		return;
	}else{
		CTutorialItem* pCurrItem		= m_items.front();
		if((pCurrItem->m_time_start+pCurrItem->m_time_length)<(Device.dwTimeContinual/1000.0f) )
			Next				();
	}
	
	if(!m_items.size()){
		Stop					();
		return;
	}

	m_items.front()->Update		();
	m_UIWindow->Update			();
}

void CUIGameTutorial::OnRender	()
{
	m_UIWindow->Draw			();
	VERIFY						(m_items.size());
	m_items.front()->OnRender	();
}

void CUIGameTutorial::Next		()
{
	CTutorialItem* pCurrItem	= m_items.front();
	bool can_stop				= pCurrItem->Stop(this);
	if	(!can_stop)				return;

	m_items.pop_front			();
	delete_data					(pCurrItem);

	if(m_items.size())
	{
		pCurrItem				= m_items.front();
		pCurrItem->Start		(this);
	}
}

bool CUIGameTutorial::GrabInput()
{
	if(m_items.size())
		return m_items.front()->GrabInput();
	else
	return false;

}

void CUIGameTutorial::IR_OnMousePress		(int btn)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnMousePress(btn);
}

void CUIGameTutorial::IR_OnMouseRelease		(int btn)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnMouseRelease(btn);
}

void CUIGameTutorial::IR_OnMouseHold		(int btn)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnMouseHold(btn);
}

void CUIGameTutorial::IR_OnMouseMove		(int x, int y)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnMouseMove(x, y);
}

void CUIGameTutorial::IR_OnMouseStop		(int x, int y)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnMouseStop(x, y);
}

void CUIGameTutorial::IR_OnKeyboardRelease	(int dik)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnKeyboardRelease(dik);
}

void CUIGameTutorial::IR_OnKeyboardHold		(int dik)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnKeyboardHold(dik);
}

void CUIGameTutorial::IR_OnMouseWheel		(int direction)
{
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnMouseWheel(direction);
}

void CUIGameTutorial::IR_OnKeyboardPress	(int dik)
{
	if(key_binding[dik]==kQUIT){
		Stop		();
		return;
	}
	if(m_items.size())	m_items.front()->OnKeyboardPress			(dik);
	
	if(!GrabInput())	m_pStoredInputReceiver->IR_OnKeyboardPress	(dik);
}

