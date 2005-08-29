// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uistatic.h"
#include "UIXmlInit.h"
#include "UITextureMaster.h"
#include "uiframewindow.h"
#include "../HUDManager.h"
#include "../../LightAnimLibrary.h"
#include "uilines.h"

const char * const	clDefault	= "default";
#define CREATE_LINES if (!m_pLines) {m_pLines = xr_new<CUILines>(); m_pLines->SetTextAlignment(CGameFont::alLeft);}

CUIStatic:: CUIStatic()
{
	m_bAvailableTexture		= false;
	m_bTextureEnable		= true;
	m_bClipper				= false;
	m_bStretchTexture		= false;

	m_iTextOffsetX			= 0.0f;
	m_iTextOffsetY			= 0.0f;
	m_iTexOffsetY			= 0.0f;
	m_iTexOffsetX			= 0.0f;

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
	m_pLines				= NULL;
}

CUIStatic::~ CUIStatic()
{
	xr_delete(m_pLines);
}

void CUIStatic::SetLightAnim(LPCSTR lanim)
{
	if(lanim && lanim[0]!=0)
		m_lanim	= LALib.FindItem(lanim);
	else
		m_lanim	= NULL;
}

void CUIStatic::Init(LPCSTR tex_name, float x, float y, float width, float height)
{
	Init(x, y, width, height);
	InitTexture(tex_name);
}

void CUIStatic::InitEx(LPCSTR tex_name, LPCSTR sh_name, float x, float y, float width, float height)
{
	Init(x, y, width, height);
	InitTextureEx(tex_name, sh_name);	
}

void CUIStatic::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	m_xxxRect.set(x,y,x+width,y+height);
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
	CUITextureMaster::InitTexture(tex_name, sh_name, &m_UIStaticItem);
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
	
	if (m_pLines)
	{
		m_pLines->SetWndSize(m_wndSize);
		Frect r = GetAbsoluteRect();
        m_pLines->Draw(r.x1 + m_iTextOffsetX, r.y1 + m_iTextOffsetY);
	}
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

void CUIStatic::Update()
{
	inherited::Update();
	//update light animation if defined
	if (m_lanim)
	{
		if(m_lainm_start_time<0.0f) m_lainm_start_time = Device.fTimeGlobal; 
		int frame;
		u32 clr					= m_lanim->CalculateRGB(Device.fTimeGlobal-m_lainm_start_time,frame);
		SetColor				(clr);
		SetTextColor			(clr);
	}
}

void CUIStatic::SetFont(CGameFont* pFont){
	CUIWindow::SetFont(pFont);
	CREATE_LINES;
	m_pLines->SetFont(pFont);
}

void CUIStatic::SetTextComplexMode(bool md){
	CREATE_LINES;
	m_pLines->SetTextComplexMode(md);
}

CGameFont* CUIStatic::GetFont(){
	CREATE_LINES;
	return m_pLines->GetFont();
}

void CUIStatic::TextureClipper(float offset_x, float offset_y, Frect* pClipRect)
{
	TextureClipper(offset_x, offset_y, pClipRect, m_UIStaticItem);
}

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


	//проверить попадает ли изображение в окно
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

	// out_rect - прямоугольная область в которую будет выводиться
	// изображение, вычисляется с учетом положения относительно родительского
	// окна, а также размеров прямоугольника на текстуре с изображением.

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

LPCSTR CUIStatic::GetText(){
	static const char empty = 0;
	if (m_pLines)
		return m_pLines->GetText();
	else
		return &empty;
}

void CUIStatic::SetTextColor(u32 color){
	CREATE_LINES;
	m_pLines->SetTextColor(color);
}

u32 CUIStatic::GetTextColor(){
	CREATE_LINES;
	return m_pLines->GetTextColor();
}

u32& CUIStatic::GetTextColorRef(){
	return m_pLines->GetTextColorRef();
}

void CUIStatic::SetText(LPCSTR str)
{
	if (!str || 0 == str[0]) 
		return;
	CREATE_LINES;
	m_pLines->SetText(str);
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
	if (m_pMask){
		AttachChild			(m_pMask);
		Frect r				= GetWndRect();
		m_pMask->SetWidth	(r.right - r.left);
		m_pMask->SetHeight	(r.bottom - r.top);
	}
}

//CGameFont::EAligment CUIStatic::GetTextAlign(){
//	return m_pLines->GetTextAlignment();
//}

CGameFont::EAligment CUIStatic::GetTextAlignment(){
	return m_pLines->GetTextAlignment();
}

//void CUIStatic::SetTextAlign(CGameFont::EAligment align){
//	CREATE_LINES;
//	m_pLines->SetTextAlignment(align);
//}

void CUIStatic::SetTextAlignment(CGameFont::EAligment align){
	CREATE_LINES;
	m_pLines->SetTextAlignment(align);
}

void CUIStatic::SetVTextAlignment(EVTextAlignment al){
	CREATE_LINES;
	m_pLines->SetVTextAlignment(al);
}

void CUIStatic::SetTextAlign_script(u32 align){
	m_pLines->SetTextAlignment((CGameFont::EAligment)align);
}

u32 CUIStatic::GetTextAlign_script(){
	return static_cast<u32>(m_pLines->GetTextAlignment());
}

void CUIStatic::PreprocessText(STRING &str, float width, CGameFont *pFont)
{
#pragma todo("Satan->Satan : bad function")
	R_ASSERT(false);
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

void CUIStatic::SetElipsis(EElipsisPosition pos, int indent)
{
#pragma todo("Satan->Satan : need adaptation")
	m_ElipsisPos		= pos;
	m_iElipsisIndent	= indent;
}

void CUIStatic::SetClipRect(Frect r)
{
	m_ClipRect = r;
}

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
	if (GetMessageTarget())
        GetMessageTarget()->SendMessage(this, STATIC_FOCUS_RECEIVED, NULL);
}

void CUIStatic::OnFocusLost(){
	if (GetMessageTarget())
		GetMessageTarget()->SendMessage(this, STATIC_FOCUS_LOST, NULL);
}

void CUIStatic::AdjustHeightToText(){
	m_pLines->SetWidth(GetWidth());
	SetHeight		(m_pLines->GetVisibleHeight());
}

void CUIStatic::RescaleRelative2Rect(const Frect& r){
	Frect my_r = m_xxxRect;
	float h_rel = my_r.width()/r.width();
	float v_rel = my_r.height()/r.height();
	float w;
	float h;
	if (h_rel < v_rel){
		w = r.width()*h_rel;
		h = r.height()*h_rel;
	}
	else{
		w = r.width()*v_rel;
		h = r.height()*v_rel;
	}
	my_r.x1 += (GetWidth() - w)/2;
	my_r.y1 += (GetHeight() - h)/2;
	my_r.x2 = my_r.x1 + w;
	my_r.y2 = my_r.y1 + h;
	SetWndRect(my_r);
}
