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

void CHitImmunity::LoadImmunities(LPCSTR section)
{
	LPCSTR imm_sect = section;
	
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;

	if(pSettings->line_exist(section, "immunities_sect"))
	{
		imm_sect = pSettings->r_string(section, "immunities_sect");
	}

	if(pSettings->line_exist(imm_sect,"burn_immunity"))
		m_HitTypeK[ALife::eHitTypeBurn]			= pSettings->r_float(imm_sect,"burn_immunity");
	if(pSettings->line_exist(imm_sect,"burn_immunity"))
		m_HitTypeK[ALife::eHitTypeStrike]		= pSettings->r_float(imm_sect,"strike_immunity");
	if(pSettings->line_exist(imm_sect,"shock_immunity"))
		m_HitTypeK[ALife::eHitTypeShock]		= pSettings->r_float(imm_sect,"shock_immunity");
	if(pSettings->line_exist(imm_sect,"wound_immunity"))
		m_HitTypeK[ALife::eHitTypeWound]		= pSettings->r_float(imm_sect,"wound_immunity");
	if(pSettings->line_exist(imm_sect,"radiation_immunity"))
		m_HitTypeK[ALife::eHitTypeRadiation]	= pSettings->r_float(imm_sect,"radiation_immunity");
	if(pSettings->line_exist(imm_sect,"telepatic_immunity"))
		m_HitTypeK[ALife::eHitTypeTelepatic]	= pSettings->r_float(imm_sect,"telepatic_immunity");
	if(pSettings->line_exist(imm_sect,"chemical_burn_immunity"))
		m_HitTypeK[ALife::eHitTypeChemicalBurn] = pSettings->r_float(imm_sect,"chemical_burn_immunity");
	if(pSettings->line_exist(imm_sect,"fire_wound_immunity"))
		m_HitTypeK[ALife::eHitTypeFireWound]	= pSettings->r_float(imm_sect,"fire_wound_immunity");
	if(pSettings->line_exist(imm_sect,"explosion_immunity"))
		m_HitTypeK[ALife::eHitTypeExplosion]	= pSettings->r_float(imm_sect,"explosion_immunity");
}

