// UIStatic.h: класс статического элемента
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uistatic.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIStatic:: CUIStatic()
{
	m_str = " ";

}

 CUIStatic::~ CUIStatic()
{

}



void CUIStatic::Init(LPCSTR tex_name, int x, int y, int width, int height)
{
	
	m_UIStaticItem.Init(tex_name,"hud\\default",x,y,alNone);

	
	CUIWindow::Init(x, y, width, height);
}



//прорисовка
void  CUIStatic::Draw()
{
	RECT rect = GetAbsoluteRect();
	m_UIStaticItem.SetPos(rect.left, rect.top);
	m_UIStaticItem.Render();
			
}

void  CUIStatic::Update()
{
	RECT rect = GetAbsoluteRect();


	GetFont()->SetColor(0xFFEEEEEE);
	GetFont()->Out((float)rect.left, (float)rect.top,  m_str);


}