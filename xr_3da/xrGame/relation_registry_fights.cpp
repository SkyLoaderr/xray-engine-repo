//////////////////////////////////////////////////////////////////////////
// relation_registry_fights.cpp:	������ ��� �������� ������ �� ��������� ��������� � 
//									������ ����������
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "relation_registry.h"



//////////////////////////////////////////////////////////////////////////

RELATION_REGISTRY::FIGHT_DATA::FIGHT_DATA			()
{
	attacker = defender = 0xffff;
	total_hit = 0;
	time = 0;
}

//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::FightRegister (u16 attacker, u16 defender, float hit_amount)
{
	UpdateFightRegister();

	FIGHT_VECTOR& fights = fight_registry();
	for(FIGHT_VECTOR_IT it = fights.begin(); it != fights.end(); it++)
	{
		FIGHT_DATA& fight_data = *it;
		if(attacker == fight_data.attacker && defender == fight_data.defender)
		{
			fight_data.time = Device.dwTimeGlobal;
			fight_data.total_hit += hit_amount;
			break;
		}
	}

	if(it == fights.end())
	{
		FIGHT_DATA fight_data;
		fight_data.attacker = attacker;
		fight_data.defender = defender;
		fight_data.total_hit = hit_amount;
		fight_data.time = Device.dwTimeGlobal;
		fights.push_back(fight_data);
	}
}

RELATION_REGISTRY::FIGHT_DATA* RELATION_REGISTRY::FindFight(u16 attacker)
{
	FIGHT_VECTOR& fights = fight_registry();
	for(FIGHT_VECTOR_IT it = fights.begin(); it != fights.end(); it++)
	{
		FIGHT_DATA& fight_data = *it;
		if(attacker == fight_data.attacker)
		{
			return &fight_data;
		}
	}

	return NULL;
}


bool fight_time_pred(RELATION_REGISTRY::FIGHT_DATA& fight_data)
{
	//(c) ����� ������� ��� ����� ������ ������ (����� ������� ������������)
	static u32 fight_remember_time	= u32(1000.f * pSettings->r_float(ACTIONS_POINTS_SECT, "fight_remember_time"));	

	u32 time_delta =  Device.dwTimeGlobal - fight_data.time;
	if( time_delta > fight_remember_time)
		return true;

	return false;
}

void RELATION_REGISTRY::UpdateFightRegister ()
{
	FIGHT_VECTOR& fights = fight_registry();
	FIGHT_VECTOR_IT it = std::remove_if(fights.begin(), fights.end(), fight_time_pred);
	fights.erase(it, fights.end());
}