// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uistatic.h"

#include <string.h>


#define RGB_ALPHA(a, r, g ,b)  ((u32) (((u8) (b) | ((u16) (g) << 8)) | (((u32) (u8) (r)) << 16)) | (((u32) (u8) (a)) << 24)) 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIStatic:: CUIStatic()
{
	m_str = NULL;
	m_bAvailableTexture = false;

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


void CUIStatic::Init(int x, int y, int width, int height)
{
	m_bAvailableTexture = false;
	CUIWindow::Init(x, y, width, height);
}



//прорисовка
void  CUIStatic::Draw()
{
	if(!m_bAvailableTexture) return;
	
	RECT rect = GetAbsoluteRect();
	m_UIStaticItem.SetPos(rect.left, rect.top);
	m_UIStaticItem.Render();
			
}


void CUIStatic::Update()
{
	if(m_str == NULL) return;
	
	if(GetFont()->SizeOf((char*)m_str) == 0) return;	
	

	//RECT rect = GetAbsoluteRect();
	GetFont()->SetAligment(CGameFont::alLeft);
	GetFont()->SetColor(0xFFEEEEEE);


	
	/////////////////////////////////////
	//форматированный вывод текста
	/////////////////////////////////////

	
	//положение пишущей каретки
	curretX = 0;
	curretY = 0;
	//выводимый текст
	outX = 0;
	outY = 0;

	u32 str_len = (u32)strlen(m_str);

	buf_str = (char*)xr_malloc(str_len);
	
	new_word = false;
	word_length = 0;
	
	space_width = (int)GetFont()->SizeOf(" ");
	

	for(u32 i = 0; i<str_len+1; i++)
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
			i++;
			break;
		case '%':
			if(i+1<str_len)
			{
				if(m_str[i+1]== 'c')
				{
					int r,g,b;
					u32 offset = ReadColor(i+2, r,g,b);
					
					//i++;
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
			i++;
			break;
		default:
			AddLetter(m_str[i]);
		}
	}

	xr_free(buf_str);
}

void CUIStatic::WordOut()
{
	RECT rect = GetAbsoluteRect();

	//вывести слово
	if(new_word)
	{
		buf_str[word_length] = 0;
		word_width = (int)GetFont()->SizeOf(buf_str);

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
		
		GetFont()->Out((float)rect.left+outX, (float)rect.top+outY,  buf_str);

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
		word_length++;
		buf_str[word_length-1] = letter;
	}
}
//прочитать цвет r,g,b
u32 CUIStatic::ReadColor(int pos, int& r, int& g, int& b)
{
	char buf[12];
	u32 symbols_to_copy;
	u32 str_offset = 0;

	if(strlen(m_str)-pos>11)
		symbols_to_copy = 11;
	else
		symbols_to_copy = (u32)strlen(m_str)-pos;

	
	for(u32 i=0; i<symbols_to_copy; i++)
	{
		buf[i] = m_str[i+pos];
	}
	 buf[11]=0;

	
	r = g = b = 0;
	
	char seps[]   = ",";
	char *token = NULL;

    token = strtok(buf, seps );

	if(token!=NULL)
	{
		r = atoi(token);
		str_offset += (u32)strlen(token);
		str_offset++;
	}
	else
		return 0;
    
	token = strtok( NULL, seps );

	if(token!=NULL)
	{
		g = atoi(token);
		str_offset += (u32)strlen(token);
		str_offset++;
	}
	else
		return 0;

	token = strtok( NULL, seps );

	if(token!=NULL)
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
    while( token != NULL )
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
