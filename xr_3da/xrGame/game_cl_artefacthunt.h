#pragma once
#include "game_cl_teamdeathmatch.h"

class game_cl_ArtefactHunt :public game_cl_TeamDeathmatch
{
	typedef game_cl_TeamDeathmatch inherited;
protected:
	ref_sound							pMessageSounds[8];

protected:
	virtual			void				TranslateGameMessage	(u32 msg, NET_Packet& P);

public:
	u8									artefactsNum;//ah
	u16									artefactBearerID;//ah,ZoneMap
	u8									teamInPossession;//ah,ZoneMap
	u16									artefactID;

public :
										game_cl_ArtefactHunt	();
		virtual							~game_cl_ArtefactHunt	();
		virtual		CUIGameCustom*		createGameUI			();
					void				net_import_state		(NET_Packet& P);
	virtual			void				GetMapEntities(xr_vector<SZoneMapEntityData>& dst);
};
