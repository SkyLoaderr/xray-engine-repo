#include "stdafx.h"
#include "UISpawnWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "../level.h"
#include "../game_cl_teamdeathmatch.h"

//////////////////////////////////////////////////////////////////////////

const char * const SPAWN_XML		= "spawn.xml";

//-----------------------------------------------------------------------------/
//  Ctor and Dtor
//-----------------------------------------------------------------------------/

CUISpawnWnd::CUISpawnWnd()
	: m_bDual(false),
	  m_iResult(-1)//,
//	  pCallbackFunc(NULL)
{
	Init("-= Green Team =-", 0xff40ff40, "-= Blue Team =-", 0xff4040ff, true);

	SetFont(HUD().pFontMedium);
}

CUISpawnWnd::~CUISpawnWnd()
{

}

//void	CUISpawnWnd::SetCallbackFunc (ButtonClickCallback pFunc)
//{
//	pCallbackFunc = pFunc;
//};
void CUISpawnWnd::Init(	const char *strCaptionPrimary, const u32 ColorPrimary,
						const char *strCaptionSecondary, const u32 ColorSecondary, 
						bool bDual)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", SPAWN_XML);
	R_ASSERT3(xml_result, "xml file not found", SPAWN_XML);

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					Device.dwWidth, Device.dwHeight);

	// „итаем из xml файла параметры окна и контролов
	AttachChild(&UIFrameWndPrimary);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWndPrimary);

	UIFrameWndPrimary.AttachChild(&UIStaticTextPrimary);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticTextPrimary);
	UIStaticTextPrimary.SetText(strCaptionPrimary);
	UIStaticTextPrimary.SetTextColor(ColorPrimary);

	// sign
	UIFrameWndPrimary.AttachChild(&UITeamSign1);
	xml_init.InitStatic(uiXml, "t1_static", 0, &UITeamSign1);

	UIFrameWndPrimary.AttachChild(&UIButtonPrimary);
	// ”станавливем получателем сообщений родительское окно, так как FrameWindow не обрабатывает сообщени€
	UIButtonPrimary.SetMessageTarget(this);
	xml_init.InitButton(uiXml, "button", 0, &UIButtonPrimary);


	// вторичный фрейм не аттачитс€, так как по умолчанию режим отображени€ одиночный
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWndSecondary);

	UIFrameWndSecondary.AttachChild(&UIStaticTextSecondary);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticTextSecondary);
	UIStaticTextSecondary.SetText(strCaptionSecondary);
	UIStaticTextSecondary.SetTextColor(ColorSecondary);

	// sign
	UIFrameWndSecondary.AttachChild(&UITeamSign2);
	xml_init.InitStatic(uiXml, "t2_static", 0, &UITeamSign2);


	UIFrameWndSecondary.AttachChild(&UIButtonSecondary);
	// ”станавливем получателем сообщений родительское окно, так как FrameWindow не ретровает сообщени€
	UIButtonSecondary.SetMessageTarget(this);
	xml_init.InitButton(uiXml, "button", 0, &UIButtonSecondary);

	SetDisplayMode(bDual);
}

void CUISpawnWnd::SetDisplayMode(bool bDual)
{
	m_bDual = bDual;

	RECT frameCoords = UIFrameWndSecondary.GetWndRect();
	int updatedX, updatedY;

	updatedY = Device.dwHeight / 2 - (frameCoords.bottom - frameCoords.top) / 2;

	if (m_bDual)
	{
		// ≈сли окна 2, то аттачим вторичные контролы
		AttachChild(&UIFrameWndSecondary);
		
		updatedX = Device.dwWidth / 4 - (frameCoords.right - frameCoords.left) / 2;
		updatedX += updatedX / 2;
		// обновить второе окно
		UIFrameWndSecondary.SetWndRect(2 * updatedX + frameCoords.right, updatedY, frameCoords.right, frameCoords.bottom);
	}
	else
	{
		DetachChild(&UIFrameWndSecondary);
		updatedX = Device.dwWidth / 2 - (frameCoords.right - frameCoords.left) / 2;
	}
	// обновить первое окно
	UIFrameWndPrimary.SetWndRect(updatedX, updatedY, frameCoords.right, frameCoords.bottom);
}

void CUISpawnWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (BUTTON_CLICKED == msg)
	{
		if (pWnd == &UIButtonPrimary)
			m_iResult = 0;
		else
			m_iResult = 1;
//		if (pCallbackFunc) 
//		{
//			pCallbackFunc(m_iResult+1);
//			m_iResult = -1;
//		};
		Game().StartStopMenu(this,true);
		game_cl_TeamDeathmatch * dm = smart_cast<game_cl_TeamDeathmatch *>(&(Game()));
		dm->OnTeamSelect(m_iResult);
	}

	inherited::SendMessage(pWnd, msg, pData);
}

////////////////////////////////////////////////////////////////////////////////

bool CUISpawnWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_ESCAPE == dik)
	{
		Game().StartStopMenu(this,true);
		return true;
	}

	return inherited::OnKeyboard(dik, keyboard_action);
}
