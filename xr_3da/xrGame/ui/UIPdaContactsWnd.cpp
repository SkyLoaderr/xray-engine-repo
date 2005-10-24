///////////////////////////////////////////////////////////
/// —писок контактов PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaContactsWnd.h"
#include "UIPdaListItem.h"
#include "UIPdaAux.h"
#include "../Pda.h"
//#include "../HUDManager.h"
#include "UIXmlInit.h"
#include "../actor.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UIScrollView.h"

#define PDA_CONTACT_HEIGHT 70

const char * const PDA_CONTACTS_XML					= "pda_contacts_new.xml";
const char * const PDA_CONTACTS_HEADER_SUFFIX		= "/Contacts";

CUIPdaContactsWnd::CUIPdaContactsWnd()
{
	m_flags.zero();
}

CUIPdaContactsWnd::~CUIPdaContactsWnd()
{
}

void CUIPdaContactsWnd::Show(bool status)
{
	inherited::Show(status);
}

void CUIPdaContactsWnd::Init()
{
	CUIXml		uiXml;
	bool xml_result =uiXml.Init(CONFIG_PATH, UI_PATH, PDA_CONTACTS_XML);
	R_ASSERT3(xml_result, "xml file not found", PDA_CONTACTS_XML);

	CUIXmlInit	xml_init;
	string128	buf;


	xml_init.InitWindow					(uiXml, "main_wnd", 0, this);

	UIFrameContacts						= xr_new<CUIFrameWindow>();UIFrameContacts->SetAutoDelete(true);
	AttachChild							(UIFrameContacts);
	xml_init.InitFrameWindow			(uiXml, "left_frame_window", 0, UIFrameContacts);


	UIContactsHeader					= xr_new<CUIFrameLineWnd>();UIContactsHeader->SetAutoDelete(true);
	UIFrameContacts->AttachChild		(UIContactsHeader);
	xml_init.InitFrameLine				(uiXml, "left_frame_line", 0, UIContactsHeader);
	strconcat							(buf, ALL_PDA_HEADER_PREFIX, PDA_CONTACTS_HEADER_SUFFIX);
	UIContactsHeader->UITitleText.SetText(buf);

	UIRightFrame						= xr_new<CUIFrameWindow>();UIRightFrame->SetAutoDelete(true);
	AttachChild							(UIRightFrame);
	xml_init.InitFrameWindow			(uiXml, "right_frame_window", 0, UIRightFrame);

	UIRightFrameHeader					= xr_new<CUIFrameLineWnd>();UIRightFrameHeader->SetAutoDelete(true);
	UIRightFrame->AttachChild			(UIRightFrameHeader);
	xml_init.InitFrameLine				(uiXml, "right_frame_line", 0, UIRightFrameHeader);

	UIArticleHeader						= xr_new<CUIStatic>();UIArticleHeader->SetAutoDelete(true);
	UIFrameContacts->AttachChild		(UIArticleHeader);
	xml_init.InitStatic					(uiXml, "article_header_static", 0, UIArticleHeader);
	UIArticleHeader->SetText			("Contacts");

	UIAnimation							= xr_new<CUIAnimatedStatic>();UIAnimation->SetAutoDelete(true);
	UIRightFrameHeader->AttachChild		(UIAnimation);
	xml_init.InitAnimatedStatic			(uiXml, "a_static", 0, UIAnimation);

	UIListWnd							= xr_new<CUIScrollView>();UIListWnd->SetAutoDelete(true);
	UIFrameContacts->AttachChild		(UIListWnd);
	xml_init.InitScrollView				(uiXml, "list", 0, UIListWnd);

	xml_init.InitAutoStatic				(uiXml, "left_auto_static", UIFrameContacts);
	xml_init.InitAutoStatic				(uiXml, "right_auto_static", UIRightFrame);
}


void CUIPdaContactsWnd::Update()
{
	inherited::Update();
	if(TRUE==m_flags.test(flNeedUpdate)){
		UIListWnd->Reset();

		UIListWnd->Clear();

		xr_vector<CPda*>	pda_list;
		CPda*	pPda		= Actor()->GetPDA	();
		if(!pPda)			return;

		pPda->ActiveContacts(pda_list);

		xr_vector<CPda*>::iterator it = pda_list.begin();

		for(; it!=pda_list.end();++it){
			AddContact(*it);
		}
		m_flags.set(flNeedUpdate, FALSE);
	}
}

void CUIPdaContactsWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	inherited::SendMessage(pWnd, msg, pData);
}

void CUIPdaContactsWnd::AddContact(CPda* pda)
{
	VERIFY(pda);


	CUIPdaListItem* pItem			= NULL;
	pItem							= xr_new<CUIPdaListItem>();
	pItem->SetAutoDelete			(true);
	UIListWnd->AddWindow			(pItem);
	pItem->Init						(0,0,UIListWnd->GetWidth(),75);
	pItem->InitCharacter			(pda->GetOriginalOwner());
	pItem->m_data					= (void*)pda;
}

void CUIPdaContactsWnd::RemoveContact(CPda* pda)
{
	u32 cnt = UIListWnd->GetSize();

	for(u32 i=0 ; i<cnt; ++i ){
		CUIWindow* w = UIListWnd->GetItem(i);
		CUIPdaListItem* itm = (CUIPdaListItem*)(w);
		if(itm->m_data==(void*)pda){
			UIListWnd->RemoveWindow(w);
			return;
		}
	}
}

//удалить все контакты из списка
void CUIPdaContactsWnd::RemoveAll()
{
	UIListWnd->Clear		();
}

void CUIPdaContactsWnd::Reload()
{
	m_flags.set(flNeedUpdate,TRUE);
}
