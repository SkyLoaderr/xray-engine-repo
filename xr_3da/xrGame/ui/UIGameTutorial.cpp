#include "stdafx.h"
#include "UIGameTutorial.h"
#include "UIWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "../../xr_input.h"

extern ENGINE_API BOOL bShowPauseString;

CUIGameTutorial::CUIGameTutorial()
{
	m_bActive = false;
}

void CUIGameTutorial::Start(LPCSTR tutor_name)
{
	VERIFY(m_items.size()==0);
	Device.seqFrame.Add(this,REG_PRIORITY_LOW-10000);
	Device.seqRender.Add(this,REG_PRIORITY_LOW-10000);
	
	m_UIWindow						= xr_new<CUIWindow>();

	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, "game_tutorials.xml");
	


	int items_count					= uiXml.GetNodesNum	(tutor_name,0,"item");
	uiXml.SetLocalRoot				(uiXml.NavigateToNode(tutor_name,0));

	CUIXmlInit xml_init;
	xml_init.InitWindow								(uiXml, "global_wnd", 0,	m_UIWindow);
	xml_init.InitAutoStaticGroup					(uiXml, "global_wnd",		m_UIWindow);
	

	for(int i=0;i<items_count;++i)
	{
		TutorialItem* pItem		= xr_new<TutorialItem>();
		m_items.push_back		(pItem);
		pItem->Load				(&uiXml,i);
	}

	TutorialItem* pCurrItem		= m_items.front();
	pCurrItem->Start			(this);
	m_pStoredInputReceiver		= pInput->CurrentIR();
	IR_Capture					();
	m_bActive					= true;
}

void CUIGameTutorial::Stop()
{
	if(m_items.size()){
		TutorialItem* pCurrItem		= m_items.front();
		pCurrItem->Stop			(this, true);
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
		TutorialItem* pCurrItem		= m_items.front();
		if((pCurrItem->m_time_start+pCurrItem->m_time_length)<(Device.dwTimeContinual/1000.0f) )
			Next				();
	}
	
	if(!m_items.size()){
		Stop			();
		return;
	}

	m_items.front()->Update	();
	m_UIWindow->Update		();
}

void CUIGameTutorial::OnRender()
{
	m_UIWindow->Draw		();
}

