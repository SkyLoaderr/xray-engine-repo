#ifndef PhysicsShellH
#define PhysicsShellH
#pragma once
typedef void __stdcall BoneCallbackFun(CBoneInstance* B);
typedef void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	void __stdcall ObjectContactCallbackFun(bool& do_colide,dContact& c);
typedef void __stdcall PushOutCallbackFun(bool& do_colide,dContact& c);

void __stdcall PushOutCallback(bool& do_colide,dContact& c);
void __stdcall PushOutCallback1(bool& do_colide,dContact& c);



class CPhysicsRefObject
{
public:
	virtual ~CPhysicsRefObject() {}
};

extern const dReal fixed_step;
IC float Erp(float k_p,float k_d)		{return ((fixed_step*(k_p)) / (((fixed_step)*(k_p)) + (k_d)));}
IC float Cfm(float k_p,float k_d)		{return (1.f / (((fixed_step)*(k_p)) + (k_d)));}
IC float Spring(float cfm,float erp)	{return ((erp)/(cfm)/fixed_step);}
IC float Damping(float cfm,float erp)	{return ((1.f-(erp))/(cfm));}
extern const dReal default_l_limit;
extern const dReal default_w_limit;
extern const dReal default_l_scale;
extern const dReal default_w_scale;

class CPhysicsJoint;
class CPhysicsElement;
struct physicsBone
{
	CPhysicsJoint* joint;
	CPhysicsElement* element;
};
DEFINE_MAP	(int,	physicsBone,	BONE_P_MAP,	BONE_P_PAIR_IT);

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

	virtual void			Deactivate				()														= 0;
	virtual void			Enable					()														= 0;
	
	virtual void			setMass					(float M)												= 0;

	virtual void			setDensity				(float M)												= 0;
	virtual float			getMass					()														= 0;

	
	virtual void			applyForce				(const Fvector& dir, float val)							= 0;
	virtual void			applyForce				(float x,float y,float z)								= 0;
	virtual void			applyImpulse			(const Fvector& dir, float val)							= 0;
	virtual void			SetAirResistance		(dReal linear=0.0002f, dReal angular=0.05f)				= 0;
	virtual void			set_DynamicLimits		(float l_limit=default_l_limit,float w_limit=default_w_limit)					= 0;
	virtual void			set_DynamicScales		(float l_scale=default_l_scale,float w_scale=default_w_scale)					= 0;
	virtual void			set_ContactCallback		(ContactCallbackFun* callback)							= 0;
	virtual void			set_ObjectContactCallback(ObjectContactCallbackFun* callback)					= 0;
	virtual void			set_PhysicsRefObject	(CPhysicsRefObject* ref_object)							= 0;
	virtual void			get_LinearVel			(Fvector& velocity)										= 0;
	virtual void			get_AngularVel			(Fvector& velocity)										= 0;
	virtual void			set_PushOut				(u32 time,PushOutCallbackFun* push_out=PushOutCallback)	= 0;
	virtual void			SetMaterial				(u32 m)													= 0;
	virtual void			SetMaterial				(LPCSTR m)												= 0;
	
	virtual ~CPhysicsBase	()																	{};
};

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class	CPhysicsElement		: public CPhysicsBase
{

public:
	virtual void			InterpolateGlobalTransform(Fmatrix* m)									= 0;
	virtual void			set_ContactCallback		(ContactCallbackFun* callback)					= 0;
	virtual	void			add_Sphere				(const Fsphere&		V)							= 0;
	virtual	void			add_Box					(const Fobb&		V)							= 0;
	virtual	void			add_Cylinder			(const Fcylinder&	V)							= 0;
	virtual void			add_Shape				(const SBoneShape& shape)						= 0;
	virtual void			add_Shape				(const SBoneShape& shape,const Fmatrix& offset)	= 0;
	virtual void			add_Mass				(const SBoneShape& shape,const Fmatrix& offset,const Fvector& mass_center,float mass)=0;
	virtual	void			set_ParentElement		(CPhysicsElement* p)							= 0;
	virtual	void			set_BoxMass				(const Fobb& box, float mass)					= 0;	
	virtual void			setInertia				(const Fmatrix& M)								= 0;
	virtual void			setMassMC				(float M,const Fvector& mass_center)			= 0;
	virtual void			setDensityMC			(float M,const Fvector& mass_center)			= 0;
	virtual	dBodyID			get_body				()												= 0;
	virtual float			getRadius				()												= 0;
	virtual ~CPhysicsElement	()																	{};
};

