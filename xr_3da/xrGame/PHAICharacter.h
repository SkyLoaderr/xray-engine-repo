#pragma once
#include "PHSimpleCharacter.h"
class CPHAICharacter : public CPHSimpleCharacter
{
	typedef CPHSimpleCharacter	inherited;

	Fvector m_vDesiredPosition;
public:
	virtual		void		InitContact							(dContact* c)		;
	virtual		void		SetPosition							(Fvector pos);
	virtual		void		SetDesiredPosition					(const Fvector& pos)									{m_vDesiredPosition.set(pos);}
	virtual		void		GetDesiredPosition					(Fvector& dpos)											{dpos.set(m_vDesiredPosition);}
	virtual		void		BringToDesired						(float time,float velocity,float force=1.f)							;
	virtual		bool		TryPosition							(Fvector pos)											;

#ifdef DEBUG
	virtual		void		OnRender							()														;
#endif

};