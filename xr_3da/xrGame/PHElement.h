/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////Implemetation//for//CPhysicsElement//////////////////
////////////////////////////////////////////////////////////////////////////////
#include "Geometry.h"
#include "phdefs.h"
//using namespace std;
#ifndef PH_ELEMENT
#define PH_ELEMENT
#include "PhysicsCommon.h"
#include "PHSynchronize.h"
class CPHElement;
class CPHShell;
class CPHFracture;
DEFINE_VECTOR(CODEGeom*,GEOM_STORAGE,GEOM_I)
class CPHFracturesHolder;
class CPHElement	:  
	public CPhysicsElement ,
	public CPHSynchronize
{
	friend class CPHFracturesHolder;
	GEOM_STORAGE			m_geoms;					//e					//bl
	float					m_start_time;				//uu ->to shell ??	//aux
	float					m_volume;					//e ??				//bl
	Fvector					m_mass_center;				//e ??				//bl
	u16						ul_material;				//e ??				//bl
	dMass					m_mass;						//e ??				//bl
	dBodyID					m_body;						//e					//st
	dSpaceID				m_group;					//e					//bl
	dReal					m_l_scale;					// ->to shell ??	//bl
	dReal					m_w_scale;					// ->to shell ??	//bl
	dReal					m_disw_param;				// ->to shell ??	//bl
	dReal					m_disl_param;				// ->to shell ??	//bl
	CPhysicsRefObject*		m_phys_ref_object;			//->to shell ??		//bl
	CPHElement				*m_parent_element;			//bool !			//bl
	CPHShell				*m_shell;					//e					//bl
	CPHInterpolation		m_body_interpolation;		//e					//bl
	CPHFracturesHolder		*m_fratures_holder;			//e					//bl
	bool					was_enabled_before_freeze;
	/////disable///////////////////////
	//dVector3 mean_w;
	//dVector3 mean_v;
	dReal						m_w_limit ;				//->to shell ??		//bl
	dReal						m_l_limit ;				//->to shell ??		//bl

	dVector3					m_safe_position;		//e					//st
	dVector3					m_safe_velocity;		//e					//st
	dVector3					previous_p;				//e					//st
	dMatrix3					previous_r;				//e//to angles		//st
	dVector3					previous_p1;			//e					//st
	dMatrix3					previous_r1;			//e//to angles		//st
	Fmatrix						m_inverse_local_transform;//e				//bt
	//dVector3 previous_f;
	//dVector3 previous_t;
	dReal						previous_dev;			//e					//st
	dReal						previous_v;				//e					//st
	UINT						dis_count_f;			//e					//st
	UINT						dis_count_f1;			//e					//st
	dReal						k_w;					//->to shell ??		//st
	dReal						k_l;//1.8f;				//->to shell ??		//st

	bool						b_contacts_saved;		//e					//aux
	dJointGroupID				m_saved_contacts;		//e					//bt
	
	ContactCallbackFun*			contact_callback;		//->to shell ??		//bt
	ObjectContactCallbackFun*	object_contact_callback;//->to shell ??		//st
	ObjectContactCallbackFun*	temp_for_push_out;		//->to shell ??		//aux

	u32							push_untill;			//->to shell ??		//st
	bool						bUpdate;				//->to shell ??		//st
public:
	IC	CODEGeom*				Geom(u16 num){R_ASSERT2 (num<m_geoms.size(),"out of range"); return m_geoms[num]; }
	/////////////////////////////////////////////////////////////////////////////

	////////////////////////////
private:
	void					build_Geom						(CODEGeom&	V);																	//aux
	void					build_Geom						(u16 i);																		//aux
	void					calculate_it_data				(const Fvector& mc,float mass);													//aux
	void					calculate_it_data_use_density	(const Fvector& mc,float density);												//aux
	void					calc_it_fract_data_use_density  (const Fvector& mc,float density);//sets element mass and fractures parts mass	//aux
	dMass					recursive_mass_summ				(u16 start_geom,FRACTURE_I cur_fracture);										//aux
	void					Disabling						();																				//caled by ph upadate state influent
	void					unset_Pushout					();																				//caled by ph upadate state influent
	void					FillInterpolation				()																				//interpolation called anywhere visual influent
	{
		m_body_interpolation.ResetPositions();
		m_body_interpolation.ResetRotations();
		bUpdate=true;
	}
IC	void					UpdateInterpolation				()																				//interpolation called from ph update visual influent
	{
		m_body_interpolation.UpdatePositions();
		m_body_interpolation.UpdateRotations();
		bUpdate=true;
	}
public:
	void					Disable							();																				//ph state influent called from ph update
	void					ReEnable						();																				//ph state influent called from ph update
	void					ResetDisable					();																				//aux
	void					Enable							();																				//aux
	virtual void			SetAirResistance				(dReal linear=default_k_l, dReal angular=default_k_w)							//aux (may not be)
	{
		k_w= angular;
		k_l=linear;
	}
	void					CallBack						(CBoneInstance* B);																//called from updateCL visual influent
	void					CallBack1						(CBoneInstance* B);																//called from updateCL visual influent
	void					PhDataUpdate					(dReal step);																	//ph update
	void					PhTune							(dReal step);																	//ph update
	virtual void			Freeze							()				;
	virtual void			UnFreeze						()				;
	virtual CPhysicsShell*	PhysicsShell					();																				//aux
	virtual void			get_Extensions					(const Fvector& axis,float center_prg,float& lo_ext, float& hi_ext);			//aux
	virtual void			set_ParentElement				(CPhysicsElement* p){m_parent_element=(CPHElement*)p;}							//aux
	virtual void			set_DisableParams				(float dis_l=default_disl,float dis_w=default_disw);							//aux (may not be)
	virtual void			applyImpulseTrace				(const Fvector& pos, const Fvector& dir, float val,const u16 id)	;					//called anywhere ph state influent


	///
	virtual	void			add_Sphere						(const Fsphere&		V);															//aux
	virtual	void			add_Box							(const Fobb&		V);															//aux
	virtual	void			add_Cylinder					(const Fcylinder&	V);															//aux
	virtual void			add_Shape						(const SBoneShape& shape);														//aux
	virtual void			add_Shape						(const SBoneShape& shape,const Fmatrix& offset);								//aux
	virtual CODEGeom*		last_geom						(){return m_geoms.back();}														//aux
	virtual bool			has_geoms						(){return !m_geoms.empty();}
	virtual void			set_ContactCallback				(ContactCallbackFun* callback);													//aux (may not be)
	virtual void			set_DynamicLimits				(float l_limit=default_l_limit,float w_limit=default_w_limit);					//aux (may not be)
	virtual void			set_DynamicScales				(float l_scale=default_l_scale,float w_scale=default_w_scale);					//aux (may not be)
	virtual void			set_ObjectContactCallback		(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
	virtual void			set_PhysicsRefObject			(CPhysicsRefObject* ref_object);												//aux
	virtual CPhysicsRefObject*	PhysicsRefObject			(){return m_phys_ref_object;}													//aux
	virtual void			set_PushOut						(u32 time,PushOutCallbackFun* push_out=PushOutCallback);						//ph state influent called anywhere

	virtual void			getQuaternion					(Fquaternion& quaternion);
	virtual void			setQuaternion					(const Fquaternion& quaternion);
	virtual void			SetGlobalPositionDynamic		(const Fvector& position);
	virtual void			GetGlobalPositionDynamic		(Fvector* v);			
	virtual void			getForce						(Fvector& force);
	virtual void			getTorque						(Fvector& torque);
	virtual void			get_LinearVel					(Fvector& velocity);															//aux
	virtual void			get_AngularVel					(Fvector& velocity);															//aux
	virtual void			set_LinearVel					(const Fvector& velocity);														//called anywhere ph state influent
	virtual void			set_AngularVel					(const Fvector& velocity);														//called anywhere ph state influent
	virtual void			setForce						(const Fvector& force);
	virtual void			setTorque						(const Fvector& torque);
	virtual	void			set_BoxMass						(const Fobb& box, float mass);													//aux
	virtual void			get_State						(		SPHNetState&	state);
	virtual void			set_State						(const	SPHNetState&	state);
	virtual void			add_Mass						(const SBoneShape& shape,const Fmatrix& offset,const Fvector& mass_center,float mass,CPHFracture* fracture=NULL);//aux
	virtual float			getRadius						();																				//aux
	virtual void			InterpolateGlobalTransform		(Fmatrix* m);																	//called UpdateCL vis influent
	virtual void			InterpolateGlobalPosition		(Fvector* v);																	//aux
	virtual void			GetGlobalTransformDynamic		(Fmatrix* m);																	//aux
														//aux
	void					SetShell						(CPHShell* p){m_shell=p;}														//aux
	//virtual CKinematics*	PKinematics						(){return m_shell->PKinematics();}
	void					SetPhObjectInGeomData			(CPHObject* O);																	//aux
	
	
	void			build									();																				//aux
	void			build									(bool disable);																				//aux
	void			destroy									();																				//called anywhere ph state influent
	Fvector			get_mc_data								();																				//aux
	Fvector			get_mc_geoms							();																				//aux
	void			calc_volume_data						();																				//aux
	void			Start									();																				//aux
	void			RunSimulation							();																			//called anywhere ph state influent
	void			RunSimulation							(const Fmatrix& start_from);
	IC CPHFracturesHolder* FracturesHolder					(){return m_fratures_holder;}													//aux

	float			get_volume								()	{calc_volume_data();return m_volume;};										//aux
	void			SetTransform							(const Fmatrix& m0);															//called anywhere ph state influent
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void			SetMaterial				(u16 m);																				//aux

	virtual void			SetMaterial				(LPCSTR m){SetMaterial(GMLib.GetMaterialIdx(m));}										//aux
	virtual	dBodyID			get_body				()		{return m_body;};																//aux
	virtual const Fvector&	mass_Center				()						;																//aux
	virtual const Fvector&	local_mass_Center		()		{return m_mass_center;}															//aux
	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);					//some isues not to be aux
	virtual void			Activate				(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable=false);//some isues not to be aux
	virtual void			Activate				(bool place_current_forms=false,bool disable=false);									//some isues not to be aux
	virtual void			Activate				(const Fmatrix& start_from, bool disable=false);										//some isues not to be aux

	virtual void			Deactivate				();																						//aux
	virtual float			getVolume				(){return m_volume;}																	//aux
	virtual void			setMass					(float M);																				//aux
	virtual float			getMass					(){return m_mass.mass;}																	//aux
	virtual bool			isBreakable				();																						//aux
	virtual void			setDensity				(float M);																				//aux
	virtual float			getDensity				(){return m_mass.mass/m_volume;}														//aux
	virtual void			setMassMC				(float M,const Fvector& mass_center);													//aux
	virtual void			setDensityMC			(float M,const Fvector& mass_center);													//aux
	virtual void			setInertia				(const Fmatrix& M)																	{}	//aux
	virtual u16				setGeomFracturable		(CPHFracture& fracture);																//aux
	virtual CPHFracture&	Fracture				(u16 num);																				//aux
	virtual u16				numberOfGeoms			();																						//aux
	dGeomID					dSpacedGeometry			();																						//aux
			void			PassEndGeoms			(u16 from,u16 to,CPHElement* dest);														//aux

			void			CreateSimulBase			();//create body & cpace																//aux
			void			ReInitDynamics			(const Fmatrix &shift_pivot,float density);//set body & geom positions					//aux
			void			ReAdjustMassPositions	(const Fmatrix &shift_pivot,float density);												//aux
			void			ResetMass				(float density);																		//aux
			void			SplitProcess			(ELEMENT_PAIR_VECTOR &new_elements);													//aux
			void			PresetActive			();			

	virtual	dMass*			getMassTensor			();	//aux
	//aux

	virtual void			applyForce				(const Fvector& dir, float val);															//aux
	virtual void			applyForce				(float x,float y,float z);																//called anywhere ph state influent
	virtual void			applyImpulse			(const Fvector& dir, float val);														//aux
	virtual void			Update					();																						//called update CL visual influence
	CPHElement										();																															//aux
	//CPHElement(){ m_space=ph_world->GetSpace();};
	virtual ~CPHElement	();																															//aux
};

#endif