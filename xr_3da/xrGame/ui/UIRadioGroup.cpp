/////////////////////////////////////////////////////////////////////
// UIRadioGroup.cpp: ����� ��� ������ � ������� ����� ������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiogroup.h"

CUIRadioGroup::CUIRadioGroup(void)
{
}

CUIRadioGroup::~CUIRadioGroup(void)
{
}


//������������ ����� ������
void CUIRadioGroup::AttachChild(CUIRadioButton* pChild)
{
	if(!pChild) return;

	pChild->SetParent(this);


	pChild->ResetCheck();

	m_ChildWndList.push_back(pChild);
}




//����������� ����� ������
void CUIRadioGroup::DetachChild(CUIRadioButton* pChild)
{
	m_ChildWndList.remove(pChild);
}


//��������� ��������� �� ����� ������
void CUIRadioGroup::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	//������������ ��������� ������ �� �������� ����
	if(this != pWnd->GetParent()) return;

	//�������������� ������ �� ������� � ������
	if(msg == CUIRadioButton::RADIO_BUTTON_SET)
	{
		//�������� ������ �� ���� �������
		for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
		{
			((CUIRadioButton*)(*it))->ResetCheck();
		}

		//���������� ������, ��� ������, ��� �������� ���������
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
		/*//�������� ������ �� ���� �������
		for(WINDOW_LIST_IT it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
		{
			((CRadioButton*)(*it))->ResetCheck();
		}

		//���������� ������, ��� ������, ��� �������� ���������
		((CRadioButton*)m_ChildWndList[new_pos])->SetCheck();*/
}
