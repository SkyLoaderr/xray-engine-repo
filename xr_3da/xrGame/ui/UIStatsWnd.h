//-----------------------------------------------------------------------------/
//  ���� ������ ���������� �� ������� � ������� ����
//-----------------------------------------------------------------------------/

#ifndef UISTATSWND_H_
#define UISTATSWND_H_

#pragma once

#include "UIDialogWnd.h"
#include "UIFrameWindow.h"
#include "UIListWnd.h"
#include "UIListItem.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIButton.h"

DEF_VECTOR (FIELDS_VECTOR, CUIButton*)

// ����� ��� ����������� ������ ����� ������
class CUIStatsListItem: public CUIListItem
{
	typedef CUIListItem inherited;
public:
	virtual ~CUIStatsListItem() {};
	void XmlInit(const char *path, CUIXml &uiXml);
	void Highlight(bool bHighlight);
	void SetSubItemColor(u32 uItemIndex, u32 uColor);

	// ���� ������
	FIELDS_VECTOR FieldsVector;
};

class CUIStatsWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIStatsWnd();
	virtual ~CUIStatsWnd();

	virtual void Init();
//	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

	// �������� 1 �������. ��������� ���� ���������� ��������������. ���������� ��������� 
	// �� ����������� �������
	CUIStatsListItem * AddItem();
	// �������� �������, ��� �		����� ������ � ����� �� ������. ����� ������
	// ������� ������� � ��������� ������
	CUIStatsListItem * FindFrom(const u32 beg_pos, const char *strCaption);
	// ������� ������� � ������� ���� ������ � ������� strCaption. � ������ Item'� ����� 
	// ������ � ������� beg_pos
	void RemoveItemFrom(const u32 beg_pos, const char *strCaption);
	// ���������� ������ �������
	void HighlightItem(const u32 uItem);
	// �������� ����� ������������ ��-��
	u32	GetHighlightedItem() { return m_uHighlightedItem; }
	// �������� ������ �������
	void SelectItem(const u32 uItem);
	
	RECT GetFrameRect () { return UIFrameWnd.GetWndRect();};
	void RemoveItem (const u32 Index) {UIStatsList.RemoveItem(Index);};
protected:
//	CUIButton			UIBtn;
	// ����� - ��������
	CUIFrameWindow		UIFrameWnd;
	// ���� ��� ����������� ������ ���������� �������
	CUIListWnd			UIStatsList;
	// ����������� �������
	u32					m_uHighlightedItem;
};

#endif
