// CUIDiaryWnd.cpp:  ������� � ��� ��� � ��� �������
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIDiaryWnd.h"
#include "UIInteractiveListItem.h"
#include "UIPdaWnd.h"
#include "../UI.h"
#include "UITreeViewItem.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../InfoPortionDefs.h"
#include "../Actor.h"
#include "../character_info.h"
#include "../ai/trader/ai_trader.h"

#include "../xrserver.h"
//#include "../xrServer_Objects_ALife.h"
//#include "../xrServer_Objects_ALife_Items.h"
#include "../xrServer_Objects_ALife_Monsters.h"


const char * const DIARY_XML = "events_new.xml";

// ID for tree view items
enum EDiaryIDs
{
	idJobsCurrent,
	idJobsAccomplished,
	idJobsFailed,

	idContracts,

	idNews
};

//-----------------------------------------------------------------------------/
//	Default constructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::CUIDiaryWnd()
	:	m_pActiveSubdialog	(NULL),
		m_pTreeItemFont		(NULL),
		m_pTreeRootFont		(NULL),
		m_pContractsTreeItem(NULL),
		m_uTreeRootColor	(0xffffffff),
		m_uTreeItemColor	(0xffffffff)
{
	Hide();
}

//-----------------------------------------------------------------------------/
//  Destructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::~CUIDiaryWnd()
{

}

void CUIDiaryWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", DIARY_XML);
	R_ASSERT3(xml_result, "xml file not found", DIARY_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UITreeViewBg);
	xml_init.InitFrameWindow(uiXml, "right_frame_window", 0, &UITreeViewBg);

	UITreeViewBg.AttachChild(&UITreeViewHeader);
	xml_init.InitFrameLine(uiXml, "right_frame_line", 0, &UITreeViewHeader);

	xml_init.InitFont(uiXml, "tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
	R_ASSERT(m_pTreeItemFont);
	xml_init.InitFont(uiXml, "tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
	R_ASSERT(m_pTreeRootFont);

	UITreeViewHeader.AttachChild(&UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimation);

	AttachChild(&UIFrameWnd);
	xml_init.InitFrameWindow(uiXml, "left_frame_window", 0, &UIFrameWnd);

	UIFrameWnd.AttachChild(&UIFrameWndHeader);
	xml_init.InitFrameLine(uiXml, "left_frame_line", 0, &UIFrameWndHeader);

	UIFrameWnd.AttachChild(&UIArticleCaption);
	xml_init.InitStatic(uiXml, "article_header_static", 0, &UIArticleCaption);
	
	UITreeViewBg.AttachChild(&UITreeView);
	xml_init.InitListWnd(uiXml, "idx_list", 0, &UITreeView);
	UITreeView.SetMessageTarget(this);

	// ����������
	UIJobsWnd.Init();
	UINewsWnd.Init();
	UINotesWnd.Init();

	m_pActiveSubdialog = NULL;
	
	InitTreeView();

	// Autostatics
	xml_init.InitAutoStatic(uiXml, "left_auto_static", &UIFrameWnd);
	xml_init.InitAutoStatic(uiXml, "right_auto_static", &UITreeViewBg);
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (&UITreeView == pWnd && CUIListWnd::LIST_ITEM_CLICKED == msg)
	{
		// ��� ������ ��������, ��� ������� ������� �� �������
		CUITreeViewItem *pTVItem = static_cast<CUITreeViewItem*>(pData);
		R_ASSERT(pTVItem);

		if (!pTVItem->IsRoot())
		{
			std::string caption = static_cast<std::string>(ALL_PDA_HEADER_PREFIX) + pTVItem->GetHierarchyAsText();
			UIFrameWndHeader.UITitleText.SetText(caption.c_str());


			if (m_pActiveSubdialog)
			{
//				m_pActiveSubdialog->Hide();
				UIFrameWnd.DetachChild(m_pActiveSubdialog);
			}

			switch (static_cast<EDiaryIDs>(pTVItem->GetValue()))
			{

			case idJobsFailed:
				UIJobsWnd.SetFilter(eTaskStateFail);
				m_pActiveSubdialog = &UIJobsWnd;
				break;
			case idJobsAccomplished:
				UIJobsWnd.SetFilter(eTaskStateCompleted);
				m_pActiveSubdialog = &UIJobsWnd;
				break;
			case idJobsCurrent:
				UIJobsWnd.SetFilter(eTaskStateInProgress);
				m_pActiveSubdialog = &UIJobsWnd;
				break;

			case idContracts:
				m_pActiveSubdialog = NULL;
				break;

			case idNews:
				m_pActiveSubdialog = &UINewsWnd;
				break;

			default:
				R_ASSERT(!"Unknown type of diary id");
				return;
			}

			if (m_pActiveSubdialog)
			{
				UIFrameWnd.AttachChild(m_pActiveSubdialog);
				m_pActiveSubdialog->Show();
				ArticleCaption(*(m_pActiveSubdialog->DialogName()));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::AddNewsItem(const char *sData)
{
	CUIString			str;
	str.SetText(sData);

	UINewsWnd.UIListWnd.AddParsedItem<CUIListItem>(str, 4, 0xffffffff);

	UINewsWnd.UIListWnd.AddItem<CUIListItem>("");
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::InitTreeView()
{
	CUITreeViewItem *pTVItem = NULL, *pTVItemSub = NULL;

	// News section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText("News & Events");
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetTextColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItemSub->SetText("News");
	pTVItemSub->SetValue(idNews);
	pTVItem->AddItem(pTVItemSub);

	// Jobs section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText("Jobs");
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetTextColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Current");
	pTVItemSub->SetValue(idJobsCurrent);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Accomplished");
	pTVItemSub->SetValue(idJobsAccomplished);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Failed");
	pTVItemSub->SetValue(idJobsFailed);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	// Contracts section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText("Contracts");
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetTextColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);
	m_pContractsTreeItem = pTVItem;

}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::Show()
{
	inherited::Show();
	InitDiary();
}

void CUIDiaryWnd::InitDiary()
{
	CActor* pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pContractsTreeItem->DeleteAllSubItems();

	if(pActor->contacts_registry.objects_ptr())
	{
		const TALK_CONTACT_VECTOR& contacts = *pActor->contacts_registry.objects_ptr();
		for(TALK_CONTACT_VECTOR::const_iterator it = contacts.begin();
			contacts.end() != it; it++)
		{
			CSE_Abstract* E = Level().Server->game->get_entity_from_eid((*it).id);
			CSE_ALifeTrader* pTrader = NULL;
			if(E) pTrader = dynamic_cast<CSE_ALifeTrader*>(E);

			if(pTrader)
			{
				CCharacterInfo character_info;
				CSE_ALifeObject *O = dynamic_cast<CSE_ALifeObject*>(E);
				bool init_default_profile =true;
				if(*O->m_sCharacterProfileID)
					init_default_profile = !character_info.Load(*O->m_sCharacterProfileID);

				CUITreeViewItem* pTVItemSub = xr_new<CUITreeViewItem>();
				pTVItemSub->SetText(init_default_profile?E->name():character_info.Name());
				m_TraderID = (*it).id;
				pTVItemSub->SetValue(idContracts);
				pTVItemSub->SetFont(m_pTreeItemFont);
				pTVItemSub->SetTextColor(m_uTreeItemColor);
				m_pContractsTreeItem->AddItem(pTVItemSub);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::ArticleCaption(LPCSTR caption)
{
	UIArticleCaption.SetText(caption);
}
