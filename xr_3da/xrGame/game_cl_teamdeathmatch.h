#pragma once
#include "game_cl_deathmatch.h"

class CUISpawnWnd;
class CUIGameTDM;

class game_cl_TeamDeathmatch :public game_cl_Deathmatch
{
typedef game_cl_Deathmatch inherited;
	CUIGameTDM*							m_game_ui;
protected:
	virtual			void				shedule_Update			(u32 dt);
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);
public :
										game_cl_TeamDeathmatch	();
	virtual								~game_cl_TeamDeathmatch	();
	virtual			void				Init					();
	virtual			CUIGameCustom*		createGameUI			();
	virtual			void				GetMapEntities			(xr_vector<SZoneMapEntityData>& dst);

	virtual			void				OnTeamSelect			(int Result);
	virtual			char*				getTeamSection			(int Team);

//from UIGameTDM
protected:
	CUISpawnWnd*		pUITeamSelectWnd;

	CUIBuyWeaponWnd*	pBuyMenuTeam1;
	CUIBuyWeaponWnd*	pBuyMenuTeam2;

	CUISkinSelectorWnd*	pSkinMenuTeam1;
	CUISkinSelectorWnd*	pSkinMenuTeam2;

	PRESET_ITEMS		PresetItemsTeam1;
	PRESET_ITEMS		PresetItemsTeam2;

	BOOL				m_bTeamSelected;

	virtual s16			ModifyTeam			(s16 Team)	{return Team-1;};

	virtual bool		CanBeReady			();
	virtual	void		SetCurrentBuyMenu	();
	virtual	void		SetCurrentSkinMenu	();

	virtual	bool		OnKeyboardPress		(int key);
};
