// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uistatic.h"
#include "../HUDManager.h"
#include "../level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const char * const	clDefault	= "default";

//////////////////////////////////////////////////////////////////////////

CUIStatic:: CUIStatic()
{
	m_str					= NULL;
	m_bAvailableTexture		= false;
	m_bTextureEnable		= true;

	m_bClipper				= false;
	m_bStretchTexture		= false;

	SetTextAlign			(CGameFont::alLeft);

	m_iTextOffsetX			= 0;
	m_iTextOffsetY			= 0;
	m_iTexOffsetY			= 0;
	m_iTexOffsetX			= 0;

	m_dwFontColor			= 0xFFFFFFFF;

	m_pMask					= NULL;
	m_ElipsisPos			= eepNone;
	m_iElipsisIndent		= 0;

	m_ClipRect.bottom		= -1;
	m_ClipRect.top			= -1;
	m_ClipRect.left			= -1;
	m_ClipRect.right		= -1;

	m_bCursorOverWindow		= false;

}

//////////////////////////////////////////////////////////////////////////

CUIStatic::~ CUIStatic()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Init(LPCSTR tex_name, int x, int y, int width, int height)
{
	m_UIStaticItem.Init(tex_name,"hud\\default",x,y,alNone);

	m_bAvailableTexture = true;
	
	CUIWindow::Init(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::InitTexture(LPCSTR tex_name)
{
	m_UIStaticItem.Init(tex_name,"hud\\default", GetAbsoluteRect().left,
						                         GetAbsoluteRect().top,alNone);
	m_bAvailableTexture = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Init(int x, int y, int width, int height)
{
	m_bAvailableTexture = false;
	CUIWindow::Init(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////
//прорисовка
//////////////////////////////////////////////////////////////////////////

void  CUIStatic::Draw()
{
	if(m_bAvailableTexture && m_bTextureEnable)
	{

		RECT rect = GetAbsoluteRect();
//		m_UIStaticItem.SetPos(rect.left + m_iTexOffsetX, rect.top + m_iTexOffsetY);

		if(m_bClipper)
		{
			if (-1 == m_ClipRect.left && -1 == m_ClipRect.right &&
				-1 == m_ClipRect.top && -1 == m_ClipRect.left)
				TextureClipper();
			else
				TextureClipper(0, 0, &m_ClipRect);
		}

		m_UIStaticItem.SetPos(rect.left + m_iTexOffsetX, rect.top + m_iTexOffsetY);

		if(m_bStretchTexture)
			//растягиваем текстуру, Clipper в таком случае игнорируется (пока)
			m_UIStaticItem.Render(0, 0, rect.right-rect.left, rect.bottom-rect.top);
		else
			m_UIStaticItem.Render();
	}

	inherited::Draw();

	//форматированный вывод текста
	if (GetFont())
	{
		GetFont()->SetAligment(GetTextAlign());
		GetFont()->SetColor(m_dwFontColor);
		RECT r = GetAbsoluteRect();
		DrawString(r);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Update()
{
	inherited::Update();
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::WordOut(const RECT &rect)
{
	//вывести слово

	CGameFont *pFont = GetFont();
	R_ASSERT(pFont);

	if(new_word)
	{
		buf_str[word_length] = 0;
		word_width = (int)GetFont()->SizeOf(&buf_str.front());

//		// If word splitting enable then cut text trail if it's length more then rect length
//		if (word_width > rect.right - rect.left)
//		{
//			int length = 0;
//			for (STRING_IT it = buf_str.begin(); it != buf_str.end(); ++it)
//			{
//				length = length + static_cast<int>(pFont->SizeOf(*it));
//				if (length > rect.right - rect.left)
//				{
//					*it = 0;
//					word_width = (int)GetFont()->SizeOf(&buf_str.front());
//					break;
//				}
//			}
//		}

		if(curretX+word_width<GetWidth())
		{
			outX = curretX;
			outY = curretY;
			curretX += word_width;
		}
		else
		{
			//перейти на новую строку
			curretX =  word_width;
   			curretY += (int)pFont->CurrentHeight();

			outX = 0;
			outY = curretY;
		}
		
		HUD().OutText(pFont, GetClipRect(), static_cast<float>(rect.left+outX + m_iTextOffsetX), 
					  static_cast<float>(rect.top + outY + m_iTextOffsetY),  &buf_str.front());
		word_length = 0;
		new_word = false;
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::AddLetter(char letter)
{
	if(!new_word)
	{
		new_word = true;
		word_length = 1;
		buf_str[word_length-1] = letter;
	}
	else
	{
		++word_length;
		buf_str[word_length-1] = letter;
	}
}

//////////////////////////////////////////////////////////////////////////

//прочитать цвет r,g,b
u32 CUIStatic::ReadColor(int pos, int& r, int& g, int& b)
{
	char buf[12];
	u32 symbols_to_copy;
	u32 str_offset = 0;

	// Try default color first
	if (strstr(static_cast<char*>(m_str + pos), clDefault)== m_str + pos)
	{
		u32 cl = GetTextColor();
		r = (cl >> 16) & 0xff;
		g = (cl >> 8) & 0xff;
		b = cl & 0xff;
		return xr_strlen(clDefault);

	}

	// Try predefined colors
	for (CUIXmlInit::ColorDefs::const_iterator it = CUIXmlInit::GetColorDefs()->begin(); it != CUIXmlInit::GetColorDefs()->end(); ++it)
	{
		if (strstr(static_cast<char*>(m_str + pos), *it->first) == m_str + pos)
		{
			r = (it->second >> 16) & 0xff;
			g = (it->second >> 8) & 0xff;
			b = it->second  & 0xff;

			return xr_strlen(*it->first);
		}
	}

	if(xr_strlen(m_str)-pos>11)
		symbols_to_copy = 11;
	else
		symbols_to_copy = (u32)xr_strlen(m_str)-pos;

	
	for(u32 i=0; i<symbols_to_copy; ++i)
	{
		buf[i] = m_str[i+pos];
	}
	 buf[11]=0;

	
	r = g = b = 0;
	
	char seps[]   = ",";
	char *token = NULL;

    token = strtok(buf, seps );

	if(NULL != token)
	{
		r = atoi(token);
		str_offset += (u32)xr_strlen(token);
		++str_offset;
	}
	else
		return 0;
    
	token = strtok( NULL, seps );

	if(NULL != token)
	{
		g = atoi(token);
		str_offset += (u32)xr_strlen(token);
		++str_offset;
	}
	else
		return 0;

	token = strtok( NULL, seps );

	if(NULL != token)
	{
		b = atoi(token);
		if(b>99)
			str_offset += 3;
		else if(b>9)
			str_offset += 2;
		else
			str_offset += 1;
	}
	else
		return 0;


	return str_offset;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::TextureClipper(int offset_x, int offset_y, RECT* pClipRect)
{
	TextureClipper(offset_x, offset_y, pClipRect, m_UIStaticItem);
}

//////////////////////////////////////////////////////////////////////////
//offset_x и offset_y - для смещения самой текстуры 
//относительно окна CUIStatic (используется для центрирования текстур)
//////////////////////////////////////////////////////////////////////////

void CUIStatic::TextureClipper(int offset_x, int offset_y, RECT* pClipRect,
							   CUIStaticItem& UIStaticItem)
{
	RECT parent_rect;
	
	if(pClipRect == NULL)
		if(GetParent())
			parent_rect = GetParent()->GetAbsoluteRect();
		else
			parent_rect = GetAbsoluteRect();
	else
		parent_rect = *pClipRect;
		
	RECT rect = GetAbsoluteRect();
	RECT wnd_rect = GetWndRect();
	RECT out_rect;


	//проверить попадает ли изображение в окно
	if(rect.left>parent_rect.right || rect.right<parent_rect.left ||
		rect.top>parent_rect.bottom ||  rect.bottom<parent_rect.top)
	{
		Irect r;
		r.set(0,0,0,0);
		UIStaticItem.SetRect(r);
		return;
	}

	
	int out_x, out_y;
	out_x = rect.left;
	out_y = rect.top;

	// out_rect - прямоугольная область в которую будет выводиться
	// изображение, вычисляется с учетом положения относительно родительского
	// окна, а также размеров прямоугольника на текстуре с изображением.
	out_rect.top =   0;
	out_rect.bottom = GetHeight();
	out_rect.left =  0;
	out_rect.right = GetWidth();


	if(wnd_rect.left<0)
	{
		out_rect.left = -wnd_rect.left - offset_x;
	}
	else if(rect.right>parent_rect.right)
	{
		out_rect.right = GetWidth() - (rect.right-parent_rect.right) - offset_x;
	}


	if(wnd_rect.top<0)
	{
		out_rect.top = -wnd_rect.top - offset_y;
	}
	else if(rect.bottom>parent_rect.bottom)
	{
		out_rect.bottom = GetHeight() - (rect.bottom-parent_rect.bottom) - offset_y;
	}
	
	Irect r;
	r.x1 = out_rect.left;
	r.x2 = out_rect.right<UIStaticItem.GetOriginalRectScaled().width()?
		   out_rect.right:UIStaticItem.GetOriginalRectScaled().width();

	r.y1 = out_rect.top;
	r.y2 = out_rect.bottom<UIStaticItem.GetOriginalRectScaled().height()?
		   out_rect.bottom:UIStaticItem.GetOriginalRectScaled().height();

	UIStaticItem.SetRect(r);
	UIStaticItem.SetPos(out_x + offset_x , out_y  + offset_y);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::ClipperOn() 
{
	m_bClipper = true;

	TextureClipper(0, 0);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::ClipperOff(CUIStaticItem& UIStaticItem)
{
	m_bClipper = false;

	RECT out_rect;

	out_rect.top =   0;
	out_rect.bottom = GetHeight();
	out_rect.left =  0;
	out_rect.right = GetWidth();
	
	Irect r;
	r.x1 = out_rect.left;
	r.x2 = out_rect.right<UIStaticItem.GetOriginalRectScaled().width()?
		   out_rect.right:UIStaticItem.GetOriginalRectScaled().width();

	r.y1 = out_rect.top;
	r.y2 = out_rect.bottom<UIStaticItem.GetOriginalRectScaled().height()?
		   out_rect.bottom:UIStaticItem.GetOriginalRectScaled().height();
	UIStaticItem.SetRect(r);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::ClipperOff() 
{
	ClipperOff(m_UIStaticItem);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetTextureScale(float new_scale)
{
	m_UIStaticItem.SetScale(new_scale);
}

//////////////////////////////////////////////////////////////////////////

float CUIStatic::GetTextureScale()
{
	return m_UIStaticItem.GetScale();
}

//////////////////////////////////////////////////////////////////////////

void  CUIStatic::SetShader(const ref_shader& sh)
{
	m_UIStaticItem.SetShader(sh);
	m_bAvailableTexture = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetText(LPCSTR str, STRING &arr) 
{
	arr.clear();

	for(u32 i=0, n=xr_strlen(str); i<n; ++i)
		arr.push_back(str[i]);
	arr.push_back(0);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetText(LPCSTR str)
{
	if(str == NULL) 
	{
		m_str = NULL;
		return;
	}

	CUIStatic::SetText(str, m_sEdit);

	m_str = &m_sEdit.front();

	buf_str.clear();
	str_len = m_sEdit.size();
	buf_str.resize(str_len+1);

	RECT r = GetAbsoluteRect();
	r.left += GetTextX() + m_iElipsisIndent;
	Elipsis(r, m_ElipsisPos);
}

//////////////////////////////////////////////////////////////////////////

Irect CUIStatic::GetClipRect()
{
	Irect	r;
	if (m_bClipper)
	{
		r.set(GetUIStaticItem().GetRect());
		r.add(GetUIStaticItem().GetPosX(), GetUIStaticItem().GetPosY());
	}
	else
		r.set(0, 0, max<u32>(UI_BASE_WIDTH, Device.dwWidth),
					max<u32>(UI_BASE_HEIGHT, Device.dwHeight));

	return r;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetMask(CUIFrameWindow *pMask)
{
	DetachChild(m_pMask);

	m_pMask = pMask;

	if (m_pMask)
	{
		AttachChild(m_pMask);
		RECT r		= GetWndRect();

		m_pMask->SetWidth(r.right - r.left);
		m_pMask->SetHeight(r.bottom - r.top);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::PreprocessText(STRING &str, u32 width, CGameFont *pFont)
{
	// Codde guards
	R_ASSERT(pFont);
	if (str.empty()) return;

	STRING		processedStr, word, tmp;
	processedStr.reserve(str.size());

	const char	delimSpace		= ' ';
	const char	delimTab		= '\t';
	STRING_IT	it				= str.begin();
	float		lineWidth		= 0;

	bool		delimiterBegin	= false;

	// Идем по словам и считаем их длину
	while (str.end() != it)
	{
		if (iFloor(lineWidth) < static_cast<int>(width))
		{
			if (delimSpace == *it || delimTab == *it)
			{
				if (!delimiterBegin)
				{
					delimiterBegin = true;
					processedStr.insert(processedStr.end(), word.begin(), word.end());
					word.clear();
				}
			}
			else
			{
				if (delimiterBegin)
				{
					delimiterBegin = false;
				}
			}

			if ('\\' == *it && 'n' == *(it + 1))
			{
				lineWidth = 0;
			}

			if ('%' == *it && 'c' == *(it + 1))
			{
				// Try default color first
				if (strstr(&*(it + 2), clDefault) == &*(it + 2))
				{
					lineWidth -= pFont->SizeOf("%c");
					lineWidth -= pFont->SizeOf(clDefault);
				}
				else
				{
					// Try predefined colors
					for (CUIXmlInit::ColorDefs::const_iterator it2 = CUIXmlInit::GetColorDefs()->begin(); it2 != CUIXmlInit::GetColorDefs()->end(); ++it2)
					{
						if (strstr(&*(it + 2), *it2->first) == &*(it + 2))
						{
							lineWidth -= pFont->SizeOf("%c");
							lineWidth -= pFont->SizeOf(*it2->first);
							break;
						}
					}
				}
			}

			word.push_back(*it);
			lineWidth += pFont->SizeOf(*it++);
		}
		else
		{
			processedStr.push_back('\\');
			processedStr.push_back('n');

			// Remove leading spaces
			tmp.clear();

			STRING_IT it = word.begin();
			for (; it != word.end(); ++it)
			{
				if (delimSpace == *it || delimTab == *it)
					break;
			}

			tmp.assign(++it, word.end());
			word.swap(tmp);
			word.push_back(0);
			lineWidth = pFont->SizeOf(&word.front());
			word.pop_back();
		}
	}
	processedStr.insert(processedStr.end(), word.begin(), word.end());
	processedStr.push_back(0);
    processedStr.swap(str);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::DrawString(const RECT &rect)
{
	// Вывод текста

	if(m_str == NULL) return;

	if(!GetFont() || !m_str || !m_str[0]) return;	

	//положение пишущей каретки
	curretX = 0;
	curretY = 0;
	//выводимый текст
	outX = 0;
	outY = 0;

	new_word = false;
	word_length = 0;

	space_width = (int)GetFont()->SizeOf(' ');
	GetFont()->SetAligment(m_eTextAlign);

	for(u32 i = 0; i<str_len+1; ++i)
	{
		char cur_char;

		if(i<str_len)
			cur_char = m_str[i];
		else
			//завершить весь вывод пробелом
			cur_char = ' ';

		switch(cur_char)
		{
		case ' ':
			WordOut(rect);

			//"нарисовать" пробел
			if(curretX+space_width >= GetWidth())
			{
				curretY += (int)GetFont()->CurrentHeight();
				curretX = space_width;
			}
			else
			{
				curretX += space_width;
			}
			break;
		case '\\':
			if(i+1<str_len)
			{
				//переход на новую строку
				if(m_str[i+1]== 'n')
				{
					WordOut(rect);

					//перевести курсор на новую строку
					curretY += (int)GetFont()->CurrentHeight();
					curretX = 0;
				}
				//вывести символ '\'
				else if(m_str[i+1]== '\\')
				{
					AddLetter('\\');
				}
			}
			++i;
			break;
		case '%':
			if(i+1<str_len)
			{
				if(m_str[i+1]== 'c')
				{
					// First of all try to applying predefined colors

					int r,g,b;
					u32 offset = ReadColor(i+2, r,g,b);

					//++i;
					i+= offset;

					u32 color = RGB_ALPHA(GetTextColor() >> 24,r,g,b);
					GetFont()->SetColor(color);
				}
				//вывести символ '%'
				else if(m_str[i+1]== '%')
				{
					AddLetter('%');				
				}
			}
			++i;
			break;
		default:
			AddLetter(m_str[i]);
		}
	}
	GetFont()->OnRender();
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Elipsis(const RECT &rect, EElipsisPosition elipsisPos)
{
	if (eepNone == elipsisPos) return;

	CUIStatic::Elipsis(m_sEdit, rect, elipsisPos, GetFont());

	// Now paste elipsis
	m_str = &m_sEdit.front();
	str_len = m_sEdit.size();
	buf_str.resize(str_len + 1);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetElipsis(EElipsisPosition pos, int indent)
{
	m_ElipsisPos		= pos;
	m_iElipsisIndent	= indent;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetClipRect(RECT r)
{
	m_ClipRect = r;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Elipsis(STRING &str, const RECT &rect, EElipsisPosition elipsisPos, CGameFont *pFont)
{
	if (eepNone == elipsisPos) return;

	R_ASSERT(pFont);
	R_ASSERT(!str.empty());

	// Quick check
	if (pFont->SizeOf(&str.front()) <= rect.right - rect.left) return;

	// Applying elipsis cut
	int length = 0, elipsisWidth = static_cast<int>(pFont->SizeOf("..."));
	STRING_IT cutPos = str.begin(), left, right;
	bool moveLeft;

	// Depend elipsis position
	switch (elipsisPos)
	{
	case eepBegin:
		for (STRING::reverse_iterator it = str.rbegin(); it != str.rend(); ++it)
		{
			length = length + static_cast<int>(pFont->SizeOf(*it));
			if (length > rect.right - rect.left - elipsisWidth)
			{
				STRING tmp;
				tmp.assign(++it.base(), str.end());
				str.swap(tmp);
				str.insert(str.end(), 0);
				cutPos = str.begin();
				break;
			}
		}
		break;
	case eepEnd:
		for (STRING_IT it = str.begin(); it != str.end(); ++it)
		{
			length = length + static_cast<int>(pFont->SizeOf(*it));
			if (length > rect.right - rect.left - elipsisWidth)
			{
				*it = 0;
				str.resize(std::distance(str.begin(), it));
				cutPos = str.end();
				break;
			}
		}
		break;
	case eepCenter:
		left = str.begin();
		right = str.end() - 1;
		moveLeft = true;
		while (length < rect.right - rect.left - elipsisWidth && left <= right)
		{
			moveLeft ?	length = length + static_cast<int>(pFont->SizeOf(*left++)):
		length = length + static_cast<int>(pFont->SizeOf(*right--));
		moveLeft = !moveLeft;
		}

		// Cut center
		if (--left < ++right)
		{
			str.erase(left, right);
			cutPos = str.begin() + std::distance(str.begin(), str.end()) / 2;
		}
		break;
	default:
		NODEFAULT;
	}

	str.insert(cutPos, 3, '.');
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::OnMouse(int x, int y, EUIMessages mouse_action)
{
	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_window;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
	{
		cursor_on_window = true;
	}
	else
	{
		cursor_on_window = false;
	}


	if(m_bCursorOverWindow != cursor_on_window)
	{
		if(cursor_on_window)
		{
			GetMessageTarget()->SendMessage(this, STATIC_FOCUS_RECEIVED, NULL);
		}
		else
		{
			GetMessageTarget()->SendMessage(this, STATIC_FOCUS_LOST, NULL);
		}
	}
	m_bCursorOverWindow = cursor_on_window;

	inherited::OnMouse(x, y, mouse_action);
}