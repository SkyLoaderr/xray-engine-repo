#pragma once

typedef void __stdcall BoneCallbackFun(CBoneInstance* B);
typedef  void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	 void __stdcall ObjectContactCallbackFun(bool& do_colide,dContact& c);
struct Fcylinder;
class CPhysicsRefObject
{
public:
	virtual ~CPhysicsRefObject() {}
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
	virtual void			Activate				(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable=false)	= 0;
	virtual void			Activate				(bool  place_current_forms=false,bool disable=false)																			= 0;

	virtual void			Deactivate				()											= 0;
	virtual void			Enable					()											= 0;
	
	virtual void			setMass					(float M)									= 0;
	virtual void			setDensity				(float M)									= 0;
	virtual float			getMass					()											= 0;

	
	virtual void			applyForce				(const Fvector& dir, float val)				= 0;
	virtual void			applyImpulse			(const Fvector& dir, float val)				= 0;
	virtual void			SetAirResistance		(dReal linear=0.0002f, dReal angular=0.05f) = 0;
	virtual void			set_ContactCallback		(ContactCallbackFun* callback)				= 0;
	virtual void			set_ObjectContactCallback(ObjectContactCallbackFun* callback)		= 0;
	virtual void			set_PhysicsRefObject	 (CPhysicsRefObject* ref_object)			= 0;
	virtual void			get_LinearVel			 (Fvector& velocity)						= 0;
	
	virtual void			SetMaterial				(u32 m)										= 0;
	virtual void			SetMaterial				(LPCSTR m)									= 0;
	virtual ~CPhysicsBase	()																	{};
};

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class	CPhysicsElement		: public CPhysicsBase
{

public:
	virtual void			set_ContactCallback		(ContactCallbackFun* callback)					= 0;
	virtual	void			add_Sphere				(const Fsphere&		V)							= 0;
	virtual	void			add_Box					(const Fobb&		V)							= 0;
	virtual	void			add_Cylinder			(const Fcylinder&	V)							= 0;
	virtual	void			set_ParentElement		(CPhysicsElement* p)							= 0;

	virtual void			setInertia				(const Fmatrix& M)								= 0;

	virtual ~CPhysicsElement	()																	{};
};

//ABSTRACT:
// Joint between two elements 
class CPhysicsJoint	
{

public:
enum enumType{
	ball,
	hinge,
	hinge2,
	full_control,
	universal_hinge,
	shoulder1,
	shoulder2,
	car_wheel,
	welding
};
protected:

CPhysicsElement* pFirst_element;
CPhysicsElement* pSecond_element;
enumType eType;

bool bActive;
public:
//	CPhysicsJoint(CPhysicsElement* first,CPhysicsElement* second,enumType type){pFirst_element=first; pSecond_element=second; eType=type;bActive=false;}


	virtual ~CPhysicsJoint	()																{};

	virtual void SetAnchor					(const Fvector& position)						=0;
	virtual void SetAnchorVsFirstElement	(const Fvector& position)						=0;
	virtual void SetAnchorVsSecondElement	(const Fvector& position)						=0;

	virtual void SetAxis					(const Fvector& orientation,const int axis_num)	=0;
	virtual void SetAxisVsFirstElement		(const Fvector& orientation,const int axis_num)	=0;
	virtual void SetAxisVsSecondElement		(const Fvector& orientation,const int axis_num)	=0;

	virtual void SetAnchor					(const float x,const float y,const float z)						=0;
	virtual void SetAnchorVsFirstElement	(const float x,const float y,const float z)						=0;
	virtual void SetAnchorVsSecondElement	(const float x,const float y,const float z)						=0;

	virtual void SetAxis					(const float x,const float y,const float z,const int axis_num)	=0;
	virtual void SetAxisVsFirstElement		(const float x,const float y,const float z,const int axis_num)	=0;
	virtual void SetAxisVsSecondElement		(const float x,const float y,const float z,const int axis_num)	=0;


	virtual void SetLimits					(const float low,const float high,const int axis_num)=0;
	virtual void SetLimitsVsFirstElement	(const float low,const float high,const int axis_num)=0;
	virtual void SetLimitsVsSecondElement	(const float low,const float high,const int axis_num)=0;

	virtual void SetForceAndVelocity		(const float force,const float velocity=0.f,const int axis_num=-1)=0;

};

// ABSTRACT: 
class CPhysicsShell			: public CPhysicsBase
{
protected:
CKinematics* p_kinematics;
public:
	BOOL					bActive;
public:
	void					set_Kinematics			(CKinematics* p)	{p_kinematics=p;}
	virtual void			set_JointResistance		(float force)										= 0;
	virtual	void			add_Element				(CPhysicsElement* E)								= 0;
	virtual	void			add_Joint				(CPhysicsJoint* E)									= 0;
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	= 0;
	virtual BoneCallbackFun* GetBonesCallback		()													= 0;
	virtual void			Update					()													= 0;
	virtual void	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const s16 element) = 0;
	};


// Implementation creator
CPhysicsJoint*				P_create_Joint			(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second);
CPhysicsElement*			P_create_Element		();
CPhysicsShell*				P_create_Shell			();
