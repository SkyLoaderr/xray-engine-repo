#pragma once

#include "game_sv_base.h"

class	game_sv_Deathmatch			: public game_sv_GameState
{
protected:
	struct		RPointData
	{
		u32		PointID;
		float	MinEnemyDist;

		RPointData(u32 ID, float Dist):
		PointID(ID),
		MinEnemyDist(Dist)
		{
		};

		bool operator<(const RPointData &x)
		{
			return MinEnemyDist < x.MinEnemyDist;
		};
	};
protected:
	void							AllowDeadBodyRemove		(u32 id);
	void							SpawnActor				(u32 id, LPCSTR N);
	bool							GetPosAngleFromActor	(u32 id, Fvector& Pos, Fvector &Angle);
	void							SpawnItem4Actor			(u32 actorId, LPCSTR N);
	void							KillPlayer				(u32 id_who);
public:
	virtual		void				Create					(LPSTR &options);

	// Events
	virtual		void				OnRoundStart			();										// ����� ������

	virtual		void				OnTeamScore				(u32 /**team/**/)						{};		// ������� ��������
	virtual		void				OnTeamsInDraw			()								{};		// �����
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P) {}; //����� ������� Hit

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerDisconnect		(u32 id_who);
	virtual		void				OnPlayerReady			(u32 id_who);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);
	virtual		void				OnPlayerWantsDie		(u32 id_who);

	virtual		void				OnPlayerBuyFinished		(u32 id_who, NET_Packet& P);
	
	virtual		void				OnFraglimitExceed		();
	virtual		void				OnTimelimitExceed		();

	// Main
	virtual		void				Update					();
				BOOL				AllPlayers_Ready		();

	virtual		void				assign_RP				(CSE_Abstract* E);
	virtual		u32					RP_2_Use				(CSE_Abstract* E);

	virtual		void				SetSkin					(CSE_Abstract* E, u16 Team, u16 ID)	{};

	virtual		void				ClearPlayerState		(game_PlayerState* ps);
};
