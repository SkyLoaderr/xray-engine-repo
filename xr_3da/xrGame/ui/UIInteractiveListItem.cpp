//=============================================================================
//  Filename:   UIInteractiveListItem.cpp
//	---------------------------------------------------------------------------
//  Representation of list item with many interactive fields
//=============================================================================

#include "StdAfx.h"
#include "UIInteractiveListItem.h"
#include "../Level.h"
#include "../HUDManager.h"

//=============================================================================
//  CUIInteractiveListItem class
//=============================================================================
CUIInteractiveListItem::CUIInteractiveListItem()
	: itCurrIItem(vPositions.end()),
m_bInteractiveBahaviour(true)
{

}

//-----------------------------------------------------------------------------/
//  str:	входная строка, уже без разделителей
//	Data:	вектор указателей на интерактивные элементы
//	IDs:	в этом массиве возвращаеются уникальные идентификаторы для интеракт.
//			элементов
//	height:	высота элемента
//	StartShift:	смещение относительно левого края в пробелах
//-----------------------------------------------------------------------------/
void CUIInteractiveListItem::Init(const char *str, const xr_vector<char *> &Data, xr_vector<int> &IDs, int height)
{
	CGameFont	*pFont	= GetFont();
	R_ASSERT(pFont);

	// Counter
	int counter = 0;
	// Cмещение в пикселях.
	u32 shift = 0;

	std::string	strTmp;
	// Начальная и конечная координата в пикселях текущего интерактивного блока
	FIELDS_COORDS_VECTOR::value_type tmpPairs;

	// В эту процедуру мы попадаем, только при гарантии, что в векторе указателей четное количество 
	// членов.
	R_ASSERT(Data.size() % 2 == 0);

	SetText(str);

	// Cначала добавляем текст

	for (xr_vector<char *>::const_iterator it = Data.begin(); it != Data.end(); ++it, ++it)
	{
		// Указатели на подстроку
		strTmp.assign(*it, *(it + 1));
		tmpPairs.subStr = strTmp;
		// Экранные координаты
		strTmp.assign(str, (*it));
		tmpPairs.pairScreenCrd.first = static_cast<int>(pFont->SizeOf(strTmp.c_str())) + shift;
		strTmp.assign(str, (*(it + 1)));
		tmpPairs.pairScreenCrd.second = static_cast<int>(pFont->SizeOf(strTmp.c_str())) + shift;
		// ID
		tmpPairs.ID = IDs[counter];
		// Save current
		vPositions.push_back(tmpPairs);
		++counter;
	}
	itCurrIItem = vPositions.end();
}

//-----------------------------------------------------------------------------/
//  Обработка событий мыши
//-----------------------------------------------------------------------------/
void CUIInteractiveListItem::OnMouse(int x, int y, EUIMessages mouse_action)
{
	std::pair<int, int> tmpPair(x, y);
	itCurrIItem = std::find_if(vPositions.begin(), vPositions.end(), 
		std::bind2nd(mouse_hit(), tmpPair));

	// Смотрим на какой интерактивный элемент куазывает курсор во время нажатия, 
	// и рапортуем об этом родителю
	if (m_bInteractiveBahaviour && WINDOW_LBUTTON_DOWN == mouse_action && itCurrIItem != vPositions.end())
	{
		GetMessageTarget()->SendMessage(this, INTERACTIVE_ITEM_CLICK, (void*)&(*itCurrIItem).ID);
	}
	else
		inherited::OnMouse(x, y, mouse_action);
}

//-----------------------------------------------------------------------------/
//  Получаем координаты интерактивного поля для подсветки
//-----------------------------------------------------------------------------/
RECT CUIInteractiveListItem::GetAbsoluteSubRect()
{
	RECT tmpRect = CUIWindow::GetAbsoluteRect();
	if (itCurrIItem != vPositions.end())
	{
		tmpRect.left	+= (*itCurrIItem).pairScreenCrd.first;
		tmpRect.right	=  tmpRect.left + (*itCurrIItem).pairScreenCrd.second;
	}
	return tmpRect;
}

void CUIInteractiveListItem::Update()
{
	m_bHighlightText = false;
	inherited::Update();
}

void CUIInteractiveListItem::SetIItemID(const u32 uIndex, const int ID)
{
	R_ASSERT(vPositions.size() > uIndex);
	vPositions[uIndex].ID = ID;
}

u32 CUIInteractiveListItem::GetIFieldsCount()
{
	return vPositions.size();
}

//////////////////////////////////////////////////////////////////////////

void CUIInteractiveListItem::Draw()
{
	inherited::Draw();

	RECT rect = GetAbsoluteSubRect();

	// Подсвечиваем интерактивный элемент
	if (itCurrIItem != vPositions.end() && m_bCursorOverWindow && m_bInteractiveBahaviour)
	{
		UpdateTextAlign();
		GetFont()->SetAligment(GetTextAlign());

		GetFont()->SetColor(m_HighlightColor);
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 1 +m_iTextOffsetX, 
			(float)rect.top + 1 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 1 +m_iTextOffsetX, 
			(float)rect.top - 1 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 1 +m_iTextOffsetX, 
			(float)rect.top + 1 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 1 +m_iTextOffsetX, 
			(float)rect.top - 1 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 1 +m_iTextOffsetX, 
			(float)rect.top + 0 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 1 +m_iTextOffsetX, 
			(float)rect.top - 0 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 0 +m_iTextOffsetX, 
			(float)rect.top + 1 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());
		HUD().OutText(GetFont(), GetClipRect(),
			(float)rect.left + 0 +m_iTextOffsetX,  
			(float)rect.top - 1 +m_iTextOffsetY,
			(*itCurrIItem).subStr.c_str());

	}
	// вывод всей строки

	GetFont()->OnRender();
}