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

//////////////////////////////////////////////////////////////////////////

// Смещение относительно родителя
const int		subShift	= 1;

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem::CUITreeViewItem()
	:	isRoot			(false),
		isOpened		(false),
		iTextShift		(0),
		pOwner			(NULL)
{
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
	if (pWnd == this && CUIListWnd::LIST_ITEM_CLICKED == msg)
	{
		if (IsRoot())
		{
			IsOpened() ? Close() : Open();
		}
	}

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
