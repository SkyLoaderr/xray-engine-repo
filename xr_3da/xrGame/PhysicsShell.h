#pragma once

// ABSTRACT:
class	CPhysicsJoint
{
public:
	virtual ~CPhysicsJoint	()																	= 0;
};

// ABSTRACT:
class	CPhysicsBase
{
public:
	Fmatrix					mXFORM;					// In parent space
	Fmatrix					mDesired;				// In parent space
	float					fDesiredStrength;		// Desire strength, [0..1]%
public:
	virtual void			Activate				(Fmatrix& m0, float dt01, Fmatrix& m2)		= 0;
	virtual void			Deactivate				()											= 0;

	virtual void			setMass					(float M)									= 0;

	virtual void			applyForce				(Fvector& dir, float val)					= 0;
	virtual void			applyImpulse			(Fvector& dir, float val)					= 0;

	virtual void			Update					()											= 0;

	virtual ~CPhysicsBase	()																	= 0;
};

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class	CPhysicsElement		: public CPhysicsBase
{
public:
	virtual	void			add_Sphere				(Fsphere&	V)								= 0;
	virtual	void			add_Box					(Fobb&		V)								= 0;
};

// ABSTRACT: 
class CPhysicsShell			: public CPhysicsBase
{
public:
	BOOL					bActive;
public:
	virtual	void			add_Element				(CPhysicsElement* E)						= 0;
	virtual	void			add_Joint				(CPhysicsJoint* E, int E1, int E2)			= 0;
	virtual void			applyImpulseTrace		(Fvector& pos, Fvector& dir, float val)		= 0;
};

// Implementation creator
CPhysicsJoint*				P_create_Joint			(int type);
CPhysicsElement*			P_create_Element		();
CPhysicsShell*				P_create_Shell			();
