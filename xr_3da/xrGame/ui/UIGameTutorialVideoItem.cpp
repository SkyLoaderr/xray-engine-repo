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
CUISequenceVideoItem::CUISequenceVideoItem(CUISequencer* owner):CUISequenceItem(owner)
{
	m_texture				= NULL;
	m_flags.set				(etiPlaying|etiNeedStart|etiDelayed|etiBackVisible,FALSE);
	m_delay					= 0.f;
	m_wnd					= NULL;
	m_delay					= 0.f;
	m_time_start			= 0;
}

CUISequenceVideoItem::~CUISequenceVideoItem()
{
	m_sound[0].stop			();
	m_sound[1].stop			();
	delete_data				(m_wnd);
}

bool CUISequenceVideoItem::IsPlaying()
{
	return					(!!m_flags.test(etiPlaying));
}

void CUISequenceVideoItem::Load(CUIXml* xml, int idx)
{
	CUISequenceItem::Load	(xml,idx);

	XML_NODE* _stored_root	= xml->GetLocalRoot();
	xml->SetLocalRoot		(xml->NavigateToNode("item",idx));
	
	LPCSTR str				= xml->Read				("pause_state",0,"ignore");
	m_flags.set										(etiNeedPauseOn,	0==_stricmp(str, "on"));
	m_flags.set										(etiNeedPauseOff,	0==_stricmp(str, "off"));
	
	str						= xml->Read				("can_be_stopped",0,"on");
	m_flags.set										(etiCanBeStopped,	0==_stricmp(str, "on"));

	str						= xml->Read				("back_show",0,"on");
	m_flags.set										(etiBackVisible,	0==_stricmp(str, "on"));

	m_flags.set										(etiGrabInput,		TRUE);

	m_delay					= _max(xml->ReadFlt		("delay",0,0.f),0.f);

	//ui-components
	m_wnd					= xr_new<CUIStatic>();
	m_wnd->SetAutoDelete	(false);
	CUIXmlInit xml_init;
	xml_init.InitStatic		(*xml, "video_wnd", 0, m_wnd);

	LPCSTR m_snd_name		= xml->Read			("sound",0,"");
	if (m_snd_name&&m_snd_name[0]){
		string_path			_l, _r;
		strconcat			(_l, m_snd_name, "_l");
		strconcat			(_r, m_snd_name, "_r");
		m_sound[0].create	(TRUE,_l,0);	VERIFY(m_sound[0]._handle());
		m_sound[1].create	(TRUE,_r,0);	VERIFY(m_sound[1]._handle());
	}
	xml->SetLocalRoot		(_stored_root);
}

void CUISequenceVideoItem::Update()
{
	// deferred start
	if (Device.dwTimeContinual>m_time_start){
		if (m_flags.test(etiDelayed)){
			m_owner->MainWnd()->AttachChild	(m_wnd);
			m_wnd->Show		(true);
			m_flags.set		(etiDelayed,FALSE);
		}
	}else return;

	// processing A&V
	if (m_texture){
		if (m_texture->video_IsPlaying()){
			m_texture->video_Sync		(Device.dwTimeContinual);
		}else{
			// sync start
			if (m_flags.test(etiNeedStart)){
				m_sound[0].play_at_pos	(NULL, Fvector().set(-0.5f,0.f,0.3f), sm_2D);
				m_sound[1].play_at_pos	(NULL, Fvector().set(+0.5f,0.f,0.3f), sm_2D);
				m_texture->video_Play	(FALSE,Device.dwTimeContinual);
				m_flags.set	(etiNeedStart,FALSE);
				CUIWindow* w			= m_owner->MainWnd()->FindChild("back");
				if (w)					w->Show(m_flags.test(etiBackVisible));
			}else{
				m_flags.set				(etiPlaying,FALSE);
			}
		}
	}
}

void CUISequenceVideoItem::OnRender()
{
	if (NULL==m_texture && m_wnd->GetShader()){
		RCache.set_Shader					(m_wnd->GetShader());
		m_texture = RCache.get_ActiveTexture(0);
		m_texture->video_Stop				();
	}
}

void CUISequenceVideoItem::Start()
{
	m_flags.set					(etiStoredPauseState, Device.Pause());

	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState)){
		Device.Pause			(TRUE);
		bShowPauseString		= FALSE;
	}

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	m_flags.set					(etiPlaying,TRUE);
	m_flags.set					(etiNeedStart,TRUE);

	m_time_start				= Device.dwTimeContinual+iFloor(m_delay*1000.f);
	m_flags.set					(etiDelayed,TRUE);

	if (m_flags.test(etiBackVisible)){
		CUIWindow* w			= m_owner->MainWnd()->FindChild("back");
		if (w)					w->Show(true);
	}
}

bool CUISequenceVideoItem::Stop	(bool bForce)
{
	if(!m_flags.test(etiCanBeStopped)&&!bForce) 
		return false;

	m_flags.set					(etiPlaying,FALSE);

	m_wnd->Show					(false);
	m_owner->MainWnd()->DetachChild(m_wnd);

	m_sound[0].stop				();
	m_sound[1].stop				();
	m_texture					= 0;

	if(m_flags.test(etiNeedPauseOn) && !m_flags.test(etiStoredPauseState))
		Device.Pause			(FALSE);

	if(m_flags.test(etiNeedPauseOff) && m_flags.test(etiStoredPauseState))
		Device.Pause			(TRUE);

	return true;
}
