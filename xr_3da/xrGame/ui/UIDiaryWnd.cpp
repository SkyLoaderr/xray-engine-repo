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
#include "../xrServer_Objects_ALife_Monsters.h"

#include "../script_space.h"
#include "../script_engine.h"

#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

const char * const	DIARY_XML			= "events_new.xml";
const int			contractsOffset		= 60;

//////////////////////////////////////////////////////////////////////////

// ID for tree view items
enum EDiaryIDs
{
	idJobsCurrent,
	idJobsAccomplished,
	idJobsFailed,
	idContracts,
	idNews,
	idActorDiary,
	idMax
};

//-----------------------------------------------------------------------------/
//	Default constructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::CUIDiaryWnd()
	:	m_pActiveSubdialog		(NULL),
		m_pTreeItemFont			(NULL),
		m_pTreeRootFont			(NULL),
		m_pContractsTreeItem	(NULL),
		m_uTreeRootColor		(0xffffffff),
		m_uTreeItemColor		(0xffffffff),
		m_pLeftHorisontalLine	(NULL),
		m_pActorDiaryRoot		(NULL)
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
	UIFrameWndHeader.UITitleText.SetElipsis(CUIStatic::eepBegin, 20);

	UIFrameWnd.AttachChild(&UIArticleCaption);
	xml_init.InitStatic(uiXml, "article_header_static", 0, &UIArticleCaption);
	
	UITreeViewBg.AttachChild(&UITreeView);
	xml_init.InitListWnd(uiXml, "idx_list", 0, &UITreeView);
	UITreeView.SetMessageTarget(this);
	UITreeView.EnableScrollBar(true);

	// ����������
	UIJobsWnd.Init();
	UINewsWnd.Init();
	UIContractsWnd.Init();
	UIActorDiaryWnd.Init(&UITreeView);
	UIActorDiaryWnd.m_pCore->m_pTreeRootFont	= m_pTreeRootFont;
	UIActorDiaryWnd.m_pCore->m_pTreeItemFont	= m_pTreeItemFont;
	UIActorDiaryWnd.m_pCore->m_uTreeRootColor	= m_uTreeRootColor;
	UIActorDiaryWnd.m_pCore->m_uTreeItemColor	= m_uTreeItemColor;

	m_pActiveSubdialog = NULL;
	
	InitTreeView();

	// Autostatics
	m_pLeftHorisontalLine = xml_init.InitAutoStatic(uiXml, "left_auto_static", &UIFrameWnd)[2];
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

			if (m_pActiveSubdialog == &UIContractsWnd)
			{
				VERIFY(m_pLeftHorisontalLine);
				RECT r = m_pLeftHorisontalLine->GetWndRect();
				m_pLeftHorisontalLine->MoveWindow(r.left, r.top - contractsOffset);
			}

			RECT r;

			EDiaryIDs id;
			if (m_pActorDiaryRoot->Find(pTVItem))
			{
				id = idActorDiary;
			}
			else
			{
				id = static_cast<EDiaryIDs>(pTVItem->GetValue());
			}

			switch (id)
			{

			case idJobsFailed:
				UIJobsWnd.SetFilter(eTaskStateFail);
				m_pActiveSubdialog = &UIJobsWnd;
				ArticleCaption(*(m_pActiveSubdialog->DialogName()));
				break;
			case idJobsAccomplished:
				UIJobsWnd.SetFilter(eTaskStateCompleted);
				m_pActiveSubdialog = &UIJobsWnd;
				ArticleCaption(*(m_pActiveSubdialog->DialogName()));
				break;
			case idJobsCurrent:
				UIJobsWnd.SetFilter(eTaskStateInProgress);
				m_pActiveSubdialog = &UIJobsWnd;
				ArticleCaption(*(m_pActiveSubdialog->DialogName()));
				break;

			case idContracts:
				VERIFY(m_pLeftHorisontalLine);
				r = m_pLeftHorisontalLine->GetWndRect();
				m_pLeftHorisontalLine->MoveWindow(r.left, r.top + contractsOffset);
				m_pActiveSubdialog = &UIContractsWnd;
				SetContractTrader();
				ArticleCaption(*(m_pActiveSubdialog->DialogName()));
				break;

			case idNews:
				m_pActiveSubdialog = &UINewsWnd;
				ArticleCaption(*(m_pActiveSubdialog->DialogName()));
				break;

			case idActorDiary:
				m_pActiveSubdialog = &UIActorDiaryWnd;
				caption = static_cast<std::string>(ALL_PDA_HEADER_PREFIX) +
						  m_pActorDiaryRoot->GetHierarchyAsText() +
						  *UIActorDiaryWnd.m_pCore->SetCurrentArtice(pTVItem);
				UIFrameWndHeader.UITitleText.SetText(caption.c_str());
				caption.erase(0, caption.find_last_of("/") + 1);
				ArticleCaption(caption.c_str());
				break;
			default:
				NODEFAULT;
			}

			if (m_pActiveSubdialog)
			{
				UIFrameWnd.AttachChild(m_pActiveSubdialog);
				m_pActiveSubdialog->Show();
			}
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::SetContractTrader()
{
	CSE_Abstract* E = Level().Server->game->get_entity_from_eid(m_TraderID);
	CSE_ALifeTrader* pTrader = NULL;
	if(E) pTrader = dynamic_cast<CSE_ALifeTrader*>(E);

	if(pTrader)
	{
		CCharacterInfo character_info;
		if(NO_PROFILE != pTrader->m_iCharacterProfile)
		{				
			character_info.Load(pTrader->m_iCharacterProfile);
			UIContractsWnd.UICharInfo.InitCharacter(&character_info);

			LPCSTR artefact_list_func = pSettings->r_string("artefacts_tasks", "script_func");
			luabind::functor<LPCSTR> lua_function;
			bool functor_exists = ai().script_engine().functor(artefact_list_func ,lua_function);
			R_ASSERT3(functor_exists, "Cannot find artefacts tasks func", artefact_list_func);
			LPCSTR artefact_list = lua_function	(NULL, m_TraderID);
			if(artefact_list)
			{
				UIContractsWnd.UIListWnd.AddParsedItem<CUIListItem>(artefact_list, 0, UIContractsWnd.UIListWnd.GetTextColor());
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
	CStringTable stbl;

	// News section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("News & Events"));
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetTextColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItemSub->SetText(*stbl("News"));
	pTVItemSub->SetValue(idNews);
	pTVItem->AddItem(pTVItemSub);

	// Jobs section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("Jobs"));
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetTextColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText(*stbl("Current"));
	pTVItemSub->SetValue(idJobsCurrent);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText(*stbl("Accomplished"));
	pTVItemSub->SetValue(idJobsAccomplished);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText(*stbl("Failed"));
	pTVItemSub->SetValue(idJobsFailed);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	// Contracts section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("Contracts"));
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetTextColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);
	m_pContractsTreeItem = pTVItem;

	// Actor diary section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("Diary"));
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetTextColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);
	m_pActorDiaryRoot = pTVItem;
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::Show()
{
	inherited::Show();
	InitDiary();
	if (m_pActiveSubdialog) m_pActiveSubdialog->Show();
}

void CUIDiaryWnd::InitDiary()
{
	CActor* pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	m_pContractsTreeItem->Close();
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
				bool init_default_profile =true;
				
				if(NO_PROFILE != pTrader->m_iCharacterProfile)
				{				
					character_info.Load(pTrader->m_iCharacterProfile);
					init_default_profile = false;
				}

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

	// ��� ����������� ������ � ������� ������
	UIActorDiaryWnd.DeleteArticles(m_pActorDiaryRoot);

	if(pActor && pActor->encyclopedia_registry.objects_ptr())
	{
		for(ARTICLE_VECTOR::const_iterator it = pActor->encyclopedia_registry.objects_ptr()->begin();
			it != pActor->encyclopedia_registry.objects_ptr()->end(); it++)
		{
			if (ARTICLE_DATA::eDiaryArticle == it->article_type)
			{
				UIActorDiaryWnd.AddArticle((*it).index);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::ArticleCaption(LPCSTR caption)
{
	UIArticleCaption.SetText(caption);
}