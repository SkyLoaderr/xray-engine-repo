// hit_immunity.cpp:	класс для тех объектов, которые поддерживают
//						коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hit_immunity.h"


CHitImmunity::CHitImmunity()
{
	m_HitTypeK.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;
}


CHitImmunity::~CHitImmunity()
{
}
void CHitImmunity::InitImmunities(LPCSTR imm_sect,CInifile* ini)
{
	R_ASSERT2	(ini->section_exist(imm_sect), imm_sect);

	m_HitTypeK[ALife::eHitTypeBurn]			= READ_IF_EXISTS(ini,r_float,imm_sect,"burn_immunity",			1.f);
	m_HitTypeK[ALife::eHitTypeStrike]		= READ_IF_EXISTS(ini,r_float,imm_sect,"strike_immunity",		1.f);
	m_HitTypeK[ALife::eHitTypeShock]		= READ_IF_EXISTS(ini,r_float,imm_sect,"shock_immunity",			1.f);
	m_HitTypeK[ALife::eHitTypeWound]		= READ_IF_EXISTS(ini,r_float,imm_sect,"wound_immunity",			1.f);
	m_HitTypeK[ALife::eHitTypeRadiation]	= READ_IF_EXISTS(ini,r_float,imm_sect,"radiation_immunity",		1.f);
	m_HitTypeK[ALife::eHitTypeTelepatic]	= READ_IF_EXISTS(ini,r_float,imm_sect,"telepatic_immunity",		1.f);
	m_HitTypeK[ALife::eHitTypeChemicalBurn] = READ_IF_EXISTS(ini,r_float,imm_sect,"chemical_burn_immunity",	1.f);
	m_HitTypeK[ALife::eHitTypeExplosion]	= READ_IF_EXISTS(ini,r_float,imm_sect,"explosion_immunity",		1.f);
	m_HitTypeK[ALife::eHitTypeFireWound]	= READ_IF_EXISTS(ini,r_float,imm_sect,"fire_wound_immunity",	1.f);
}

void CHitImmunity::LoadImmunities (LPCSTR section, LPCSTR line)
{
	InitImmunities	(READ_IF_EXISTS(pSettings,r_string,section,line,section),pSettings);
}

void CHitImmunity::LoadImmunities(LPCSTR section)
{
	LoadImmunities	(section, "immunities_sect");
}


float CHitImmunity::AffectHit (float power, ALife::EHitType hit_type)
{
	return power*m_HitTypeK[hit_type];
}