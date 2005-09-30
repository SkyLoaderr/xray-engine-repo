////////////////////////////////////////////////
// UITalkDialogWnd.cpp
// ���� ������� �� ��������� � ������ ���������
////////////////////////////////////////////////

#include "stdafx.h"
#include "UITalkDialogWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIScrollView.h"
#include "UI3tButton.h"
#include "../UI.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

//////////////////////////////////////////////////////////////////////////

const char * const TALK_XML					= "talk.xml";
const char * const TRADE_CHARACTER_XML		= "trade_character.xml";

//////////////////////////////////////////////////////////////////////////

CUITalkDialogWnd::CUITalkDialogWnd()
	:	m_pNameTextFont		(NULL)
{
	m_iClickedQuestion = -1;
}
CUITalkDialogWnd::~CUITalkDialogWnd()
{
	xr_delete(m_uiXml);
}

void CUITalkDialogWnd::Init(float x, float y, float width, float height)
{
	m_uiXml = xr_new<CUIXml>();
	bool xml_result = m_uiXml->Init(CONFIG_PATH, UI_PATH, TALK_XML);
	R_ASSERT3(xml_result, "xml file not found", TALK_XML);
	CUIXmlInit xml_init;

	inherited::Init(x, y, width, height);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,UI_BASE_HEIGHT-32,UI_BASE_HEIGHT,32);

	//������ � ����������� ��� � �������� �� ��������
	AttachChild(&UIOurIcon);
	xml_init.InitStatic(*m_uiXml, "left_character_icon", 0, &UIOurIcon);
	AttachChild(&UIOthersIcon);
	xml_init.InitStatic(*m_uiXml, "right_character_icon", 0, &UIOthersIcon);
	UIOurIcon.AttachChild(&UICharacterInfoLeft);
	UICharacterInfoLeft.Init(0.0f, 0.0f, UIOurIcon.GetWidth(), UIOurIcon.GetHeight(), TRADE_CHARACTER_XML);
	UIOthersIcon.AttachChild(&UICharacterInfoRight);
	UICharacterInfoRight.Init(0.0f, 0.0f, UIOthersIcon.GetWidth(), UIOthersIcon.GetHeight(), TRADE_CHARACTER_XML);

	//�������� ����� �������
	AttachChild(&UIDialogFrame);
	xml_init.InitFrameWindow(*m_uiXml, "frame_window", 0, &UIDialogFrame);
	UIDialogFrame.UITitleText.SetElipsis(CUIStatic::eepEnd, 10);
	// ����� � ������ �������
	AttachChild(&UIOurPhrasesFrame);
	xml_init.InitFrameWindow(*m_uiXml, "frame_window", 1, &UIOurPhrasesFrame);
	UIOurPhrasesFrame.UITitleText.SetElipsis(CUIStatic::eepEnd, 10);

	//������
	UIAnswersList = xr_new<CUIScrollView>();UIAnswersList->SetAutoDelete(true);
	UIDialogFrame.AttachChild(UIAnswersList);
	xml_init.InitScrollView(*m_uiXml, "answers_list", 0, UIAnswersList);
	UIAnswersList->SetWindowName("---UIAnswersList");

	//�������
	UIQuestionsList = xr_new<CUIScrollView>();UIQuestionsList->SetAutoDelete(true);
	UIOurPhrasesFrame.AttachChild(UIQuestionsList);
	xml_init.InitScrollView(*m_uiXml, "questions_list", 0, UIQuestionsList);
	UIQuestionsList->SetWindowName("---UIQuestionsList");


	//������ �������� � ����� ��������
	AttachChild(&UIToTradeButton);
	xml_init.InitButton(*m_uiXml, "button", 0, &UIToTradeButton);
	UIToTradeButton.SetWindowName("trade_btn");

	//�������� ��������������� ����������
	xml_init.InitAutoStatic(*m_uiXml, "auto_static", this);

	// ����� ��� ��������� ����� ��������� � ���� ���������
	xml_init.InitFont(*m_uiXml, "font", 0, m_iNameTextColor, m_pNameTextFont);

	CGameFont * pFont = NULL;
	xml_init.InitFont(*m_uiXml, "font", 1, m_uOurReplicsColor, pFont);


	SetWindowName("----CUITalkDialogWnd");

	Register						(&UIToTradeButton);
	AddCallback						("question_item",LIST_ITEM_CLICKED,boost::bind(&CUITalkDialogWnd::OnQuestionClicked,this,_1,_2));
	AddCallback						("trade_btn",BUTTON_CLICKED,boost::bind(&CUITalkDialogWnd::OnTradeClicked,this));
}


void CUITalkDialogWnd::Show()
{
	inherited::Show(true);
	inherited::Enable(true);

	ResetAll();
}

void CUITalkDialogWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}

void CUITalkDialogWnd::OnQuestionClicked(CUIWindow* w, void*)
{
		m_iClickedQuestion = ((CUIQuestionItem*)w)->m_value;
		GetMessageTarget()->SendMessage(this, TALK_DIALOG_QUESTION_CLICKED);
}

void CUITalkDialogWnd::OnTradeClicked()
{
		GetTop()->SendMessage(this, TALK_DIALOG_TRADE_BUTTON_CLICKED);
}

