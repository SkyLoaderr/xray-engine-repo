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

const char * const DIARY_XML = "events.xml";

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
	: m_pActiveSubdialog(NULL)
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
	uiXml.Init("$game_data$", DIARY_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UITreeViewBg);
	xml_init.InitFrameWindow(uiXml, "eventslist_frame_window", 0, &UITreeViewBg);

	UITreeViewBg.AttachChild(&UITreeViewHeader);
	xml_init.InitFrameLine(uiXml, "eventslist_frame_line", 0, &UITreeViewHeader);

	UITreeViewHeader.AttachChild(&UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimation);

	AttachChild(&UIFrameWnd);
	xml_init.InitFrameWindow(uiXml, "eventsinfo_frame_window", 0, &UIFrameWnd);

	UIFrameWnd.AttachChild(&UIFrameWndHeader);
	xml_init.InitFrameLine(uiXml, "eventsinfo_frame_line", 0, &UIFrameWndHeader);
	
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
	xml_init.InitAutoStatic(uiXml, "ei_auto_static", &UIFrameWnd);
	xml_init.InitAutoStatic(uiXml, "el_auto_static", &UITreeViewBg);
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
			case idJobsAccomplished:
			case idJobsCurrent:
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
	UITreeView.AddItem(pTVItem);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("News");
	pTVItemSub->SetValue(idNews);
	pTVItem->AddItem(pTVItemSub);

	// Jobs section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText("Jobs");
	pTVItem->SetRoot(true);
	UITreeView.AddItem(pTVItem);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Current");
	pTVItemSub->SetValue(idJobsCurrent);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Accomplished");
	pTVItemSub->SetValue(idJobsAccomplished);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Failed");
	pTVItemSub->SetValue(idJobsFailed);
	pTVItem->AddItem(pTVItemSub);

	// Contracts section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText("Contracts");
	pTVItem->SetRoot(true);
	UITreeView.AddItem(pTVItem);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Current");
	pTVItemSub->SetValue(idContractsCurrent);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Accomplished");
	pTVItemSub->SetValue(idContractsAccomplished);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText("Failed");
	pTVItemSub->SetValue(idContractsFailed);
	pTVItem->AddItem(pTVItemSub);
}