// UICharacterInfo.h:  окошко, с информацией о персонаже
// 
//////////////////////////////////////////////////////////////////////

#include "uiwindow.h"
#include "uistatic.h"
#include "UIFrameWindow.h"

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
	void InitCharacter(CInventoryOwner* pInvOwner, bool withPrefixes = true);
	void InitCharacter(CCharacterInfo* pCharInfo, bool withPrefixes = true);
	void SetRelation(ALife::ERelationType relation, bool withPrefix = true);
	
	void ResetAllStrings();

	CUIStatic UIIcon;

	CUIStatic UIName;
	CUIStatic UIRank, UIRankCaption;
	CUIStatic UICommunity, UICommunityCaption;
	CUIStatic UIRelation, UIRelationCaption;
};