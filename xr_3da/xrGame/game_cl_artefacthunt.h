#pragma once
#include "game_cl_teamdeathmatch.h"

class CUIGameAHunt;

class game_cl_ArtefactHunt :public game_cl_TeamDeathmatch
{
	CUIGameAHunt*						m_game_ui;
	typedef game_cl_TeamDeathmatch inherited;
protected:
	ref_sound							pMessageSounds[8];

protected:
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);
	virtual			void				shedule_Update			(u32 dt);

	virtual			BOOL				CanCallBuyMenu			();
	virtual			bool				CanBeReady				();

	virtual			void				OnObjectEnterTeamBase	(u16 player_id, u8 zone_team_id);
	virtual			void				OnObjectLeaveTeamBase	(u16 player_id, u8 zone_team_id);

public:
	u8									artefactsNum;//ah
	u16									artefactBearerID;//ah,ZoneMap
	u8									teamInPossession;//ah,ZoneMap
	u16									artefactID;
	s32									dReinforcementTime;

public :
										game_cl_ArtefactHunt	();
	virtual							~game_cl_ArtefactHunt		();
	virtual			void				Init					();
	virtual			CUIGameCustom*		createGameUI			();
					void				net_import_state		(NET_Packet& P);
	virtual			void				GetMapEntities(xr_vector<SZoneMapEntityData>& dst);
	virtual			char*				getTeamSection			(int Team);
};
