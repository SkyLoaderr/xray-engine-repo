// UICharacterInfo.h:  окошко, с информацией о персонаже
// 
//////////////////////////////////////////////////////////////////////

#include "uiwindow.h"
#include "uistatic.h"
#include "UIFrameWindow.h"
#include "UIListWnd.h"

#include "..\InventoryOwner.h"

#pragma once


class CUICharacterInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUICharacterInfo();
	virtual		~CUICharacterInfo();

	void		Init(int x, int y, int width, int height, const char* xml_name);
	void		InitCharacter(CInventoryOwner* pInvOwner);
	void		InitCharacter(CCharacterInfo* pCharInfo);
//	void		SetRelation(ALife::ERelationType relation);
	
	void		ResetAllStrings();

	CUIStatic	UIIcon;

	CUIStatic	UIName;
	CUIStatic	UIRank, UIRankCaption;
	CUIStatic	UICommunity, UICommunityCaption;
	CUIStatic	UIRelation, UIRelationCaption;
	// Biography
	CUIListWnd	UIBio;

	// Для автоматического выравнивания текста после кепшинов установить этот флаг
	bool		m_bInfoAutoAdjust;
};