//ABSTRACT:
// Joint between two elements 

class CPhysicsJoint	
{

public:

 
enum enumType{				//joint type

	ball,					// ball-socket
	hinge,					// standart hinge 1 - axis
	hinge2,					// for car wheels 2-axes 
	full_control,			// 3 - axes control (eiler - angles)
	universal_hinge,		//uu
	shoulder1,				//uu
	shoulder2,				//uu
	car_wheel,				//uu
	welding					//spesial - creates one rigid body 
};


	enumType eType;          //type of the joint




protected:
	CPhysicsElement* pFirst_element;
	CPhysicsElement* pSecond_element;
	bool bActive;
//	CPhysicsJoint(CPhysicsElement* first,CPhysicsElement* second,enumType type){pFirst_element=first; pSecond_element=second; eType=type;bActive=false;}

public:
	virtual ~CPhysicsJoint	()																{};
	
	//virtual void SetAxis					(const SPHAxis& axis,const int axis_num)		=0;
	virtual void Activate					()												=0;
	virtual void SetAnchor					(const Fvector& position)						=0;
	virtual void SetAxisSDfactors			(float spring_factor,float damping_factor,int axis_num)=0;
	virtual void SetJointSDfactors			(float spring_factor,float damping_factor)		=0;
	virtual void SetAnchorVsFirstElement	(const Fvector& position)						=0;
	virtual void SetAnchorVsSecondElement	(const Fvector& position)						=0;

	virtual void SetAxisDir					(const Fvector& orientation,const int axis_num)	=0;
	virtual void SetAxisDirVsFirstElement	(const Fvector& orientation,const int axis_num)	=0;
	virtual void SetAxisDirVsSecondElement	(const Fvector& orientation,const int axis_num)	=0;

	virtual void SetAnchor					(const float x,const float y,const float z)						=0;
	virtual void SetAnchorVsFirstElement	(const float x,const float y,const float z)						=0;
	virtual void SetAnchorVsSecondElement	(const float x,const float y,const float z)						=0;

	virtual void SetAxisDir					(const float x,const float y,const float z,const int axis_num)	=0;
	virtual void SetAxisDirVsFirstElement		(const float x,const float y,const float z,const int axis_num)	=0;
	virtual void SetAxisDirVsSecondElement		(const float x,const float y,const float z,const int axis_num)	=0;


	virtual void SetLimits					(const float low,const float high,const int axis_num)=0;
	virtual void SetLimitsVsFirstElement	(const float low,const float high,const int axis_num)=0;
	virtual void SetLimitsVsSecondElement	(const float low,const float high,const int axis_num)=0;

	virtual void SetForceAndVelocity		(const float force,const float velocity=0.f,const int axis_num=-1)=0;
	virtual dJointID GetDJoint				()																  =0;
	virtual void GetLimits					(float& lo_limit,float& hi_limit,int axis_num)					  =0;
};

// ABSTRACT: 
class CPhysicsShell			: public CPhysicsBase
{
protected:
CKinematics* m_pKinematics;
public:
	BOOL					bActive;
public:
	void						set_Kinematics			(CKinematics* p)	{m_pKinematics=p;}
	virtual void				set_JointResistance		(float force)										= 0;
	virtual	void				add_Element				(CPhysicsElement* E)								= 0;
	virtual	void				add_Joint				(CPhysicsJoint* E)									= 0;
	virtual void				applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	= 0;
	virtual BoneCallbackFun*	GetBonesCallback		()													= 0;
	virtual void				Update					()													= 0;
	virtual void				applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const s16 element) = 0;
	virtual void				setMass1				(float M)											= 0;
	virtual void				SmoothElementsInertia	(float k)											= 0;
	virtual CPhysicsElement*	get_Element				(s16 bone_id)										= 0;
	virtual CPhysicsElement*	get_Element				(LPCSTR bone_name)									= 0;
	virtual void				build_FromKinematics	(CKinematics* K,BONE_P_MAP* p_geting_map=NULL)		= 0;
	//build_FromKinematics		in returns elements  & joint pointers according bone IDs;
	};


// Implementation creator
CPhysicsJoint*				P_create_Joint			(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second);
CPhysicsElement*			P_create_Element		();
CPhysicsShell*				P_create_Shell			();

#endif // PhysicsShellH