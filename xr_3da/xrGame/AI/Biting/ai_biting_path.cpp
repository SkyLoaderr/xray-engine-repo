////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../graph_engine.h"
#include "../ai_monsters_misc.h"
#include "../../game_graph.h"
#include "../../game_level_cross_table.h"

// ���������� ������ � ����������� �������� �� ����
void CAI_Biting::SetDirectionLook(bool bReversed)
{
	float yaw,pitch;
	
	// get prev
	CDetailPathManager::direction().getHP(yaw,pitch);

	if (fsimilar(yaw,0.f)) return;

	m_body.target.yaw = -yaw;

	if (bReversed) m_body.target.yaw = angle_normalize(m_body.target.yaw + PI);
	else m_body.target.yaw = angle_normalize(m_body.target.yaw);
}

// ������ ������ ����� ��-������� ���������� ��� ���� (e.g. �������� � ��������� ������ � �.�.)
void CAI_Biting::LookPosition(Fvector to_point, float angular_speed)
{
	// ��-��������� ������ �������� m_body.target.yaw
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());	
	
	// �������� ������ ����������� � ��������� ����� � ��� ������� ����
	float		yaw,p;
	dir.getHP(yaw,p);

	// ���������� ������� ����
	m_body.target.yaw = angle_normalize(-yaw);
}


void CAI_Biting::FaceTarget(const CEntity *entity) 
{
	float yaw, pitch;
	Fvector dir;
	
	dir.sub(entity->Position(), Position());
	dir.getHP(yaw,pitch);
	yaw *= -1;
	yaw = angle_normalize(yaw);
	m_body.target.yaw = yaw;

}

void CAI_Biting::FaceTarget(const Fvector &position) 
{
	float yaw, pitch;
	Fvector dir;

	dir.sub(position, Position());
	dir.getHP(yaw,pitch);
	yaw *= -1;
	yaw = angle_normalize(yaw);
	m_body.target.yaw = yaw;
}


