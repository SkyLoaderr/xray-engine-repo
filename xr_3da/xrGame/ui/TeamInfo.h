#pragma once

class CTeamInfo {
public:
	static u32			GetTeam1_color();
	static u32			GetTeam2_color();
	static shared_str	GetTeam1_name();
	static shared_str	GetTeam2_name();

protected:
	static u32			team1_color;
	static u32			team2_color;
	static shared_str	team1_name;
	static shared_str	team2_name;

	enum {
		flTeam1_color = 1,
		flTeam2_color = 1<<1,
		flTeam1_name  = 1<<2,
		flTeam2_name  = 1<<3
	};
	static Flags32		flags;
};