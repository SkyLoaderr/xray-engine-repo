// Wound.cpp: класс описания раны
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "wound.h"


CWound::CWound(u16 bone_num)
{
	m_Wounds.resize	(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
	{
		m_Wounds[i] = 0.f;
	}

	m_iBoneNum = bone_num;
	m_iParticleBoneNum = BI_NONE;
}
CWound::~CWound(void)
{
}

float CWound::TotalSize()
{
	float total_size = 0.f;
	for(int i=0; i<ALife::eHitTypeMax; i++)
	{
		total_size += m_Wounds[i];
	}
	return total_size;
}

float CWound::TypeSize(ALife::EHitType hit_type)
{
	return m_Wounds[hit_type];
}

void CWound::AddHit(float hit_power, ALife::EHitType hit_type)
{
	m_Wounds[hit_type] += hit_power;
}
void CWound::Incarnation	(float percent)
{
	float total_size = TotalSize();

	//заживить все раны пропорционально их размеру
	for(int i=0; i<ALife::eHitTypeMax; i++)
	{
		m_Wounds[i] -= percent*m_Wounds[i]/total_size;
		if(m_Wounds[i]<0) m_Wounds[i] = 0;
	}
}