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
	if(msg == RADIOBUTTON_SET)
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
	int res = -1;
	int i = 0;
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it,++i){
		if(((CUIRadioButton*)(*it))->GetCheck() == true){
			res=i;
			return res;
		}
	}

	return res;
}
	

void CUIRadioGroup::SetCheckedPos(int new_pos)
{
		//�������� ������ �� ���� �������
		WINDOW_LIST_it it = m_ChildWndList.begin();
		for(; it != m_ChildWndList.end() ; ++it)
			((CUIRadioButton*)(*it))->ResetCheck();

		if(new_pos>=m_ChildWndList.size()) return;
		//���������� ������, ��� ������, ��� �������� ���������
		it = m_ChildWndList.begin();
		std::advance(it,new_pos);
		((CUIRadioButton*)(*it))->SetCheck();
}
