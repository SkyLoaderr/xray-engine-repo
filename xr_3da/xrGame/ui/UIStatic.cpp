// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uistatic.h"
#include "../HUDManager.h"

#include <string.h>

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

	m_iTextOffsetX = 0;
	m_iTextOffsetY = 0;

	m_dwFontColor = 0xFFFFFFFF;
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



//прорисовка
void  CUIStatic::Draw()
{
	if(!m_bAvailableTexture)
	{
		inherited::Draw();
		return;
	}
	
	RECT rect = GetAbsoluteRect();
	m_UIStaticItem.SetPos(rect.left, rect.top);

	if(m_bClipper) TextureClipper();

	if(m_bStretchTexture)
		//растягиваем текстуру, Clipper в таком случае игнорируется (пока)
		m_UIStaticItem.Render(0, 0, rect.right-rect.left, rect.bottom-rect.top);
	else
		m_UIStaticItem.Render();

	inherited::Draw();
}


void CUIStatic::Update()
{
	inherited::Update();

	if(m_str == NULL) return;
	
	if(!GetFont() || GetFont()->SizeOf((char*)m_str) == 0) return;	
	
	GetFont()->SetAligment(GetTextAlign());
	GetFont()->SetColor(m_dwFontColor);


	/////////////////////////////////////
	//форматированный вывод текста
	/////////////////////////////////////

	//положение пишущей каретки
	curretX = 0;
	curretY = 0;
	//выводимый текст
	outX = 0;
	outY = 0;

	new_word = false;
	word_length = 0;
	
	space_width = (int)GetFont()->SizeOf(" ");
	

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
			WordOut();
			
			//"нарисовать" пробел
			if(curretX+space_width>=GetWidth())
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
					WordOut();
					
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
					int r,g,b;
					u32 offset = ReadColor(i+2, r,g,b);
					
					//++i;
					i+= offset;
					
					
					u32 color = RGB_ALPHA(0xFF,r,g,b);
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
}

void CUIStatic::WordOut()
{
	RECT rect = GetAbsoluteRect();

	//вывести слово
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
			//перейти на новую строку
			curretX =  word_width;
   			curretY += (int)GetFont()->CurrentHeight();

			outX = 0;
			outY = curretY;
		}
		
		GetFont()->Out(static_cast<float>(HUD().GetScale() * (rect.left+outX + m_iTextOffsetX)), 
						 static_cast<float>(HUD().GetScale() * (rect.top+outY + m_iTextOffsetY)),  &buf_str.front());

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
//прочитать цвет r,g,b
u32 CUIStatic::ReadColor(int pos, int& r, int& g, int& b)
{
	char buf[12];
	u32 symbols_to_copy;
	u32 str_offset = 0;

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



	//strcpy(buf_str, m_str);

/*
	char seps[]   = " \t\n";
    char *token;

	int token_length;
	int space_length = (int)GetFont()->SizeOf(" ");

    token = strtok( buf_str, seps );
    while( NULL != token )
    {
	   token_length=(int)GetFont()->SizeOf(token);

 	   if(curretX+token_length<GetWidth())
	   {
			outX = curretX;
			outY = curretY;
			curretX += token_length + space_length;
	   }
	   else
	   {
		   //перейти на новую строку
		   curretX = token_length + space_length;
   		   curretY += (int)GetFont()->CurrentHeight();

   		   outX = 0;
		   outY = curretY;

	   }

	    GetFont()->Out((float)rect.left+outX, (float)rect.top+outY,  token);

		// Get next token: 
		token = strtok( NULL, seps );
    }*/

void CUIStatic::TextureClipper(int offset_x, int offset_y, RECT* pClipRect)
{
	TextureClipper(offset_x, offset_y, pClipRect, m_UIStaticItem);
}

//offset_x и offset_y - для смещения самой текстуры 
//относительно окна CUIStatic (используется для центрирования текстур)
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


void CUIStatic::SetText(LPCSTR str) 
{
	m_sEdit.clear();

	if(str == NULL) 
	{
		m_str = NULL;
		return;
	}

	for(u32 i=0, n=xr_strlen(str); i<n; ++i)
		m_sEdit.push_back(str[i]);
	m_sEdit.push_back(0);

	m_str = &m_sEdit.front();
	
	buf_str.clear();
	str_len = m_sEdit.size();
	buf_str.resize(str_len+1);
	
}