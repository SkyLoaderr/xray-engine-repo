#pragma once

#include "game_cl_base.h"
#include "script_export_space.h"

class CUIChatWnd;

class game_cl_mp :public game_cl_GameState
{
	typedef game_cl_GameState	inherited;
protected:
	CUIChatWnd*						pChatWnd;

	void					ChatSayTeam				(const ref_str &phrase);
	void					ChatSayAll				(const ref_str &phrase);

public:
public :
									game_cl_mp();
	virtual							~game_cl_mp();

		virtual		void				TranslateGameMessage	(u32 msg, NET_Packet& P);
		virtual		bool				OnKeyboardPress			(int key);
		virtual		bool				OnKeyboardRelease		(int key);

		virtual		bool				CanBeReady				();

		virtual		CUIGameCustom*		createGameUI			();
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(game_cl_mp)
#undef script_type_list
#define script_type_list save_type_list(game_cl_mp)
