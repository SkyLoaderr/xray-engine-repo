#ifndef PhysicsShellH
#define PhysicsShellH
#pragma once

#include "PHDefs.h"
#include "PhysicsCommon.h"
#include "alife_space.h"
#include "script_export_space.h"

ObjectContactCallbackFun PushOutCallback;
class CPhysicsJoint;
class CPhysicsElement;
class CPhysicsShell;
class CPHFracture;
class CPHJointDestroyInfo;
class CODEGeom;
class CPHSynchronize;
class CPhysicsShellHolder;
class CGameObject;
struct physicsBone
{
	CPhysicsJoint* joint;
	CPhysicsElement* element;
	physicsBone()
	{
		joint=NULL;
		element=NULL;
	}
};
DEFINE_MAP	(u16,	physicsBone,	BONE_P_MAP,	BONE_P_PAIR_IT);
typedef const  BONE_P_MAP :: iterator			BONE_P_PAIR_CIT;
// ABSTRACT:
class	CPhysicsBase
{
public:

	BOOL					bActive;
	bool					bActivating;
	Fmatrix					mXFORM;					// In parent space
	Fmatrix					mDesired;				// In parent space
	float					fDesiredStrength;		// Desire strength, [0..1]%
public:
	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false)						= 0;
	virtual void			Activate				(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable=false)	= 0;
	virtual void			Activate				(bool disable=false)									    = 0;
	virtual void			Activate				(const Fmatrix& form,bool disable=false)				= 0;
	virtual void			InterpolateGlobalTransform(Fmatrix* m)											= 0;
	virtual void			GetGlobalTransformDynamic(Fmatrix* m)											= 0;
	virtual void			InterpolateGlobalPosition (Fvector* v)											= 0;
	virtual	void			net_Import				(NET_Packet& P)											= 0;
	virtual	void			net_Export				(NET_Packet& P)											= 0;
	virtual void			GetGlobalPositionDynamic (Fvector* v)											= 0;
	virtual bool			isBreakable				()														= 0;
	virtual bool			isEnabled				()														= 0;
	virtual void			Deactivate				()														= 0;
	virtual void			Enable					()														= 0;
	
	virtual void			setMass					(float M)												= 0;

	virtual void			setDensity				(float M)												= 0;
	virtual float			getMass					()														= 0;
	virtual float			getDensity				()														= 0;
	virtual float			getVolume				()														= 0;
	
	virtual void			applyForce				(const Fvector& dir, float val)							= 0;
	virtual void			applyForce				(float x,float y,float z)								= 0;
	virtual void			applyImpulse			(const Fvector& dir, float val)							= 0;
	virtual void			applyGravityAccel		(const Fvector& accel)									= 0;
	virtual void			SetAirResistance		(float linear=default_k_l, float angular=default_k_w)	= 0;
	virtual void			GetAirResistance		(float	&linear, float &angular)						= 0;
	virtual void			set_DynamicLimits		(float l_limit=default_l_limit,float w_limit=default_w_limit)= 0;
	virtual void			set_DynamicScales		(float l_scale=default_l_scale,float w_scale=default_w_scale)= 0;
	virtual void			set_ContactCallback		(ContactCallbackFun* callback)							= 0;
	virtual void			set_ObjectContactCallback(ObjectContactCallbackFun* callback)					= 0;
	virtual void			set_PhysicsRefObject	(CPhysicsShellHolder* ref_object)							= 0;
	virtual void			get_LinearVel			(Fvector& velocity)										= 0;
	virtual void			get_AngularVel			(Fvector& velocity)										= 0;
	virtual void			set_LinearVel			(const Fvector& velocity)								= 0;
	virtual void			set_AngularVel			(const Fvector& velocity)								= 0;
	virtual void			set_ApplyByGravity		(bool flag)												= 0;
	virtual void			set_PushOut				(u32 time,ObjectContactCallbackFun* push_out=PushOutCallback)	= 0;
	virtual void			SetMaterial				(u16 m)													= 0;
	virtual void			SetMaterial				(LPCSTR m)												= 0;
	virtual void			set_DisableParams		(const SAllDDOParams& params)							= 0;
	virtual void			SetTransform			(const Fmatrix& m0)										= 0;
	virtual					~CPhysicsBase	()																{};
};

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class	CPhysicsElement		: public CPhysicsBase
{

public:
	u16						m_SelfID;
	virtual CPhysicsShell*	PhysicsShell			()												= 0;		
	virtual void			set_ContactCallback		(ContactCallbackFun* callback)					= 0;
	virtual CPhysicsShellHolder*PhysicsRefObject	()												= 0;
	virtual	void			add_Sphere				(const Fsphere&		V)							= 0;
	virtual	void			add_Box					(const Fobb&		V)							= 0;
	virtual	void			add_Cylinder			(const Fcylinder&	V)							= 0;
	virtual void			add_Shape				(const SBoneShape& shape)						= 0;
	virtual void			add_Shape				(const SBoneShape& shape,const Fmatrix& offset)	= 0;
	virtual CODEGeom*		last_geom				()												= 0;
	virtual bool			has_geoms				()												= 0;
	virtual void			add_Mass				(const SBoneShape& shape,const Fmatrix& offset,const Fvector& mass_center,float mass,CPHFracture* fracture=NULL)=0;
	virtual	void			set_ParentElement		(CPhysicsElement* p)							= 0;

	virtual	void			set_BoxMass				(const Fobb& box, float mass)					= 0;	
	virtual void			setInertia				(const dMass& M)								= 0;																
	virtual void			addInertia				(const dMass& M)								= 0;
	virtual void			setMassMC				(float M,const Fvector& mass_center)			= 0;
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const u16 id) = 0;
	virtual void			setDensityMC			(float M,const Fvector& mass_center)			= 0;
	virtual u16				setGeomFracturable		(CPHFracture &fracture)							= 0;
	virtual CPHFracture&	Fracture				(u16 num)										= 0;
	//get
	virtual u16				numberOfGeoms			()												= 0;
	virtual	dBodyID			get_body				()												= 0;
	virtual const Fvector&	mass_Center				()												= 0;
	virtual const Fvector&	local_mass_Center		()												= 0;
	virtual float			getRadius				()												= 0;
	virtual	dMass*			getMassTensor			()												= 0;
	virtual void			get_Extensions			(const Fvector& axis,float center_prg,float& lo_ext, float& hi_ext)=0;
	virtual	void			get_MaxAreaDir			(Fvector& dir)									= 0;
	virtual	void			Fix						()												= 0;
	virtual bool			isFixed					()												= 0;

	virtual ~CPhysicsElement						()												{};
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPhysicsElement)
#undef script_type_list
#define script_type_list save_type_list(CPhysicsElement)

