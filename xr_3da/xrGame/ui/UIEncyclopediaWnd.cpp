//=============================================================================
//  Filename:   UIEncyclopediaWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#include "StdAfx.h"
#include "UIEncyclopediaWnd.h"
#include "UIPdaWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../HUDManager.h"
#include "../level.h"

//////////////////////////////////////////////////////////////////////////

const char * const ENCYCLOPEDIA_DIALOG_XML	= "encyclopedia.xml";

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaWnd::CUIEncyclopediaWnd()
	:	m_pCurrArticle		(NULL)
{
}

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaWnd::~CUIEncyclopediaWnd()
{
	if (m_pCurrArticle)
		UIEncyclopediaInfoBkg.DetachChild(&m_pCurrArticle->image);
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::Init()
{
	CUIXml		uiXml;
	bool xml_result = uiXml.Init("$game_data$", ENCYCLOPEDIA_DIALOG_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit	xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	// Load xml data
	AttachChild(&UIEncyclopediaIdxBkg);
	xml_init.InitFrameWindow(uiXml, "elist_frame_window", 0, &UIEncyclopediaIdxBkg);

	UIEncyclopediaIdxBkg.AttachChild(&UIEncyclopediaIdxHeader);
	xml_init.InitFrameLine(uiXml, "elist_frame_line", 0, &UIEncyclopediaIdxHeader);

	UIEncyclopediaIdxHeader.AttachChild(&UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimation);

	AttachChild(&UIEncyclopediaInfoBkg);
	xml_init.InitFrameWindow(uiXml, "einfo_frame_window", 0, &UIEncyclopediaInfoBkg);

	UIEncyclopediaInfoBkg.AttachChild(&UIEncyclopediaInfoHeader);
	xml_init.InitFrameLine(uiXml, "einfo_frame_line", 0, &UIEncyclopediaInfoHeader);

	UIEncyclopediaIdxBkg.AttachChild(&UIIdxList);
	xml_init.InitListWnd(uiXml, "idx_list", 0, &UIIdxList);
	UIIdxList.SetMessageTarget(this);

	UIEncyclopediaInfoBkg.AttachChild(&UIInfoList);
	xml_init.InitListWnd(uiXml, "info_list", 0, &UIInfoList);
	UIInfoList.AddItem<CUIListItem>("Test of UIInfoList");
	UIInfoList.ActivateList(false);

	// mask
	xml_init.InitFrameWindow(uiXml, "item_static:mask_frame_window", 0, &UIImgMask);

	string256 header;
	strconcat(header, ALL_PDA_HEADER_PREFIX, "/Encyclopedia");
	m_InfosHeaderStr = header;

	AddArticle("one");
	AddArticle("two");
	AddArticle("three");
	AddArticle("four");
	AddArticle("five");
	AddArticle("six");
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::AddArticle(const ref_str &ID)
{
	static ref_str encyclopediaDB[] =
	{
		"encyclopedia_infos.xml"
	};
	
	const u32 encyclopediaDB_size = 1;

	CUIXml uiXml;
	XML_NODE *pNode = NULL;

	// ���� �� ���� xml'�� ������ ������
	for (u32 i = 0; i < encyclopediaDB_size; ++i)
	{
		bool xml_result = uiXml.Init("$game_data$", *encyclopediaDB[i]);
		R_ASSERT2(xml_result, "xml file not found");
		
		pNode = uiXml.SearchForAttribute(uiXml.GetRoot(), "article", "id", *ID);
		if (pNode) break;
	}

	// ���� ����� �� ��������� ��� � ������ ��������� ��������, � ��������� ����������
	if (!pNode) return;

	// ��������� �������
	m_ArticlesDB.resize(m_ArticlesDB.size() + 1);
	Article &a = m_ArticlesDB.back();

	// �������������� �����������
	CUIXml localXml;
	VERIFY2(localXml.Init("$game_data$", ENCYCLOPEDIA_DIALOG_XML), "xml file not found");
	CUIXmlInit xml_init;
	xml_init.InitStatic(localXml, "item_static", 0, &a.image);

	// ��������
	localXml.SetLocalRoot(pNode);
	xml_init.InitTexture(localXml, "", 0, &a.image);

	// � �����
	a.info = uiXml.Read(pNode, "");
	R_ASSERT(a.info != "");
	std::string group = uiXml.ReadAttrib(pNode, "group", "");
	R_ASSERT(!group.empty());

	// ������ ������ ������ ��� ����������� �����������
	GroupTree					groupTree;

	std::string::size_type		pos;
	std::string					oneLevel;
	
	while (true)
	{
		pos = group.find('/');
		if (pos != std::string::npos)
		{
			oneLevel.assign(group, 0, pos - 1);
			groupTree.push_back(oneLevel.c_str());
			group.erase(0, pos + 1);
		}
		else
		{
			groupTree.push_back(group.c_str());
			break;
		}
	}

	// ������ ���� ��� �� ������������� ����� ��� � �������
	CUITreeViewItem *pTVItem = NULL, *pTVItemChilds = NULL;
	bool status = false;

	// ��� ���� ������� ���������
	for (int i = 0; i < UIIdxList.GetSize(); ++i)
	{
		pTVItem = dynamic_cast<CUITreeViewItem*>(UIIdxList.GetItem(i));
		R_ASSERT(pTVItem);

		pTVItem->Close();

		std::string	caption = pTVItem->GetText();
		// Remove "+" sign
		caption.erase(0, 1);

		// ���� �� �������� �� �� ������ �������� � ��������� ����� �������� ���� �� �������
		if (0 == xr_strcmp(caption.c_str(), *groupTree.front()))
		{
			// ��� ��������. ���� ������ ������
			pTVItemChilds = pTVItem;
			for (GroupTree_it it = groupTree.begin() + 1; it != groupTree.end(); ++it)
			{
				pTVItem = pTVItemChilds->Find(*(*it));
				// �� �����, ���� ��������� ����� ������ �����������
				if (!pTVItem)
				{
					pTVItemChilds = AddTreeTail(it, groupTree, pTVItemChilds);
					status = true;
					break;
				}
				pTVItemChilds = pTVItem;
			}
		}

		if (status) break;
	}

	// ������ ��� ������������ ������, � �� �����? ����� ��������� ����� ��������
	if (!pTVItemChilds)
	{
		pTVItemChilds = xr_new<CUITreeViewItem>();
		pTVItemChilds->SetText(*groupTree.front());
		pTVItemChilds->SetRoot(true);
		UIIdxList.AddItem<CUITreeViewItem>(pTVItemChilds);

		// ���� � ������ ����������� 1 �������, �� ������ ���, � �������������� ������ �� ���������
		if (groupTree.size() > 1)
			pTVItemChilds = AddTreeTail(groupTree.begin() + 1, groupTree, pTVItemChilds);
	}

	// ������ ������ ��� � xml � ��������� ��������� �������
	const ref_str name = uiXml.ReadAttrib(pNode, "name", "");

	// � ����� ������� pTVItemChilds ����������� ������ ���� �� NULL
	R_ASSERT(pTVItemChilds);

	// C������ ��������� ��� �� ������ � ����� ���������, � ��������� ���� ���
//	if (!pTVItemChilds->Find(*name))
//	{
	pTVItem		= xr_new<CUITreeViewItem>();
	pTVItem->SetText(*name);
	pTVItem->SetValue(m_ArticlesDB.size() - 1);
	pTVItemChilds->AddItem(pTVItem);
//	}
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem * CUIEncyclopediaWnd::AddTreeTail(GroupTree_it it, GroupTree &cont, CUITreeViewItem *pItemToIns)
{
	// ��������� �������� �������� � ������������
	CUITreeViewItem *pNewItem = NULL;

	for (GroupTree_it it2 = it; it2 != cont.end(); ++it2)
	{
		pNewItem = xr_new<CUITreeViewItem>();
		pItemToIns->AddItem(pNewItem);
		pNewItem->SetText(*(*it2));
		pNewItem->SetRoot(true);
		pItemToIns = pNewItem;
	}

	return pNewItem;
}

//////////////////////////////////////////////////////////////////////////

bool CUIEncyclopediaWnd::OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action)
{
	if (KEY_RELEASED == keyboard_action) return false;
	if (DIK_T == dik)
	{
		AddArticle("one");
		AddArticle("two");
		AddArticle("three");
		AddArticle("four");
		AddArticle("five");
		AddArticle("six");
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaWnd::SendMessage(CUIWindow *pWnd, s16 msg, void* pData)
{
	if (&UIIdxList == pWnd && CUIListWnd::LIST_ITEM_CLICKED == msg)
	{
		// ��� ������ ��������, ��� ������� ������� �� �������
		CUITreeViewItem *pTVItem = static_cast<CUITreeViewItem*>(pData);
		R_ASSERT(pTVItem);

		if (!pTVItem->IsRoot())
		{
			// ������� ������� ������� � �����
			if (m_pCurrArticle)
			{
				UIEncyclopediaInfoBkg.DetachChild(&m_pCurrArticle->image);
				m_pCurrArticle->image.SetMask(NULL);
			}
			UIInfoList.RemoveAll();

			// ���������� �����
			CUIString str;
			str.SetText(*m_ArticlesDB[pTVItem->GetValue()].info);
			UIInfoList.AddParsedItem<CUIListItem>(str, 0, 0xffffffff);
			UIEncyclopediaInfoBkg.AttachChild(&m_ArticlesDB[pTVItem->GetValue()].image);
			m_ArticlesDB[pTVItem->GetValue()].image.SetMask(&UIImgMask);

			std::string caption = static_cast<std::string>(*m_InfosHeaderStr) + pTVItem->GetHierarchyAsText();
			UIEncyclopediaInfoHeader.UITitleText.SetText(caption.c_str());

			// ���������� ������� �������
			m_pCurrArticle = &m_ArticlesDB[pTVItem->GetValue()];
		}
	}
}
