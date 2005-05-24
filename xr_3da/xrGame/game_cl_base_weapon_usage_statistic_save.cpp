#include "stdafx.h"
#include "game_cl_base.h"
#include "Level.h"

void		WeaponUsageStatistic::SaveData()
{
	if (!CollectData()) return;
	if (aPlayersStatistic.empty()) return;
	string1024 Name, NameX;
	SYSTEMTIME Time;
	GetSystemTime(&Time);
	sprintf(Name, "logs\\x_ray-%s-%02d-%02d-%d.wus", *(Level().name()), Time.wDay, Time.wMonth, Time.wYear);
};