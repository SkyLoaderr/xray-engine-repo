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
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;

	R_ASSERT2(ini->section_exist(imm_sect), imm_sect);

	if(ini->line_exist(imm_sect,"burn_immunity"))
		m_HitTypeK[ALife::eHitTypeBurn]			= ini->r_float(imm_sect,"burn_immunity");
	if(ini->line_exist(imm_sect,"burn_immunity"))
		m_HitTypeK[ALife::eHitTypeStrike]		= ini->r_float(imm_sect,"strike_immunity");
	if(ini->line_exist(imm_sect,"shock_immunity"))
		m_HitTypeK[ALife::eHitTypeShock]		= ini->r_float(imm_sect,"shock_immunity");
	if(ini->line_exist(imm_sect,"wound_immunity"))
		m_HitTypeK[ALife::eHitTypeWound]		= ini->r_float(imm_sect,"wound_immunity");
	if(ini->line_exist(imm_sect,"radiation_immunity"))
		m_HitTypeK[ALife::eHitTypeRadiation]	= ini->r_float(imm_sect,"radiation_immunity");
	if(ini->line_exist(imm_sect,"telepatic_immunity"))
		m_HitTypeK[ALife::eHitTypeTelepatic]	= ini->r_float(imm_sect,"telepatic_immunity");
	if(ini->line_exist(imm_sect,"chemical_burn_immunity"))
		m_HitTypeK[ALife::eHitTypeChemicalBurn] = ini->r_float(imm_sect,"chemical_burn_immunity");
	if(ini->line_exist(imm_sect,"fire_wound_immunity"))
		m_HitTypeK[ALife::eHitTypeFireWound]	= ini->r_float(imm_sect,"fire_wound_immunity");
	if(ini->line_exist(imm_sect,"explosion_immunity"))
		m_HitTypeK[ALife::eHitTypeExplosion]	= ini->r_float(imm_sect,"explosion_immunity");
}

void CHitImmunity::LoadImmunities (LPCSTR section, LPCSTR line)
{
	LPCSTR imm_sect = section;

	if(pSettings->line_exist(section, line))
	{
		imm_sect = pSettings->r_string(section, line);
	}

	InitImmunities (imm_sect,pSettings);
}

void CHitImmunity::LoadImmunities(LPCSTR section)
{
	LoadImmunities(section, "immunities_sect");
}


float CHitImmunity::AffectHit (float power, ALife::EHitType hit_type)
{
	return power*m_HitTypeK[hit_type];
}