//ABSTRACT:
// Joint between two elements 

class CPhysicsJoint	
{

public:
	bool bActive;
enum eVs 
	{				//coordinate system 
			vs_first,			//in first local
			vs_second,			//in second local 
			vs_global			//in global		
	};
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


//	CPhysicsJoint(CPhysicsElement* first,CPhysicsElement* second,enumType type){pFirst_element=first; pSecond_element=second; eType=type;bActive=false;}

public:
	virtual ~CPhysicsJoint									()												{};
		
	//virtual void SetAxis					(const SPHAxis& axis,const int axis_num)		=0;
	virtual u16				 	BoneID						()												=0;
	virtual	void				SetBoneID					(u16 bone_id)									=0;
	virtual CPhysicsElement*	PFirst_element				()												=0;
	virtual CPhysicsElement*	PSecond_element				()												=0;
	virtual	u16					GetAxesNumber				()												=0;
	virtual void 				Activate					()												=0;
	virtual void 				Create						()												=0;
	virtual void 				RunSimulation				()												=0;
	virtual void 				Deactivate					()												=0;
	virtual void 				SetBackRef					(CPhysicsJoint** j)								=0;
	virtual void 				SetAnchor					(const Fvector& position)						=0;
	virtual void 				SetAxisSDfactors			(float spring_factor,float damping_factor,int axis_num)=0;
	virtual void 				SetJointSDfactors			(float spring_factor,float damping_factor)		=0;
	virtual void 				SetAnchorVsFirstElement		(const Fvector& position)						=0;
	virtual void 				SetAnchorVsSecondElement	(const Fvector& position)						=0;

