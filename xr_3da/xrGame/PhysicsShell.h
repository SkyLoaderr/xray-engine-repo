#pragma once

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
// 
class CPhysicsElement
{
public:
	Fmatrix					mXFORM;					// In 'shell' space
	Fmatrix					mDesired;				// In 'shell' space
	BOOL					bDesiredUse;			// Desired matrix valid or no
	float					fDesiredStrength;		// Desire strength, [0..1]%
public:
	virtual	void			add_Sphere				(Fsphere&	V)			= 0;
	virtual	void			add_Box					(Fobb&		V)			= 0;

	virtual void			Update					()						= 0;
};

// ABSTRACT: 
class CPhysicsShell
{
public:
	BOOL					bActive;
	Fmatrix					mXFORM;
public:
	virtual	void			add_Element				(Fvector&	C, float	R)					= 0;

	virtual void			Create					()											= 0;
	virtual void			Destroy					()											= 0;

	virtual void			Update					()											= 0;

	virtual void			Activate				()											= 0;
	virtual void			Deactivate				()											= 0;

	virtual void			applyForce				(Fvector& dir, float val)					= 0;
	virtual void			applyImpulse			(Fvector& dir, float val)					= 0;
	virtual void			applyImpulseTrace		(Fvector& pos, Fvector& dir, float val)		= 0;

	virtual ~CPhysicsShell	()																	= 0;
};

// Implementation creator
CPhysicsElement*			P_create_Element		();
CPhysicsShell*				P_create_Shell			();
