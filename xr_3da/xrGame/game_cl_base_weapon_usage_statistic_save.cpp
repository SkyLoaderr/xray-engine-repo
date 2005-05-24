#include "stdafx.h"
#include "game_cl_base.h"
#include "Level.h"

#define WUS_IDENT	(('S'<<24)+('U'<<16)+('W'<<8)+' ')
#define WUS_VERSION	1
void		WeaponUsageStatistic::SaveData()
{
	if (OnClient()) return;
	if (!CollectData()) return;
	if (aPlayersStatistic.empty()) return;
	string1024 Name, GameType;
	SYSTEMTIME Time;	
	switch (GameID())
	{
	case GAME_DEATHMATCH: sprintf(GameType, "dm"); break;
	case GAME_TEAMDEATHMATCH: sprintf(GameType, "tdm"); break;
	case GAME_ARTEFACTHUNT: sprintf(GameType, "afh"); break;
	default:
		return;
		break;
	};
	GetLocalTime(&Time);	
	sprintf(Name, "logs\\(%s)_(%s)_%02d.%02d.%02d_%02d.%02d.%02d.wus", *(Level().name()), GameType, Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
	//---------------------------------------------------------
	FILE* SFile = fopen(Name, "wb");
	if (!SFile) return;
	//---------------------------------------------------------
	u32 IDENT = WUS_IDENT;
	fwrite(&IDENT, 4, 1, SFile);
	u32 Ver = WUS_VERSION;
	fwrite(&Ver, 4, 1, SFile);
	//---------------------------------------------------------
	Write(SFile);
	//---------------------------------------------------------
	fclose(SFile);
};

void				WeaponUsageStatistic::Write						(FILE* pFile)
{
	if (!pFile) return;	
	//---------------------------------------------	
	fwrite(&m_dwTotalPlayersAliveTime, 4, 1, pFile);
	fwrite(&m_dwTotalPlayersMoneyRound, 4, 1, pFile);
	fwrite(&m_dwTotalNumRespawns, 4, 1, pFile);	
	//----------------------------------------------
	u32 NumPlayers = aPlayersStatistic.size();
	fwrite(&NumPlayers, 4, 1, pFile);
	//----------------------------------------------
	for (u32 i=0; i<NumPlayers; i++)
	{
		Player_Statistic& PS = aPlayersStatistic[i];
		PS.Write(pFile);
	}
}

void				Player_Statistic::Write						(FILE* pFile)
{
	if (!pFile) return;
	//----------------------------------------------
	fwrite(*PName, xr_strlen(PName)+1, 1, pFile);
	fwrite(&TotalShots, 4, 1, pFile);	
	fwrite(&m_dwTotalAliveTime, 4, 1, pFile);	
	fwrite(&m_dwTotalMoneyRound, 4, 1, pFile);	
	fwrite(&m_dwNumRespawned, 4, 1, pFile);	
	//----------------------------------------------
	u32 NumWeapons = aWeaponStats.size();
	fwrite(&NumWeapons, 4, 1, pFile);
	//----------------------------------------------
	for (u32 i=0; i<aWeaponStats.size(); i++)
	{
		Weapon_Statistic& WS = aWeaponStats[i];
		WS.Write(pFile);
	}
};

void				Weapon_Statistic::Write						(FILE* pFile)
{
	if (!pFile) return;
	//----------------------------------------------
	fwrite(*WName, xr_strlen(WName)+1, 1, pFile);
	fwrite(*InvName, xr_strlen(InvName)+1, 1, pFile);
	//----------------------------------------------
	fwrite(&NumBought, 4, 1, pFile);	
	fwrite(&RoundsFired, 4, 1, pFile);	
	fwrite(&BulletsFired, 4, 1, pFile);	
	fwrite(&HitsScored, 4, 1, pFile);	
	fwrite(&KillsScored, 4, 1, pFile);	
	fwrite(&m_Basket, 4, MAX_BASKET, pFile);
	//----------------------------------------------
	fwrite(&m_dwNumCompleted, 4, 1, pFile);
	for (u32 i=0; i<m_Hits.size(); i++)
	{
		HitData& Hit = m_Hits[i];
		if (!Hit.Completed) continue;
		Hit.Write(pFile);
	};
};

void				HitData::Write						(FILE* pFile)
{
	if (!pFile) return;
	//----------------------------------------------
	fwrite(&Pos0.x, 4, 1, pFile);
	fwrite(&Pos0.y, 4, 1, pFile);
	fwrite(&Pos0.z, 4, 1, pFile);

	fwrite(&Pos1.x, 4, 1, pFile);
	fwrite(&Pos1.y, 4, 1, pFile);
	fwrite(&Pos1.z, 4, 1, pFile);

	fwrite(&BoneID, 2, 1, pFile);
	fwrite(&Deadly, 1, 1, pFile);

	fwrite(*TargetName, xr_strlen(TargetName)+1, 1, pFile);
};