	virtual void				SetAxisDir					(const Fvector& orientation,const int axis_num)	=0;
	virtual void 				SetAxisDirVsFirstElement	(const Fvector& orientation,const int axis_num)	=0;
	virtual void 				SetAxisDirVsSecondElement	(const Fvector& orientation,const int axis_num)	=0;

	virtual void 				SetAnchor					(const float x,const float y,const float z)						=0;
	virtual void 				SetAnchorVsFirstElement		(const float x,const float y,const float z)						=0;
	virtual void 				SetAnchorVsSecondElement	(const float x,const float y,const float z)						=0;

	virtual void 				SetAxisDir					(const float x,const float y,const float z,const int axis_num)		=0;
	virtual void 				SetAxisDirVsFirstElement	(const float x,const float y,const float z,const int axis_num)	=0;
	virtual void 				SetAxisDirVsSecondElement	(const float x,const float y,const float z,const int axis_num)	=0;


	virtual void 				SetLimits					(const float low,const float high,const int axis_num)			  =0;
	virtual void 				SetLimitsVsFirstElement		(const float low,const float high,const int axis_num)			  =0;
	virtual void 				SetLimitsVsSecondElement	(const float low,const float high,const int axis_num)			  =0;

	virtual void 				SetBreakable				(float force, float torque)										  =0;
virtual CPHJointDestroyInfo*	JointDestroyInfo			()																  =0;
	virtual void				SetForceAndVelocity			(const float force,const float velocity=0.f,const int axis_num=-1)=0;
	virtual void				GetMaxForceAndVelocity		(float &force,float &velocity,int axis_num)						  =0;
	virtual float				GetAxisAngle				(int axis_num)													  =0;
	virtual dJointID			GetDJoint					()																  =0;
	virtual void 				GetLimits					(float& lo_limit,float& hi_limit,int axis_num)					  =0;
	virtual void 				GetAxisDir					(int num,Fvector& axis,eVs& vs)									  =0;
	virtual void 				GetAxisDirDynamic			(int num,Fvector& axis)											  =0;
	virtual void 				GetAnchorDynamic			(Fvector& anchor)												  =0;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CPhysicsJoint)
#undef script_type_list
#define script_type_list save_type_list(CPhysicsJoint)
// ABSTRACT: 
class CPHIsland;
class CPhysicsShell			: public CPhysicsBase
{
protected:
CKinematics* m_pKinematics;
public:

public:
IC	CKinematics*				PKinematics				()					{return m_pKinematics;};
	void						set_Kinematics			(CKinematics* p)	{m_pKinematics=p;}
	virtual void				set_JointResistance		(float force)										= 0;
	virtual	void				add_Element				(CPhysicsElement* E)								= 0;
	virtual	void				add_Joint				(CPhysicsJoint* E)									= 0;
	virtual CPHIsland*			PIsland					()													= 0;

