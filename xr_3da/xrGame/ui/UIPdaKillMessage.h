// File:		UIPdaKillMessage.h
// Description:	HUD message about player death. Implementation of visual behavior
// Created:		10.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
// 
// Copyright 2005 GSC GameWorld

#include "KillMessageStruct.h"
#include "UIPdaMsgListItem.h"

class CUIPdaKillMessage : public CUIPdaMsgListItem{
typedef CUIListItem inherited;
public:
	CUIPdaKillMessage();
	CUIPdaKillMessage(int iDelay);
	virtual ~CUIPdaKillMessage();

	virtual void Init(int x, int y, int width, int height);
			void Init(KillMessageStruct& msg);
	virtual void SetFont(CGameFont* pFont);

protected:
			int InitText(CUIStatic& refStatic, int x, PlayerInfo& info);
			int InitIcon(CUIStatic& refStatic, int x, IconInfo& info);

    CUIStatic	m_victim_name;
	CUIStatic	m_initiator;
	CUIStatic	m_killer_name;
	CUIStatic	m_ext_info;
};
