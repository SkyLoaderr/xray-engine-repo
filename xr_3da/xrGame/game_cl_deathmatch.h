#pragma once
#include "game_cl_mp.h"

class CUIBuyWeaponWnd;
class CUISkinSelectorWnd;
class CUIDialogWnd;
class CUIGameDM;
class CUIPdaWnd;
class CUIInventoryWnd;
class CUIMapDesc;

//void OnBuySpawn(CUIWindow* pWnd, void* p);

class game_cl_Deathmatch :public game_cl_mp
{
typedef game_cl_mp inherited;
/*
protected:
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);
*/
	CUIGameDM*						m_game_ui;
	shared_str						Actor_Spawn_Effect;
public :
									game_cl_Deathmatch();
	virtual							~game_cl_Deathmatch();
	s32								fraglimit; //dm,tdm,ah
	s32								timelimit; //dm
	u32								damageblocklimit;//dm,tdm
	bool							m_bDamageBlockIndicators;
	xr_vector<game_TeamState>		teams;//dm,tdm,ah
	u32								m_u32ForceRespawn;

	u32								m_cl_dwWarmUp_Time;
	string64						WinnerName;
	
	virtual		CUIGameCustom*		createGameUI			();
	virtual		void				net_import_state	(NET_Packet& P);
	virtual		void				Init					();
	virtual		CUIInventoryWnd*	GetInventoryWnd			() {return pInventoryMenu;};

	virtual		void				LoadSndMessages				();

// from UIGameDM
protected:
//	DEF_VECTOR						(TEAMS_LIST, xr_string);
	DEF_VECTOR						(PRESET_ITEMS, s16);

//	TEAMS_LIST						m_aTeamSections;
	PRESET_ITEMS					PresetItemsTeam0;
	PRESET_ITEMS*					pCurPresetItems;
	PRESET_ITEMS					PlayerDefItems;

	CUIBuyWeaponWnd*				pBuyMenuTeam0;
	CUIBuyWeaponWnd*				pCurBuyMenu;

	CUISkinSelectorWnd*				pSkinMenuTeam0;
	CUISkinSelectorWnd*				pCurSkinMenu;
	CUIInventoryWnd*				pInventoryMenu;

	CUIMapDesc*						pMapDesc;
	//-------------------------------------------------
	CUIPdaWnd*						pPdaMenu;
	//-------------------------------------------------	
	BOOL							m_bFirstRun;
	BOOL							m_bMenuCalledFromReady;
	BOOL							m_bSkinSelected;
	
	BOOL							m_bBuyEnabled;
	s32								m_iCurrentPlayersMoney;

	u32								m_dwVoteEndTime;

			void					CheckItem				(PIItem pItem, PRESET_ITEMS* pPresetItems);

			void					ClearBuyMenu			();
			CUIBuyWeaponWnd*		InitBuyMenu				(LPCSTR BasePriceSection, s16 Team);
			CUISkinSelectorWnd*		InitSkinMenu			(s16 Team = -1);
			void					SetBuyMenuItems			();
	virtual bool					CanBeReady				();
	virtual BOOL					CanCallBuyMenu			();
	virtual BOOL					CanCallSkinMenu			();
	virtual	BOOL					CanCallInventoryMenu	();

			void					Check_Invincible_Players ();

	virtual		void				shedule_Update			(u32 dt);
	virtual		bool				OnKeyboardPress			(int key);
	virtual		bool				OnKeyboardRelease		(int key);

	virtual		void				LoadTeamDefaultPresetItems	(LPCSTR caSection, CUIBuyWeaponWnd* pBuyMenu, PRESET_ITEMS* pPresetItems);
	virtual		void				LoadPlayerDefItems			(char* TeamName, CUIBuyWeaponWnd* pBuyMenu);
	virtual		void				LoadDefItemsForRank			(CUIBuyWeaponWnd* pBuyMenu);
	virtual		void				ChangeItemsCosts			(CUIBuyWeaponWnd* pBuyMenu);
	virtual		s16					GetBuyMenuItemIndex			(u8 SlotID, u8 ItemID);

	virtual		void				ConvertTime2String		(string64* str, u32 Time);
	virtual		int					GetPlayersPlace			(game_PlayerState* ps);

	virtual		void				PlayParticleEffect		(LPCSTR EffName, Fvector& pos);

public:
	virtual s16						ModifyTeam				(s16 Team)	{return Team;};

	virtual		char*				getTeamSection			(int Team);
	virtual	void					SetCurrentBuyMenu		();
	virtual	void					SetCurrentSkinMenu		()	{pCurSkinMenu = pSkinMenuTeam0; };

	virtual	void					OnSpectatorSelect		();

	virtual	void					OnMapInfoAccept			();
	virtual		void				OnSkinMenuBack			();
	virtual void					OnBuyMenu_Ok			();
	virtual	void					OnBuyMenu_DefaultItems	();
	virtual void					OnSkinMenu_Ok			();
	virtual		void				OnSkinMenu_Cancel		();

	virtual		void				OnGameMenuRespond_ChangeSkin	(NET_Packet& P);

	virtual CUIDialogWnd*			GetBuyWnd				()	{ return (CUIDialogWnd*) pCurBuyMenu; };
	virtual CUIDialogWnd*			GetSkinWnd				()	{ return (CUIDialogWnd*) pCurSkinMenu; };

	virtual		void				OnVoteStart				(NET_Packet& P);
	virtual		void				OnVoteStop				(NET_Packet& P);
	virtual		void				OnVoteEnd				(NET_Packet& P);

	virtual		void				GetMapEntities			(xr_vector<SZoneMapEntityData>& dst);

	virtual		void				OnRender				();
	virtual		bool				IsEnemy					(game_PlayerState* ps);
	virtual		bool				IsEnemy					(CEntityAlive* ea1, CEntityAlive* ea2);

	virtual		void				OnSpawn					(CObject* pObj);
	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase);	
	virtual		void				OnRankChanged			();
	virtual		void				OnTeamChanged			();
	virtual		void				OnMoneyChanged			();

	virtual		void				OnSwitchPhase_InProgress();

	virtual		u8					GetTeamCount			() { return 1; };
	
	virtual		void				OnPlayerFlagsChanged	(game_PlayerState* ps);
};

IC bool	DM_Compare_Players		(LPVOID v1, LPVOID v2);