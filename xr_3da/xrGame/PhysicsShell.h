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
	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false)		= 0;
	virtual void			Deactivate				()											= 0;

	virtual void			setMass					(float M)									= 0;

	virtual void			applyForce				(const Fvector& dir, float val)				= 0;
	virtual void			applyImpulse			(const Fvector& dir, float val)				= 0;

	virtual ~CPhysicsBase	()																	{};
};

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class	CPhysicsElement		: public CPhysicsBase
{
public:
	virtual	void			add_Sphere				(const Fsphere&	V)									= 0;
	virtual	void			add_Box					(const Fobb&		V)								= 0;
};

// ABSTRACT: 
class CPhysicsShell			: public CPhysicsBase
{
public:
	BOOL					bActive;
public:
	virtual	void			add_Element				(CPhysicsElement* E)								= 0;
	virtual	void			add_Joint				(CPhysicsJoint* E, int E1, int E2)					= 0;
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	= 0;

	virtual void			Update					()													= 0;
};

// Implementation creator
CPhysicsJoint*				P_create_Joint			(int type);
CPhysicsElement*			P_create_Element		();
CPhysicsShell*				P_create_Shell			();
