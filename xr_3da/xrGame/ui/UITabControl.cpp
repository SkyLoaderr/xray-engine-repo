// Реализация окна с закладками.

#include "StdAfx.h"
#include "UITabControl.h"

CUITabControl::CUITabControl()
	: m_iPushedIndex(0),
	  m_cGlobalColor(0xFFFFFFFF),
	  m_cActiveColor(0xFFFFFFFF)
{

}

CUITabControl::~CUITabControl()
{
	RemoveAll();
}

// добавление кнопки-закладки в список закладок контрола
bool CUITabControl::AddItem(const char *pItemName, const char *pTexName, int x, int y, int width, int height)
{
	CUIButton *pNewButton = xr_new<CUIButton>();
	if (!pNewButton) return false;

	pNewButton->Init(pTexName, x, y, width, height);
	pNewButton->SetText(pItemName);
	pNewButton->SetTextColor(m_cGlobalColor);

	return AddItem(pNewButton);
}

bool CUITabControl::AddItem(CUIButton *pButton)
{
	pButton->Show(true);
	pButton->Enable(true);
	pButton->SetButtonAsSwitch(true);

	// Нажимаем кнопку по умолчанию
	if (m_iPushedIndex == static_cast<int>(m_TabsArr.size() - 1))
	{
		m_TabsArr[m_iPushedIndex]->OnMouse(1, 1, CUIWindow::LBUTTON_DOWN);
		SetCapture(m_TabsArr[m_iPushedIndex], false);
	}

	AttachChild(pButton);
	m_TabsArr.push_back(pButton);
	return true;
}

// Удаление элемента по индексу. Индексы начинаются с 0
void CUITabControl::RemoveItem(u32 Index)
{
	R_ASSERT(m_TabsArr.size() > Index);
	DetachChild(m_TabsArr[Index]);

	// Меняем значение заданного элемента, и последнего элемента.
	// Так как у нас хранятся указатели операция будет проходить быстро.
	TABS_VECTOR::value_type tmp = m_TabsArr[Index];
	m_TabsArr[Index] = m_TabsArr.back();
	m_TabsArr.back() = tmp;

	xr_delete(m_TabsArr.back());
	m_TabsArr.pop_back();
}

// Удаление всех элементов
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

// переключение закладок.
void CUITabControl::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	// если нажали на активную кнопку, то ничего не делать.
	TABS_VECTOR::value_type pushedItem = dynamic_cast<TABS_VECTOR::value_type>(pWnd);
	if (!pushedItem) return;

	if (msg != CUIButton::BUTTON_CLICKED || m_iPushedIndex > -1 && pushedItem == m_TabsArr[m_iPushedIndex]) return;

	for (u32 i = 0; i < m_TabsArr.size(); ++i)
	{
		if (m_TabsArr[i] == pWnd)
		{
			int iPrevPushedIndex = m_iPushedIndex;
			m_iPushedIndex = i;
			m_TabsArr[iPrevPushedIndex]->SetButtonMode(CUIButton::BUTTON_NORMAL);
			m_TabsArr[iPrevPushedIndex]->OnMouse(-1, -1, CUIWindow::MOUSE_MOVE);
			GetMessageTarget()->SendMessage(this, TAB_CHANGED, static_cast<void*>(&iPrevPushedIndex));
			break;
		}
	}
}

void CUITabControl::Init(int x, int y, int width, int height)
{
	CUIWindow::Init(x, y, width, height);
}

void CUITabControl::SetNewActiveTab(const int iNewTab)
{
	R_ASSERT(iNewTab > 0 || iNewTab < GetTabsCount());
	m_TabsArr[iNewTab]->OnMouse(1, 1, LBUTTON_DOWN);
	SendMessage(m_TabsArr[iNewTab], CUIButton::BUTTON_CLICKED, NULL);
}
