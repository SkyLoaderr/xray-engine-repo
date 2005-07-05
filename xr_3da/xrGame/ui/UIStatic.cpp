// UIStatic.h: ����� ������������ ��������
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uistatic.h"
#include "UIXmlInit.h"
#include "UITextureMaster.h"
#include "uiframewindow.h"
#include "../HUDManager.h"
#include "../../LightAnimLibrary.h"

const char * const	clDefault	= "default";

CUIStatic:: CUIStatic()
{
	//m_str					= NULL;
	m_bAvailableTexture		= false;
	m_bTextureEnable		= true;

	m_bClipper				= false;
	m_bStretchTexture		= false;

	SetTextAlign			(CGameFont::alLeft);

	m_iTextOffsetX			= 0.0f;
	m_iTextOffsetY			= 0.0f;
	m_iTexOffsetY			= 0.0f;
	m_iTexOffsetX			= 0.0f;

	m_dwFontColor			= 0xFFFFFFFF;

	m_pMask					= NULL;
	m_ElipsisPos			= eepNone;
	m_iElipsisIndent		= 0;

	m_ClipRect.bottom		= -1;
	m_ClipRect.top			= -1;
	m_ClipRect.left			= -1;
	m_ClipRect.right		= -1;

	m_bCursorOverWindow		= false;
	m_bHeading				= false;
	m_fHeading				= 0.0f;
	m_lanim					= NULL;	
	m_lainm_start_time		= -1.0f;
}

CUIStatic::~ CUIStatic()
{

}

void CUIStatic::SetLightAnim(LPCSTR lanim)
{
	if(lanim&&xr_strlen(lanim))
		m_lanim	= LALib.FindItem(lanim);
	else
		m_lanim	= NULL;
}

void CUIStatic::Init(LPCSTR tex_name, float x, float y, float width, float height)
{
	Init(x, y, width, height);
	InitTexture(tex_name);
}

void CUIStatic::SetWndRect(float x, float y, float width, float height){
	CUIWindow::SetWndRect(x,y,width,height);
	m_lines.SetWndRect(x,y,width,height);
}

void CUIStatic::InitEx(LPCSTR tex_name, LPCSTR sh_name, float x, float y, float width, float height)
{
	Init(x, y, width, height);
	InitTextureEx(tex_name, sh_name);
}

void CUIStatic::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	m_lines.Init(x,y,width,height);
}

void CUIStatic::InitTexture(LPCSTR texture){
	InitTextureEx(texture);
}

void CUIStatic::CreateShader(const char* tex, const char* sh){
	m_UIStaticItem.CreateShader(tex,sh);	
}

ref_shader& CUIStatic::GetShader(){
	return m_UIStaticItem.GetShader();
}

void CUIStatic::SetTextureColor(u32 color){
	m_UIStaticItem.SetColor(color);
}

u32 CUIStatic::GetTextureColor() const{
	return m_UIStaticItem.GetColor();
}

void CUIStatic::InitTextureEx(LPCSTR tex_name, LPCSTR sh_name)
{
	CUITextureMaster::InitTexture(tex_name, &m_UIStaticItem);
	m_UIStaticItem.SetPos(m_wndPos.x, m_wndPos.y);
	m_bAvailableTexture = true;
}





void  CUIStatic::Draw()
{
	if(m_bClipper){
		Frect clip_rect;
		if (-1 == m_ClipRect.left && -1 == m_ClipRect.right && -1 == m_ClipRect.top && -1 == m_ClipRect.left){
			Frect our_rect	= GetAbsoluteRect();
			clip_rect		= our_rect;
			if(GetParent())	clip_rect.intersection(our_rect,GetParent()->GetAbsoluteRect());			
		}else				
			clip_rect		= m_ClipRect;

		UI()->PushScissor	(clip_rect);
	}

	DrawTexture				();	
	inherited::Draw			();
	DrawText				();

	if(m_bClipper)	UI()->PopScissor();
}


void CUIStatic::DrawText(){
	//if (GetFont()){
	//	GetFont()->SetAligment(GetTextAlign());
	//	GetFont()->SetColor(m_dwFontColor);
	//	Irect r = GetAbsoluteRect();
	//	DrawString(r);
	//}
	Frect r = GetAbsoluteRect();
	m_lines.Draw(r.x1 + m_iTextOffsetX, r.y1 + m_iTextOffsetY);
}

