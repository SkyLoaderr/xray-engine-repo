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

	virtual void* GetData() {return m_pData;}
	virtual void SetData(void* pData) { m_pData = pData;}

	virtual int GetIndex() {return m_iIndex;}
	virtual void SetIndex(int index) {m_iIndex = index; m_iGroupID = index;}

	virtual int GetValue() {return m_iValue;}
	virtual void SetValue(int value) {m_iValue = value;}

	virtual int	GetGroupID() { return m_iGroupID; }
	virtual void SetGroupID(int ID) { m_iGroupID = ID; }

	//���������� ����� � �������� �������
	virtual int GetSignWidht();

	// �������������� �������� ������������� ������
	virtual bool IsHighlightText();
	virtual void SetHighlightText(bool Highlight)		{ m_bHighlightText = Highlight; }

	// �������������� ����������
	void	ManualDelete(bool value) { m_bManualDelete = value; }
	bool	IsManualDelete() { return m_bManualDelete; }

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

	// ������� item ��� ������?
	bool m_bManualDelete;
};

#endif