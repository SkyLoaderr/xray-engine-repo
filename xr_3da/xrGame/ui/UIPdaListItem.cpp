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

	AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0,width, height, "pda_character.xml");
}
void CUIPdaListItem::Update()
{
	inherited::Update();
}

void CUIPdaListItem::Draw()
{
	inherited::Draw();
}

void CUIPdaListItem::InitCharacter(CInventoryOwner* pInvOwner)
{
	UICharacterInfo.InitCharacter(pInvOwner);
}