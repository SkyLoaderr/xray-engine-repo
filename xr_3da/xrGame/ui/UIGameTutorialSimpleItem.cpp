#include "stdafx.h"
#include "UIGameTutorial.h"
#include "UIWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "../../xr_input.h"
#include "../xr_level_controller.h"

extern ENGINE_API BOOL bShowPauseString;

//-----------------------------------------------------------------------------
// Tutorial Item
//-----------------------------------------------------------------------------
CUISequenceSimpleItem::~CUISequenceSimpleItem()
{
	SubItemVecIt _I			= m_subitems.begin	();
	SubItemVecIt _E			= m_subitems.end	();
	for(;_I!=_E;++_I)		_I->Stop			();
	m_subitems.clear		();
	m_sound.stop			();
	delete_data				(m_UIWindow);
}

bool CUISequenceSimpleItem::IsPlaying()
{
	return (m_time_start+m_time_length)>(Device.dwTimeContinual/1000.0f);
}

CUIWindow* find_child_window(CUIWindow* parent, const shared_str& _name)
{
	CUIWindow::WINDOW_LIST& wl		= parent->GetChildWndList();
	CUIWindow::WINDOW_LIST_it _I	= wl.begin();
	CUIWindow::WINDOW_LIST_it _E	= wl.end();
	for(;_I!=_E;++_I) if((*_I)->WindowName()==_name) return (*_I);
	return NULL;
}

void CUISequenceSimpleItem::Load(CUIXml* xml, int idx)
{
	CUISequenceItem::Load	(xml,idx);

	XML_NODE* _stored_root	= xml->GetLocalRoot();
	xml->SetLocalRoot		(xml->NavigateToNode("item",idx));
	
	LPCSTR m_snd_name		= xml->Read				("sound",0,""			);
	if (m_snd_name&&m_snd_name[0]){
		m_sound.create		(TRUE,m_snd_name,0);	
		VERIFY				(m_sound._handle());
	}
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
		str					= NULL;
	}
	if(str){
		for(int i=0; keybind[i].name; ++i) {
			if(0==_stricmp(keybind[i].name,str)){
				m_continue_dik_guard = keybind[i].DIK;
				break;
			}
		}
	}

	m_flags.set						(etiCanBeStopped,	(m_continue_dik_guard==-1));
	m_flags.set						(etiGrabInput,		1==xml->ReadInt("grab_input",0,1));


	//ui-components
	m_UIWindow						= xr_new<CUIWindow>();
	m_UIWindow->SetAutoDelete		(false);
	XML_NODE* _lsr					= xml->GetLocalRoot();
	CUIXmlInit xml_init;
	xml_init.InitWindow				(*xml, "main_wnd", 0,	m_UIWindow);
	xml_init.InitAutoStaticGroup	(*xml, "main_wnd",		m_UIWindow);
	xml->SetLocalRoot				(_lsr);

	// initialize auto_static
	int cnt							= xml->GetNodesNum	("main_wnd",0,"auto_static");
	m_subitems.resize				(cnt);
	for(int i=0;i<cnt;++i){
		XML_NODE* _sr				= xml->GetLocalRoot();
		xml->SetLocalRoot			(xml->NavigateToNode("main_wnd",0));

		XML_NODE* _wn				= xml->NavigateToNode("auto_static",i);
		LPCSTR wnd_name				= xml->Read			(_wn,"window_name",0,"not_cpecified");
		SSubItem* _si				= &m_subitems[i];
		_si->m_start				= xml->ReadAttribFlt("auto_static",i,"start_time",0);
		_si->m_length				= xml->ReadAttribFlt("auto_static",i,"length_sec",0);
		_si->m_visible				= false;
		_si->m_wnd					= smart_cast<CUIStatic*>(find_child_window(m_UIWindow, wnd_name)); VERIFY(_si->m_wnd);
		_si->m_wnd->Show			(false);

		xml->SetLocalRoot			(_sr);
	}
	xml->SetLocalRoot				(_stored_root);
}

void CUISequenceSimpleItem::SSubItem::Start	()
{
	m_wnd->Show						(true);
	m_wnd->ResetAnimation			();
	m_visible						= true;
}

void CUISequenceSimpleItem::SSubItem::Stop	()
{
	m_wnd->Show						(false);
	m_visible						= false;
}

void CUISequenceSimpleItem::Update			()
{
	float gt						= float(Device.dwTimeContinual)/1000.0f;
	SubItemVecIt _I					= m_subitems.begin();
	SubItemVecIt _E					= m_subitems.end();
	for(;_I!=_E;++_I){
		SSubItem& s					= *_I;
		bool bPlaying				= (gt>(m_time_start+s.m_start))&&(gt<(m_time_start+s.m_start+s.m_length));
		if (true==bPlaying&&(false==s.m_visible))			s.Start	();
		else if ((false==bPlaying)&&(true==s.m_visible))	s.Stop	();
	}
}

#include "../UIGameSp.h"
#include "../HUDManager.h"
#include "../level.h"
#include "UIPdaWnd.h"
void CUISequenceSimpleItem::Start()
{
	m_flags.set					(etiStoredPauseState, Device.Pause());
	
	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState)){
		Device.Pause			(TRUE);
		bShowPauseString		= FALSE;
	}

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	GetUICursor()->SetPos		(m_desired_cursor_pos.x, m_desired_cursor_pos.y);
	m_time_start				= float(Device.dwTimeContinual)/1000.0f;
	m_owner->MainWnd()->AttachChild	(m_UIWindow);

	if (m_sound._handle())		m_sound.play(NULL, sm_2D);

	if (g_pGameLevel){
		bool bShowPda			= false;
		CUIGameSP* ui_game_sp	= smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(!stricmp(m_pda_section,"pda_contacts")){
			ui_game_sp->PdaMenu->SetActiveSubdialog(eptContacts);
			bShowPda = true;
		}else{
			if(!stricmp(m_pda_section,"pda_map")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptMap);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_quests")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptQuests);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_diary")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptDiary);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_ranking")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptRanking);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_statistics")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptActorStatistic);
				bShowPda = true;
			}else if(!stricmp(m_pda_section,"pda_encyclopedia")){
				ui_game_sp->PdaMenu->SetActiveSubdialog(eptEncyclopedia);
				bShowPda = true;
			}
		}
		if( (!ui_game_sp->PdaMenu->IsShown() && bShowPda) || 
			(ui_game_sp->PdaMenu->IsShown() && !bShowPda))
			HUD().GetUI()->StartStopMenu			(ui_game_sp->PdaMenu,true);
	}
}

bool CUISequenceSimpleItem::Stop			(bool bForce)
{
	if(!m_flags.test(etiCanBeStopped)&&!bForce) 
		return false;

	m_owner->MainWnd()->DetachChild	(m_UIWindow);
	m_sound.stop				();

	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(TRUE);

	return true;
}

void CUISequenceSimpleItem::OnKeyboardPress	(int dik)
{
	if	(!m_flags.test(etiCanBeStopped) )
	{
		VERIFY		(m_continue_dik_guard!=-1);
		if(m_continue_dik_guard==-1)m_flags.set(etiCanBeStopped, TRUE); //not binded action :(

		if(m_continue_dik_guard==9999 || key_binding[dik] == m_continue_dik_guard)
			m_flags.set(etiCanBeStopped, TRUE); //match key

	}
}

