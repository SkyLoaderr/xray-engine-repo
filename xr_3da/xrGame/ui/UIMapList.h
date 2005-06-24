
#pragma once

#include "UIWindow.h"

class CUIListWnd;
class CUILabel;
class CUIFrameWindow;
class CUI3tButton;

class CUIXml;

class CUIMapList : public CUIWindow {
public:
	CUIMapList();
	virtual void	Init(float x, float y, float width, float height);
			void	InitFromXml(CUIXml& xml_doc, const char* path);

private:
	CUIListWnd*			m_pList1;
	CUIListWnd*			m_pList2;
	CUIFrameWindow*		m_pFrame1;
	CUIFrameWindow*		m_pFrame2;
	CUILabel*			m_pLbl1;
	CUILabel*			m_pLbl2;
	CUI3tButton*		m_pBtnLeft;
	CUI3tButton*		m_pBtnRight;
	CUI3tButton*		m_pBtnUp;
	CUI3tButton*		m_pBtnDown;
};