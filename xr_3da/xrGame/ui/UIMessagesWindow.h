// File:		UIMessagesWindow.h
// Description:	Window with MP chat and Game Log ( with PDA messages in single and Kill Messages in MP)
// Created:		22.04.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UIWindow.h"
#include "KillMessageStruct.h"
#include "../pda_space.h"
#include "../InfoPortionDefs.h"

class CUIGameLog;
class CUIChatLog;
class CUIChatWnd;
class game_cl_GameState;
class CUIPdaMsgListItem;
class CUIListWnd;

class CUIMessagesWindow : public CUIWindow {
public:
	CUIMessagesWindow();
	~CUIMessagesWindow();

	void DrawPdaMessages();

	void AddPdaMessage(CInventoryOwner* pSender, EPdaMsg msg, INFO_ID info_id);
	void AddPdaMessage(LPCSTR message, int iId, int iDelay);
	void AddIconedPdaMessage(LPCSTR textureName, Frect originalRect, LPCSTR message, int iId, int iDelay);
	void AddPersonalPdaMessage(CInventoryOwner* pSender, LPCSTR message, int iId, int iDelay);
	bool SetDelayForPdaMessage(int iValue, int iDelay);

	void AddLogMessage(const shared_str& msg);
	void AddLogMessage(KillMessageStruct& msg);
	void AddChatMessage(shared_str msg, shared_str author);
	void SetChatOwner(game_cl_GameState* owner);
	CUIChatWnd* GetChatWnd() {return m_pChatWnd;}

	virtual void Update();


protected:
CUIPdaMsgListItem* AddMessageToList(LPCSTR message, CUIListWnd* pListWnd, int iId, int iDelay);
	
	virtual void Init(float x, float y, float width, float height);


	CUIChatLog*	m_pChatLog;
	CUIChatWnd*	m_pChatWnd;
	CUIGameLog*	m_pGameLog;
	CUIGameLog*	m_pGameLog2;
};