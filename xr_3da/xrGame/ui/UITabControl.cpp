// ���������� ���� � ����������.

#include "StdAfx.h"
#include "UITabControl.h"
#include "UITabButton.h"

CUITabControl::CUITabControl()
	: m_iPushedIndex		(0),
      m_iPrevPushedIndex	(0),
	  m_cGlobalTextColor	(0xFFFFFFFF),
	  m_cActiveTextColor	(0xFFFFFFFF),
	  m_cActiveButtonColor	(0xFFFFFFFF),
	  m_cGlobalButtonColor	(0xFFFFFFFF),
	  m_bAcceleratorsEnable	(true),
	  m_bChangeColors		(false)
{

}

CUITabControl::~CUITabControl()
{
	RemoveAll();
}

// ���������� ������-�������� � ������ �������� ��������
bool CUITabControl::AddItem(const char *pItemName, const char *pTexName, int x, int y, int width, int height)
{
	CUITabButton *pNewButton = xr_new<CUITabButton>();
	if (!pNewButton) return false;

	pNewButton->Init(pTexName, x, y, width, height);
	pNewButton->SetText(pItemName);
	pNewButton->SetTextColor(m_cGlobalTextColor);
	pNewButton->GetUIStaticItem().SetColor(m_cGlobalButtonColor);

	return AddItem(pNewButton);
}

bool CUITabControl::AddItem(CUITabButton *pButton)
{
	pButton->Show(true);
	pButton->Enable(true);
	pButton->SetButtonAsSwitch(true);

	// �������� ������ �� ���������
	if (m_iPushedIndex == static_cast<int>(m_TabsArr.size() - 1))
        m_TabsArr[m_iPushedIndex]->SendMessage(m_TabsArr[m_iPushedIndex], TAB_CHANGED, NULL);

	//{
	//	m_TabsArr[m_iPushedIndex]->OnMouse(1, 1, WINDOW_LBUTTON_DOWN);
	//	SetCapture(m_TabsArr[m_iPushedIndex], false);
	//}

	AttachChild(pButton);
	m_TabsArr.push_back(pButton);
	return true;
}

// �������� �������� �� �������. ������� ���������� � Index
void CUITabControl::RemoveItem(u32 Index)
{
	R_ASSERT(m_TabsArr.size() > Index);
	DetachChild(m_TabsArr[Index]);

	// ������ �������� ��������� ��������, � ���������� ��������.
	// ��� ��� � ��� �������� ��������� �������� ����� ��������� ������.
	TABS_VECTOR::value_type tmp = m_TabsArr[Index];
	m_TabsArr[Index] = m_TabsArr.back();
	m_TabsArr.back() = tmp;

	xr_delete(m_TabsArr.back());
	m_TabsArr.pop_back();
}

// �������� ���� ���������
void CUITabControl::RemoveAll()
{
	TABS_VECTOR_it it = m_TabsArr.begin();
	for (; it != m_TabsArr.end(); ++it)
	{
		DetachChild(*it);
		xr_delete(*it);
	}
	m_TabsArr.clear();
}

// ������������ ��������.
void CUITabControl::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (TAB_CHANGED == msg)
	{
		for (u32 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (m_TabsArr[i] == pWnd)
			{
				m_iPushedIndex = i;
				if (m_iPrevPushedIndex == m_iPushedIndex)
					return; // return if nothing was changed

				OnTabChange(m_iPushedIndex, m_iPrevPushedIndex);
				m_iPrevPushedIndex = m_iPushedIndex;							
				break;
			}
		}
	}
	//else if (BUTTON_CLICKED == msg)
	//{
	//	// ���� ������ �� �������� ������, �� ������ �� ������.
	//	TABS_VECTOR::value_type pushedItem = smart_cast<TABS_VECTOR::value_type>(pWnd);
	//	if (!pushedItem) return;

	//	if (msg != BUTTON_CLICKED		||
	//		m_iPushedIndex > -1						&&
	//		pushedItem == m_TabsArr[m_iPushedIndex])
	//		return;

	//	for (u32 i = 0; i < m_TabsArr.size(); ++i)
	//	{
	//		if (m_TabsArr[i] == pWnd)
	//		{
	//			int iPrevPushedIndex = m_iPushedIndex;
	//			m_iPushedIndex = i;
	//			m_TabsArr[iPrevPushedIndex]->SetButtonMode(CUIButton::BUTTON_NORMAL);
	//			m_TabsArr[iPrevPushedIndex]->OnMouse(-1, -1, WINDOW_MOUSE_MOVE);
	//			GetMessageTarget()->SendMessage(this, TAB_CHANGED, static_cast<void*>(&iPrevPushedIndex));
	//			break;
	//		}
	//	}
	//}
	else if (STATIC_FOCUS_RECEIVED	== msg	||
			 STATIC_FOCUS_LOST		== msg)
	{
		for (u8 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (pWnd == m_TabsArr[i])
			{				
				if (msg == STATIC_FOCUS_RECEIVED)
                    OnStaticFocusReceive(pWnd);
				else
					OnStaticFocusLost(pWnd);

			}
		}
	}
	else
	{
		inherited::SendMessage(pWnd, msg, pData);
	}
}

void CUITabControl::OnStaticFocusReceive(CUIWindow* pWnd){
	GetMessageTarget()->SendMessage(this, STATIC_FOCUS_RECEIVED, static_cast<void*>(pWnd));
}

void CUITabControl::OnStaticFocusLost(CUIWindow* pWnd){
	GetMessageTarget()->SendMessage(this, STATIC_FOCUS_LOST, static_cast<void*>(pWnd));
}

void CUITabControl::OnTabChange(int iCur, int iPrev){
	m_TabsArr[iPrev]->SendMessage(m_TabsArr[iCur], TAB_CHANGED, NULL);
	m_TabsArr [iCur]->SendMessage(m_TabsArr[iCur], TAB_CHANGED, NULL);	
	GetMessageTarget()->SendMessage(this, TAB_CHANGED, NULL);
}

void CUITabControl::Init(int x, int y, int width, int height)
{
	CUIWindow::Init(x, y, width, height);
}

void CUITabControl::SetNewActiveTab(const int iNewTab)
{
	if (m_iPushedIndex == iNewTab)
		return;
    m_iPrevPushedIndex = m_iPushedIndex;
	m_iPushedIndex = iNewTab;

	OnTabChange(m_iPushedIndex, m_iPrevPushedIndex);
}

bool CUITabControl::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (m_bAcceleratorsEnable && WINDOW_KEY_PRESSED == keyboard_action)
	{
		for (u32 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (m_TabsArr[i]->GetAccelerator() == static_cast<u32>(dik))
			{
				SetNewActiveTab(i);
				return true;
			}
		}
	}
	return false;
}

void CUITabControl::Update()
{
	if (m_bChangeColors)
	{
		m_bChangeColors = false;

		for (u32 i = 0; i < m_TabsArr.size(); ++i)
			if (i == static_cast<u32>(m_iPushedIndex))
			{
				m_TabsArr[i]->SetTextColor(m_cActiveTextColor);
				m_TabsArr[i]->GetUIStaticItem().SetColor(m_cActiveButtonColor);
			}
			else
			{
				m_TabsArr[i]->SetTextColor(m_cGlobalTextColor);
				m_TabsArr[i]->GetUIStaticItem().SetColor(m_cGlobalButtonColor);
			}
	}

	inherited::Update();
}