#pragma once
#include "game_sv_base.h"
#include "game_sv_mp_team.h"

class game_sv_mp :public game_sv_GameState
{
	typedef game_sv_GameState inherited;

protected:
	//список трупов для удаления
	DEF_DEQUE(CORPSE_LIST, u16);

	CORPSE_LIST						m_CorpseList;

	TEAM_DATA_LIST					TeamList;

	virtual		void				SendPlayerKilledMessage	(ClientID id_killer, ClientID id_killed);
	virtual		void				RespawnPlayer			(ClientID id_who, bool NoSpectator);
				void				SpawnPlayer				(ClientID id, LPCSTR N);
	virtual		void				SetSkin					(CSE_Abstract* E, u16 Team, u16 ID);
				bool				GetPosAngleFromActor	(ClientID id, Fvector& Pos, Fvector &Angle);
				TeamStruct*			GetTeamData				(u8 Team);
				void				AllowDeadBodyRemove		(ClientID id, u16 GameID);
				void				SpawnWeapon4Actor		(u16 actorId,  LPCSTR N, u8 Addons );
				void				SpawnWeaponForActor		(u16 actorId,  LPCSTR N, bool isScope, bool isGrenadeLauncher, bool isSilencer);
public:
									game_sv_mp				();
	virtual		void				Create					(shared_str &options);
	virtual		void				OnPlayerConnect			(ClientID id_who);
	virtual		void				OnPlayerDisconnect		(ClientID id_who, LPSTR Name, u16 GameID);
	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_target){return true;};			// TRUE=allow ownership, FALSE=denied
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_target){return true;};			// TRUE=allow ownership, FALSE=denied
	virtual		void				OnPlayerKillPlayer		(ClientID id_killer, ClientID id_killed){SendPlayerKilledMessage(id_killer, id_killed);};

	virtual		void				OnDestroyObject			(u16 eid_who);			

	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);

	virtual		void				OnRoundStart			();												// старт раунда
	virtual		void				OnRoundEnd				(LPCSTR reason);								// конец раунда
	virtual		bool				OnNextMap				();
	virtual		void				OnPrevMap				();

	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );
	virtual		void				Update					();
				void				KillPlayer				(ClientID id_who, u16 GameID);
	virtual		BOOL				CanHaveFriendlyFire		()	{return TRUE;};

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(game_sv_mp)
#undef script_type_list
#define script_type_list save_type_list(game_sv_mp)

