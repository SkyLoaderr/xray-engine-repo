//=============================================================================
//  Filename:   UITreeViewItem.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  TreeView Item class
//=============================================================================

#include "stdafx.h"
#include "UITreeViewItem.h"
#include "UIListWnd.h"
#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

// Смещение относительно родителя
const int				subShift					= 1;
const char * const		treeItemBackgroundTexture	= "ui\\ui_pda_over_list";

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem::CUITreeViewItem()
	:	isRoot			(false),
		isOpened		(false),
		iTextShift		(0),
		pOwner			(NULL)
{
	AttachChild(&UIBkg);
	UIBkg.InitTexture(treeItemBackgroundTexture);
	UIBkg.TextureOff();
	UIBkg.SetTextureOffset(-20, 0);
	EnableTextHighlighting(false);
	EnableDoubleClick(false);
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem::~CUITreeViewItem()
{
	DeleteAllSubItems();
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::OnRootChanged()
{
	std::string str;
	if (isRoot)
	{
		// Вставляем после последнего пробела перед текстом знак + или -
		str = GetText();

		std::string::size_type pos = str.find_first_not_of(" ");
		if (std::string::npos == pos) pos = 0;

		if (pos == 0)
		{
			++iTextShift;
			str.insert(0, " ");
		}
		else
			--pos;

		if (isOpened)
			// Add minus sign
			str.replace(pos, 1, "-");
		else
			// Add plus sign
			str.replace(pos, 1, "+");

		inherited::SetText(str.c_str());
	}
	else
	{
		str = GetText();
		// Remove "+/-" sign
		std::string::size_type pos = str.find_first_of("+-");

		if (pos == 0)
		{
			for (int i = 0; i < iTextShift; ++i)
				str.insert(pos, " ");
		}
		else
			str.replace(pos, 1, " ");

		inherited::SetText(str.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::OnOpenClose()
{
	// Если мы не являемся узлом дерева, значит ничего не делаем
	if (!isRoot) return;

	std::string str;

	str = GetText();
	std::string::size_type pos = str.find_first_of("+-");

	if (std::string::npos != pos)
	{
		if (isOpened)
			// Change minus sign to plus
			str.replace(pos, 1, "-");
		else
			// Change plus sign to minus
			str.replace(pos, 1, "+");
	}

	inherited::SetText(str.c_str());
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::Open()
{
	// Если не рут или уже открыты, то ничего не делаем
	if (!isRoot || isOpened) return;
	isOpened = true;

	// Изменяем состояние
	OnOpenClose();
	
	// Аттачим все подэлементы к родтельскому листбоксу
	CUIListWnd *pList = dynamic_cast<CUIListWnd*>(GetParent());
	
	R_ASSERT(pList);
	if (!pList) return;

	int pos = pList->GetItemPos(this);

	for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
	{
		pList->AddItem(*it, ++pos);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::Close()
{
	// Если не рут или уже открыты, то ничего не делаем
	if (!isRoot || !isOpened) return;
	isOpened = false;

	// Изменяем состояние
	OnOpenClose();

	// Детачим все подэлементы
	CUIListWnd *pList = dynamic_cast<CUIListWnd*>(GetParent());

	R_ASSERT(pList);
	if (!pList) return;

	int pos;

	// Сначала все закрыть
	for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
	{
		(*it)->Close();
	}

	// Затем все датачим
	for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
	{
		pos = pList->GetItemPos(*it);
		pList->RemoveItem(pos);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::AddItem(CUITreeViewItem *pItem)
{
	R_ASSERT(pItem);
	if (!pItem) return;

	pItem->SetTextShift(subShift + iTextShift);

	vSubItems.push_back(pItem);
	pItem->ManualDelete(true);

	pItem->SetOwner(this);

	pItem->SetText(pItem->GetText());

}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::DeleteAllSubItems()
{
	for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
	{
		CUIWindow *pWindow = (*it)->GetParent();

		if (pWindow)
			pWindow->DetachChild(*it);

		xr_delete(*it);
		(*it) = NULL;
	}

	vSubItems.clear();
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::SetRoot(bool set)
{
	if (isRoot) return;

	isRoot = set;
	OnRootChanged();
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::SetText(LPCSTR str)
{
	std::string s = str;
	std::string::size_type pos = s.find_first_not_of(" +-");

	if (pos < static_cast<std::string::size_type>(iTextShift))
	{
		for (u32 i = 0; i < iTextShift - pos; ++i)
			s.insert(0, " ");
	}
	else if (pos > static_cast<std::string::size_type>(iTextShift))
	{
		s.erase(0, pos - iTextShift);
	}

	inherited::SetText(s.c_str());
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pWnd == this && BUTTON_CLICKED == msg)
	{
		if (IsRoot())
		{
			IsOpened() ? Close() : Open();
		}
	}
	else if (pWnd == this && BUTTON_FOCUS_RECEIVED == msg)
	{
		static CUITreeViewItem *pPrevFocusedItem = NULL;
		UIBkg.TextureOn();

		if (pPrevFocusedItem)
		{
			pPrevFocusedItem->UIBkg.TextureOff();
		}
		pPrevFocusedItem = this;
	}
	else
		inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem * CUITreeViewItem::Find(LPCSTR text) const
{
	// Пробегаемся по списку подчиненных элементов, и ищем элемент с заданным текстом
	// Если среди подч. эл-тов есть root'ы, то ищем рекурсивно в них
	CUITreeViewItem *pResult = NULL;
	std::string caption;

	for (SubItems::const_iterator it = vSubItems.begin(); it != vSubItems.end(); ++it)
	{
		caption = (*it)->GetText();
		std::string::size_type pos = caption.find_first_not_of(" +-");
		if (pos != std::string::npos)
		{
			caption.erase(0, pos);
		}

		if (xr_strcmp(caption.c_str(), text) == 0)
			pResult = *it;

		if ((*it)->IsRoot() && !pResult)
			pResult = (*it)->Find(text);

		if (pResult) break;
	}

	return pResult;
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem * CUITreeViewItem::Find(CUITreeViewItem * pItem) const
{
	CUITreeViewItem *pResult = NULL;

	for (SubItems::const_iterator it = vSubItems.begin(); it != vSubItems.end(); ++it)
	{
		if ((*it)->IsRoot() && !pResult)
			pResult = (*it)->Find(pItem);
		else
			if (pItem == *it) pResult = *it;

		if (pResult) break;
	}

	return pResult;
}

//////////////////////////////////////////////////////////////////////////

std::string CUITreeViewItem::GetHierarchyAsText()
{
	std::string name;

	if (GetOwner())
	{
		name = GetOwner()->GetHierarchyAsText();
	}

	std::string::size_type prevPos = name.size() + 1;
	name += static_cast<std::string>("/") + static_cast<std::string>(GetText());

	// Удаляем мусор: [ +-]
	std::string::size_type pos = name.find_first_not_of("/ +-", prevPos);
	if (std::string::npos != pos)
	{
		name.erase(prevPos, pos - prevPos);
	}

	return name;
}

//////////////////////////////////////////////////////////////////////////
// Standalone function for tree hierarchy creation
//////////////////////////////////////////////////////////////////////////

void CreateTreeBranch(ref_str nesting, ref_str leafName, CUIListWnd *pListToAdd, int leafProperty,
					  CGameFont *pRootFont, u32 rootColor, CGameFont *pLeafFont, u32 leafColor)
{
	// Nested function emulation
	class AddTreeTail_
	{
	private:
		CGameFont	*pRootFnt;
		u32			rootItemColor;
	public:
		AddTreeTail_(CGameFont *f, u32 cl)
			:	pRootFnt		(f),
				rootItemColor	(cl)
		{}

		CUITreeViewItem * operator () (GroupTree_it it, GroupTree &cont, CUITreeViewItem *pItemToIns)
		{
			// Вставляем иерархию разделов в энциклопедию
			CUITreeViewItem *pNewItem = NULL;

			for (GroupTree_it it2 = it; it2 != cont.end(); ++it2)
			{
				pNewItem = xr_new<CUITreeViewItem>();
				pItemToIns->AddItem(pNewItem);
				pNewItem->SetFont(pRootFnt);
				pNewItem->SetText(*(*it2));
				pNewItem->SetTextColor(rootItemColor);
				pNewItem->SetRoot(true);
				pItemToIns = pNewItem;
			}

			return pNewItem;
		}
	} AddTreeTail(pRootFont, rootColor);

	//-----------------------------------------------------------------------------
	//  Function body
	//-----------------------------------------------------------------------------

	// Начинаем алгоритм определения группы вещи в иерархии энциклопедии
	R_ASSERT(*nesting);
	R_ASSERT(pListToAdd);
	R_ASSERT(pLeafFont);
	R_ASSERT(pRootFont);
	std::string group = *nesting;

	// Парсим строку группы для определения вложенности
	GroupTree					groupTree;

	std::string::size_type		pos;
	std::string					oneLevel;

	while (true)
	{
		pos = group.find('/');
		if (pos != std::string::npos)
		{
			oneLevel.assign(group, 0, pos);
			ref_str str(oneLevel.c_str());
			groupTree.push_back(CStringTable()(str));
			group.erase(0, pos + 1);
		}
		else
		{
			groupTree.push_back(CStringTable()(group.c_str()));
			break;
		}
	}

	// Теперь ищем нет ли затребованных групп уже в наличии
	CUITreeViewItem *pTVItem = NULL, *pTVItemChilds = NULL;
	bool status = false;

	// Для всех рутовых элементов
	for (int i = 0; i < pListToAdd->GetSize(); ++i)
	{
		pTVItem = dynamic_cast<CUITreeViewItem*>(pListToAdd->GetItem(i));
		R_ASSERT(pTVItem);

		pTVItem->Close();

		std::string	caption = pTVItem->GetText();
		// Remove "+" sign
		caption.erase(0, 1);

		// Ищем не содержит ли он данной иерархии и добавляем новые элементы если не найдено
		if (0 == xr_strcmp(caption.c_str(), *groupTree.front()))
		{
			// Уже содержит. Надо искать глубже
			pTVItemChilds = pTVItem;
			for (GroupTree_it it = groupTree.begin() + 1; it != groupTree.end(); ++it)
			{
				pTVItem = pTVItemChilds->Find(*(*it));
				// Не нашли, надо вставлять хвост списка вложенности
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

	// Прошли все существующее дерево, и не нашли? Тогда добавляем новую иерархию
	if (!pTVItemChilds)
	{
		pTVItemChilds = xr_new<CUITreeViewItem>();
		pTVItemChilds->SetFont(pRootFont);
		pTVItemChilds->SetText(*groupTree.front());
		pTVItemChilds->SetTextColor(rootColor);
		pTVItemChilds->SetRoot(true);
		pListToAdd->AddItem<CUITreeViewItem>(pTVItemChilds);

		// Если в списке вложенности 1 элемент, то хвоста нет, и соответственно ничего не добавляем
		if (groupTree.size() > 1)
			pTVItemChilds = AddTreeTail(groupTree.begin() + 1, groupTree, pTVItemChilds);
	}

	// К этому моменту pTVItemChilds обязательно должна быть не NULL
	R_ASSERT(pTVItemChilds);

	// Cначала проверяем нет ли записи с таким названием, и добавляем если нет
	//	if (!pTVItemChilds->Find(*name))
	//	{
	pTVItem		= xr_new<CUITreeViewItem>();
	pTVItem->SetFont(pLeafFont);
	pTVItem->SetTextColor(leafColor);
	pTVItem->SetText(*CStringTable()(*leafName));
	pTVItem->SetValue(leafProperty);
	pTVItemChilds->AddItem(pTVItem);
	//	}
}