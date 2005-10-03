#include "stdafx.h"
#include "UIVideoPlayerWnd.h"
#include "UITabControl.h"
#include "UIStatic.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "UIXmlInit.h"
#include "../level.h"
#include "../hudmanager.h"

void CUIVideoPlayerWnd::SendMessage	(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent			(pWnd, msg, pData);
}

void CUIVideoPlayerWnd::Init	(LPCSTR file_name)
{
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, "video_templ.xml");
	R_ASSERT3						(xml_result, "xml file not found", "video_templ.xml");
	Init							(&uiXml,"video_player");
	SetFile							(file_name);
}

void CUIVideoPlayerWnd::Init			(CUIXml* doc, LPCSTR start_from)
{
	m_flags.zero					();
	CUIXmlInit						xml_init;
	string512						str;
	strcpy							(str,start_from);
	xml_init.InitWindow				(*doc, str, 0, this);
	

	strconcat						(str,start_from,":surface");
	m_surface						= xr_new<CUIStatic>(); m_surface->SetAutoDelete(true);
	AttachChild						(m_surface);
	xml_init.InitStatic				(*doc, str, 0, m_surface);

	strconcat						(str,start_from,":buttons_tab");
	m_tabControl					= xr_new<CUITabControl>(); m_tabControl->SetAutoDelete(true);
	AttachChild						(m_tabControl);
	xml_init.InitTabControl			(*doc, str, 0, m_tabControl);
	m_tabControl->SetWindowName		("buttons_tab");
	Register						(m_tabControl);
    AddCallback						("buttons_tab",TAB_CHANGED,boost::bind(&CUIVideoPlayerWnd::OnTabChanged,this,_1,_2));

	int flag						=doc->ReadAttribInt(start_from, 0, "auto_play", 0);
	m_flags.set						(eAutoPlay, flag?true:false);
	SetFile							( doc->ReadAttrib(start_from, 0, "file", 0) );
}

void CUIVideoPlayerWnd::SetFile		(LPCSTR fn)
{
	m_fn = fn;
	if(fn && fn[0]){
		VERIFY(!m_surface->GetShader());
		m_surface->InitTexture			(fn);

		::Sound->create(m_sound, true, fn);

		if(m_flags.test(eAutoPlay))
			Play	();
		else
			Stop	();

	}
}

void CUIVideoPlayerWnd::Draw		()
{
	inherited::Draw	();
	if(!m_texture && m_surface->GetShader()){
		RCache.set_Shader		(m_surface->GetShader());
		m_texture = RCache.get_ActiveTexture		(0);
	}
}

void CUIVideoPlayerWnd::Update	()
{
	inherited::Update	();

	if(m_flags.test(ePlaying) && !m_flags.test(eStarted) && m_texture && !m_texture->video_IsPlaying()){
		m_texture->video_Play		(TRUE);
		m_flags.set					(eStarted,TRUE);
	};
	if(!m_flags.test(ePlaying) && m_texture && m_texture->video_IsPlaying()){
		m_texture->video_Stop		();
		m_flags.set					(eStarted,FALSE);
	};
}

void CUIVideoPlayerWnd::OnTabChanged			(CUIWindow* pWnd, void* pData)
{
	if(m_tabControl->GetCommandName( m_tabControl->GetActiveIndex())=="play_btn" ){
		Play		();
	}else
	if(m_tabControl->GetCommandName( m_tabControl->GetActiveIndex())=="stop_btn" ){
		Stop		();
	}
}

void CUIVideoPlayerWnd::Play	()
{
	m_flags.set(ePlaying, TRUE);
	if (m_sound._handle())
        m_sound.play(NULL, sm_2D);
}

void CUIVideoPlayerWnd::Stop	()
{
	m_flags.set(ePlaying, FALSE);
	if (m_sound._handle())
        m_sound.stop();
}

bool CUIVideoPlayerWnd::IsPlaying	()
{
	return m_texture && m_texture->video_IsPlaying();
}

void CUIActorSleepVideoPlayer::Activate	()
{
	HUD().GetUI()->StartStopMenu(this,true);
}

void CUIActorSleepVideoPlayer::DeActivate	()
{
	HUD().GetUI()->StartStopMenu(this,true);
}