	virtual bool				isFractured				()													= 0;
	virtual void				SplitProcess			(PHSHELL_PAIR_VECTOR &out_shels)					= 0;
	virtual void				applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	= 0;
	virtual void				applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const u16 id) = 0;
	virtual void				applyHit				(const Fvector& pos, const Fvector& dir, float val,const u16 id,ALife::EHitType hit_type)=0;
	virtual BoneCallbackFun*	GetBonesCallback		()													= 0;
	virtual BoneCallbackFun*	GetStaticObjectBonesCallback()												= 0;
	virtual void				Update					()													= 0;

	virtual void				setMass1				(float M)											= 0;
	virtual void				SmoothElementsInertia	(float k)											= 0;
	virtual	void				setEquelInertiaForEls	(const dMass& M)									= 0;
	virtual	void				addEquelInertiaToEls	(const dMass& M)									= 0;
	virtual ELEMENT_STORAGE&	Elements				()													= 0;
	virtual CPhysicsElement*	get_Element				(u16 bone_id)										= 0;
	virtual CPhysicsElement*	get_Element				(const shared_str & bone_name)							= 0;
	virtual CPhysicsElement*	get_Element				(LPCSTR bone_name)									= 0;
	virtual CPhysicsElement*	get_ElementByStoreOrder (u16 num)											= 0;
	virtual u16					get_ElementsNumber		()													= 0;
	virtual CPHSynchronize*		get_ElementSync			(u16 element)										= 0;
	virtual CPhysicsJoint*		get_Joint				(u16 bone_id)										= 0;
	virtual CPhysicsJoint*		get_Joint				(const shared_str & bone_name)							= 0;
	virtual CPhysicsJoint*		get_Joint				(LPCSTR bone_name)									= 0;
	virtual CPhysicsJoint*		get_JointByStoreOrder	(u16 num)											= 0;
	virtual	u16					get_JointsNumber		()													= 0;
	virtual void				Freeze					()													= 0;
	virtual void				UnFreeze				()													= 0;
	virtual void				NetInterpolationModeON	()													= 0;
	virtual void				NetInterpolationModeOFF ()													= 0;
	virtual void				Disable					()													= 0;
	virtual CPhysicsElement*	NearestToPoint			(const Fvector& point)								= 0;
	virtual void				build_FromKinematics	(CKinematics* K,BONE_P_MAP* p_geting_map=NULL)		= 0;
	virtual void				preBuild_FromKinematics	(CKinematics* K,BONE_P_MAP* p_geting_map=NULL)		= 0;
	virtual	void				Build					(bool disable=false)	= 0;
	virtual	void				SetMaxAABBRadius		(float size)										 {};
	virtual void				AddTracedGeom			(u16 element=0,u16 geom=0)							= 0;
	virtual	void				RunSimulation			(bool place_current_forms=true)						= 0;
	virtual void				UpdateRoot				()													= 0;
	virtual void                ZeroCallbacks			()													= 0;
	virtual void				ResetCallbacks			(u16 id,Flags64 &mask)								= 0;
	virtual void				SetCallbacks			(BoneCallbackFun* callback)							= 0;
	virtual void				EnabledCallbacks		(BOOL val)											= 0;
	virtual Fmatrix&			ObjectInRoot			()													= 0;
	virtual	void				ObjectToRootForm		(const Fmatrix& form)							    = 0;
	virtual void				SetPrefereExactIntegration	()												= 0;
	virtual						~CPhysicsShell		    (){}
	//build_FromKinematics		in returns elements  & joint pointers according bone IDs;
	DECLARE_SCRIPT_REGISTER_FUNCTION
	};
add_to_type_list(CPhysicsShell)
#undef script_type_list
#define script_type_list save_type_list(CPhysicsShell)

# define DET(a) 	 (( a._11 * ( a._22 * a._33 - a._23 * a._32 ) -\
a._12 * ( a._21 * a._33 - a._23 * a._31 ) +\
a._13 * ( a._21 * a._32 - a._22 * a._31 ) ))
// Implementation creator
CPhysicsJoint*				P_create_Joint			(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second);
CPhysicsElement*			P_create_Element		();
CPhysicsShell*				P_create_Shell			();
CPhysicsShell*				P_create_splited_Shell	();
CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,LPCSTR	fixed_bones);
CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,U16Vec& fixed_bones);
CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map,LPCSTR	fixed_bones);
CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map=NULL);
CPhysicsShell*				P_build_SimpleShell		(CGameObject* obj,float mass,bool not_active_state);
#endif // PhysicsShellH