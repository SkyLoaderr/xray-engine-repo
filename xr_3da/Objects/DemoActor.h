#ifndef __DEMO_ACTOR_H__
#define __DEMO_ACTOR_H__

#include "entity.h"

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CSoundStream;
class CDummyObject;
class CObjectAnimator;

class CDemoActor:		public CEntity
{
	CObjectAnimator*	animator;
	Fvector				start_position;

	CSoundStream*		music;
	char*				attached;

	CCameraBase*		camera;
	void				UpdateCamera	( );

	float				time2start,time2start_ltx;
private:
	typedef CEntity		inherited;
public:
						CDemoActor		();
	virtual				~CDemoActor		();

	virtual void		Load			(CInifile* ini, const char * section);

	void				PlayDemo		(const char* name);
	void				StopDemo		();

	// Update
	virtual void		Update			( DWORD DT );

	// virtual
	virtual void		HitSignal		(int HitAmount,Fvector& vLocalDir,CEntity* E) {};
	virtual void		Die				(){};
	virtual void		g_fireParams	(Fvector& P, Fvector& D);
};

#endif //__DEMO_ACTOR_H__
