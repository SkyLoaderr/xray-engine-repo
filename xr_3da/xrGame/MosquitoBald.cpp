#include "stdafx.h"
#include "mosquitobald.h"
#include "hudmanager.h"
#include "ParticlesObject.h"


CMosquitoBald::CMosquitoBald(void) 
{
	m_dwDeltaTime = 0;
	m_fHitImpulseScale = 1.f;
}

CMosquitoBald::~CMosquitoBald(void) 
{
}

void CMosquitoBald::Load(LPCSTR section) 
{
	inherited::Load(section);
	m_pHitEffect = pSettings->r_string(section,"hit_effect");
}


void CMosquitoBald::Postprocess(f32 /**val/**/) 
{
}
