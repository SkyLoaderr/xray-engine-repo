// UIStatic.h: ����� ������������ ��������
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uistatic.h"
#include "../HUDManager.h"
#include "../level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIStatic:: CUIStatic()
{
	m_str = NULL;
	m_bAvailableTexture = false;

	m_bClipper = false;
	m_bStretchTexture = false;

	SetTextAlign(CGameFont::alLeft);

	m_iTextOffsetX	= 0;
	m_iTextOffsetY	= 0;
	m_iTexOffsetY	= 0;
	m_iTexOffsetX	= 0;

	m_dwFontColor = 0xFFFFFFFF;

	m_pMask = NULL;

}

 CUIStatic::~ CUIStatic()
{
}



void CUIStatic::Init(LPCSTR tex_name, int x, int y, int width, int height)
{
	m_UIStaticItem.Init(tex_name,"hud\\default",x,y,alNone);

	m_bAvailableTexture = true;
	
	CUIWindow::Init(x, y, width, height);
}

void CUIStatic::InitTexture(LPCSTR tex_name)
{
	m_UIStaticItem.Init(tex_name,"hud\\default", GetAbsoluteRect().left,
						                         GetAbsoluteRect().top,alNone);
	m_bAvailableTexture = true;
}

void CUIStatic::Init(int x, int y, int width, int height)
{
	m_bAvailableTexture = false;
	CUIWindow::Init(x, y, width, height);
}



//����������
void  CUIStatic::Draw()
{
	if(m_bAvailableTexture)
	{

		RECT rect = GetAbsoluteRect();
//		m_UIStaticItem.SetPos(rect.left + m_iTexOffsetX, rect.top + m_iTexOffsetY);

		if(m_bClipper) TextureClipper();

		m_UIStaticItem.SetPos(rect.left + m_iTexOffsetX, rect.top + m_iTexOffsetY);

		if(m_bStretchTexture)
			//����������� ��������, Clipper � ����� ������ ������������ (����)
			m_UIStaticItem.Render(0, 0, rect.right-rect.left, rect.bottom-rect.top);
		else
			m_UIStaticItem.Render();
	}

	inherited::Draw();

	//��������������� ����� ������
	if (GetFont())
	{
		GetFont()->SetAligment(GetTextAlign());
		GetFont()->SetColor(m_dwFontColor);
		RECT r = GetAbsoluteRect();
		DrawString(r);
	}
}


void CUIStatic::Update()
{
	inherited::Update();
}

void CUIStatic::WordOut(const RECT &rect)
{
	//������� �����
	if(new_word)
	{
		buf_str[word_length] = 0;
		word_width = (int)GetFont()->SizeOf(&buf_str.front());

		if(curretX+word_width<GetWidth())
		{
			outX = curretX;
			outY = curretY;
			curretX += word_width;
		}
		else
		{
			//������� �� ����� ������
			curretX =  word_width;
   			curretY += (int)GetFont()->CurrentHeight();

			outX = 0;
			outY = curretY;
		}
		
		HUD().OutText(GetFont(), GetClipRect(), static_cast<float>(rect.left+outX + m_iTextOffsetX), 
					  static_cast<float>(rect.top + outY + m_iTextOffsetY),  &buf_str.front());
		word_length = 0;
		new_word = false;
	}
}

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

//��������� ���� r,g,b
u32 CUIStatic::ReadColor(int pos, int& r, int& g, int& b)
{
	char buf[12];
	u32 symbols_to_copy;
	u32 str_offset = 0;
	const char * const	clDefault	= "default";

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
	for (CUIXmlInit::ColorDefs_it it = CUIXmlInit::m_ColorDefs.begin(); it != CUIXmlInit::m_ColorDefs.end(); ++it)
	{
		if (strstr(static_cast<char*>(m_str + pos), *it->first) == m_str + pos)
		{
			r = (it->second >> 16) & 0xff;
			g = (it->second >> 8) & 0xff;
			b = it->second  & 0xff;

			return xr_strlen(it->first);
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
//offset_x � offset_y - ��� �������� ����� �������� 
//������������ ���� CUIStatic (������������ ��� ������������� �������)
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


	//��������� �������� �� ����������� � ����
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

	// out_rect - ������������� ������� � ������� ����� ����������
	// �����������, ����������� � ������ ��������� ������������ �������������
	// ����, � ����� �������� �������������� �� �������� � ������������.
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


void CUIStatic::ClipperOn() 
{
	m_bClipper = true;

	TextureClipper(0, 0);
}

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


void CUIStatic::ClipperOff() 
{
	ClipperOff(m_UIStaticItem);
}

void CUIStatic::SetTextureScale(float new_scale)
{
	m_UIStaticItem.SetScale(new_scale);
}

float CUIStatic::GetTextureScale()
{
	return m_UIStaticItem.GetScale();
}

void  CUIStatic::SetShader(const ref_shader& sh)
{
	m_UIStaticItem.SetShader(sh);
	m_bAvailableTexture = true;
}


void CUIStatic::SetText(LPCSTR str, STRING &arr) 
{
	arr.clear();

	for(u32 i=0, n=xr_strlen(str); i<n; ++i)
		arr.push_back(str[i]);
	arr.push_back(0);
}

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
	STRING_IT	it				= str.begin(), wordBegin = str.begin();
	float		lineWidth		= 0;

	bool		delimiterBegin	= false;

	// ���� �� ������ � ������� �� �����
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
					wordBegin = it;
				}
			}

			if ('\\' == *it && it != str.end() && 'n' == *(it + 1))
			{
				lineWidth = 0;
			}

			word.push_back(*it);
			lineWidth += pFont->SizeOf(*it++);
		}
		else
		{
			lineWidth = 0;
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
		}
	}
	processedStr.insert(processedStr.end(), word.begin(), word.end());
	processedStr.push_back(0);
    processedStr.swap(str);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::DrawString(const RECT &rect)
{
	// ����� ������

	if(m_str == NULL) return;

	if(!GetFont() || !m_str || !m_str[0]) return;	

	//��������� ������� �������
	curretX = 0;
	curretY = 0;
	//��������� �����
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
			//��������� ���� ����� ��������
			cur_char = ' ';

		switch(cur_char)
		{
		case ' ':
			WordOut(rect);

			//"����������" ������
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
				//������� �� ����� ������
				if(m_str[i+1]== 'n')
				{
					WordOut(rect);

					//��������� ������ �� ����� ������
					curretY += (int)GetFont()->CurrentHeight();
					curretX = 0;
				}
				//������� ������ '\'
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
				//������� ������ '%'
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
