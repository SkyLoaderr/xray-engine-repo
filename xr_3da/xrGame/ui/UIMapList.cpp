
#include "StdAfx.h"
#include "UIMapList.h"
#include "UIListWnd.h"
#include "UILabel.h"
#include "UIFrameWindow.h"
#include "UI3tButton.h"

CUIMapList::CUIMapList(){
	m_pList1 = xr_new<CUIListWnd>();
	m_pList2 = xr_new<CUIListWnd>();
	m_pFrame1 = xr_new<CUIFrameWindow>();
	m_pFrame2 = xr_new<CUIFrameWindow>();
	m_pLbl1 = xr_new<CUILabel>();
	m_pLbl2 = xr_new<CUILabel>();
	m_pBtnLeft = xr_new<CUI3tButton>();
	m_pBtnRight = xr_new<CUI3tButton>();
	m_pBtnUp = xr_new<CUI3tButton>();
	m_pBtnDown = xr_new<CUI3tButton>();

	m_pList1->SetAutoDelete(true);
	m_pList2->SetAutoDelete(true);
	m_pFrame1->SetAutoDelete(true);
	m_pFrame2->SetAutoDelete(true);
	m_pLbl1->SetAutoDelete(true);
	m_pLbl2->SetAutoDelete(true);
	m_pBtnLeft->SetAutoDelete(true);
	m_pBtnRight->SetAutoDelete(true);
	m_pBtnUp->SetAutoDelete(true);
	m_pBtnDown->SetAutoDelete(true);

	AttachChild(m_pLbl1);
	AttachChild(m_pLbl2);
	AttachChild(m_pFrame1);
	AttachChild(m_pFrame2);
	AttachChild(m_pList1);
	AttachChild(m_pList2);
	AttachChild(m_pBtnLeft);
	AttachChild(m_pBtnRight);
	AttachChild(m_pBtnUp);
	AttachChild(m_pBtnDown);
}

void CUIMapList::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);	
}

void CUIMapList::InitFromXml(CUIXml& xml_doc, const char* path){
	CUIXmlInit::InitWindow(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf, path, ":header_1"),	0, m_pLbl1);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf, path, ":header_2"),	0, m_pLbl2);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf, path, ":frame_1"),		0, m_pFrame1);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf, path, ":frame_2"),		0, m_pFrame2);
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf, path, ":list_1"),		0, m_pList1);
	m_pList1->EnableScrollBar(true);
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf, path, ":list_2"),		0, m_pList2);
	m_pList2->EnableScrollBar(true);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_left"),	0, m_pBtnLeft);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_right"),	0, m_pBtnRight);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_up"),		0, m_pBtnUp);
	CUIXmlInit::Init3tButton	(xml_doc, strconcat(buf, path, ":btn_down"),	0, m_pBtnDown);
}