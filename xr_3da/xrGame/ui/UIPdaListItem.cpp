//////////////////////////////////////////////////////////////////////
// UIPdaListItem.cpp: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIPdaListItem.h"
#include "../actor.h"
#include "../level.h"


#define PDA_CONTACT_CHAR "pda_character.xml"

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
	UICharacterInfo.Init(0,0,width, height, PDA_CONTACT_CHAR);
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
	VERIFY(pInvOwner);
	UICharacterInfo.InitCharacter(pInvOwner);
	
	CActor* pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
		CEntityAlive* ContactEA = dynamic_cast<CEntityAlive*>(pInvOwner);
		UICharacterInfo.SetRelation(ContactEA->tfGetRelationType(pActor));
	}

}