void CUIStatic::DrawTexture(){

	if(m_bAvailableTexture && m_bTextureEnable){
		Frect rect = GetAbsoluteRect();
		m_UIStaticItem.SetPos	(rect.left + m_iTexOffsetX, rect.top + m_iTexOffsetY);

		if(m_bStretchTexture)
			m_UIStaticItem.SetRect(0, 0, rect.width(), rect.height());
		else{
			Frect r={0.0f,0.0f,
				m_UIStaticItem.GetOriginalRectScaled().width(),
				m_UIStaticItem.GetOriginalRectScaled().height()};
			if (r.width()&&r.height())	m_UIStaticItem.SetRect(r);
		}

		if( Heading() ){
			m_UIStaticItem.Render( GetHeading() );
		}else
			m_UIStaticItem.Render();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Update()
{
	inherited::Update();
	//update light animation if defined
	if (m_lanim)
	{
		if(m_lainm_start_time<0.0f) m_lainm_start_time = Device.fTimeGlobal; 
		int frame;
		u32 clr					= m_lanim->CalculateRGB(Device.fTimeGlobal-m_lainm_start_time,frame);
		SetColor(clr);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::WordOut(const Frect &rect)
{
	//������� �����

	CGameFont *pFont = GetFont();
	R_ASSERT(pFont);

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
   			curretY += (int)pFont->CurrentHeight();

			outX = 0;
			outY = curretY;
		}
		
		UI()->OutText(pFont, GetSelfClipRect(), static_cast<float>(rect.left+outX + m_iTextOffsetX), 
					  static_cast<float>(rect.top + outY + m_iTextOffsetY),  &buf_str.front());
		word_length = 0;
		new_word = false;
	}
}

void CUIStatic::SetFont(CGameFont* pFont){
	CUIWindow::SetFont(pFont);
	m_lines.SetFont(pFont);
}

void CUIStatic::AddLetter(char letter)
{
	m_lines.AddChar(letter);

	//if(!new_word)
	//{
	//	new_word = true;
	//	word_length = 1;
	//}
	//else
	//	++word_length;

	//buf_str[word_length-1] = letter;
}

//��������� ���� r,g,b
u32 CUIStatic::ReadColor(int pos, int& r, int& g, int& b)
{
	return 0;
	//char buf[12];
	//u32 symbols_to_copy;
	//u32 str_offset = 0;

	//// Try default color first
	//if (strstr(static_cast<char*>(m_str + pos), clDefault)== m_str + pos)
	//{
	//	u32 cl = GetTextColor();
	//	r = (cl >> 16) & 0xff;
	//	g = (cl >> 8) & 0xff;
	//	b = cl & 0xff;
	//	return xr_strlen(clDefault);

	//}

	//// Try predefined colors
	//for (CUIXmlInit::ColorDefs::const_iterator it = CUIXmlInit::GetColorDefs()->begin(); it != CUIXmlInit::GetColorDefs()->end(); ++it)
	//{
	//	if (strstr(static_cast<char*>(m_str + pos), *it->first) == m_str + pos)
	//	{
	//		r = (it->second >> 16) & 0xff;
	//		g = (it->second >> 8) & 0xff;
	//		b = it->second  & 0xff;

	//		return xr_strlen(*it->first);
	//	}
	//}

	//if(xr_strlen(m_str)-pos>11)
	//	symbols_to_copy = 11;
	//else
	//	symbols_to_copy = (u32)xr_strlen(m_str)-pos;

	//
	//for(u32 i=0; i<symbols_to_copy; ++i)
	//{
	//	buf[i] = m_str[i+pos];
	//}
	// buf[11]=0;

	//
	//r = g = b = 0;
	//
	//char seps[]   = ",";
	//char *token = NULL;

 //   token = strtok(buf, seps );

	//if(NULL != token)
	//{
	//	r = atoi(token);
	//	str_offset += (u32)xr_strlen(token);
	//	++str_offset;
	//}
	//else
	//	return 0;
 //   
	//token = strtok( NULL, seps );

	//if(NULL != token)
	//{
	//	g = atoi(token);
	//	str_offset += (u32)xr_strlen(token);
	//	++str_offset;
	//}
	//else
	//	return 0;

	//token = strtok( NULL, seps );

	//if(NULL != token)
	//{
	//	b = atoi(token);
	//	if(b>99)
	//		str_offset += 3;
	//	else if(b>9)
	//		str_offset += 2;
	//	else
	//		str_offset += 1;
	//}
	//else
	//	return 0;


	//return str_offset;
}

void CUIStatic::TextureClipper(float offset_x, float offset_y, Frect* pClipRect)
{
	TextureClipper(offset_x, offset_y, pClipRect, m_UIStaticItem);
}

//////////////////////////////////////////////////////////////////////////
//offset_x � offset_y - ��� �������� ����� �������� 
//������������ ���� CUIStatic (������������ ��� ������������� �������)
//////////////////////////////////////////////////////////////////////////

void CUIStatic::TextureClipper(float offset_x, float offset_y, Frect* pClipRect,
							   CUIStaticItem& UIStaticItem)
{
	Frect parent_rect;
	
	if(pClipRect == NULL)
		if(GetParent())
			parent_rect = GetParent()->GetAbsoluteRect();
		else
			parent_rect = GetAbsoluteRect();
	else
		parent_rect = *pClipRect;
		
	Frect rect = GetAbsoluteRect();
	Frect out_rect;


	//��������� �������� �� ����������� � ����
	if(rect.left>parent_rect.right || rect.right<parent_rect.left ||
		rect.top>parent_rect.bottom ||  rect.bottom<parent_rect.top)
	{
		Frect r;
		r.set(0.0f,0.0f,0.0f,0.0f);
		UIStaticItem.SetRect(r);
		return;
	}

	
	float out_x, out_y;
	out_x = rect.left;
	out_y = rect.top;

	// out_rect - ������������� ������� � ������� ����� ����������
	// �����������, ����������� � ������ ��������� ������������ �������������
	// ����, � ����� �������� �������������� �� �������� � ������������.

	out_rect.intersection(parent_rect,rect);
	out_rect.left	-= out_x;
	out_rect.top	-= out_y;
	out_rect.right	-= out_x;
	out_rect.bottom -= out_y;

	if( m_bStretchTexture )
		UIStaticItem.SetRect(out_rect);
	else{
		Frect r;
		r.x1 = out_rect.left;
		r.x2 = out_rect.right<UIStaticItem.GetOriginalRectScaled().width()?
			out_rect.right:UIStaticItem.GetOriginalRectScaled().width();

		r.y1 = out_rect.top;
		r.y2 = out_rect.bottom<UIStaticItem.GetOriginalRectScaled().height()?
			out_rect.bottom:UIStaticItem.GetOriginalRectScaled().height();

		UIStaticItem.SetRect(r);
	}
}

void CUIStatic::ClipperOn() 
{
	m_bClipper = true;

	TextureClipper(0, 0);
}

void CUIStatic::ClipperOff(CUIStaticItem& UIStaticItem)
{
	m_bClipper = false;

	Frect out_rect;

	out_rect.top =   0;
	out_rect.bottom = GetHeight();
	out_rect.left =  0;
	out_rect.right = GetWidth();
	
	Frect r;
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

void  CUIStatic::SetShader(const ref_shader& sh)
{
	m_UIStaticItem.SetShader(sh);
	m_bAvailableTexture = true;
}

void CUIStatic::SetText(LPCSTR str, STRING &arr) 
{
#pragma todo("Satan->Satan : useless function")
	arr.clear();

	for(u32 i=0, n=xr_strlen(str); i<n; ++i)
		arr.push_back(str[i]);
	arr.push_back(0);
}

void CUIStatic::SetLines(LPCSTR str){
	m_lines.SetWndRect(GetAbsoluteRect());
	m_lines.SetText(str);	
}

void CUIStatic::SetText(LPCSTR str)
{
	m_lines.SetText(str);
	//if(str == NULL) 
	//{
	//	m_str = NULL;
	//	return;
	//}

	//CUIStatic::SetText(str, m_sEdit);

	//m_str = &m_sEdit.front();

	//buf_str.clear();
	//str_len = m_sEdit.size();
	//buf_str.resize(str_len+1);

	//Irect r = GetAbsoluteRect();
	//r.left += GetTextX() + m_iElipsisIndent;
	//Elipsis(r, m_ElipsisPos);
}

void CUIStatic::SetTextColor(u32 color, E4States state){
	m_dwTextColor[state] = color;
	m_bUseTextColor[state] = true;
}

Frect CUIStatic::GetClipperRect()
{
	if (m_bClipper)
		return m_ClipRect;
	else
		return GetSelfClipRect();
}

Frect CUIStatic::GetSelfClipRect()
{
	Frect	r;
	if (m_bClipper)
	{
		r.set(GetUIStaticItem().GetRect());
		r.add(GetUIStaticItem().GetPosX(), GetUIStaticItem().GetPosY());
	}
	else
		r.set(0.0f, 0.0f, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	return r;
}

void CUIStatic::SetMask(CUIFrameWindow *pMask)
{
	DetachChild(m_pMask);

	m_pMask = pMask;

	if (m_pMask)
	{
		AttachChild			(m_pMask);
		Frect r				= GetWndRect();
		m_pMask->SetWidth	(r.right - r.left);
		m_pMask->SetHeight	(r.bottom - r.top);
	}
}

void CUIStatic::PreprocessText(STRING &str, float width, CGameFont *pFont)
{
#pragma todo("Satan->Satan : bad function")
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

	// ���� �� ������ � ������� �� �����
	while (str.end() != it)
	{
		if (lineWidth < width)
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

void CUIStatic::DrawString(const Frect &rect)
{
	// ����� ������
#pragma todo("Satan->Satan : must be unreachable")

	//if(m_str == NULL) return;

	//if(!GetFont() || !m_str || !m_str[0]) return;	

	////��������� ������� �������
	//curretX = 0;
	//curretY = 0;
	////��������� �����
	//outX = 0;
	//outY = 0;

	//new_word = false;
	//word_length = 0;

	//space_width = (int)GetFont()->SizeOf(' ');
	//GetFont()->SetAligment(m_eTextAlign);

	//for(u32 i = 0; i<str_len+1; ++i)
	//{
	//	char cur_char;

	//	if(i<str_len)
	//		cur_char = m_str[i];
	//	else
	//		//��������� ���� ����� ��������
	//		cur_char = ' ';

	//	switch(cur_char)
	//	{
	//	case ' ':
	//		WordOut(rect);

	//		//"����������" ������
	//		if(curretX+space_width >= GetWidth())
	//		{
	//			curretY += (int)GetFont()->CurrentHeight();
	//			curretX = space_width;
	//		}
	//		else
	//		{
	//			curretX += space_width;
	//		}
	//		break;
	//	case '\\':
	//		if(i+1<str_len)
	//		{
	//			//������� �� ����� ������
	//			if(m_str[i+1]== 'n')
	//			{
	//				WordOut(rect);

	//				//��������� ������ �� ����� ������
	//				curretY += (int)GetFont()->CurrentHeight();
	//				curretX = 0;
	//			}
	//			//������� ������ '\'
	//			else if(m_str[i+1]== '\\')
	//			{
	//				AddLetter('\\');
	//			}
	//		}
	//		++i;
	//		break;
	//	case '%':
	//		if(i+1<str_len)
	//		{
	//			if(m_str[i+1]== 'c')
	//			{
	//				// First of all try to applying predefined colors

	//				int r,g,b;
	//				u32 offset = ReadColor(i+2, r,g,b);

	//				//++i;
	//				i+= offset;

	//				u32 color = RGB_ALPHA(GetTextColor() >> 24,r,g,b);
	//				GetFont()->SetColor(color);
	//			}
	//			//������� ������ '%'
	//			else if(m_str[i+1]== '%')
	//			{
	//				AddLetter('%');				
	//			}
	//		}
	//		++i;
	//		break;
	//	default:
	//		AddLetter(m_str[i]);
	//	}
	//}
	//GetFont()->OnRender();
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Elipsis(const Frect &rect, EElipsisPosition elipsisPos)
{
#pragma todo("Satan->Satan : need adaptation")
	//if (eepNone == elipsisPos) return;

	//CUIStatic::Elipsis(m_sEdit, rect, elipsisPos, GetFont());

	//// Now paste elipsis
	//m_str = &m_sEdit.front();
	//str_len = m_sEdit.size();
	//buf_str.resize(str_len + 1);
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetElipsis(EElipsisPosition pos, int indent)
{
#pragma todo("Satan->Satan : need adaptation")
	m_ElipsisPos		= pos;
	m_iElipsisIndent	= indent;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::SetClipRect(Frect r)
{
	m_ClipRect = r;
}

//////////////////////////////////////////////////////////////////////////

void CUIStatic::Elipsis(STRING &str, const Frect &rect, EElipsisPosition elipsisPos, CGameFont *pFont)
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

void CUIStatic::OnFocusReceive(){
	GetMessageTarget()->SendMessage(this, STATIC_FOCUS_RECEIVED, NULL);
}

void CUIStatic::OnFocusLost(){
	GetMessageTarget()->SendMessage(this, STATIC_FOCUS_LOST, NULL);
}

// Note: you can use "limit = -1" to fit text to 
// element's width
void CUIStatic::PerformTextLengthLimit(int limit){
	// our minimal length is length of two spaces
	int minLength = (int)this->GetFont()->SizeOf("  ");
	int selfWidth = iFloor(this->GetWidth());

	// no coment
	if (limit < 0)
		limit = selfWidth;
	if (limit < minLength)
		limit = minLength;

	int currentLength = (int)this->GetFont()->SizeOf(this->GetText());

	if (currentLength <= limit)
		return;
	else
		do
		{
			// delete one character (last) and recalculate current length
			this->DeleteLastCharacter();
			currentLength = (int)this->GetFont()->SizeOf(this->GetText());
		}while(currentLength > limit);	    
}

void CUIStatic::DeleteLastCharacter(){
	LPCSTR str = this->GetText();
	STRING arr;

	// exit if there is void string
	if (xr_strlen(str) == 0)
		return;

	// get string without last character
	for(u32 i=0, n=xr_strlen(str) - 1; i<n; ++i)
		arr.push_back(str[i]);

	arr.push_back(0);
	str = &arr.front();

	this->SetText(str);
}