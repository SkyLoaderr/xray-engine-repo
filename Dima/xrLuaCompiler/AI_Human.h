// AI_Human.h: interface for the CAI_Human class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_HUMAN_H__0B3F315C_F8BC_41ED_88BF_F0841CA6B869__INCLUDED_)
#define AFX_AI_HUMAN_H__0B3F315C_F8BC_41ED_88BF_F0841CA6B869__INCLUDED_
#pragma once

#include "CustomMonster.h"

class CAI_Human : public CCustomMonster  
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

	CAI_Human			();
	virtual ~CAI_Human	();
};

#endif // !defined(AFX_AI_HUMAN_H__0B3F315C_F8BC_41ED_88BF_F0841CA6B869__INCLUDED_)
