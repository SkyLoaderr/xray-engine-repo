////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters.h
//	Created 	: 24.04.2002
//  Modified 	: 24.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for all the monsters
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_MONSTERS__
#define __XRAY_AI_MONSTERS__

const DWORD _FB_hit_RelevantTime	= 10;
const DWORD _FB_sense_RelevantTime	= 10;
const float _FB_look_speed			= PI;
const float _FB_invisible_hscale	= 2.f;

struct SEnemySelected
{
	CEntity*	Enemy;
	bool		bVisible;
	float		fCost;
};

#endif