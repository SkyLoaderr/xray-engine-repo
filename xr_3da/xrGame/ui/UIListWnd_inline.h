//=============================================================================
//  Filename:   UIListWnd_inline.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Реализация темплейтовых функций листбокса
//=============================================================================

template <class Element>
bool CUIListWnd::AddItem(const char*  str, const int shift, void* pData,
						 int value, int insertBeforeIdx)
{
	//создать новый элемент и добавить его в список
	Element* pItem = NULL;
	pItem = xr_new<Element>();

	VERIFY(pItem);

	pItem->Init(str, shift, m_bVertFlip?GetHeight()-GetSize()* m_iItemHeight-m_iItemHeight:GetSize()* m_iItemHeight, 
		m_iItemWidth, m_iItemHeight);

	pItem->SetData(pData);
	pItem->SetValue(value);
	pItem->SetTextColor(m_dwFontColor);

	return AddItem<Element>(pItem, insertBeforeIdx);
}

//////////////////////////////////////////////////////////////////////////

template <class Element>
bool CUIListWnd::AddParsedItem(const CUIString &str, const int shift,
							   const u32 &MsgColor, CGameFont* pFont,
							   void* pData, int value, int insertBeforeIdx)
{
	bool	ReturnStatus	= true;
	// I know, i know. :) But for optimization purposes I really need this.
	// Actually I didn't change string contents. I only temorary replace some symbols
	STRING&	text			= const_cast<STRING&>(str.m_str);
	STRING	buf;

	LPCSTR	clDefault		= "default";

	CGameFont *elementFont = (NULL == pFont) ? this->GetFont() : pFont;
	R_ASSERT(elementFont);

	u32			last_pos		= 0;
	int			GroupID			= GetSize();
	bool		wrapWord		= false;
	int			symbolsToSkip	= 0;
	int			lineWidth		= this->GetItemWidth() - shift - this->GetRightIndention();
	int			width			= 0;
	LPCSTR		nextWord		= &text.front();
	LPCSTR		memorizedWord	= 0;
	char		tmpC			= 0;
	int			lastWordWidth	= 0;
	int			charsCount		= 0;

	for(int i = 0; i < static_cast<int>(text.size()) - 2;)
	{
		// '\n' - переход на новую строку
		//			if((text[i] == '\\' && text[i+1]== 'n') ||
		//			   (10 == text[i] && 13 == text[i + 1]) ||
		//			   (length >= this->GetItemWidth() - shift - this->GetRightIndention()))
		if ('\\' == text[i] && 'n' == text[i + 1])
		{
			wrapWord		= true;
			symbolsToSkip	+= 2;
		}

		if ('%' == text[i] && 'c' == text[i + 1])
		{
			// Try default color first
			if (strstr(&text[i + 2], clDefault) == &text[i + 2])
			{
				i += 2 + xr_strlen(clDefault);
			}
			else
			{
				// Try predefined colors
				for (CUIXmlInit::ColorDefs::const_iterator it2 = CUIXmlInit::GetColorDefs()->begin(); it2 != CUIXmlInit::GetColorDefs()->end(); ++it2)
				{
					if (strstr(&text[i + 2], *it2->first) == &text[i + 2])
					{
						i += 2 + xr_strlen(*it2->first);
						break;
					}
				}
			}
		}

		lastWordWidth = WordTailSize(nextWord, elementFont, charsCount);
		if (width + lastWordWidth > lineWidth)
		{
			wrapWord	= true;
			width		= 0;
		}
		memorizedWord	= nextWord;
		nextWord		= FindNextWord(&text[i]);

		width	+= static_cast<int>(elementFont->SizeOf(' ') * (nextWord - memorizedWord - charsCount) + lastWordWidth);

		if (wrapWord || !nextWord)
		{	
			if (nextWord && i != 0)
			{
				tmpC = text[i - 1];
				text[i - 1] = 0;
			}
			//			buf.insert(buf.begin(), text.begin()+last_pos, text.begin()+i);
			//			buf.push_back(0);
			ReturnStatus &= AddItem<Element>(&text[last_pos], shift, pData, value, insertBeforeIdx);
			Element *pLocalItem = smart_cast<Element*>(GetItem(GetSize() - 1));
			pLocalItem->SetGroupID(GroupID);
			pLocalItem->SetTextColor(MsgColor);
			pLocalItem->SetFont(pFont);
			if (nextWord && i != 0)
			{
				text[i - 1]		= tmpC;
			}
			i				+= symbolsToSkip;
			symbolsToSkip	= 0;
			last_pos		= i;
			wrapWord		= false;
		}

		if (nextWord && memorizedWord)
			i		+= static_cast<int>(nextWord - memorizedWord);
		else
			i		= static_cast<int>(text.size());
	}

//	if(last_pos<text.size())
//	{
//		buf.clear();
//		buf.insert(buf.begin(), text.begin()+last_pos, text.end());
//		buf.push_back(0);
//		AddItem<Element>(&buf.front(), shift, pData, value, insertBeforeIdx);
//		GetItem(GetSize() - 1)->SetGroupID(GroupID);
//		Element *pLocalItem = smart_cast<Element*>(GetItem(GetSize() - 1));
//		pLocalItem->SetGroupID(GroupID);
//		pLocalItem->SetTextColor(MsgColor);
//		pLocalItem->SetFont(pFont);
//	}

	return ReturnStatus;
}

//////////////////////////////////////////////////////////////////////////

template <class Element>
bool CUIListWnd::AddItem(Element* pItem, int insertBeforeIdx)
{	
	AttachChild(pItem);

	pItem->Init(pItem->GetWndRect().left, m_bVertFlip?GetHeight()-GetSize()* m_iItemHeight-m_iItemHeight:GetSize()* m_iItemHeight, 
		m_iItemWidth, m_iItemHeight);

	pItem->SetNewRenderMethod(m_bNewRenderMethod);

	//добавление в конец или начало списка
	if(-1 == insertBeforeIdx)
	{
		m_ItemList.push_back(pItem);
		pItem->SetIndex(m_ItemList.size()-1);
	}
	else
	{
		//изменить значения индексов уже добавленых элементов
		if (!m_ItemList.empty())
			R_ASSERT(static_cast<u32>(insertBeforeIdx) <= m_ItemList.size());

		LIST_ITEM_LIST_it it2 = m_ItemList.begin();
		std::advance(it2, insertBeforeIdx);
		for(LIST_ITEM_LIST_it it = it2; m_ItemList.end() != it; ++it)
		{
			(*it)->SetIndex((*it)->GetIndex()+1);
		}
		m_ItemList.insert(it2, pItem);
		pItem->SetIndex(insertBeforeIdx);
	}

	UpdateList();

	//обновить полосу прокрутки
	m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
	m_ScrollBar.SetPageSize(s16(
		(u32)m_iRowNum<m_ItemList.size()?m_iRowNum:m_ItemList.size()));
	m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

	UpdateScrollBar();

	return true;
}
