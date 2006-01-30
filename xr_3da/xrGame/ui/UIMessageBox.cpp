#include "stdafx.h"
#include "UIMessageBox.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "UIEditBox.h"
#include "../string_table.h"

CUIMessageBox::CUIMessageBox()
{
	m_UIButtonYesOk		= NULL;
	m_UIButtonNo		= NULL;
	m_UIButtonCancel	= NULL;
	m_UIStaticPicture	= NULL;
	m_UIStaticText		= NULL;

	m_UIEditPass		= NULL;
	m_UIEditHost		= NULL;
	m_UIStaticPass		= NULL;
	m_UIStaticHost		= NULL;
}

CUIMessageBox::~CUIMessageBox()
{
	Clear();
}

#define BUTTON_UP_OFFSET 75
#define BUTTON_WIDTH 140

void CUIMessageBox::Clear(){
	xr_delete(m_UIButtonYesOk);
	xr_delete(m_UIButtonNo);
	xr_delete(m_UIButtonCancel);
	xr_delete(m_UIStaticPicture);
	xr_delete(m_UIStaticText);
	xr_delete(m_UIEditPass);
	xr_delete(m_UIEditHost);
	xr_delete(m_UIStaticPass);
	xr_delete(m_UIStaticHost);
}

bool CUIMessageBox::OnMouse(float x, float y, EUIMessages mouse_action)
{
	return inherited::OnMouse(x, y, mouse_action);
}

void CUIMessageBox::Init	(LPCSTR box_template)
{
	Clear();
	CUIXml uiXml;
	bool xml_result = uiXml.Init	(CONFIG_PATH, UI_PATH, "message_box.xml");
	R_ASSERT3						(xml_result, "xml file not found", "message_box.xml");

	CUIXmlInit xml_init;

	string512 str;

	strconcat								(str,box_template,":picture");
	m_UIStaticPicture						= xr_new<CUIStatic>();AttachChild(m_UIStaticPicture);
	xml_init.InitStatic						(uiXml, str, 0, m_UIStaticPicture);

	strconcat								(str,box_template,":message_text");
	if (uiXml.NavigateToNode(str,0)){
        m_UIStaticText							= xr_new<CUIStatic>();AttachChild(m_UIStaticText);
        xml_init.InitStatic						(uiXml, str, 0, m_UIStaticText);
	}

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
	}else
	if(0==stricmp(_type,"direct_ip")){
		m_eMessageBoxStyle	= MESSAGEBOX_DIRECT_IP;
	}else
	if(0==stricmp(_type,"password")){
		m_eMessageBoxStyle	= MESSAGEBOX_PASSWORD;
	};
	

	switch (m_eMessageBoxStyle){

		case MESSAGEBOX_OK:{
			strconcat							(str,box_template,":button_ok");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);
		}break;

		case MESSAGEBOX_DIRECT_IP:
			strconcat							(str,box_template,":cap_host");
			m_UIStaticHost						= xr_new<CUIStatic>();
			AttachChild							(m_UIStaticHost);
			xml_init.InitStatic					(uiXml, str, 0, m_UIStaticHost);

			strconcat							(str,box_template,":edit_host");
			m_UIEditHost						= xr_new<CUIEditBox>();
			AttachChild							(m_UIEditHost);
			xml_init.InitEditBox				(uiXml, str, 0, m_UIEditHost);			

		case MESSAGEBOX_PASSWORD:
			strconcat							(str,box_template,":cap_password");
			m_UIStaticPass						= xr_new<CUIStatic>();
			AttachChild							(m_UIStaticPass);
			xml_init.InitStatic					(uiXml, str, 0, m_UIStaticPass);

			strconcat							(str,box_template,":edit_password");
			m_UIEditPass						= xr_new<CUIEditBox>();
			AttachChild							(m_UIEditPass);
			xml_init.InitEditBox				(uiXml, str, 0, m_UIEditPass);

		case MESSAGEBOX_YES_NO:{
			strconcat							(str,box_template,":button_yes");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);

			strconcat							(str,box_template,":button_no");
			m_UIButtonNo						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonNo);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonNo);
		}break;

		case MESSAGEBOX_YES_NO_CANCEL:{
			strconcat							(str,box_template,":button_yes");
			m_UIButtonYesOk						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonYesOk);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonYesOk);

			strconcat							(str,box_template,":button_no");
			m_UIButtonNo						= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonNo);
			xml_init.Init3tButton				(uiXml, str, 0, m_UIButtonNo);

			strconcat							(str,box_template,":button_cancel");
			m_UIButtonCancel					= xr_new<CUI3tButton>();
			AttachChild							(m_UIButtonCancel);
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
		case MESSAGEBOX_DIRECT_IP:
		case MESSAGEBOX_PASSWORD:
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

void CUIMessageBox::SetText(LPCSTR str)
{
	m_UIStaticText->SetText(*(CStringTable().translate(str)));
}

LPCSTR CUIMessageBox::GetHost(){
	if (m_UIEditHost){
		xr_string tmp= m_UIEditHost->GetText();
		xr_string::size_type pos = tmp.find(":");

		if (xr_string::npos != pos)
		{
			m_ret_val.assign(tmp.begin(), tmp.begin()+pos);
			tmp.erase(tmp.begin(), tmp.begin()+pos + 1);

			m_ret_val += "/port=";
			m_ret_val += tmp;
		}

		return m_ret_val.c_str();
	}
	else return NULL;
}

LPCSTR CUIMessageBox::GetPassword(){
	if (m_UIEditPass)
		return m_UIEditPass->GetText();
	else return NULL;
}



