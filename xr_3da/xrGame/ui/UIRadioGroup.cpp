/////////////////////////////////////////////////////////////////////
// UIRadioGroup.cpp: класс для работы с группой радио кнопок
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiogroup.h"

CUIRadioGroup::CUIRadioGroup(void)
{
}

CUIRadioGroup::~CUIRadioGroup(void)
{
}


//присоединить радио кнопку
void CUIRadioGroup::AttachChild(CUIRadioButton* pChild)
{
	if(!pChild) return;

	pChild->SetParent(this);


	pChild->ResetCheck();

	m_ChildWndList.push_back(pChild);
}




//отсоединить радио кнопку
void CUIRadioGroup::DetachChild(CUIRadioButton* pChild)
{
	m_ChildWndList.remove(pChild);
}


//обработка сообщений от радио кнопок
void CUIRadioGroup::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	//обрабатываем сообщения только от дочерних окон
	if(this != pWnd->GetParent()) return;

	//переустановить флажки на кнопках в группе
	if(msg == RADIOBUTTON_SET)
	{
		//сбросить флажки на всех кнопках
		for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
		{
			((CUIRadioButton*)(*it))->ResetCheck();
		}

		//установить флажок, той кнопке, что вызывала сообщение
		((CUIRadioButton*)pWnd)->SetCheck();
	}
}


int CUIRadioGroup::GetCheckedPos()
{
	
	/*for(int i=0; i<m_ChildWndList.size(); ++i, ++it)
	{
		if(PtInRect(&(*it)->GetWndRect(), cursor_pos))
		{
			if((*it)->IsEnabled())
			{
				(*it)->OnMouse(cursor_pos.x -(*it)->GetWndRect().left, 
							   cursor_pos.y -(*it)->GetWndRect().top, mouse_action);
				return;
			}
		}
	}*/

	return 0;
}
	

void CUIRadioGroup::SetCheckedPos(int new_pos)
{
		/*//сбросить флажки на всех кнопках
		for(WINDOW_LIST_IT it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
		{
			((CRadioButton*)(*it))->ResetCheck();
		}

		//установить флажок, той кнопке, что вызывала сообщение
		((CRadioButton*)m_ChildWndList[new_pos])->SetCheck();*/
}