void CUIGameTutorial::Next		()
{
	TutorialItem* pCurrItem		= m_items.front();
	bool can_stop				= pCurrItem->Stop(this);
	if	(!can_stop)				return;

	m_items.pop_front			();
	delete_data					(pCurrItem);

	if(m_items.size())
	{
		pCurrItem					= m_items.front();
		pCurrItem->Start			(this);
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

#include "../xr_level_controller.h"
void CUIGameTutorial::IR_OnKeyboardPress	(int dik)
{
	if(key_binding[dik]==kQUIT){
		Stop		();
		return;
	}
	if(m_items.size())
	m_items.front()->OnKeyboardPress		(dik);
	
	if(!GrabInput())
		m_pStoredInputReceiver->IR_OnKeyboardPress(dik);
}


TutorialItem::~TutorialItem()
{
	delete_data			(m_UIWindow);
	m_sound.stop		();
}

void TutorialItem::Load(CUIXml* xml, int idx)
{
	XML_NODE* _stored_root = xml->GetLocalRoot();
	xml->SetLocalRoot				(xml->NavigateToNode("item",idx));
	
	m_snd_name				= xml->Read				("sound",0,""			);
	m_time_length			= xml->ReadFlt			("length_sec",0,0		);
	m_desired_cursor_pos.x	= xml->ReadAttribFlt	("cursor_pos",0,"x",1024);
	m_desired_cursor_pos.y	= xml->ReadAttribFlt	("cursor_pos",0,"y",768	);
	strcpy					(m_pda_section, xml->Read("pda_section",0,"")	);

	LPCSTR str				= xml->Read				("pause_state",0,"ignore");

	m_flags.set										(etiNeedPauseOn, 0==_stricmp(str, "on"));
	m_flags.set										(etiNeedPauseOff, 0==_stricmp(str, "off"));

	str						= xml->Read				("guard_key",0,NULL		);
	m_continue_dik_guard	= -1;
	if (str && !_stricmp(str,"any")){
		m_continue_dik_guard = 9999;
		str = NULL;
	}
	if(str){
		for(int i=0; keybind[i].name; ++i) {
			if(0==_stricmp(keybind[i].name,str)){
				m_continue_dik_guard = keybind[i].DIK;
				break;
			}
		}
	}

	m_flags.set										(etiCanBeStopped,(m_continue_dik_guard==-1));

	m_flags.set										(etiGrabInput, 1==xml->ReadInt("grab_input",0,1));

	int cnt = xml->GetNodesNum						("main_wnd",0,"auto_static");
	m_ui_highlighting.resize						(cnt);
	for(int i=0;i<cnt;++i){
		XML_NODE* _sr								= xml->GetLocalRoot();
		xml->SetLocalRoot							(xml->NavigateToNode("main_wnd",0));

		XML_NODE* _wn								= xml->NavigateToNode("auto_static",i);
		m_ui_highlighting[i].m_wnd_name				= xml->Read			(_wn,"window_name",0,"not_cpecified");

		m_ui_highlighting[i].m_start				= xml->ReadAttribFlt("auto_static",i,"start_time",0);
		m_ui_highlighting[i].m_length				= xml->ReadAttribFlt("auto_static",i,"length_sec",0);
		m_ui_highlighting[i].m_bshown				= false;

		xml->SetLocalRoot							(_sr);
	}

	//ui-components
	m_UIWindow										= xr_new<CUIWindow>();
	m_UIWindow->SetAutoDelete						(false);
	CUIXmlInit xml_init;
	xml_init.InitWindow								(*xml, "main_wnd", 0,	m_UIWindow);
	xml_init.InitAutoStaticGroup					(*xml, "main_wnd",		m_UIWindow);

	xml->SetLocalRoot								(_stored_root);
}

CUIWindow* find_child_window(CUIWindow* parent, const shared_str& _name)
{
	CUIWindow::WINDOW_LIST& wl				= parent->GetChildWndList();
	CUIWindow::WINDOW_LIST_it it			= wl.begin();
	CUIWindow::WINDOW_LIST_it it_e			= wl.end();
	for(;it!=it_e;++it)
		if((*it)->WindowName()==_name)
			return (*it);
	
	return NULL;
}

void TutorialItem::Update()
{
	xr_vector<SUIHighlightedAreas>::iterator it		= m_ui_highlighting.begin();
	xr_vector<SUIHighlightedAreas>::iterator it_e	= m_ui_highlighting.end();
	
	for(;it!=it_e;++it)
	{
		SUIHighlightedAreas& s = *it;
		CUIStatic* w = smart_cast<CUIStatic*>(find_child_window(m_UIWindow, s.m_wnd_name));
		R_ASSERT(w);
		float gt = Device.dwTimeContinual/1000.0f;
		if( (gt > (m_time_start+s.m_start))				&&
			(gt < (m_time_start+s.m_start+s.m_length))   )
		{
				w->Show				(true);
				if(!s.m_bshown){
					w->ResetAnimation	();
					s.m_bshown			= true;
				}
		}else
				w->Show				(false);

	}

}
#include "../UIGameSp.h"
#include "../HUDManager.h"
#include "../level.h"
#include "UIPdaWnd.h"
void TutorialItem::Start		(CUIGameTutorial* t)
{
	m_flags.set					(etiStoredPauseState, Device.Pause());
	
	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState)){
		Device.Pause			(TRUE);
		bShowPauseString		= FALSE;
	}

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	GetUICursor()->SetPos		(m_desired_cursor_pos.x, m_desired_cursor_pos.y);
	m_time_start				= Device.dwTimeContinual/1000.0f;
	t->MainWnd()->AttachChild	(m_UIWindow);
	if(m_snd_name.size()){
		::Sound->create			(m_sound, true, *m_snd_name); 
		VERIFY					(m_sound._handle());
		m_sound.play			(NULL, sm_2D);
	}

	bool bShowPda = false;

	CUIGameSP* ui_game_sp			= smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!stricmp(m_pda_section,"pda_contacts")){
		ui_game_sp->PdaMenu->SetActiveSubdialog(eptContacts);
		bShowPda = true;
	}else
	if(!stricmp(m_pda_section,"pda_map")){
		ui_game_sp->PdaMenu->SetActiveSubdialog(eptMap);
		bShowPda = true;
	}else
		if(!stricmp(m_pda_section,"pda_quests")){
		ui_game_sp->PdaMenu->SetActiveSubdialog(eptQuests);
		bShowPda = true;
	}else
	if(!stricmp(m_pda_section,"pda_diary")){
		ui_game_sp->PdaMenu->SetActiveSubdialog(eptDiary);
		bShowPda = true;
	}else
	if(!stricmp(m_pda_section,"pda_ranking")){
		ui_game_sp->PdaMenu->SetActiveSubdialog(eptRanking);
		bShowPda = true;
	}else
	if(!stricmp(m_pda_section,"pda_statistics")){
		ui_game_sp->PdaMenu->SetActiveSubdialog(eptActorStatistic);
		bShowPda = true;
	}else
		if(!stricmp(m_pda_section,"pda_encyclopedia")){
		ui_game_sp->PdaMenu->SetActiveSubdialog(eptEncyclopedia);
		bShowPda = true;
	}

	if( (!ui_game_sp->PdaMenu->IsShown() && bShowPda) || 
		(ui_game_sp->PdaMenu->IsShown() && !bShowPda))
		HUD().GetUI()->StartStopMenu			(ui_game_sp->PdaMenu,true);
}

bool TutorialItem::Stop			(CUIGameTutorial* t, bool bForce)
{
	if(!m_flags.test(etiCanBeStopped)&&!bForce) 
		return false;

	t->MainWnd()->DetachChild	(m_UIWindow);
	m_sound.stop				();

	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(TRUE);

	return true;
}

void TutorialItem::OnKeyboardPress	(int dik)
{
	if	(!m_flags.test(etiCanBeStopped) )
	{
		VERIFY		(m_continue_dik_guard!=-1);
		if(m_continue_dik_guard==-1)m_flags.set(etiCanBeStopped, TRUE); //not binded action :(

		if(m_continue_dik_guard==9999 || key_binding[dik] == m_continue_dik_guard)
			m_flags.set(etiCanBeStopped, TRUE); //match key

	}
}
