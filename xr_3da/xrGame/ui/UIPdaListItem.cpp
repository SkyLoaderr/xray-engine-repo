//////////////////////////////////////////////////////////////////////
// UIPdaListItem.cpp: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIPdaListItem.h"


CUIPdaListItem::CUIPdaListItem(void)
{
}

CUIPdaListItem::~CUIPdaListItem(void)
{
}

void CUIPdaListItem::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);

//	AttachChild(&m_UIStaticIcon);
//	m_UIStaticIcon.Init(0,0,100,100);
//	m_UIStaticIcon.SetText("a texty");


}
void CUIPdaListItem::Update()
{
	inherited::Update();
}

void CUIPdaListItem::Draw()
{
	inherited::Draw();
}