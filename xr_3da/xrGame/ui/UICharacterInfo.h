// UICharacterInfo.h:  ������, � ����������� � ���������
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

	void		Init(float x, float y, float width, float height, const char* xml_name);
	void		InitCharacter(CInventoryOwner* pInvOwner);
	void		InitCharacter(CCharacterInfo* pCharInfo);
	void		SetRelation(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill);
	
	void		ResetAllStrings();

	virtual void	Update				();

	CUIStatic	UIIcon;
	CInventoryOwner* pInvOwner;
	CUIStatic	UIName;
	CUIStatic	UIRank, UIRankCaption;
	CUIStatic	UICommunity, UICommunityCaption;
	CUIStatic	UIReputation, UIReputationCaption;
	CUIStatic	UIRelation, UIRelationCaption;
	// Biography
	CUIListWnd	UIBio;

	// ��� ��������������� ������������ ������ ����� �������� ���������� ���� ����
	bool		m_bInfoAutoAdjust;
};