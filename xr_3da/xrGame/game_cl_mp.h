#pragma once

#include "game_cl_base.h"
#include "script_export_space.h"

class CUIChatWnd;
class CUIChatLog;

struct cl_TeamStruct
{
	shared_str			caSection;		// имя секции комманды
	//-----------------------------------
	ref_shader			IndicatorShader;
	ref_shader			InvincibleShader;

	Fvector				IndicatorPos;
	float				Indicator_r1;
	float				Indicator_r2;
};

DEF_DEQUE(CL_TEAM_DATA_LIST, cl_TeamStruct);

class game_cl_mp :public game_cl_GameState
{
	typedef game_cl_GameState	inherited;
protected:

	CL_TEAM_DATA_LIST				TeamList;

	CUIChatWnd*						pChatWnd;
	CUIChatLog*						pChatLog;

	virtual void			LoadTeamData			(char* TeamName);
	virtual	void			ChatSayTeam				(const shared_str &phrase);
	virtual	void			ChatSayAll				(const shared_str &phrase);
	virtual	void			OnChatMessage			(NET_Packet* P);

	bool								m_bVotingActive;

public:
public :
									game_cl_mp();
	virtual							~game_cl_mp();

	virtual		void				TranslateGameMessage	(u32 msg, NET_Packet& P);
	virtual		bool				OnKeyboardPress			(int key);
	virtual		bool				OnKeyboardRelease		(int key);

	virtual		bool				CanBeReady				();
	virtual		CUIGameCustom*		createGameUI			();
	virtual		void				shedule_Update			(u32 dt);

	virtual		bool				IsVotingActive			()	{ return m_bVotingActive; };
	virtual		void				SetVotingActive			( bool Active )	{ m_bVotingActive = Active; };
	virtual		void				SendStartVoteMessage	(LPCSTR args);
	virtual		void				SendVoteYesMessage		();
	virtual		void				SendVoteNoMessage		();
	virtual		void				OnVoteStart				(NET_Packet& P);
	virtual		void				OnVoteStop				(NET_Packet& P);
	virtual		void				OnVoteEnd				(NET_Packet& P);

	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase);	

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_cl_mp)
#undef script_type_list
#define script_type_list save_type_list(game_cl_mp)
