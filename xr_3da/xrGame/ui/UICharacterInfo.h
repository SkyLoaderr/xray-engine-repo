// UICharacterInfo.h:  окошко, с информацией о персонаже
// 
//////////////////////////////////////////////////////////////////////
#pragma once

#include "uiwindow.h"
#include "../alife_space.h"
#include "../character_info_defs.h"

class CInventoryOwner;
class CUIStatic;
class CUIListWnd;
class CCharacterInfo;

class CUICharacterInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUICharacterInfo();
	virtual		~CUICharacterInfo();

	void		Init(float x, float y, float width, float height, const char* xml_name);
	void		InitCharacter(CInventoryOwner* pInvOwner);
	void		InitCharacter(CCharacterInfo* pCharInfo);
	void		SetRelation(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill);
	
	void		ResetAllStrings();

	virtual void	Update				();
			void	UpdateRelation		();
	enum{
		eUIIcon	=	0,
		eUIName,
		eUIRank,
		eUIRankCaption,
		eUICommunity,
		eUICommunityCaption,
		eUIReputation,
		eUIReputationCaption,
		eUIRelation,
		eUIRelationCaption,
		eMaxCaption,
	};
	CUIStatic*	m_icons[eMaxCaption];
	CInventoryOwner* pInvOwner;
/*
	CUIStatic	UIIcon;
	CUIStatic	UIName;
	CUIStatic	UIRank, UIRankCaption;
	CUIStatic	UICommunity, UICommunityCaption;
	CUIStatic	UIReputation, UIReputationCaption;
	CUIStatic	UIRelation, UIRelationCaption;*/
	// Biography
	CUIListWnd*	pUIBio;

	// Для автоматического выравнивания текста после кепшинов установить этот флаг
	bool		m_bInfoAutoAdjust;

	CUIStatic&	UIIcon()		{return *m_icons[eUIIcon];}	
	CUIStatic&	UIName()		{return *m_icons[eUIName];}	
};