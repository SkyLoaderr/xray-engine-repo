#include "stdafx.h"
#include "UIMessageBox.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"

CUIMessageBox::CUIMessageBox()
{
	m_UIButtonYesOk		= NULL;
	m_UIButtonNo		= NULL;
	m_UIButtonCancel	= NULL;
	m_UIStaticPicture	= NULL;
	m_UIStaticText		= NULL;
}

CUIMessageBox::~CUIMessageBox()
{}

#define BUTTON_UP_OFFSET 75
#define BUTTON_WIDTH 140

bool CUIMessageBox::OnMouse(float x, float y, EUIMessages mouse_action)
{
	return inherited::OnMouse(x, y, mouse_action);
}

void CUIMessageBox::Init	(LPCSTR box_template)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init	(CONFIG_PATH, UI_PATH, "message_box.xml");
	R_ASSERT3						(xml_result, "xml file not found", "message_box.xml");

	CUIXmlInit xml_init;

	string512 str;
	strcpy		(str,box_template);

	xml_init.InitStatic						(uiXml, str, 0, this);

	LPCSTR _type							= uiXml.ReadAttrib(str,0,"type",NULL);
	R_ASSERT								(_type);
	
	m_eMessageBoxStyle	= MESSAGEBOX_OK;
	if(0==stricmp(_type,"ok")){
		m_eMessageBoxStyle	= MESSAGEBOX_OK;
	}else
	if(0==stricmp(_type,"yes_no")){
		m_eMessageBoxStyle	= MESSAGEBOX_YES_NO;
	}else
	if(0==stricmp(_type,"yes_no_cancel")){
		m_eMessageBoxStyle	= MESSAGEBOX_YES_NO_CANCEL;
	};

	strconcat								(str,box_template,":picture");
	m_UIStaticPicture						= xr_new<CUIStatic>();m_UIStaticPicture->SetAutoDelete(true);AttachChild(m_UIStaticPicture);
	xml_init.InitStatic						(uiXml, str, 0, m_UIStaticPicture);

	strconcat								(str,box_template,":message_text");
	m_UIStaticText							= xr_new<CUIStatic>();m_UIStaticText->SetAutoDelete(true);AttachChild(m_UIStaticText);
	xml_init.InitStatic						(uiXml, str, 0, m_UIStaticText);

	switch (m_eMessageBoxStyle){

		case MESSAGEBOX_OK:{
			strconcat							(str,box_template,":button_ok");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();m_UIButtonYesOk->SetAutoDelete(true);AttachChild(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);
		}break;

		case MESSAGEBOX_YES_NO:{
			strconcat							(str,box_template,":button_yes");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();m_UIButtonYesOk->SetAutoDelete(true);AttachChild(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);

			strconcat							(str,box_template,":button_no");
			m_UIButtonNo						= xr_new<CUI3tButton>();m_UIButtonNo->SetAutoDelete(true);AttachChild(m_UIButtonNo);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonNo);
		}break;

		case MESSAGEBOX_YES_NO_CANCEL:{
			strconcat							(str,box_template,":button_yes");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();m_UIButtonYesOk->SetAutoDelete(true);AttachChild(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);

			strconcat							(str,box_template,":button_no");
			m_UIButtonNo						= xr_new<CUI3tButton>();m_UIButtonNo->SetAutoDelete(true);AttachChild(m_UIButtonNo);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonNo);

			strconcat							(str,box_template,":button_cancel");
			m_UIButtonCancel					= xr_new<CUI3tButton>();m_UIButtonCancel->SetAutoDelete(true);AttachChild(m_UIButtonCancel);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonCancel);
		}break;
	};
}



void CUIMessageBox::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(msg == BUTTON_CLICKED){
		switch(m_eMessageBoxStyle)
		{
		case MESSAGEBOX_OK:
			if(pWnd == m_UIButtonYesOk)
					GetMessageTarget()->SendMessage(this, MESSAGE_BOX_OK_CLICKED);
			break;
		case MESSAGEBOX_YES_NO:
			if(pWnd == m_UIButtonYesOk)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_YES_CLICKED);
			}
			else if(pWnd == m_UIButtonNo)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
			}
			break;
		case MESSAGEBOX_YES_NO_CANCEL:
			if(pWnd == m_UIButtonYesOk)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_YES_CLICKED);
			}
			else if(pWnd == m_UIButtonNo)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
			}
			else if(pWnd == m_UIButtonCancel)
			{
				GetMessageTarget()->SendMessage(this, MESSAGE_BOX_CANCEL_CLICKED);
			}
			break;
		};
	};
	inherited::SendMessage(pWnd, msg, pData);
}
/*
void CUIMessageBox::SetStyle(E_MESSAGEBOX_STYLE messageBoxStyle)
{
	m_eMessageBoxStyle = messageBoxStyle;

	float width = GetWidth();

	switch(m_eMessageBoxStyle)
	{
	case MESSAGEBOX_OK:
		m_UIButtonYesOk.Show(true);
		m_UIButtonYesOk.Enable(true);
		m_UIButtonYesOk.SetText("OK");

		m_UIButtonYesOk.SetWndRect(width/2 - BUTTON_WIDTH/2, 
						 m_UIButtonYesOk.GetWndRect().top,
						 float(BUTTON_WIDTH),50.0f);

		m_UIButtonNo.Show(false);
		m_UIButtonNo.Enable(false);
		m_UIButtonCancel.Show(false);
		m_UIButtonCancel.Enable(false);

		break;
	case MESSAGEBOX_YES_NO:
		m_UIButtonYesOk.Show(true);
		m_UIButtonYesOk.Enable(true);
		m_UIButtonYesOk.SetText("Yes");
		
		m_UIButtonYesOk.SetWndRect(width/4 - BUTTON_WIDTH/2, 
							 m_UIButtonYesOk.GetWndRect().top,
							 BUTTON_WIDTH,50);
				

		m_UIButtonNo.Show(true);
		m_UIButtonNo.Enable(true);
		m_UIButtonNo.SetText("No");


		m_UIButtonNo.SetWndRect(width/2 + BUTTON_WIDTH/2, 
							 m_UIButtonNo.GetWndRect().top,
							 float(BUTTON_WIDTH),50.0f);

		m_UIButtonCancel.Show(false);
		m_UIButtonCancel.Enable(false);
		break;
	case MESSAGEBOX_YES_NO_CANCEL:
		m_UIButtonYesOk.Show(true);
		m_UIButtonYesOk.Enable(true);
		m_UIButtonYesOk.SetText("Yes");

		m_UIButtonNo.Show(true);
		m_UIButtonNo.Enable(true);
		m_UIButtonNo.SetText("No");

		m_UIButtonCancel.Show(true);
		m_UIButtonCancel.Enable(true);
		m_UIButtonCancel.SetText("Cancel");
		break;
	}
}*/

void CUIMessageBox::SetText(LPCSTR str)
{
	m_UIStaticText->SetText(str);
}

