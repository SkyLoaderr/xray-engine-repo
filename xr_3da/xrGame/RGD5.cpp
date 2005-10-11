#include "stdafx.h"
#include "rgd5.h"

CRGD5::CRGD5(void)
{
	m_flags.set				(Fbelt, TRUE);
	m_weight				= .1f;
	m_slot					= GRENADE_SLOT;
}

CRGD5::~CRGD5(void)
{
}
