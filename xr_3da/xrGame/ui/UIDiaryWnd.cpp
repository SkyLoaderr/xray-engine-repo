// CUIDiaryWnd.cpp:  дневник и все что с ним связано
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

const char * const DIARY_XML = "events_new.xml";

// ID for tree view items
enum EDiaryIDs
{
	idJobsCurrent = 0,
	idJobsAccomplished,
	idJobsFailed,

	idContractsCurrent,
	idContractsAccomplished,
	idContractsFailed,

	idNews
};

//-----------------------------------------------------------------------------/
//	Default constructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::CUIDiaryWnd()
	:	m_pActiveSubdialog	(NULL),
		m_pTreeItemFont		(NULL),
		m_pTreeRootFont		(NULL),
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

	// Поддиалоги
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
		// для начала проверим, что нажатый элемент не рутовый
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

			case idContractsCurrent:
			case idContractsAccomplished:
			case idContractsFailed:
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

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Current");
	pTVItemSub->SetValue(idContractsCurrent);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Accomplished");
	pTVItemSub->SetValue(idContractsAccomplished);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Failed");
	pTVItemSub->SetValue(idContractsFailed);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetTextColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::ArticleCaption(LPCSTR caption)
{
	UIArticleCaption.SetText(caption);
}
