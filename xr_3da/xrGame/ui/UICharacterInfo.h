// UICharacterInfo.h:  окошко, с информацией о персонаже
// 
//////////////////////////////////////////////////////////////////////

#include "uiwindow.h"
#include "uistatic.h"

#include "..\InventoryOwner.h"

#pragma once


class CUICharacterInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUICharacterInfo();
	virtual ~CUICharacterInfo();

	void Init(int x, int y, int width, int height, const char* xml_name);
	void InitCharacter(CInventoryOwner* pInvOwner);
	void SetRelation(ALife::ERelationType relation);
	
	void ResetAllStrings();

	CUIStatic UIIcon;
	CUIStatic UIName;
	CUIStatic UIRank;
	CUIStatic UICommunity;
	CUIStatic UIText;
	CUIStatic UIRelation;
};