//////////////////////////////////////////////////////////////////////
// UIListItem.h: ������� ���� ������ CUIListWnd
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_ITEM_H_
#define _UI_LIST_ITEM_H_


#pragma once
#include "UIbutton.h"

class CUIListItem :
	public CUIButton
{
public:
	CUIListItem(void);
	virtual ~CUIListItem(void);

	virtual void Init(char* str, int x, int y, int width, int height);
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);
	virtual void Draw();

	virtual void* GetData() {return m_pData;}
	virtual void SetData(void* pData) { m_pData = pData;}

	virtual int GetIndex() {return m_iIndex;}
	virtual void SetIndex(int index) {m_iIndex = index;}

	virtual int GetValue() {return m_iValue;}
	virtual void SetValue(int value) {m_iValue = value;}

	//���������� ����� � �������� �������
	virtual int GetSignWidht();

protected:
	//��������� �� ������������ ������, ������� �����
	//������������� � ��������
	void* m_pData;
	
	//������������ �����, ����������� �������
	int m_iValue;
	
	//������ � ������
	int m_iIndex;
};


#endif