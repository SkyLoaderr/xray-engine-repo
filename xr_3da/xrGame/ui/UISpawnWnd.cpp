#include "stdafx.h"

#include "UISpawnWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../hudmanager.h"

//-----------------------------------------------------------------------------/
//  Ctor and Dtor
//-----------------------------------------------------------------------------/

CUISpawnWnd::CUISpawnWnd()
	: m_bDual(false),
	  m_iResult(-1),
	  pCallbackFunc(NULL)
{
	Init("-= Red Team =-", "-= Green Team =-", true);

	SetFont(HUD().pFontMedium);
}

CUISpawnWnd::~CUISpawnWnd()
{

}

void	CUISpawnWnd::SetCallbackFunc (ButtonClickCallback pFunc)
{
	pCallbackFunc = pFunc;
};
void CUISpawnWnd::Init(const char *strCaptionPrimary, const char *strCaptionSecondary, bool bDual)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", "spawn.xml");
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	// ������ �� xml ����� ��������� ���� � ���������
	AttachChild(&UIFrameWndPrimary);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWndPrimary);

	UIFrameWndPrimary.AttachChild(&UIStaticTextPrimary);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticTextPrimary);
	UIStaticTextPrimary.SetText(strCaptionPrimary);

	UIFrameWndPrimary.AttachChild(&UIButtonPrimary);
	// ������������ ����������� ��������� ������������ ����, ��� ��� FrameWindow �� ������������ ���������
	UIButtonPrimary.SetMessageTarget(this);
	xml_init.InitButton(uiXml, "button", 0, &UIButtonPrimary);

	// ��������� ����� �� ���������, ��� ��� �� ��������� ����� ����������� ���������
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIFrameWndSecondary);

	UIFrameWndSecondary.AttachChild(&UIStaticTextSecondary);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticTextSecondary);
	UIStaticTextSecondary.SetText(strCaptionSecondary);

	UIFrameWndSecondary.AttachChild(&UIButtonSecondary);
	// ������������ ����������� ��������� ������������ ����, ��� ��� FrameWindow �� ��������� ���������
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
		// ���� ���� 2, �� ������� ��������� ��������
		AttachChild(&UIFrameWndSecondary);
		
		updatedX = Device.dwWidth / 4 - (frameCoords.right - frameCoords.left) / 2;
		updatedX += updatedX / 2;
		// �������� ������ ����
		UIFrameWndSecondary.SetWndRect(2 * updatedX + frameCoords.right, updatedY, frameCoords.right, frameCoords.bottom);
	}
	else
	{
		DetachChild(&UIFrameWndSecondary);
		updatedX = Device.dwWidth / 2 - (frameCoords.right - frameCoords.left) / 2;
	}
	// �������� ������ ����
	UIFrameWndPrimary.SetWndRect(updatedX, updatedY, frameCoords.right, frameCoords.bottom);
}

void CUISpawnWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (CUIButton::BUTTON_CLICKED == msg)
	{
		if (pWnd == &UIButtonPrimary)
			m_iResult = 0;
		else
			m_iResult = 1;
		if (pCallbackFunc) 
		{
			pCallbackFunc(m_iResult+1);
			m_iResult = -1;
		};
		HUD().GetUI()->UIGame()->StartStopMenu(this);
	}

	inherited::SendMessage(pWnd, msg, pData);
}
