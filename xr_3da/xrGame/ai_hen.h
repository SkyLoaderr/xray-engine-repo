////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.h
//	Created 	: 05.04.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN__
#define __XRAY_AI_HEN__

#pragma once

#include "CustomMonster.h"

class CAI_Hen : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};
	typedef	CCustomMonster inherited;
	friend	class AI::State;
protected:
	// media
	sound3D				sndHit[SND_HIT_COUNT];
	sound3D				sndDie[SND_DIE_COUNT];
public:
	virtual void		Update					(DWORD DT);
	virtual void		HitSignal				(int amount, Fvector& vLocalDir, CEntity* who);
	virtual void		Die						();
	virtual void		Load					( CInifile* ini, const char* section );

	CAI_Hen			();
	virtual ~CAI_Hen	();
};

#endif