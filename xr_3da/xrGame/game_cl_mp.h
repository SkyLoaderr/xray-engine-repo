#pragma once

#include "game_cl_base.h"
#include "script_export_space.h"
#include "game_cl_mp_snd_messages.h"
#include "../Sound.h"

class CUIChatWnd;
class CUIChatLog;
class CUIGameLog;

struct SND_Message{
	ref_sound	pSound;
	u32			priority;
	u32			SoundID;
	u32			LastStarted;
	bool operator == (u32 ID){return SoundID == ID;}
	void Load(u32 ID, u32 prior, LPCSTR name)
	{
		SoundID = ID;
		priority = prior;
		pSound.create(TRUE, name);
		LastStarted = 0;
	}
	~SND_Message()
	{
		SoundID = 0;
		priority = 0;
		pSound.destroy();
	}
};

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

	DEF_VECTOR(SNDMESSAGES, SND_Message);
	SNDMESSAGES						m_pSndMessages;
	bool							m_bJustRestarted;
	DEF_VECTOR(SNDMESSAGESINPLAY, SND_Message*);
	SNDMESSAGESINPLAY				m_pSndMessagesInPlay;

//	CUIChatWnd*						pChatWnd;
//	CUIChatLog*						pChatLog;
//	CUIGameLog*						pGameLog;

	virtual void			LoadTeamData			(char* TeamName);
	virtual	void			ChatSayTeam				(const shared_str &phrase);
	virtual	void			ChatSayAll				(const shared_str &phrase);
	virtual	void			OnChatMessage			(NET_Packet* P);

	bool								m_bVotingActive;

	virtual void			UpdateMapLocations		() {};
//-----------------------------------------------------------------------------------
	ref_shader				m_EquipmentIconsShader;
	ref_shader				m_KillEventIconsShader;
	ref_shader				m_RadiationIconsShader;
	ref_shader				m_BloodLossIconsShader;
	ref_shader				m_RankIconsShader;

	virtual ref_shader		GetEquipmentIconsShader	();
	virtual ref_shader		GetKillEventIconsShader	();
	virtual ref_shader		GetRadiationIconsShader	();
	virtual ref_shader		GetBloodLossIconsShader	();
	virtual ref_shader		GetRankIconsShader();

	virtual void			OnPlayerKilled			(NET_Packet& P);

	virtual bool			NeedToSendReady_Actor			(int key, game_PlayerState* ps);
	virtual bool			NeedToSendReady_Spectator		(int key, game_PlayerState* ps);

	virtual	void			LoadSndMessage			(LPCSTR caSection, LPCSTR caLine, u32 ID);
	virtual		void				LoadSndMessages				();
	virtual		void				PlaySndMessage			(u32 ID);	
	virtual		void				UpdateSndMessages		();

public:
									game_cl_mp();
	virtual							~game_cl_mp();


	virtual		void				TranslateGameMessage	(u32 msg, NET_Packet& P);
	virtual		void				CommonMessageOut		(LPCSTR msg);

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
	virtual		void				OnPlayerChangeName		(NET_Packet& P);

	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase);	
	virtual		void				net_import_update		(NET_Packet& P);
	virtual		void				net_import_state		(NET_Packet& P);
	virtual		void				OnRankChanged			();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_cl_mp)
#undef script_type_list
#define script_type_list save_type_list(game_cl_mp)
