#include "stdafx.h"
#include <dinput.h>
#include "UISpawnWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "../level.h"
#include "../game_cl_teamdeathmatch.h"
#include "UIStatix.h"

CUISpawnWnd::CUISpawnWnd()
//	: m_bDual(false),
//	  m_iResult(-1)
{	
	m_pBackground	= xr_new<CUIStatic>();	AttachChild(m_pBackground);	
	m_pCaption		= xr_new<CUIStatic>();	AttachChild(m_pCaption);	
	m_pImage1		= xr_new<CUIStatix>();	AttachChild(m_pImage1);
	m_pImage2		= xr_new<CUIStatix>();	AttachChild(m_pImage2);

	m_pFrames[0]	= xr_new<CUIStatic>();	AttachChild(m_pFrames[0]);
	m_pFrames[1]	= xr_new<CUIStatic>();	AttachChild(m_pFrames[1]);
	m_pFrames[2]	= xr_new<CUIStatic>();	AttachChild(m_pFrames[2]);
	Init();
	
}

CUISpawnWnd::~CUISpawnWnd()
{
	xr_delete(m_pCaption);
	xr_delete(m_pBackground);
	xr_delete(m_pFrames[0]);
	xr_delete(m_pFrames[1]);
	xr_delete(m_pFrames[2]);
	xr_delete(m_pImage1);
	xr_delete(m_pImage2);
}



void CUISpawnWnd::Init()
{
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "spawn.xml");
	R_ASSERT3(xml_result, "xml file not found", "spawn.xml");

	CUIXmlInit::InitWindow(xml_doc,"team_selector",						0,	this);
	CUIXmlInit::InitStatic(xml_doc,"team_selector:caption",				0,	m_pCaption);
	CUIXmlInit::InitStatic(xml_doc,"team_selector:background",			0,	m_pBackground);
	CUIXmlInit::InitStatic(xml_doc,"team_selector:image_frames_tl",		0,	m_pFrames[0]);
	CUIXmlInit::InitStatic(xml_doc,"team_selector:image_frames_tr",		0,	m_pFrames[1]);
	CUIXmlInit::InitStatic(xml_doc,"team_selector:image_frames_bottom",	0,	m_pFrames[2]);

	Frect r;
	r.set(0,0,256,256);

	CUIXmlInit::InitStatic(xml_doc,"team_selector:image_0",0,m_pImage1);
	m_pImage1->SetStretchTexture(true);
	m_pImage1->RescaleRelative2Rect(r);
	CUIXmlInit::InitStatic(xml_doc,"team_selector:image_1",0,m_pImage2);
	m_pImage2->SetStretchTexture(true);
	m_pImage2->RescaleRelative2Rect(r);

	InitTeamLogo();
}

void CUISpawnWnd::InitTeamLogo(){
	R_ASSERT(pSettings->section_exist("team_logo"));
	R_ASSERT(pSettings->line_exist("team_logo", "team1"));
	R_ASSERT(pSettings->line_exist("team_logo", "team2"));

	m_pImage1->InitTexture(pSettings->r_string("team_logo", "team1"));
	m_pImage2->InitTexture(pSettings->r_string("team_logo", "team2"));
}

void CUISpawnWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (BUTTON_CLICKED == msg)
	{
		Game().StartStopMenu(this,true);
		game_cl_TeamDeathmatch * dm = smart_cast<game_cl_TeamDeathmatch *>(&(Game()));
		if (pWnd == m_pImage1)
			dm->OnTeamSelect(0);
		else
			dm->OnTeamSelect(1);		
	}

	inherited::SendMessage(pWnd, msg, pData);
}

////////////////////////////////////////////////////////////////////////////////

bool CUISpawnWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if ((DIK_1 == dik || DIK_2 == dik ) && WINDOW_KEY_PRESSED == keyboard_action)
	{
		Game().StartStopMenu(this,true);
		game_cl_TeamDeathmatch * dm = smart_cast<game_cl_TeamDeathmatch *>(&(Game()));
		if (DIK_1 == dik)
			dm->OnTeamSelect(0);
		else
			dm->OnTeamSelect(1);
		return true;
	}

	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_ESCAPE == dik)
	{
		Game().StartStopMenu(this,true);
		return true;
	}

	return inherited::OnKeyboard(dik, keyboard_action);
}
