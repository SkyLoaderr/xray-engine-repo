#pragma once

IC	CMonsterSquadManager &monster_squad()
{
	static CMonsterSquadManager  monster_squad_man;
	return						(monster_squad_man);
}