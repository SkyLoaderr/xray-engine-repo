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

	//CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
	//				CUIXmlInit::ApplyAlignY(0, alCenter),
	//				UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//// „итаем из xml файла параметры окна и контролов
	//AttachChild(&UIFrameWndPrimary);
	//xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWndPrimary);

	//UIFrameWndPrimary.AttachChild(&UIStaticTextPrimary);
	//xml_init.InitStatic(uiXml, "static", 0, &UIStaticTextPrimary);
	//UIStaticTextPrimary.SetText(strCaptionPrimary);
	//UIStaticTextPrimary.SetTextColor(ColorPrimary);

	//// sign
	//UIFrameWndPrimary.AttachChild(&UITeamSign1);
	//xml_init.InitStatic(uiXml, "t1_static", 0, &UITeamSign1);

	//UIFrameWndPrimary.AttachChild(&UIButtonPrimary);
	//// ”станавливем получателем сообщений родительское окно, так как FrameWindow не обрабатывает сообщени€
	//UIButtonPrimary.SetMessageTarget(this);
	//xml_init.InitButton(uiXml, "button", 0, &UIButtonPrimary);


	//// вторичный фрейм не аттачитс€, так как по умолчанию режим отображени€ одиночный
	//xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWndSecondary);

	//UIFrameWndSecondary.AttachChild(&UIStaticTextSecondary);
	//xml_init.InitStatic(uiXml, "static", 0, &UIStaticTextSecondary);
	//UIStaticTextSecondary.SetText(strCaptionSecondary);
	//UIStaticTextSecondary.SetTextColor(ColorSecondary);

	//// sign
	//UIFrameWndSecondary.AttachChild(&UITeamSign2);
	//xml_init.InitStatic(uiXml, "t2_static", 0, &UITeamSign2);


	//UIFrameWndSecondary.AttachChild(&UIButtonSecondary);
	//// ”станавливем получателем сообщений родительское окно, так как FrameWindow не ретровает сообщени€
	//UIButtonSecondary.SetMessageTarget(this);
	//xml_init.InitButton(uiXml, "button", 0, &UIButtonSecondary);

	//SetDisplayMode(bDual);
}

void CUISpawnWnd::SetDisplayMode(bool bDual)
{
	//m_bDual = bDual;

	//Frect frameCoords = UIFrameWndSecondary.GetWndRect();
	//float updatedX, updatedY;

	//updatedY = UI_BASE_HEIGHT / 2 - (frameCoords.bottom - frameCoords.top) / 2;

	//if (m_bDual)
	//{
	//	// ≈сли окна 2, то аттачим вторичные контролы
	//	AttachChild(&UIFrameWndSecondary);
	//	
	//	updatedX = UI_BASE_WIDTH / 4 - (frameCoords.right - frameCoords.left) / 2;
	//	updatedX += updatedX / 2;
	//	// обновить второе окно
	//	UIFrameWndSecondary.SetWndRect(2 * updatedX + frameCoords.right, updatedY, frameCoords.right, frameCoords.bottom);
	//}
	//else
	//{
	//	DetachChild(&UIFrameWndSecondary);
	//	updatedX = UI_BASE_WIDTH / 2 - (frameCoords.right - frameCoords.left) / 2;
	//}
	//// обновить первое окно
	//UIFrameWndPrimary.SetWndRect(updatedX, updatedY, frameCoords.right, frameCoords.bottom);
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
