#include "stdafx.h"
#include "psy_aura.h"

CPsyAura::CPsyAura()
{
	object		= 0;
	m_power		= 1.0f;
	m_active	= false;
}

CPsyAura::~CPsyAura()
{
}

void CPsyAura::init_external(CObject *obj)
{
	object = obj;
}	