//���������� ��������� ������������� ���� ��� ���������
//� ��������� ���� ��� ������ �� ������ ��������� ����
void CUITalkDialogWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUITalkDialogWnd::ClearAll()
{
	UIAnswersList->Clear	();
	ClearQuestions			();
}

void CUITalkDialogWnd::ClearQuestions()
{
	UIQuestionsList->Clear();
}


void CUITalkDialogWnd::AddQuestion(const char* str, int value)
{
	CUIQuestionItem* itm			= xr_new<CUIQuestionItem>(m_uiXml,"question_item");
	itm->Init						(value, str);
	UIQuestionsList->AddWindow		(itm);
	itm->SetWindowName				("question_item");
	Register						(itm);
}

void CUITalkDialogWnd::AddAnswer(const char* SpeakerName, const char* str, bool bActor)
{
	CUIAnswerItem* itm				= xr_new<CUIAnswerItem>(m_uiXml,bActor?"actor_answer_item":"other_answer_item");
	itm->Init						(str, SpeakerName);
	UIAnswersList->AddWindow		(itm);
	UIAnswersList->ScrollToEnd		();
}

void CUITalkDialogWnd::AddIconedAnswer(LPCSTR text, LPCSTR texture_name, Frect texture_rect)
{
	CUIAnswerItemIconed* itm				= xr_new<CUIAnswerItemIconed>(m_uiXml,"iconed_answer_item");
	itm->Init								(text, texture_name, texture_rect);
	UIAnswersList->AddWindow				(itm);
	UIAnswersList->ScrollToEnd				();

}


void CUIQuestionItem::SendMessage				(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

CUIQuestionItem::CUIQuestionItem			(CUIXml* xml_doc, LPCSTR path)
{
	m_text							= xr_new<CUI3tButton>();m_text->SetAutoDelete(true);
	AttachChild						(m_text);

	string512						str;
	CUIXmlInit						xml_init;

	strcpy							(str,path);
	xml_init.InitWindow				(*xml_doc, str, 0, this);

	m_min_height					= xml_doc->ReadAttribFlt(path,0,"min_height",15.0f);

	strconcat						(str,path,":content_text");
	xml_init.Init3tButton			(*xml_doc, str, 0, m_text);

	Register						(m_text);
	m_text->SetWindowName			("text_button");
	AddCallback						("text_button",BUTTON_CLICKED,boost::bind(&CUIQuestionItem::OnTextClicked,this));

}

void CUIQuestionItem::Init			(int val, LPCSTR text)
{
	m_value							= val;
	m_text->SetText					(text);
	m_text->AdjustHeightToText		();
	float new_h						= _max(m_min_height, m_text->GetWndPos().y+m_text->GetHeight());
	SetHeight						(new_h);
}

void	CUIQuestionItem::OnTextClicked()
{
	GetMessageTarget()->SendMessage(this, LIST_ITEM_CLICKED, (void*)this);
}


CUIAnswerItem::CUIAnswerItem			(CUIXml* xml_doc, LPCSTR path)
{
	m_text							= xr_new<CUIStatic>();m_text->SetAutoDelete(true);
	m_name							= xr_new<CUIStatic>();m_name->SetAutoDelete(true);
	AttachChild						(m_text);
	AttachChild						(m_name);

	string512						str;
	CUIXmlInit						xml_init;

	strcpy							(str,path);
	xml_init.InitWindow				(*xml_doc, str, 0, this);

	m_min_height					= xml_doc->ReadAttribFlt(path,0,"min_height",15.0f);
	m_bottom_footer					= xml_doc->ReadAttribFlt(path,0,"bottom_footer",0.0f);
	strconcat						(str,path,":content_text");
	xml_init.InitStatic				(*xml_doc, str, 0, m_text);

	strconcat						(str,path,":name_caption");
	xml_init.InitStatic				(*xml_doc, str, 0, m_name);
	SetAutoDelete					(true);
}

void CUIAnswerItem::Init			(LPCSTR text, LPCSTR name)
{
	m_name->SetText					(name);
	m_text->SetText					(text);
	m_text->AdjustHeightToText		();
	float new_h						= _max(m_min_height, m_text->GetWndPos().y+m_text->GetHeight());
	new_h							+= m_bottom_footer;
	SetHeight						(new_h);
}

CUIAnswerItemIconed::CUIAnswerItemIconed		(CUIXml* xml_doc, LPCSTR path)
:CUIAnswerItem(xml_doc, path)
{
	m_icon							= xr_new<CUIStatic>();m_icon->SetAutoDelete(true);
	AttachChild						(m_icon);

	string512						str;
	CUIXmlInit						xml_init;

	strconcat						(str,path,":msg_icon");
	xml_init.InitStatic				(*xml_doc, str, 0, m_icon);
}

void CUIAnswerItemIconed::Init		(LPCSTR text, LPCSTR texture_name, Frect texture_rect)
{
	inherited::Init					(text,"");
	m_icon->CreateShader			(texture_name,"hud\\default");
	m_icon->GetUIStaticItem().SetOriginalRect(texture_rect.x1,texture_rect.y1,texture_rect.x2,texture_rect.y2);
	m_icon->TextureAvailable		(true);
	m_icon->TextureOn				();
	m_icon->SetStretchTexture		(true);

}
