//////////////////////////////////////////////////////////////////////
// UIListItem.h: ������� ���� ������ CUIListWnd
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_ITEM_H_
#define _UI_LIST_ITEM_H_


#pragma once
#include "UIbutton.h"

class CUIListItem :	public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUIListItem(void);
	virtual ~CUIListItem(void);

	virtual void Init(int x, int y, int width, int height);
	virtual void Init(const char* str, int x, int y, int width, int height);
	
	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void Draw();

			void* GetData() {return m_pData;}
			void SetData(void* pData) { m_pData = pData;}

			int GetIndex() {return m_iIndex;}
			void SetIndex(int index) {m_iIndex = index; m_iGroupID = index;}

			int GetValue() {return m_iValue;}
			void SetValue(int value) {m_iValue = value;}

			int	GetGroupID() { return m_iGroupID; }
			void SetGroupID(int ID) { m_iGroupID = ID; }

			//���������� ����� � �������� �������
			int GetSignWidht();

	// �������������� �������� ������������� ������
	virtual bool IsHighlightText();
	virtual void SetHighlightText(bool Highlight)		{ m_bHighlightText = Highlight; }

protected:
	//��������� �� ������������ ������, ������� �����
	//������������� � ��������
	void* m_pData;
	
	//������������ �����, ����������� �������
	int m_iValue;
	
	//������ � ������
	int m_iIndex;

	// ������������� ������
	int m_iGroupID;

	// �������������� ������ ��� ���?
	bool m_bHighlightText;
};

#endif