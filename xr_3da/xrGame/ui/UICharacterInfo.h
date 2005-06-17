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
class CUIXml;
class CUICharacterInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUICharacterInfo();
	virtual		~CUICharacterInfo();

	void		Init					(float x, float y, float width, float height, CUIXml* xml_doc);
	void		Init					(float x, float y, float width, float height, const char* xml_name);
	void		InitCharacter			(CInventoryOwner* pInvOwner);
	void		InitCharacter			(CCharacterInfo* pCharInfo);
	void		SetRelation				(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill);
	
	void		ResetAllStrings			();

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
	u32				m_ownerID;
	// Biography
	CUIListWnd*	pUIBio;

	// Для автоматического выравнивания текста после кепшинов установить этот флаг
	bool		m_bInfoAutoAdjust;

	CUIStatic&	UIIcon()		{VERIFY(m_icons[eUIIcon]);return *m_icons[eUIIcon];}	
	CUIStatic&	UIName()		{VERIFY(m_icons[eUIName]);return *m_icons[eUIName];}	
};