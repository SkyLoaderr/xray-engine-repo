#pragma once

//#include "gameobject.h"

class CInventory;
class CActor;

class CHolderCustom
{
private:
	CActor*					m_owner;
protected:
	CActor*					Owner				(){return m_owner;}
public:
							CHolderCustom		(){m_owner=0;}
	virtual					~CHolderCustom		(){;}

	virtual void			OnMouseMove			(int x, int y)	= 0;
	virtual void			OnKeyboardPress		(int dik)		= 0;
	virtual void			OnKeyboardRelease	(int dik)		= 0;
	virtual void			OnKeyboardHold		(int dik)		= 0;

	// Inventory for the car
	virtual CInventory*		GetInventory		()				= 0;

	virtual void			cam_Update			(float dt)		= 0;

	virtual bool			Use					(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)=0;
	virtual bool			attach_Actor		(CActor* actor)	= 0;
	virtual void			detach_Actor		()				= 0;

	virtual Fvector			ExitPosition		()				= 0;

	virtual CCameraBase*	Camera				()				= 0;
};
